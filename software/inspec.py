import sensor
import os
import ujson
import machine
import utime
from lsd import lucid_scribe_data

class inspec_sensor:
    def __init__(self, config):
        sensor.reset()

        if config['Brightness']:
            sensor.set_brightness(config['Brightness'])

        if config['FrameSize'] == 'VGA':
            sensor.set_framesize(sensor.VGA)
        elif config['FrameSize'] == 'QVGA':
            sensor.set_framesize(sensor.QVGA)
        elif config['FrameSize'] == 'QQVGA':
            sensor.set_framesize(sensor.QQVGA)
        else:
            sensor.set_framesize(sensor.QVGA)

        if config['Color']:
            sensor.set_pixformat(sensor.RGB565)
            print("Color")
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.RGB565)
        else:
            sensor.set_pixformat(sensor.GRAYSCALE)
            print("GRAYSCALE")
            self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.GRAYSCALE)

        if config['AutoGain']:
            sensor.set_auto_gain(True)
        else:
            sensor.set_auto_gain(False)

        if config['AutoExposure']:
            sensor.set_auto_exposure(True)
        else:
            sensor.set_auto_exposure(False)

        self.config = config

        self.img = sensor.snapshot()
        self.extra_fb.replace(self.img)
        diff = self.img.variance(self.extra_fb, 128) / 100000

        self.create_session_directory()
        
        machine.RTC().datetime((self.config['Year'], self.config['Month'], self.config['Day'], 0, 0, 0, 0, 0))
        
        self.lsd = lucid_scribe_data(self.config, self.session_directory)

    def create_session_directory(self):
        root_dir_exists = False
        entries = os.listdir()
        for entry in entries:
            if entry == "visions":
                root_dir_exists = True

        if not root_dir_exists:
            os.mkdir("visions")

        vision_index = 0
        entries = os.listdir("visions")
        for entry in entries:
            if ("vision_" in entry):
                directory_index = entry.replace("vision_", "")
                if int(directory_index) >= vision_index:
                    vision_index = int(directory_index) + 1

        self.session_directory = "visions/" + "vision_" + str(vision_index)
        os.mkdir(self.session_directory)

        config_file = open(self.session_directory + '/config.txt', 'w')
        config_file.write(ujson.dumps(self.config))
        config_file.close()

    def snapshot(self):
        self.img = sensor.snapshot()
        return self.img

    def variance(self):
        diff = self.img.variance(self.extra_fb, 128) / 100000
        self.extra_fb.replace(self.img)
        self.lsd.log(diff)
        return diff
        