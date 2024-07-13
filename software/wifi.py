import network
import socket
import time
import uselect as select

class inspec_stream:

    def __init__(self, interface, ssid, password):
        self.connected = False
        self.error = None

        if interface == "AccessPoint":
            self.start_access_point(ssid, password)

        if interface == "Station":
            self.connect_network(ssid, password)

        self.ip = self.wlan.ifconfig()[0]
        print("IP", self.ip)

        self.server = None

        self.start_server()

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

    def start_server(self):
        if self.server == None:
            self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)

            host = ""
            port = 8080
            self.server.bind([host, port])
            self.server.listen(1)

            print("Waiting for connection on " + self.ip)

        try:
            sockets, w, err = select.select((self.server,), (), (), 0)
            if sockets:
                for entry in sockets:
                    self.client, self.addr = self.server.accept()

                    try:
                        self.client.settimeout(5.0)
                        print("Connected to " + self.addr[0] + ":" + str(self.addr[1]))
                        self.start_streaming()
                    except OSError as e:
                        self.connected = False
                        self.client.close()
                        print("client socket error:", e)
                        self.start_server()

        except OSError as e:
            self.connected = False
            self.server.close()
            self.server = None
            print("server socket error:", e)
            self.start_server()

    def start_streaming(self):
        data = self.client.recv(1024)
        self.client.send(
            "HTTP/1.1 200 OK\r\n"
            "Server: INSPEC\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: multipart/x-mixed-replace;boundary=inspec\r\n"
            "Cache-Control: no-cache\r\n"
            "Pragma: no-cache\r\n\r\n"
        )
        self.connected = True

    def send_image(self, image):
        if self.connected == False:
            return

        cframe = image.compressed(quality=35)
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
            self.start_server()
            self.error = e
            print("client socket error:", e)
