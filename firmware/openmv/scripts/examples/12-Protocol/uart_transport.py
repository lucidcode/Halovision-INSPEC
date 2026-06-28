"""
UART Transport Example for OpenMV Protocol

This example shows how to create a UART-based transport for the OpenMV
protocol. Save this file as boot.py to run automatically on startup.

The transport class implements the physical layer interface required by
the protocol module:
- is_active(): Returns True if the connection is available.
- size(): Returns the number of bytes available to read.
- read(offset, size): Reads bytes from the UART into a buffer.
- write(offset, data): Writes data to the UART.
- flush(): Flushes the UART transmit buffer.
"""
import protocol
from machine import UART


class UartTransport:
    """UART-based transport channel for OpenMV Protocol"""

    def __init__(self, uart_id=1, baudrate=115200, timeout=1000, rxbuf=1024):
        self.uart = UART(
            uart_id, baudrate, rxbuf=rxbuf, timeout=timeout, timeout_char=500, flow=UART.RTS | UART.CTS
        )
        self.buf = memoryview(bytearray(rxbuf))

    def is_active(self):
        return True

    def size(self):
        return self.uart.any()

    def read(self, offset, size):
        size = self.uart.readinto(self.buf, size)
        return None if size is None else self.buf[:size]

    def write(self, offset, data):
        return self.uart.write(data)

    def flush(self):
        self.uart.flush()


if __name__ == "__main__":
    # Buffer(4096) - Header(10) - CRC(4) = 4082
    MAX_PAYLOAD = 4096 - 10 - 4

    # Initialize and configure the protocol
    protocol.init(
        crc=True,               # Enable CRC
        seq=True,               # Enable sequence checking
        ack=True,               # Wait for CKs
        events=True,            # Enable async-events
        max_payload=MAX_PAYLOAD,  # Max packet payload
        rtx_retries=3,          # Retransmission retry count
        rtx_timeout_ms=500,     # Timeout before retransmission (doubled after each try)
        lock_interval_ms=10,    # Minimum locking interval
        poll_ms=10,             # Schedules the protocol task every 10ms
    )

    # Register the transport
    protocol.register(
        name="uart",
        flags=protocol.CHANNEL_FLAG_PHYSICAL,
        backend=UartTransport(7, timeout=5000, rxbuf=8 * 1024, baudrate=921600),
    )
