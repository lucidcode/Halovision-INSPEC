import ujson
import os

class inspec_config:
    def __init__(self):
        self.default = {}
        self.default['Researcher'] = 'Researcher'
        self.default['Algorithm'] = 'REM Detection'
        self.default['LEDs'] = 'RGB'
        self.default['LEDInterval'] = 500
        self.default['LEDFlashes'] = 5
        self.default['AccessPoint'] = False
        self.default['AccessPointName'] = "INSPEC"
        self.default['AccessPointPassword'] = "1234567890"
        self.default['WiFi'] = False
        self.default['WiFiNetworkName'] = "INSPEC"
        self.default['WiFiKey'] = "1234567890"
        self.default['PixelFormat'] = 'Grayscale'
        self.default['PixelThreshold'] = 32
        self.default['PixelRange'] = 8
        self.default['Brightness'] = 0
        self.default['Contrast'] = 3
        self.default['Saturation'] = 0
        self.default['FrameSize'] = 'VGA'
        self.default['CreateLogs'] = 0
        self.default['TriggerThreshold'] = 32
        self.default['TossThreshold'] = 8000
        self.default['TriggerDelay'] = 0
        self.default['TriggerInterval'] = 1000 * 60 * 1
        self.default['TimeBetweenTosses'] = 1000 * 60 * 5
        self.default['Year'] = 2024
        self.default['Month'] = 1
        self.default['Day'] = 1
        self.default['CreateGifs'] = 1
        self.default['TrackFace'] = 1
        self.default['TrackEyes'] = 0
        self.default['FaceFeatures'] = 16
        self.default['DrawFaceRegion'] = 0
        self.default['FaceStages'] = 25
        self.default['FaceThreshold'] = 0.75
        self.default['FaceScaleFactor'] = 1.25
        self.default['FaceAngles'] = [-15, 15, -30, 30]
        self.default['BlinkCount'] = 8
        self.default['AutoGain'] = 0
        self.default['AutoExposure'] = 0
        self.default['GainCeiling'] = 16
        self.default['HorizontalMirror'] = 1
        self.default['VerticalFlip'] = 0
        self.default['SecondStream'] = 0

        config_exists = False
        entries = os.listdir()
        for entry in entries:
            if entry == "config.txt":
                config_exists = True

        if config_exists:
            config_file = open('config.txt')
            text = config_file.read()
            self.config = ujson.loads(text)
            config_file.close()
        else:
            self.config = self.default
            self.save()

        print(self.config)

    def get(self, setting):
        try:
            if self.config[setting] != None:
                return self.config[setting]
        except Exception as e:
            self.config[setting] = self.default[setting]
            print(str(e))

        if self.default[setting] != None:
            return self.default[setting]

        return ""

    def set(self, setting, value):
        string_settings = ["PixelFormat", "FrameSize", "LEDs", "Researcher", "Algorithm", "Mode", "AccessPointName", "AccessPointPassword", "WiFiNetworkName", "WiFiKey"]
        if setting in string_settings:
            self.config[setting] = value
            return

        float_settings = ["FaceThreshold", "FaceScaleFactor"]
        if setting in float_settings:
            self.config[setting] = float(value)
            return

        self.config[setting] = int(value)

    def is_sensor_setting(self, setting):
        sensor_settings = ["PixelFormat", "PixelThreshold", "TriggerThreshold", "TossThreshold", "PixelRange", "FrameSize", "Brightness", "Contrast", "Saturation", "AutoGain", "AutoExposure", "FaceStages", "HorizontalMirror", "VerticalFlip"]
        if setting in sensor_settings:
            return True
        return False

    def save(self):
        config_file = open('config.txt', 'w')
        json = ujson.dumps(self.config)
        json = json.replace("{", "{\r\n  ")
        json = json.replace(", ", ",\r\n  ")
        json = json.replace("}", "\r\n}")
        config_file.write(json)
        config_file.close()
