import ujson
import os

def inspec_config():
    config = {}
    config['PixelThreshold'] = 32
    config['TriggerThreshold'] = 20
    config['TossThreshold'] = 800
    config['TimeBetweenTriggers'] = 1000 * 60 * 1
    config['TimeBetweenTosses'] = 1000 * 60 * 5
    config['Year'] = 2024
    config['Month'] = 1
    config['Day'] = 1
    config['CreateGifs'] = 1
    config['Color'] = 1
    config['TrackFace'] = 0
    config['FaceFeatures'] = 12
    config['BlinkCount'] = 8
    config['AutoGain'] = 0
    config['AutoExposure'] = 0
    config['Brightness'] = -3
    config['FrameSize'] = 'VGA'
    config['Researcher'] = 'Researcher'
    config['Algorithm'] = 'Motion Detection'
    config['Mode'] = 'Research'

    config_exists = False
    entries = os.listdir()
    for entry in entries:
        if entry == "config.txt":
            config_exists = True

    if config_exists:
        config_file = open('config.txt')
        text = config_file.read()
        config = ujson.loads(text)
        config_file.close()
    else:
        config_file = open('config.txt', 'w')
        config_file.write(ujson.dumps(config))
        config_file.close()

    print(config)
    return config