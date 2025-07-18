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
from nrem import non_rapid_eye_movement
from quality import sleep_quality
from face import face_detection
from config import inspec_config
from ble import inspec_comms
from wifi import inspec_stream
from version import version
from machine import LED

class inspec_sensor:
    def __init__(self):
        print(f'INSPEC {version}')
        self.error = None
        self.config = inspec_config()
        self.configure_sensor()

        self.comms = inspec_comms(self.ble_message_received)
        self.led = lights(self.config)
        self.lsd = lucid_scribe_data(self.config)
        self.face = face_detection(self.config, self.comms)
        self.rem = rapid_eye_movement(self.config, self.face)
        self.nrem = non_rapid_eye_movement(self.config, self.face)
        self.quality = sleep_quality(self.config, self.process_api)

        self.img = sensor.snapshot()
        self.extra_fb.replace(self.img)
        self.peak_variance = 0
        self.global_variance = 0
        
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
        
        sensor.set_framesize(sensor.HQVGA)

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

                self.face.detect(self.img, self.global_variance)
                self.global_variance, self.variance = self.img.variation(self.extra_fb, self.config.get('PixelThreshold'), self.config.get('PixelRange'), self.face.face_object)
                
                if self.variance > self.peak_variance:
                    self.peak_variance = self.variance
                self.extra_fb.replace(self.img)
                self.face.draw_region(self.img)

                self.detect_motion()
                self.detect_face()
                self.detect_rem()
                self.detect_nrem()
                self.quality.measure(self.variance)
                
                self.led.process()
                self.process_trigger()
                self.process_api("variance", self.variance)

                if (utime.ticks_ms() - self.last_update > 128):
                    face = "1" if self.face.has_face else "0"
                    data = f'{str(self.peak_variance)};{self.rem.eye_movements};{face};{self.quality.indicator}'
                    self.comms.send_data(data)
                    self.send_stream()
                    self.peak_variance = 0
                    self.last_update = utime.ticks_ms()

                    if self.comms.sending_image or self.comms.sending_file:
                        self.comms.process_file()

                if self.error != None and self.comms.send_errors:
                    self.comms.send_data(f'error:{self.error}')
                    self.error = None

            except Exception as e:
                self.error = str(e)
                print("Error", self.error)
                self.led.blink("R", 8)
                if self.error == "IDE interrupt":
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

        if message == "request.version":
            self.comms.send_data(f'version:{version}')

        if message == "request.errors":
            self.comms.send_errors = True

        if message.startswith("update.setting."):
            message = message.replace("update.setting.", "")
            setting, value = message.split(':')

            self.config.set(setting, value)
            self.config.save()
            self.led.blink("B", 8)

            if (setting == "AccessPoint" or setting == "WiFi") and value == "1":
                if self.stream != None and self.stream.ip != None:
                    self.comms.send_data(f'ip:{self.stream.ip}')
                    return

                self.init_stream()
                
                if self.stream != None and self.stream.ip != None:
                    self.comms.send_data(f'ip:{self.stream.ip}')

            if self.config.is_sensor_setting(setting):
                self.configure_sensor()

        if message == "flash.leds":
            self.led.flash()

        if message == "disconnect":
            self.comms.disconnect()

        if message == "restart":
            machine.reset()

    def detect_motion(self):
        if self.config.get('Algorithm') != "Motion Detection":
            return

        motion = 0
        if self.variance > self.config.get('TriggerThreshold'):
            motion = 8
            self.lsd.add_image(self.img, motion)
            self.trigger()

        self.lsd.log(self.variance, motion, 0)

    def detect_face(self):
        if self.config.get('Algorithm') != "Face Detection":
            return

        motion = 0
        if self.face.has_face:
            motion = 8
            self.lsd.add_image(self.img, motion)
            self.trigger()

        self.lsd.log(self.variance, motion, 0)
        
    def detect_rem(self):
        eye_movements = self.rem.detect(self.variance, self.global_variance)
        
        if self.config.get('Algorithm') == "REM Detection":
            self.lsd.log(self.variance, self.rem.eye_movements, self.quality.indicator)

        if self.eye_movements != eye_movements:
            self.eye_movements = eye_movements
            self.process_api("rem", self.eye_movements)

            if self.config.get('Algorithm') != "REM Detection":
                return

            if self.eye_movements >= 8:
                self.trigger()

            if self.eye_movements >= 5:
                self.lsd.add_image(self.img, self.eye_movements)
        
    def detect_nrem(self):
        if self.config.get('Algorithm') != "NREM1 Detection":
            return

        eye_movements = self.nrem.detect(self.variance)
        self.lsd.log(self.variance, self.nrem.nrem1, self.quality.indicator)

        if self.eye_movements != eye_movements:
            self.eye_movements = eye_movements
            self.process_api("nrem1", self.eye_movements)

            if self.eye_movements >= 8:
                self.trigger()
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
            self.led.flash()
            self.comms.send_data(f'trigger:{str(self.variance)}')
            self.process_api("trigger", self.variance)

            if self.stream == None or not self.stream.connected:
                self.comms.send_image(self.img)        
                
    def init_stream(self):
        try:
            self.stream = None
            self.stream = inspec_stream(self.config)

        except Exception as e:
            self.error = str(e)
            print("init_stream error: ", self.error)
            self.stream = None

    def send_stream(self):
        if self.stream == None:
            return

        if not self.config.get('AccessPoint') and not self.config.get('WiFi'):
            return

        if not self.stream.connected[0]:
            self.stream.start_server(0)
            if self.stream.error:
                self.stream.error = None
                self.comms.send_data(f'ip:{self.stream.ip}')
        else:
            self.stream.send_image(self.img)

        if not self.stream.connected[1] and self.config.get('SecondStream'):
            self.stream.start_server(1)

    def process_api(self, endpoint, data):
        if self.stream == None:
            return

        if not self.config.get('WebhookApi'):
            return

        if not self.config.get('AccessPoint') and not self.config.get('WiFi'):
            return

        if not self.stream.connected[2]:
            self.stream.start_server(2)
            if self.stream.error:
                self.stream.error = None
        else:
            api_data = f'{endpoint}:{str(data)}'
            if endpoint == "variance":
                api_data = str(data)
            api_data = api_data + '\r\n'
            self.stream.send_data(2, api_data.encode(), None)
