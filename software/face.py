import image
import utime

class face_detection:
    def __init__(self, config, comms):
        self.config = config
        self.comms = comms
        self.has_face = False
        self.face_cascade = image.HaarCascade("frontalface", stages=self.config.config['FaceFeatures'])
        self.last_change = utime.ticks_ms() - 1000

    def detect(self, img):
        if not self.config.config['TrackFace']:
            return

        face_objects = img.find_features(self.face_cascade, threshold=0.9, scale_factor=1.5)
        has_face = True if face_objects else False

        now = utime.ticks_ms()
        if now - self.last_change > 1000:
            if self.has_face != has_face:
                self.has_face = has_face
                self.last_change = now
                self.comms.send_data("face", "1" if self.has_face else "0")
