import ujson
import os

class inspec_config:
    def __init__(self):
        self.config = {}
        self.config['Algorithm'] = 'Motion Detection'
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
        self.config['Contrast'] = 0
        self.config['Saturation'] = 0
        self.config['FrameSize'] = 'VGA'
        self.config['CreateLogs'] = 0
        self.config['TriggerThreshold'] = 20
        self.config['TossThreshold'] = 8000
        self.config['TimeBetweenTriggers'] = 1000 * 60 * 1
        self.config['TimeBetweenTosses'] = 1000 * 60 * 5
        self.config['Year'] = 2024
        self.config['Month'] = 1
        self.config['Day'] = 1
        self.config['CreateGifs'] = 1
        self.config['TrackFace'] = 0
        self.config['FaceFeatures'] = 12
        self.config['BlinkCount'] = 8
        self.config['AutoGain'] = 0
        self.config['AutoExposure'] = 0
        self.config['Researcher'] = 'Researcher'

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
        return self.config[setting]

    def set(self, setting, value):
        string_settings = ["PixelFormat", "FrameSize", "Researcher", "Algorithm", "Mode", "AccessPointName", "AccessPointPassword", "WiFiNetworkName", "WiFiKey"]
        if setting in string_settings:
            self.config[setting] = value
        else:
            self.config[setting] = int(value)

    def is_sensor_setting(self, setting):
        sensor_settings = ["PixelFormat", "PixelThreshold", "TriggerThreshold", "TossThreshold", "PixelRange", "FrameSize", "Brightness", "Contrast", "Saturation", "AutoGain", "AutoExposure"]
        if setting in sensor_settings:
            return True
        return False

    def save(self):
        config_file = open('config.txt', 'w')
        config_file.write(ujson.dumps(self.config))
        config_file.close()
