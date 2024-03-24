import sensor

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

        self.img = sensor.snapshot()
        self.extra_fb.replace(self.img)
        diff = self.img.variance(self.extra_fb, 128) / 100000

    def snapshot(self):
        self.img = sensor.snapshot()
        return self.img

    def variance(self):
        diff = self.img.variance(self.extra_fb, 128) / 100000
        self.extra_fb.replace(self.img)
        return diff
        