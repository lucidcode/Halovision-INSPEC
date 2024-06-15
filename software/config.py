import ujson
import os

class inspec_config:
    def __init__(self):
        self.config = {}
        self.config['Researcher'] = 'Researcher'
        self.config['Algorithm'] = 'REM Detection'
        self.config['LEDs'] = 'RGB'
        self.config['LEDInterval'] = 500
        self.config['LEDFlashes'] = 5
        self.config['AccessPoint'] = False
        self.config['AccessPointName'] = "INSPEC"
        self.config['AccessPointPassword'] = "1234567890"
        self.config['WiFi'] = False
        self.config['WiFiNetworkName'] = "INSPEC"
        self.config['WiFiKey'] = "1234567890"
        self.config['PixelFormat'] = 'Grayscale'
        self.config['PixelThreshold'] = 32
        self.config['PixelRange'] = 8
        self.config['Brightness'] = 0
        self.config['Contrast'] = 3
        self.config['Saturation'] = 0
        self.config['FrameSize'] = 'VGA'
        self.config['CreateLogs'] = 0
        self.config['TriggerThreshold'] = 32
        self.config['TossThreshold'] = 8000
        self.config['TimeBetweenTriggers'] = 1000 * 60 * 1
        self.config['TimeBetweenTosses'] = 1000 * 60 * 5
        self.config['Year'] = 2024
        self.config['Month'] = 1
        self.config['Day'] = 1
        self.config['CreateGifs'] = 1
        self.config['TrackFace'] = 0
        self.config['FaceFeatures'] = 16
        self.config['DrawFaceRegion'] = 0
        self.config['FaceStages'] = 25
        self.config['FaceThreshold'] = 0.75
        self.config['FaceScaleFactor'] = 1.25
        self.config['BlinkCount'] = 8
        self.config['AutoGain'] = 0
        self.config['AutoExposure'] = 0
        self.config['GainCeiling'] = 16

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
            self.save()

        print(self.config)

    def get(self, setting):
        try:
            return self.config[setting]
        except Exception:
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
        sensor_settings = ["PixelFormat", "PixelThreshold", "TriggerThreshold", "TossThreshold", "PixelRange", "FrameSize", "Brightness", "Contrast", "Saturation", "AutoGain", "AutoExposure", "FaceStages"]
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
