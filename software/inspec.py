import sensor
import os
import ujson
import machine
import utime
import time
from lsd import lucid_scribe_data
from config import inspec_config
from ble import inspec_comms

class inspec_sensor:
    def __init__(self):
        sensor.reset()

        self.comms = inspec_comms()
        self.config = inspec_config()

        if self.config['Brightness']:
            sensor.set_brightness(self.config['Brightness'])

        if self.config['FrameSize'] == 'VGA':
            sensor.set_framesize(sensor.VGA)
        elif self.config['FrameSize'] == 'QVGA':
            sensor.set_framesize(sensor.QVGA)
        elif self.config['FrameSize'] == 'QQVGA':
            sensor.set_framesize(sensor.QQVGA)
        else:
            sensor.set_framesize(sensor.QVGA)

        if self.config['Color']:
            sensor.set_pixformat(sensor.RGB565)
            print("Color")
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.RGB565)
        else:
            sensor.set_pixformat(sensor.GRAYSCALE)
            print("GRAYSCALE")
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.GRAYSCALE)

        if self.config['AutoGain']:
            sensor.set_auto_gain(True)
        else:
            sensor.set_auto_gain(False)

        if self.config['AutoExposure']:
            sensor.set_auto_exposure(True)
        else:
            sensor.set_auto_exposure(False)

        self.img = sensor.snapshot()
        self.extra_fb.replace(self.img)
        diff = self.img.variance(self.extra_fb, 128) / 100000
        
        machine.RTC().datetime((self.config['Year'], self.config['Month'], self.config['Day'], 0, 0, 0, 0, 0))
        
        self.lsd = lucid_scribe_data(self.config)
        self.last_trigger = utime.ticks_ms() - self.config['TimeBetweenTriggers']

    def snapshot(self):
        self.img = sensor.snapshot()
        return self.img

    def monitor(self):
        self.snapshot()
        self.diff = self.img.variance(self.extra_fb, 128) / 100000
        self.extra_fb.replace(self.img)
        self.lsd.log(int(self.diff))

        if self.comms.requested_directories:
            self.comms.requested_directories = False
            self.comms.send_data("directories", self.lsd.list_directories())
            return

        self.comms.send_data("metrics", str(self.diff))

        if self.comms.requested_image:
            self.comms.send_image(self.img)

        if self.comms.sending_image:
            self.comms.process_image()

        self.detect()
        
        time.sleep_ms(128)
        
    def detect(self):
        if self.config['Algorithm'] == "Motion Detection":
            if self.diff > self.config['TriggerThreshold']:
                self.trigger()
        
    def trigger(self):
        now = utime.ticks_ms()
        if (now - self.last_trigger > self.config['TimeBetweenTriggers']):
            self.comms.send_data("trigger", str(self.diff))
            self.comms.send_image(self.img)
            self.last_trigger = now
            print("trigger")
