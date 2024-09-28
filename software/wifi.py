import network
import socket
import time
import uselect as select

class inspec_stream:

    def __init__(self, interface, ssid, password):
        self.error = None
        self.error2 = None

        if interface == "AccessPoint":
            self.start_access_point(ssid, password)

        if interface == "Station":
            self.connect_network(ssid, password)

        self.ip = self.wlan.ifconfig()[0]
        print("IP", self.ip)

        self.server = None
        self.client = None
        self.port = 8080
        self.connected = False
        
        self.server2 = None
        self.client2 = None
        self.port2 = 8082
        self.connected2 = False

    def start_access_point(self, ssid, password):
        self.wlan = network.WLAN(network.AP_IF)
        self.wlan.config(ssid=ssid, key=password, channel=2)
        self.wlan.active(True)

    def connect_network(self, ssid, password):
        self.wlan = network.WLAN(network.STA_IF)
        self.wlan.active(True)
        self.wlan.connect(ssid=ssid, key=password)
        
        attempts = 0
        while not self.wlan.isconnected():
            print("Connecting to " + ssid)
            time.sleep_ms(1000)
            attempts = attempts + 1
            if attempts > 10:
                raise Exception("Failed to connect to " + ssid)

    def start_server(self, server_id):
        server = None
        client = None
        port = 0
        
        if server_id == 0:
            server = self.server
            client = self.client
            port = self.port

        if server_id == 1:
            server = self.server2
            client = self.client2
            port = self.port2

        if server == None:
            server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)

            host = ""
            server.bind([host, port])
            server.listen(1)

            print(f'Waiting for connection on {self.ip}:{port}')

        try:
            sockets, w, err = select.select((server,), (), (), 0)
            
            if server_id == 0:
                self.server = server
            if server_id == 1:
                self.server2 = server

            if sockets:
                for entry in sockets:
                    client, addr = server.accept()
                    
                    if server_id == 0:
                        self.client = client
                    if server_id == 1:
                        self.client2 = client

                    try:
                        client.settimeout(5.0)
                        print("Connected to " + addr[0] + ":" + str(addr[1]))
                        self.start_streaming(server_id)
                        if port == self.port:
                            self.connected = True
                        if port == self.port2:
                            self.connected2 = True
                    except OSError as e:
                        if port == self.port:
                            self.connected = False
                        if port == self.port2:
                            self.connected2 = False
                        client.close()
                        print("client socket error:", server_id, e)
                        self.start_server(server_id)

        except OSError as e:
            print("server socket error:", e)
            if port == self.port:
                self.connected = False
            if port == self.port2:
                self.connected2 = False
            server.close()
            server = None
            self.start_server(server_id)

    def start_streaming(self, server_id):
        client = None

        if server_id == 0:
            client = self.client

        if server_id == 1:
            client = self.client2

        data = client.recv(1024)
        client.send(
            "HTTP/1.1 200 OK\r\n"
            "Server: INSPEC\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: multipart/x-mixed-replace;boundary=inspec\r\n"
            "Cache-Control: no-cache\r\n"
            "Pragma: no-cache\r\n\r\n"
        )

    def send_image(self, image):
        if self.connected == False:
            return

        cframe = image.to_jpeg(quality=35, copy=True)
        header = (
            "\r\n--inspec\r\n"
            "Content-Type: image/jpeg\r\n"
            "Content-Length:" + str(cframe.size()) + "\r\n\r\n"
        )

        try:
            self.client.sendall(header)
            self.client.sendall(cframe)
        except OSError as e:
            self.connected = False
            self.start_server(0)
            self.error = e
            print("client socket error 0:", e)

        if self.connected2 == False:
            return

        try:
            self.client2.sendall(header)
            self.client2.sendall(cframe)
        except OSError as e:
            self.connected2 = False
            self.start_server(1)
            self.error2 = e
            print("client socket error 1:", e)

