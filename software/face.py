import image
import utime

class face_detection:
    def __init__(self, config, comms):
        self.config = config
        self.comms = comms
        self.has_face = False
        self.face_cascade = image.HaarCascade("frontalface", stages=self.config.config['FaceFeatures'])
        self.last_change = utime.ticks_ms() - 1000
        self.face_object = [0, 0, 1, 1]

    def detect(self, img):
        if not self.config.config['TrackFace']:
            return

        now = utime.ticks_ms()
        has_face = False
        face_objects = img.find_features(self.face_cascade, threshold=0.9, scale_factor=1.5)
        if face_objects:
            self.face_object = face_objects[0]
            has_face = True 
            self.last_change = now
            if self.has_face != has_face:
                self.has_face = has_face
                self.comms.send_data("face", "1")
        
        if self.config.get('DrawFace'):
            if self.face_object[2] != img.width():
                img.draw_rectangle(self.face_object, color=(70, 130, 180))

        if now - self.last_change > 1000:
            if not self.has_face:
                self.face_object = [0, 0, img.width(), img.height()]
            if self.has_face != has_face:
                self.has_face = has_face
                self.last_change = now
                self.comms.send_data("face", "1" if self.has_face else "0")
