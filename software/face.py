import image

class face_detection:
    def __init__(self, config, comms):
        self.config = config
        self.comms = comms
        self.has_face = False
        self.face_cascade = image.HaarCascade("frontalface", stages=self.config.config['FaceFeatures'])

    def detect(self, img):
        if not self.config.config['TrackFace']:
            return

        face_objects = img.find_features(self.face_cascade, threshold=0.5, scale_factor=1.25)
        
        has_face = False
        if face_objects:
            has_face = True
        else:
            has_face = False

        if self.has_face != has_face:
            self.has_face = has_face

            self.comms.send_data("face", "1" if self.has_face else "0")
