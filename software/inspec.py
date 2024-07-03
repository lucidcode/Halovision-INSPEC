import sensor
import os
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
        
        machine.RTC().datetime((self.config.config['Year'], self.config.config['Month'], self.config.config['Day'], 0, 0, 0, 0, 0))

        self.eye_movements = 0
        self.last_trigger = utime.ticks_ms() - self.config.config['TimeBetweenTriggers']
        self.last_update = utime.ticks_ms()

        self.init_stream()

    def configure_sensor(self):
        sensor.reset()
        sensor.set_hmirror(True if self.config.get('HorizontalMirror') else False)
        sensor.set_vflip(True if self.config.get('VerticalFlip') else False)
        
        if self.config.config['TrackFace']:
            sensor.set_framesize(sensor.HQVGA)
        elif self.config.config['FrameSize'] == 'VGA':
            sensor.set_framesize(sensor.VGA)
        elif self.config.config['FrameSize'] == 'QVGA':
            sensor.set_framesize(sensor.QVGA)
        elif self.config.config['FrameSize'] == 'QQVGA':
            sensor.set_framesize(sensor.QQVGA)
        elif self.config.config['FrameSize'] == 'HQVGA':
            sensor.set_framesize(sensor.HQVGA)
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

        if self.config.config['TrackFace']:
            sensor.set_contrast(3)
            sensor.set_gainceiling(16)
            return
        
        if self.config.config['Brightness']:
            sensor.set_brightness(self.config.config['Brightness'])
        if self.config.config['Contrast']:
            sensor.set_contrast(self.config.config['Contrast'])
        if self.config.config['Saturation']:
            sensor.set_brightness(self.config.config['Saturation'])
        if self.config.config['GainCeiling']:
            sensor.set_gainceiling(self.config.config['GainCeiling'])
        
        if self.config.config['AutoGain']:
            sensor.set_auto_gain(True)
        else:
            sensor.set_auto_gain(False)
#
        if self.config.config['AutoExposure']:
            sensor.set_auto_exposure(True)
        else:
            sensor.set_auto_exposure(False)

    def monitor(self):
        while True:
            #try:
            self.img = sensor.snapshot()

            if self.config.config['TrackFace']:
                self.img.gamma(gamma=1.0, contrast=1.5, brightness=0.0)

            self.variance = self.img.variance(self.extra_fb, self.config.config['PixelThreshold'], self.config.config['PixelRange'], self.face.face_object)
            if self.variance > 0:
                self.total_variances = self.total_variances + self.variance
                self.variances = self.variances + 1
            self.extra_fb.replace(self.img)

            self.face.detect(self.img)
            
            if self.config.config['AccessPoint'] or self.config.config['WiFi']:
                self.manage_stream()

            self.detect()
            self.led.process()

            if (utime.ticks_ms() - self.last_update > 128):
                average = 0
                if self.variances > 0:
                    average = int(self.total_variances / self.variances)

                self.comms.send_data("variance", str(average))
                self.total_variances = 0
                self.variances = 0
                self.last_update = utime.ticks_ms()

                if self.comms.sending_image or self.comms.sending_file:
                    self.comms.process_file()
                
            #except Exception as e:
            #    print(e)
            #    if str(e) == "IDE interrupt":
            #        break

    def ble_message_received(self, message):
        print("ble message", message)

        if message == "request.image":
            self.comms.send_data("face", "1" if self.face.has_face else "0")
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
            self.comms.send_data("face", "1" if self.face.has_face else "0")

        if message.startswith("update.setting."):
            message = message.replace("update.setting.", "")
            setting, value = message.split(':')

            if (setting == "AccessPoint" or setting == "WiFi") and value == "1" and self.stream == None:
                self.init_stream()
                print("self.stream", self.stream)
                if self.stream != None:
                    print("send ip", self.stream.ip)
                    self.comms.send_data("ip", self.stream.ip)

            self.config.set(setting, value)
            self.config.save()

            if self.config.is_sensor_setting(setting):
                self.configure_sensor()

        if message == "disconnect":
            self.comms.disconnect()
        
    def detect(self):
        if self.config.config['Algorithm'] == "Motion Detection":
            self.detect_motion()

        if self.config.config['Algorithm'] == "Face Detection":
            self.detect_face()

        if self.config.config['Algorithm'] == "REM Detection":
            self.detect_rem()

    def detect_motion(self):
        motion = 0
        if self.variance > self.config.config['TriggerThreshold']:
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
            self.comms.send_data("rem", str(eye_movements))

            if self.eye_movements >= 8:
                self.trigger()

            if self.eye_movements >= 5:
                self.lsd.add_image(self.img, self.eye_movements)

    def trigger(self):
        now = utime.ticks_ms()
        if (now - self.last_trigger > self.config.config['TimeBetweenTriggers']):
            print(f'{utime.time()} trigger')
            self.last_trigger = now
            self.led.blink()
            self.comms.send_data("trigger", str(self.variance))

            if self.stream == None or not self.stream.connected:
                self.comms.send_image(self.img)
                
    def init_stream(self):
        self.stream = None
        print("init stream")

        try:
            if self.config.config['AccessPoint']:
                print("init AP")
                self.stream = inspec_stream("AccessPoint", self.config.config['AccessPointName'], self.config.config['AccessPointPassword'])

            print("WiFi", self.config.config['WiFi']) 
            if self.config.config['WiFi']:
                print("init WiFi")
                self.stream = inspec_stream("Station", self.config.config['WiFiNetworkName'], self.config.config['WiFiKey'])
            else:
                print("not wifi")
            print("inited stream")
                
        except Exception as e:
            print("init_stream error: " + str(e))
            self.stream = None

    def manage_stream(self):
        if self.stream == None:
            return

        if not self.stream.connected:
            self.stream.start_server()
            if self.stream.error:
                self.stream.error = None
                self.comms.send_data("ip", self.stream.ip)
        else:
            self.stream.send_image(self.img)
