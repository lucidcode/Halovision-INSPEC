/**
  @file main.c
  @brief
  FreeRTOS LWIP Example.

 */
/*
 * ============================================================================
 * History
 * =======
 * 2018-09-06 : Created v2
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd 
 * ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#include <stdint.h>
#include <stdbool.h>
#include "ft900.h"
#include "tinyprintf.h"

/* lwIP Headers. */
#include "lwip/sockets.h"
#include "lwip/ip4_addr.h"

/* netif Abstraction Header. */
#include "net.h"

/* FreeRTOS Headers. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINTF(...) do {tfp_printf(__VA_ARGS__);} while (0)
#else
#define DEBUG_PRINTF(...)
#endif

/* Two Demo Tasks are implemented here to illustrate the usage of lwIP using static/dynamic IP address.
 *
 * SERVER DEMO
 * ------------------------------
 * A TCP/UDP server runs listening for requests on PORT 80. Received data is processed
 * and then a response message is sent. The socket is then closed.
 *
 * CLIENT DEMO
 * ------------------------------
 * A TCP/UDP client tries to connect to a predefined server IP address. Once connection is successful, a message
 * is sent and the response from the server is received. The socket is then closed.
 *
 * Both demos work with the Python test scripts to be run on host PC.
 *
 */

#define UDP                 0           // UDP transport
#define TCP                 1           // TCP transport
#define TRANSPORT_TYPE      TCP         // Change me! UDP, TCP or TLS

#define SERVER              0           // FT9XX is a TCP/UDP Server
#define CLIENT              1           // FT9XX is a TCP/UDP Client.
#define DEMO_TYPE           SERVER      // Change me! SERVER or CLIENT

#define USE_DHCP            1           // Change me! 1: Dynamic IP, 0: Static IP
#if !USE_DHCP
#define IP_ADDR             PP_HTONL(LWIP_MAKEU32(192, 168, 1, 190))
#define IP_ADDR_GATEWAY     PP_HTONL(LWIP_MAKEU32(192, 168, 1, 1))
#define IP_SUBNET_MASK      PP_HTONL(LWIP_MAKEU32(255, 255, 255, 0))
#endif


/*
 * NOTE: Be sure to update IP_ADDR_SERVER to the correct Host PC IP address when USE_DHCP = 1
 */
#if DEMO_TYPE == CLIENT
#define IP_ADDR_SERVER                  PP_HTONL(LWIP_MAKEU32(192, 168, 1, 10))  // The "server" should run elsewhere, eg: on a PC
// Choose a port that is unlikely to be blocked by a firewall running on a PC
#if TRANSPORT_TYPE == TCP
#define PORT_NUM_SERVER                 9990
#elif TRANSPORT_TYPE == UDP
#define PORT_NUM_SERVER                 9990
#endif
#else
// There are no firewalls on FT900, so we are free to choose 80!
#if TRANSPORT_TYPE == TCP
#define PORT_NUM_SERVER                 80
#elif TRANSPORT_TYPE == UDP
#define PORT_NUM_SERVER                 80
#endif
#endif

/* Task Configurations. */
#define TASK_STATS_STACK_SIZE           (500)           //Task Stack Size
#define TASK_STATS_PRIORITY             (1)             //Task Priority
#define TASK_STATS_PERIOD_MS            (5000)

#define TASK_SERVER_STACK_SIZE          (500)           //Task Stack Size
#define TASK_SERVER_PRIORITY            (1)             //Task Priority
#define TASK_SERVER_PERIOD_MS           (100)

#define TASK_CLIENT_STACK_SIZE          (500)           //Task Stack Size
#define TASK_CLIENT_PRIORITY            (1)             //Task Priority
#define TASK_CLIENT_PERIOD_MS           (2000)

/* Number of pending request on server's listening port. */
#define SERVER_LISTEN_BACKLOG_SIZE      5

/* Debug LwIP resource free in TIME-WAIT state. */
#define DEBUG_LWIP_RESOURCE_FREE        0

static TaskHandle_t gx_Task_Handle_Connect;
static TaskHandle_t gx_Task_Handle;
#if DEMO_TYPE == CLIENT
static char msg[] = "Hello server\n\0";
#else
static char msg[] = "Hello client\n\0";
#endif

#define TASK_NOTIFY_NETIF_UP    0x01
#define TASK_NOTIFY_LINK_UP     0x02
#define TASK_NOTIFY_LINK_DOWN   0x03

static void app_ethif_status_cb(int netif_up, int link_up, int packet_available);

#if DEMO_TYPE == CLIENT
void vTaskClient(void *pvParameters);
static void process_client(int i, int conn, struct sockaddr_in* addr_server);
#else
void vTaskServer(void *pvParameters);
static void process_server(int i, int conn, struct sockaddr_in* addr_server, struct sockaddr_in* addr_client);
#endif
void vTaskConnect(void *pvParameters);

/** tfp_printf putc
 *  @param p Parameters
 *  @param c The character to write */
void myputc(void* p, char c) {
    uart_write((ft900_uart_regs_t*) p, (uint8_t) c);
}

int main(void)
{
    sys_reset_all();
    interrupt_disable_globally();
    /* enable uart */
    sys_enable(sys_device_uart0);
    gpio_function(48, pad_func_3);
    gpio_function(49, pad_func_3);

    uart_open(UART0, 1,
            UART_DIVIDER_115200_BAUD, uart_data_bits_8, uart_parity_none,
            uart_stop_bits_1);
    /* Enable tfp_printf() functionality... */
    init_printf(UART0, myputc);

    /* Set up Ethernet */
    sys_enable(sys_device_ethernet);

#ifdef NET_USE_EEPROM
    /* Set up I2C */
    sys_enable(sys_device_i2c_master);

    /* Setup I2C channel 0 pins */
    /* Use sys_i2c_swop(0) to activate. */
    gpio_function(44, pad_i2c0_scl); /* I2C0_SCL */
    gpio_function(45, pad_i2c0_sda); /* I2C0_SDA */

    /* Setup I2C channel 1 pins for EEPROM */
    /* Use sys_i2c_swop(1) to activate. */
    gpio_function(46, pad_i2c1_scl); /* I2C1_SCL */
    gpio_function(47, pad_i2c1_sda); /* I2C1_SDA */
#endif

#if DEMO_TYPE == SERVER
    /* Print out a welcome message... */
    uart_puts(UART0,
            "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
            "\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
            "Copyright (C) Bridgetek Pte Ltd \r\n"
            "--------------------------------------------------------------------- \r\n"
            "Welcome to Free RTOS LWIP Example... \r\n"
            "\r\n"
            "Demonstrate a TCP/UDP Server using the LWIP Stack running on FreeRTOS \r\n"
            "--------------------------------------------------------------------- \r\n");

    if (xTaskCreate(vTaskServer,
            "Server",
            TASK_SERVER_STACK_SIZE,
            NULL,
            TASK_SERVER_PRIORITY,
            &gx_Task_Handle) != pdTRUE) {
        DEBUG_PRINTF("Connect Monitor failed\n");
    }
#else
    /* Print out a welcome message... */
    uart_puts(UART0,
            "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
            "\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
            "Copyright (C) Bridgetek Pte Ltd \r\n"
            "--------------------------------------------------------------------- \r\n"
            "Welcome to Free RTOS LWIP Test Example... \r\n"
            "\r\n"
            "Demonstrate a TCP/UDP Client using the LWIP Stack running on FreeRTOS \r\n"
            "--------------------------------------------------------------------- \r\n");

    if (xTaskCreate(vTaskClient,
            "Client",
            TASK_CLIENT_STACK_SIZE,
            NULL,
            TASK_CLIENT_PRIORITY,
            &gx_Task_Handle) != pdTRUE) {
        DEBUG_PRINTF("Client failed\r\n");
    }
#endif  //DEMO_TYPE

    DEBUG_PRINTF("Starting Scheduler.. \r\n");
    /* Start the scheduler so the created tasks start executing. */
    vTaskStartScheduler();

    DEBUG_PRINTF("Should never reach here!\r\n");

    for (;;)
        ;
}

/**
 * This callback is executed in task TCP/IP.
 * @param [in]      up                  0: interface is down, 1: interface is up.
 */
static void app_ethif_status_cb(int netif_up, int link_up, int packet_available)
{
    if (netif_up) {
        //DEBUG_PRINTF("NETIF UP\r\n");
        // Notify task that the interface is UP and IP address is acquired
        xTaskNotify(gx_Task_Handle,TASK_NOTIFY_NETIF_UP,eSetBits);
    }
    else {
        //DEBUG_PRINTF("NETIF DOWN\r\n");
    }

    if (link_up) {
        //DEBUG_PRINTF("LINK UP\r\n");
        // Notify task that the interface is UP and IP address is acquired
        xTaskNotify(gx_Task_Handle,TASK_NOTIFY_LINK_UP,eSetBits);
    }
    else {
        //DEBUG_PRINTF("LINK DOWN\r\n");
        // Notify task that the interface is UP and IP address is acquired
        xTaskNotify(gx_Task_Handle,TASK_NOTIFY_LINK_DOWN,eSetBits);
    }
}

void vTaskConnect(void *pvParameters)
{
    while (1)
    {
        // Check for ethernet disconnection.
        if (net_is_link_up())
        {
            if (!ethernet_is_link_up())
            {
                net_set_link_down();
                DEBUG_PRINTF("Ethernet disconnected.\r\n");
            }
        }
        else
        {
            if (ethernet_is_link_up())
            {
                DEBUG_PRINTF("Ethernet connected.\r\n");
                net_set_link_up();
            }
        }
        for (int i = 0; i < 10; i++)
        {
            net_tick();/* 10 ms delay */
            vTaskDelay(1 * portTICK_PERIOD_MS);
        }
    }
}

#if DEMO_TYPE == SERVER

/**
 * Server.
 * @param [in]      i               Iteration.
 * @param [in]      conn            Connected socket.
 */
static void process_server(int i, int conn, struct sockaddr_in* addr_server, struct sockaddr_in* addr_client)
{
#if TRANSPORT_TYPE == TCP
    DEBUG_PRINTF("Serving TCP connection on %s on port %d ... [%d]\r\n\r\n",
            ipaddr_ntoa((ip_addr_t*)&addr_server->sin_addr), ntohs(addr_server->sin_port), i);
#elif TRANSPORT_TYPE == UDP
    DEBUG_PRINTF("Serving UDP connection on %s on port %d ... [%d]\r\n\r\n",
            ipaddr_ntoa((ip_addr_t*)&addr_server->sin_addr), ntohs(addr_server->sin_port), i);
#endif // TRANSPORT_TYPE == TCP

    int size_rx;
    int size_tx;
    char data[1024] = {0};


#if TRANSPORT_TYPE == TCP
    size_rx = recv(conn, &data, sizeof(data), 0);
#elif TRANSPORT_TYPE == UDP
    socklen_t len = sizeof(*addr_client);
    size_rx = recvfrom(conn, &data, sizeof(data), 0, (struct sockaddr *)addr_client, &len);
#endif // TRANSPORT_TYPE == TCP
    char* addr = ipaddr_ntoa((ip_addr_t*)&addr_client->sin_addr);
    int port = ntohs(addr_client->sin_port);
    DEBUG_PRINTF(">> Received %d bytes from: ('%s', %d)\r\n", size_rx, addr, port);
    DEBUG_PRINTF(">> %s\r\n", data);


#if TRANSPORT_TYPE == TCP
    size_tx = send(conn, msg, sizeof(msg), 0);
#elif TRANSPORT_TYPE == UDP
    size_tx = sendto(conn, msg, sizeof(msg), 0, (struct sockaddr *)addr_client, sizeof(*addr_client));
#endif // TRANSPORT_TYPE == TCP
    if (size_tx != sizeof(msg)) {
        DEBUG_PRINTF("Server send() mismatch, expecting %d got %d\r\n", sizeof(msg), size_tx);
    }
    DEBUG_PRINTF(">> Sent %d bytes to: ('%s', %d)\r\n", size_tx, addr, port);
    DEBUG_PRINTF(">> %s\r\n", msg);
}


/**
 * This is the server task.
 *
 * @param [in]      pvParameters        Unused.
 */
void vTaskServer(void *pvParameters)
{
    int count = 0;
    uint32_t ulNotifiedValue;
#if USE_DHCP
    ip_addr_t ip_addr = {0};
    ip_addr_t gw_addr = {0};
    ip_addr_t net_mask = {0};
#else
    ip_addr_t ip_addr = {IP_ADDR};
    ip_addr_t gw_addr = {IP_ADDR_GATEWAY};
    ip_addr_t net_mask = {IP_SUBNET_MASK};
#endif
    (void) pvParameters;

    DEBUG_PRINTF("\r\n%s\r\n\r\n", __FUNCTION__);

    /* Initialise Ethernet module and LwIP service. */
    net_init(ip_addr, gw_addr, net_mask, USE_DHCP, "FT90x_lwIP_Example",
            app_ethif_status_cb);

    DEBUG_PRINTF("Waiting for netif to come up...\r\n");
    /* Wait till network interface is up (IP address is valid in the case of DHCP) */
    while (xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
            TASK_NOTIFY_NETIF_UP, /* Clear all bits on exit. */
            &ulNotifiedValue, /* Stores the notified value. */
            portMAX_DELAY ) == false)
        ;

    if (xTaskCreate(vTaskConnect,
            "Connect",
            TASK_STATS_STACK_SIZE,
            NULL,
            TASK_STATS_PRIORITY,
            &gx_Task_Handle_Connect) != pdTRUE) {
        DEBUG_PRINTF("Connect Monitor failed\n");
    }

    DEBUG_PRINTF("Waiting for link...\r\n");

    /* Wait till network interface is up (IP address is valid in the case of DHCP) */
    while (xTaskNotifyWait( pdTRUE,    /* Don't clear bits on entry. */
            TASK_NOTIFY_LINK_UP, /* Clear all bits on exit. */
            &ulNotifiedValue, /* Stores the notified value. */
            portMAX_DELAY ) == false)
        ;

    DEBUG_PRINTF("Network connected.\r\n");

    /* Wait for valid IP address */
    if (net_get_ip().addr == IPADDR_ANY)
    {
        DEBUG_PRINTF("\r\nWaiting for configuration");
        while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            if (net_get_ip().addr != IPADDR_ANY)
            {
                DEBUG_PRINTF("\r\n");
                break;
            }
        }
        DEBUG_PRINTF(".");
    }

    struct sockaddr_in addr_server;
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(PORT_NUM_SERVER);
    ip_addr = net_get_ip();
    addr_server.sin_addr.s_addr = ip4_addr_get_u32(&ip_addr);

#if TRANSPORT_TYPE == TCP
#elif TRANSPORT_TYPE == UDP
    while (1) {
#endif // TRANSPORT_TYPE == TCP

        /* Start of socket programming. */
        int sock;
        struct sockaddr_in __attribute__ ((aligned (4))) addr_client;

        /* create a TCP/UDP socket */
#if TRANSPORT_TYPE == TCP
        int newconn, size;

        vTaskDelay(pdMS_TO_TICKS(TASK_SERVER_PERIOD_MS));

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
#elif TRANSPORT_TYPE == UDP
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
#endif // TRANSPORT_TYPE == TCP
        {
            DEBUG_PRINTF("Cannot create socket\r\n");
            while(1);
        }

        DEBUG_PRINTF("Server %s:%d\r\n",
                inet_ntoa(addr_server.sin_addr.s_addr),
                PORT_NUM_SERVER);

        if (bind(sock, (struct sockaddr *) &addr_server, sizeof(addr_server)) < 0)
        {
            DEBUG_PRINTF("Cannot bind socket\r\n");
            while(1);
        }

#if TRANSPORT_TYPE == TCP
        /* listen for incoming connections (TCP listen backlog = SERVER_LISTEN_BACKLOG_SIZE) */
        if (listen(sock, SERVER_LISTEN_BACKLOG_SIZE) < 0)
        {
            DEBUG_PRINTF("Listen failed\r\n");
            while(1);
        }

        vTaskDelay(pdMS_TO_TICKS(TASK_SERVER_PERIOD_MS));

        DEBUG_PRINTF("Waiting for connection...\r\n");
        for (;;)
        {
            vTaskDelay(pdMS_TO_TICKS(TASK_SERVER_PERIOD_MS));

            size = sizeof(addr_client);

            newconn = accept(sock, (struct sockaddr *) &addr_client, (socklen_t *) &size);
            if (newconn > 0)
            {
                process_server(count++, newconn, &addr_server, &addr_client);

                if (close(newconn) < 0)
                {
                    DEBUG_PRINTF("Server connection close failed\r\n");
                }
                else
                {
                    DEBUG_PRINTF("Connection closed!\r\n\r\n");
                    DEBUG_PRINTF("============================================================================\r\n\r\n");
                }
            }
            else
            {
                DEBUG_PRINTF("Restart accept connection\r\n");
            }
        }
#elif TRANSPORT_TYPE == UDP
        process_server(count++, sock, &addr_server, &addr_client);
        if (close(sock) < 0)
        {
            DEBUG_PRINTF("Server connection close failed\r\n");
        }
        else
        {
            DEBUG_PRINTF("Connection closed!\r\n\r\n");
            DEBUG_PRINTF("============================================================================\r\n\r\n");
        }
#endif // TRANSPORT_TYPE == TCP

#if TRANSPORT_TYPE == TCP
#elif TRANSPORT_TYPE == UDP
    }
#endif // TRANSPORT_TYPE == TCP

}

#else        //DEMO_TYPE

/**
 * This is the client task.
 *
 * @param [in]      pvParameters        Unused.
 */
void vTaskClient(void *pvParameters)
{
    int count = 0;
    uint32_t ulNotifiedValue;
#if USE_DHCP
    ip_addr_t ip_addr = {0};
    ip_addr_t gw_addr = {0};
    ip_addr_t net_mask = {0};
#else
    ip_addr_t ip_addr = {IP_ADDR};
    ip_addr_t gw_addr = {IP_ADDR_GATEWAY};
    ip_addr_t net_mask = {IP_SUBNET_MASK};
#endif
    int ret;
    (void) pvParameters;

    DEBUG_PRINTF("\r\n%s\r\n\r\n", __FUNCTION__);

    /* Initialise Ethernet module and LwIP service. */
    net_init(ip_addr, gw_addr, net_mask, USE_DHCP, "FT90x_lwIP_Example",
            app_ethif_status_cb);

    DEBUG_PRINTF("Waiting for netif to come up...\r\n");
    /* Wait till network interface is up (IP address is valid in the case of DHCP) */
    while (xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
            TASK_NOTIFY_NETIF_UP, /* Clear all bits on exit. */
            &ulNotifiedValue, /* Stores the notified value. */
            portMAX_DELAY ) == false)
        ;

    if (xTaskCreate(vTaskConnect,
            "Connect",
            TASK_STATS_STACK_SIZE,
            NULL,
            TASK_STATS_PRIORITY,
            &gx_Task_Handle_Connect) != pdTRUE) {
        DEBUG_PRINTF("Connect Monitor failed\n");
    }

    DEBUG_PRINTF("Waiting for link...\r\n");

    /* Wait till network interface is up (IP address is valid in the case of DHCP) */
    while (xTaskNotifyWait( pdTRUE,    /* Don't clear bits on entry. */
            TASK_NOTIFY_LINK_UP, /* Clear all bits on exit. */
            &ulNotifiedValue, /* Stores the notified value. */
            portMAX_DELAY ) == false)
        ;

    DEBUG_PRINTF("Network connected.\r\n");

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(TASK_SERVER_PERIOD_MS));

        /* Start of socket programming. */
        int sock;
        struct sockaddr_in addr_server;

        /* IP of server this client wishes to connect to. */
        addr_server.sin_family = AF_INET;
        addr_server.sin_port = htons(PORT_NUM_SERVER);
        addr_server.sin_addr.s_addr = IP_ADDR_SERVER;

#if TRANSPORT_TYPE == TCP
        /* create a TCP socket */
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
#elif TRANSPORT_TYPE == UDP
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
#endif // TRANSPORT_TYPE == TCP
        {
            DEBUG_PRINTF("Cannot create socket\r\n");
            while(1);
        }

#if TRANSPORT_TYPE == TCP
        /* Connect to server. */
        ret = connect(sock, (struct sockaddr *) &addr_server, sizeof(addr_server));
        if (ret < 0)
        {
            DEBUG_PRINTF("Client connecting to %s:%d failed, %d\r\n",
                    inet_ntoa(addr_server.sin_addr.s_addr),
                    PORT_NUM_SERVER,
                    ret);
            close(sock);
            sleep(1);
            continue;
        }
#endif // TRANSPORT_TYPE == TCP

        process_client(count++, sock, &addr_server);

        if (close(sock) < 0)
        {
            DEBUG_PRINTF("Client connection close failed\r\n");
        }
        else
        {
            DEBUG_PRINTF("Connection closed!\r\n\r\n");
            DEBUG_PRINTF("============================================================================\r\n\r\n");
        }
    }
}


/**
 * Client.
 * @param [in]      i               Iteration.
 * @param [in]      conn            Connected socket.
 */
static void process_client(int i, int conn, struct sockaddr_in* addr_server)
{
    char* addr = ipaddr_ntoa((ip_addr_t*)&addr_server->sin_addr);
    int port = ntohs(addr_server->sin_port);

#if TRANSPORT_TYPE == TCP
    DEBUG_PRINTF("Connecting to TCP server %s on port %d ... [%d]\r\n\r\n",
            addr, port, i);
#elif TRANSPORT_TYPE == UDP
    DEBUG_PRINTF("Connecting to UDP server %s on port %d ... [%d]\r\n\r\n",
            addr, port, i);
#endif // TRANSPORT_TYPE == TCP

    int size_rx = 0;
    int size_tx = 0;
    char response[1024] = {0};


#if TRANSPORT_TYPE == TCP
    size_tx = send(conn, msg, sizeof(msg), 0);
#elif TRANSPORT_TYPE == UDP
    size_tx = sendto(conn, msg, sizeof(msg), 0, (struct sockaddr *)addr_server, sizeof(*addr_server));
#endif // TRANSPORT_TYPE == TCP
    if (size_tx != sizeof(msg))
    {
        DEBUG_PRINTF("Error: Client send() mismatch, expecting %d got %d. Please check if server is running!\r\n\r\n", sizeof(msg), size_tx);
        sleep(1);
        return;
    }
    DEBUG_PRINTF(">> Sent %d bytes to: ('%s', %d)\r\n", size_tx, addr, port);
    DEBUG_PRINTF(">> %s\r\n", msg);


#if TRANSPORT_TYPE == TCP
    size_rx = recv(conn, response, sizeof(response), 0);
#elif TRANSPORT_TYPE == UDP
    struct timeval timeout;
    timeout.tv_sec = 5;
    setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    socklen_t len = sizeof(*addr_server);
    size_rx = recvfrom(conn, response, sizeof(response), 0, (struct sockaddr *)addr_server, &len);
#endif // TRANSPORT_TYPE == TCP
    if (size_rx <= 0)
    {
        DEBUG_PRINTF("Error: timed out could not connect to server. Please check if server is running! size_rx=%d\r\n\r\n", size_rx);
        sleep(1);
        return;
    }
    DEBUG_PRINTF(">> Received %d bytes from: ('%s', %d)\r\n", size_rx, addr, port);
    DEBUG_PRINTF(">> %s\r\n", response);
}

#endif
