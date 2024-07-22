import bluetooth
import random
import struct
import ubinascii
from ble_advertising import advertising_payload
from micropython import const
import gc

class inspec_comms:
    _APPEARANCE_HUMAN_INTERFACE_DEVICE = const(960)

    _IRQ_CENTRAL_CONNECT = const(1)
    _IRQ_CENTRAL_DISCONNECT = const(2)
    _IRQ_GATTS_WRITE = const(3)
    _IRQ_GATTS_INDICATE_DONE = const(20)

    def __init__(self):
        self._connections = set()
        self.sending_image = False
        self.sending_file = False

        self.connected = False

        self.ble = bluetooth.BLE()
        self.ble.active(True)

        self.ble.irq(self.irq)
        self.register()
        self.advertise()

        self.wait = 0
        self.message_received = None

    def register(self):
        BLE_UUID = '13370001-763c-4507-99fb-100f72f2300a'
        RX_UUID = '13370002-763c-4507-99fb-100f72f2300a'
        TX_UUID = '13370003-763c-4507-99fb-100f72f2300a'
        IMG_UUID = '13370004-763c-4507-99fb-100f72f2300a'
        FILE_UUID = '13370005-763c-4507-99fb-100f72f2300a'

        BLE_NUS = bluetooth.UUID(BLE_UUID)
        BLE_RX = (bluetooth.UUID(RX_UUID), bluetooth.FLAG_WRITE)
        BLE_TX = (bluetooth.UUID(TX_UUID), bluetooth.FLAG_NOTIFY | bluetooth.FLAG_READ | bluetooth.FLAG_INDICATE)
        BLE_TX_IMG = (bluetooth.UUID(IMG_UUID), bluetooth.FLAG_NOTIFY | bluetooth.FLAG_READ | bluetooth.FLAG_INDICATE)
        BLE_TX_FILE = (bluetooth.UUID(FILE_UUID), bluetooth.FLAG_NOTIFY | bluetooth.FLAG_READ | bluetooth.FLAG_INDICATE)

        BLE_UART = (BLE_NUS, (BLE_TX, BLE_RX, BLE_TX_IMG, BLE_TX_FILE))
        SERVICES = (BLE_UART,)
        ((self.tx, self.rx, self.tx_img, self.tx_file),) = self.ble.gatts_register_services(SERVICES)
        self.ble.gatts_set_buffer(self.rx, 256, True)

    def irq(self, event, data):
        if event == _IRQ_CENTRAL_CONNECT:
            conn_handle, _, _ = data
            self._connections.add(conn_handle)
            self.connected = True
        elif event == _IRQ_CENTRAL_DISCONNECT:
            self.connected = False
            conn_handle, _, _ = data
            self._connections.remove(conn_handle)
            self.advertise()
        elif event == _IRQ_GATTS_INDICATE_DONE:
            conn_handle, value_handle, status = data

        if event == _IRQ_GATTS_WRITE:
            buffer = self.ble.gatts_read(self.rx)
            message = buffer.decode('UTF-8')

            if self.message_received is not None:
                self.message_received(message)

    def advertise(self):
        self.payload = advertising_payload(
            name="INSPEC",
            services=[bluetooth.UUID(0x181A)],
            appearance=_APPEARANCE_HUMAN_INTERFACE_DEVICE,
        )
        self.ble.gap_advertise(100, adv_data=self.payload)

    def send_data(self, data):
        if self.sending_image:
            return
        if self.connected:
            for conn_handle in self._connections:
                try:
                    self.ble.gatts_write(self.tx, data)
                    self.ble.gatts_notify(conn_handle, self.tx)
                except:
                    print("BLE write error")
                    self.connected = False

    def send_image(self, image):
        if self.sending_image:
            return

        gc.collect()
        
        while image.width() >= 240:
            image = image.scale(x_scale=0.5, y_scale=0.5)

        self.compressed = image.to_jpeg(quality=70).bytearray()

        self.cframe = bytearray(len(self.compressed))
        self.cframe[:] = self.compressed

        self.chunks_sent = 0
        self.sending_image = True

    def send_config(self, directory, config):
        if self.sending_file:
            return
        if self.sending_image:
            return

        self.send_data("config", str(directory))

        gc.collect()

        self.cframe = bytearray(len(config))
        self.cframe[:] = config

        self.chunks_sent = 0
        self.sending_file = True

    def process_file(self):
        if self.wait > 0:
            self.wait -= 1
            return

        iterations = 0
        for chunk in range(self.chunks_sent * 200, len(self.cframe), 200):
            self.chunks_sent += 1
            part = self.cframe[chunk:chunk + 200]

            if self.sending_file:
                self.ble.gatts_write(self.tx_file, part)

            if self.sending_image:
                self.ble.gatts_write(self.tx_img, part)
            
            for conn_handle in self._connections:
                if self.sending_file:
                    self.ble.gatts_notify(conn_handle, self.tx_file)
                if self.sending_image:
                    self.ble.gatts_notify(conn_handle, self.tx_img)

            if len(part) + ((self.chunks_sent - 1) * 200) == len(self.cframe):
                self.sending_file = False
                self.sending_image = False

            iterations += 1
            if iterations > 10:
                self.wait = 5
                break

    def disconnect(self):
        if self.connected:
            for conn_handle in self._connections:
                try:
                    self.ble.gap_disconnect(conn_handle)
                except:
                    print("BLE disconnect error")
                    self.connected = False
        self.advertise()
