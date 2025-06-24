import network
import socket
import time
import uselect as select

class inspec_stream:

    def __init__(self, config):
        self.config = config
        self.error = None
        self.wlan = None
        self.ip = None
        self.servers = [None, None, None]
        self.clients = [None, None, None]
        self.ports = [8080, 8082, 5000]
        self.connected = [False, False, False]

        if self.config.get('WiFi'):
            self.connect_network(self.config.get('WiFiNetworkName'), self.config.get('WiFiKey'))

        if self.config.get('AccessPoint'):
            self.start_access_point(self.config.get('AccessPointName'), self.config.get('AccessPointPassword'))

        if self.wlan == None:
            return

        self.ip = self.wlan.ifconfig()[0]
        print("IP", self.ip)

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
            time.sleep_ms(1024)
            attempts = attempts + 1
            if attempts > 12:
                raise Exception("Failed to connect to " + ssid)

    def start_server(self, id):
        if self.servers[id] == None:
            self.servers[id] = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.servers[id].setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)

            host = ""
            self.servers[id].bind([host, self.ports[id]])
            self.servers[id].listen(1)

            print(f'Waiting for connection on {self.ip}:{self.ports[id]}')

        try:
            sockets, w, err = select.select((self.servers[id],), (), (), 0)

            if sockets:
                for entry in sockets:
                    self.clients[id], addr = self.servers[id].accept()
                    
                    try:
                        self.clients[id].settimeout(5.0)
                        print("Connected to " + addr[0] + ":" + str(addr[1]))
                        self.start_streaming(id)
                        self.connected[id] = True
                    except OSError as e:
                        self.connected[id] = False
                        self.clients[id].close()
                        print("client socket error:", id, e)
                        self.start_server(id)

        except OSError as e:
            print("server socket error:", e)
            self.connected[id] = False
            self.servers[id].close()
            self.servers[id] = None
            self.start_server(id)

    def start_streaming(self, id):
        data = self.clients[id].recv(1024)
        self.clients[id].send(
            "HTTP/1.1 200 OK\r\n"
            "Server: INSPEC\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Type: multipart/x-mixed-replace;boundary=inspec\r\n"
            "Cache-Control: no-cache\r\n"
            "Pragma: no-cache\r\n\r\n"
        )

    def send_image(self, image):
        if self.connected[0] == False:
            return

        cframe = image.to_jpeg(quality=self.config.get('WiFiImageQuality'), copy=True)
        header = (
            "\r\n--inspec\r\n"
            "Content-Type: image/jpeg\r\n"
            "Content-Length:" + str(cframe.size()) + "\r\n\r\n"
        )
        self.send_data(0, cframe, header)
        self.send_data(1, cframe, header)

    def send_data(self, id, cframe, header):
        if self.connected[id] == False:
            return

        try:
            if header != None:
                self.clients[id].sendall(header)
            self.clients[id].sendall(cframe)
        except OSError as e:
            self.connected[id] = False

            if id != 2:
                self.start_server(id)

            if id == 0:
                self.error = e
            print("client socket error", id, e)
