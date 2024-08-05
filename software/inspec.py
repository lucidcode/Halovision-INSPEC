import sensor
import os
import sys
import ujson
import machine
import utime
import time
import image
from led import lights
from lsd import lucid_scribe_data
from rem import rapid_eye_movement
from face import face_detection
from config import inspec_config
from ble import inspec_comms
from wifi import inspec_stream
from machine import LED

class inspec_sensor:
    def __init__(self):
        self.config = inspec_config()
        self.configure_sensor()

        self.comms = inspec_comms()
        self.comms.message_received = self.ble_message_received

        self.led = lights(self.config)
        self.lsd = lucid_scribe_data(self.config)
        self.face = face_detection(self.config, self.comms)
        self.rem = rapid_eye_movement(self.config, self.face)

        self.img = sensor.snapshot()
        self.extra_fb.replace(self.img)
        self.total_variances = 0
        self.variances = 0
        
        machine.RTC().datetime((self.config.get('Year'), self.config.get('Month'), self.config.get('Day'), 0, 0, 0, 0, 0))

        self.eye_movements = 0
        self.last_trigger = utime.ticks_ms() - self.config.get('TriggerInterval')
        self.trigger_time = sys.maxsize
        self.last_update = utime.ticks_ms()

        self.init_stream()

    def configure_sensor(self):
        sensor.reset()
        sensor.set_hmirror(True if self.config.get('HorizontalMirror') else False)
        sensor.set_vflip(True if self.config.get('VerticalFlip') else False)
        
        if self.config.get('TrackFace'):
            sensor.set_framesize(sensor.HQVGA)
        elif self.config.get('FrameSize') == 'VGA':
            sensor.set_framesize(sensor.VGA)
        elif self.config.get('FrameSize') == 'QVGA':
            sensor.set_framesize(sensor.QVGA)
        elif self.config.get('FrameSize') == 'QQVGA':
            sensor.set_framesize(sensor.QQVGA)
        elif self.config.get('FrameSize') == 'HQVGA':
            sensor.set_framesize(sensor.HQVGA)
        else:
            sensor.set_framesize(sensor.QVGA)

        if self.config.get('PixelFormat') == 'RGB565':
            sensor.set_pixformat(sensor.RGB565)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.RGB565)
        if self.config.get('PixelFormat') == 'Grayscale':
            sensor.set_pixformat(sensor.GRAYSCALE)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.GRAYSCALE)
        if self.config.get('PixelFormat') == 'BAYER':
            sensor.set_pixformat(sensor.BAYER)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.BAYER)
        if self.config.get('PixelFormat') == 'YUV422':
            sensor.set_pixformat(sensor.YUV422)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.YUV422)
        if self.config.get('PixelFormat') == 'JPEG':
            sensor.set_pixformat(sensor.JPEG)
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.JPEG)

        if self.config.get('TrackFace'):
            sensor.set_contrast(3)
            sensor.set_gainceiling(16)
            return
        
        if self.config.get('Brightness'):
            sensor.set_brightness(self.config.get('Brightness'))
        if self.config.get('Contrast'):
            sensor.set_contrast(self.config.get('Contrast'))
        if self.config.get('Saturation'):
            sensor.set_brightness(self.config.get('Saturation'))
        if self.config.get('GainCeiling'):
            sensor.set_gainceiling(self.config.get('GainCeiling'))
        
        if self.config.get('AutoGain'):
            sensor.set_auto_gain(True)
        else:
            sensor.set_auto_gain(False)
#
        if self.config.get('AutoExposure'):
            sensor.set_auto_exposure(True)
        else:
            sensor.set_auto_exposure(False)

    def monitor(self):
        while True:
            try:
                self.img = sensor.snapshot()

                if self.config.get('TrackFace'):
                    self.img.gamma(gamma=1.0, contrast=1.5, brightness=0.0)

                self.face.detect(self.img)
                self.variance = self.img.variance(self.extra_fb, self.config.get('PixelThreshold'), self.config.get('PixelRange'), self.face.face_object)
                if self.variance > 0:
                    self.total_variances = self.total_variances + self.variance
                    self.variances = self.variances + 1
                self.extra_fb.replace(self.img)

                self.detect()
                self.led.process()
                self.process_trigger()

                if (utime.ticks_ms() - self.last_update > 128):
                    self.send_stream()
                    average = 0
                    if self.variances > 0:
                        average = int(self.total_variances / self.variances)

                    face = "1" if self.face.has_face else "0"
                    data = f'{str(average)};{self.eye_movements};{face}'
                    self.comms.send_data(data)
                    self.total_variances = 0
                    self.variances = 0
                    self.last_update = utime.ticks_ms()

                    if self.comms.sending_image or self.comms.sending_file:
                        self.comms.process_file()
                
            except Exception as e:
                print(e)
                if str(e) == "IDE interrupt":
                    break

    def ble_message_received(self, message):
        print("ble message", message)

        if message == "request.image":
            self.comms.send_image(self.img)

        if message == "request.directories":
            directories = self.lsd.list_directories()
            self.comms.send_data(f'directories:{directories}')

        if message.startswith("request.directory"):
            request, directory = message.split(':')
            config = self.lsd.get_config(directory)
            self.comms.send_config(directory, config)

        if message == "request.ip" and not self.stream == None:
            self.comms.send_data(f'ip:{self.stream.ip}')

        if message.startswith("update.setting."):
            message = message.replace("update.setting.", "")
            setting, value = message.split(':')

            self.config.set(setting, value)
            self.config.save()

            if (setting == "AccessPoint" or setting == "WiFi") and value == "1" and self.stream == None:
                self.init_stream()
                if self.stream != None:
                    self.comms.send_data(f'ip:{self.stream.ip}')

            if self.config.is_sensor_setting(setting):
                self.configure_sensor()

        if message == "disconnect":
            self.comms.disconnect()
        
    def detect(self):
        if self.config.get('Algorithm') == "Motion Detection":
            self.detect_motion()

        if self.config.get('Algorithm') == "Face Detection":
            self.detect_face()

        if self.config.get('Algorithm') == "REM Detection":
            self.detect_rem()

    def detect_motion(self):
        motion = 0
        if self.variance > self.config.get('TriggerThreshold'):
            motion = 8
            self.lsd.add_image(self.img, motion)
            self.trigger()

        self.lsd.log(self.variance, motion)

    def detect_face(self):
        motion = 0
        if self.face.has_face:
            motion = 8
            self.lsd.add_image(self.img, motion)
            self.trigger()

        self.lsd.log(self.variance, motion)
        
    def detect_rem(self):
        eye_movements = self.rem.detect(self.variance)
        self.lsd.log(self.variance, eye_movements)

        if self.eye_movements != eye_movements:
            self.eye_movements = eye_movements

            if self.eye_movements >= 8:
                self.trigger()

            if self.eye_movements >= 5:
                self.lsd.add_image(self.img, self.eye_movements)

    def trigger(self):
        now = utime.ticks_ms()
        if now - self.last_trigger > self.config.get('TriggerInterval'):
            self.last_trigger = now
            self.trigger_time = now + self.config.get('TriggerDelay')

    def process_trigger(self):
        now = utime.ticks_ms()
        if self.trigger_time - now < 0:
            self.trigger_time = sys.maxsize
            self.led.blink()
            self.comms.send_data(f'trigger:{str(self.variance)}')

            if self.stream == None or not self.stream.connected:
                self.comms.send_image(self.img)        
                
    def init_stream(self):
        try:
            self.stream = None
            if self.config.get('AccessPoint'):
                self.stream = inspec_stream("AccessPoint", self.config.get('AccessPointName'), self.config.get('AccessPointPassword'))

            if self.config.get('WiFi'):
                self.stream = inspec_stream("Station", self.config.get('WiFiNetworkName'), self.config.get('WiFiKey'))
                
        except Exception as e:
            print("init_stream error: ", str(e))
            self.stream = None

    def send_stream(self):
        if self.stream == None:
            return

        if not self.config.get('AccessPoint') and not self.config.get('WiFi'):
            return

        if not self.stream.connected:
            self.stream.start_server()
            if self.stream.error:
                self.stream.error = None
                self.comms.send_data(f'ip:{self.stream.ip}')
        else:
            self.stream.send_image(self.img)
