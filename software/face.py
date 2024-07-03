import sensor
import image
import utime

class face_detection:
    def __init__(self, config, comms):
        self.config = config
        self.comms = comms
        self.has_face = False
        self.face_cascade = image.HaarCascade("frontalface", stages=self.config.get('FaceStages'))
        self.last_change = utime.ticks_ms() - 1000
        self.face_object = [0, 0, 1, 1]
        
        self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.GRAYSCALE)

    def detect(self, img):
        if not self.config.get('TrackFace'):
            return

        now = utime.ticks_ms()
        has_face = False
        face_objects = img.find_features(self.face_cascade, threshold=self.config.get('FaceThreshold'), scale_factor=self.config.get('FaceScaleFactor'))

        if len(face_objects) == 0 and self.config.get('FaceAngles'):
            for angle in self.config.get('FaceAngles'):
                self.extra_fb.replace(img)
                self.extra_fb.rotation_corr(x_rotation=0.0, y_rotation=0.0, z_rotation=angle)
                face_objects = self.extra_fb.find_features(self.face_cascade, threshold=self.config.get('FaceThreshold'), scale_factor=self.config.get('FaceScaleFactor'))
                if face_objects:
                    break

        if face_objects:
            self.face_object = face_objects[0]
            has_face = True 
            self.last_change = now
            if self.has_face != has_face:
                self.has_face = has_face
                self.comms.send_data("face", "1")
        
        if self.config.get('DrawFaceRegion'):
            if self.face_object[2] != img.width():
                if self.config.get('PixelFormat') == 'RGB565':
                    img.draw_rectangle(self.face_object, color=(70, 130, 180))
                else:
                    img.draw_rectangle(self.face_object)

        if now - self.last_change > 1000:
            if not self.has_face:
                self.face_object = [0, 0, img.width(), img.height()]
            if self.has_face != has_face:
                self.has_face = has_face
                self.last_change = now
                self.comms.send_data("face", "1" if self.has_face else "0")
