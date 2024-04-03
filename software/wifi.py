import network
import socket

class inspec_stream:

    def __init__(self, name="INSPEC"):
        self.connected = False
        self.wlan = network.WLAN(network.AP_IF)
        
        key = "1234567890"
        self.wlan.config(ssid=name, key=key, channel=2)
        self.wlan.active(True)
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)

        host = ""
        port = 8080
        self.server.bind([host, port])
        self.server.listen(5)
        self.server.setblocking(True)

        self.start_server()

    def start_server(self):
        try:
            print("Waiting for connection..")
            self.client, self.addr = self.server.accept()
        except OSError as e:
            self.connected = False
            self.server.close()
            self.server = None
            print("server socket error:", e)
            self.start_server()

        try:
            self.client.settimeout(5.0)
            print("Connected to " + self.addr[0] + ":" + str(self.addr[1]))
            self.start_streaming()
        except OSError as e:
            self.connected = False
            self.client.close()
            print("client socket error:", e)
            self.start_server()

    def start_streaming(self):
        data = self.client.recv(1024)
        self.client.send(
            "HTTP/1.1 200 OK\r\n"
            "Server: INSPEC\r\n"
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
            self.client.close()
            self.start_server()
            print("client socket error:", e)