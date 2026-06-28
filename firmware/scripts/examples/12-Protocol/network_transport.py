"""
Network Transport Example for OpenMV Protocol (UDP)

This example shows how to create a network transport channel using UDP.
The camera connects to a network, sets its mDNS hostname (reachable at
<hostname>.local), and listens for UDP datagrams on a known port.

The IDE (or any client) sends datagrams to <hostname>.local:<port> and
communicates using the OpenMV protocol over UDP.

The transport implements the physical layer interface required by the protocol:
- read(): Read from UDP socket
- write(): Buffer data for the next datagram
- is_active(): Check if a client address is known
- size(): Peek at available bytes without consuming
- flush(): Send buffered writes as a single UDP datagram
"""
import time
import errno
import network
import socket
import protocol
from machine import LED

LED_B = LED("LED_BLUE")

SSID = ""  # Network SSID
KEY = ""  # Network key

HOST = ""  # Use first available interface
PORT = 5555  # Protocol UDP port
HOSTNAME = "openmv"  # mDNS hostname (reachable at openmv.local)


class NetworkTransport:
    """UDP-based network transport for OpenMV Protocol"""

    def __init__(self, host="", port=5555, max_payload=4096):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)
        self.sock.bind((host, port))
        self.sock.setblocking(False)

        self.client = None
        self.txbuf = bytearray(max_payload + 14)
        self.txlen = 0
        self.rxbuf = bytearray(max_payload + 14)
        self.rxlen = 0
        self.rxpos = 0
        LED_B.off()

    def is_active(self):
        return self.client is not None or self.size() > 0

    def size(self):
        avail = self.rxlen - self.rxpos
        if avail > 0:
            return avail
        try:
            data, addr = self.sock.recvfrom(len(self.rxbuf))
        except OSError as e:
            if e.args[0] != errno.EAGAIN:
                self.client = None
            return 0
        if not data:
            return 0
        n = len(data)

        self.rxpos = 0
        self.rxlen = n
        self.rxbuf[:n] = data
        self.client = addr
        return n

    def read(self, offset, size):
        avail = self.rxlen - self.rxpos
        if avail <= 0:
            return None
        n = min(size, avail)
        end = self.rxpos + n
        data = self.rxbuf[self.rxpos:end]
        self.rxpos = end
        return data

    def write(self, offset, data):
        if self.client is None:
            return -1
        n = len(data)
        self.txbuf[self.txlen:self.txlen + n] = data
        self.txlen += n
        return n

    def flush(self):
        if self.client is None or self.txlen == 0:
            return 0
        mv = memoryview(self.txbuf)
        try:
            self.sock.sendto(mv[:self.txlen], self.client)
        except OSError as e:
            self.txlen = 0
            if e.args[0] != errno.EAGAIN:
                self.client = None
            return -1
        self.txlen = 0
        return 0


if __name__ == "__main__":
    # Max payload must fit in one UDP datagram to avoid IP fragmentation.
    # MTU(1500) - IP(20) - UDP(8) - Header(10) - CRC(4) = 1458
    MAX_PAYLOAD = 1400

    # Connect to WiFi first (before protocol.init starts the poll timer)
    network.hostname(HOSTNAME)
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(SSID, KEY)
    wlan.config(pm=wlan.PM_NONE)

    while not wlan.isconnected():
        time.sleep_ms(100)
        LED_B.toggle()

    LED_B.off()

    # Initialize and configure the protocol
    protocol.init(
        crc=True,  # Enable CRC (UDP has no integrity guarantee)
        seq=True,  # Enable sequence checking (UDP has no ordering)
        ack=False,  # Disable ACKs (avoid blocking the main thread)
        events=True,  # Enable async-events
        max_payload=MAX_PAYLOAD,  # Max packet payload
        rtx_retries=3,  # Retransmission retry count
        rtx_timeout_ms=100,  # Timeout before retransmission (WiFi RTT ~5ms)
        lock_interval_ms=10,  # Minimum locking interval
        poll_ms=10,  # Schedules the protocol task every 10ms
    )

    # Register the network transport
    protocol.register(
        name="network",
        flags=protocol.CHANNEL_FLAG_PHYSICAL,
        backend=NetworkTransport(host=HOST, port=PORT, max_payload=MAX_PAYLOAD),
    )
