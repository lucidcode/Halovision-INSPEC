import sensor
import os
import ujson
import machine
import utime
import time
from lsd import lucid_scribe_data
from config import inspec_config
from ble import inspec_comms
from wifi import inspec_stream
from machine import LED

class inspec_sensor:
    def __init__(self):
        sensor.reset()

        self.config = inspec_config()
        self.comms = inspec_comms()
        self.comms.message_received = self.ble_message_received

        self.configure_sensor()

        self.img = sensor.snapshot()
        self.extra_fb.replace(self.img)
        
        machine.RTC().datetime((self.config.config['Year'], self.config.config['Month'], self.config.config['Day'], 0, 0, 0, 0, 0))
        
        self.lsd = lucid_scribe_data(self.config)
        self.last_trigger = utime.ticks_ms() - self.config.config['TimeBetweenTriggers']

        self.stream = None
        if self.config.config['AccessPoint']:
            self.stream = inspec_stream("AccessPoint", self.config.config['AccessPointName'], self.config.config['AccessPointPassword'])
            
        if self.config.config['WiFi']:
            self.stream = inspec_stream("Station", self.config.config['WiFiNetworkName'], self.config.config['WiFiKey'])

    def configure_sensor(self):
        if self.config.config['Brightness']:
            sensor.set_brightness(self.config.config['Brightness'])
        if self.config.config['Contrast']:
            sensor.set_brightness(self.config.config['Contrast'])
        if self.config.config['Saturation']:
            sensor.set_brightness(self.config.config['Saturation'])

        if self.config.config['FrameSize'] == 'VGA':
            sensor.set_framesize(sensor.VGA)
        elif self.config.config['FrameSize'] == 'QVGA':
            sensor.set_framesize(sensor.QVGA)
        elif self.config.config['FrameSize'] == 'QQVGA':
            sensor.set_framesize(sensor.QQVGA)
        else:
            sensor.set_framesize(sensor.QVGA)

        if self.config.config['PixelFormat'] == 'RGB565':
            sensor.set_pixformat(sensor.RGB565)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.RGB565)
        if self.config.config['PixelFormat'] == 'Grayscale':
            sensor.set_pixformat(sensor.GRAYSCALE)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.GRAYSCALE)
        if self.config.config['PixelFormat'] == 'BAYER':
            sensor.set_pixformat(sensor.BAYER)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.BAYER)
        if self.config.config['PixelFormat'] == 'YUV422':
            sensor.set_pixformat(sensor.YUV422)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.YUV422)
        if self.config.config['PixelFormat'] == 'JPEG':
            sensor.set_pixformat(sensor.JPEG)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.JPEG)

        self.pixelThreshold = self.config.config['PixelThreshold']
        self.pixelRange = self.config.config['PixelRange']

        if self.config.config['AutoGain']:
            sensor.set_auto_gain(True)
        else:
            sensor.set_auto_gain(False)

        if self.config.config['AutoExposure']:
            sensor.set_auto_exposure(True)
        else:
            sensor.set_auto_exposure(False)

    def snapshot(self):
        self.img = sensor.snapshot()
        return self.img

    def monitor(self):
        self.snapshot()
        self.diff = self.img.variance(self.extra_fb, self.pixelThreshold, self.pixelRange)
        self.extra_fb.replace(self.img)
        self.lsd.log(int(self.diff))

        self.comms.send_data("metrics", str(self.diff))
        
        if self.config.config['AccessPoint'] or self.config.config['WiFi']:
            self.manage_stream()

        if self.comms.sending_image or self.comms.sending_file:
            self.comms.process_file()

        self.detect()
        
        time.sleep_ms(128)

    def manage_stream(self):
        if not self.stream.connected:
            self.stream.start_server()
            if self.stream.error:
                self.stream.error = None
                self.comms.send_data("ip", self.stream.ip)
        else:
            self.stream.send_image(self.img)


    def ble_message_received(self, message):
        print("ble message", message)

        if message == "request.image":
            self.comms.send_image(self.img)

        if message == "request.directories":
            directories = self.lsd.list_directories()
            self.comms.send_data("directories", directories)

        if message.startswith("request.directory"):
            request, directory = message.split(':')
            config = self.lsd.get_config(directory)
            self.comms.send_config(directory, config)

        if message == "request.ip" and not self.stream == None:
            self.comms.send_data("ip", self.stream.ip)

        if message.startswith("update.setting."):
            message = message.replace("update.setting.", "")
            setting, value = message.split(':')

            if setting == "AccessPoint" and value == "1" and self.stream == None:
                self.stream = inspec_stream("AccessPoint", self.config.config['AccessPointName'], self.config.config['AccessPointPassword'])
                self.comms.send_data("ip", self.stream.ip)

            if setting == "WiFi" and value == "1" and self.stream == None:
                self.stream = inspec_stream("Station", self.config.config['WiFiNetworkName'], self.config.config['WiFiKey'])
                self.comms.send_data("ip", self.stream.ip)

            self.config.set(setting, value)
            self.config.save()

            if self.config.is_sensor_setting(setting):
                self.configure_sensor()
        
    def detect(self):
        if self.config.config['Algorithm'] == "Motion Detection":
            if self.diff > self.config.config['TriggerThreshold']:
                self.trigger()
        
    def trigger(self):
        now = utime.ticks_ms()
        if (now - self.last_trigger > self.config.config['TimeBetweenTriggers']):
            self.comms.send_data("trigger", str(self.diff))
            self.comms.send_image(self.img)
            self.last_trigger = now
            print("trigger")
