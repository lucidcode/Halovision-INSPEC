import csi
import image
import math
import ml
from ml.postprocessing.edgeimpulse import Fomo
from ml.utils import NMS

class face_detection:
    def __init__(self, config, comms, sensor):
        self.config = config
        self.comms = comms
        self.sensor = sensor
        self.has_face = False
        self.face_cascade = image.HaarCascade("/rom/haarcascade_frontalface.cascade", stages=self.config.get('FaceStages'))
        self.face_object = [0, 0, 1, 1]
        self.face_angle = 0
        self.correct_angle = False
        self.ml_model = None
        self.detector = ""

        self.extra_fb = image.Image(self.sensor.width(), self.sensor.height(), csi.GRAYSCALE)

    def detect(self, img, global_variance):
        if not self.config.get('TrackFace') and not self.config.get('TensorFlow'):
            self.face_object = [0, 0, img.width(), img.height()]
            self.has_face = False
            return

        if global_variance >= self.config.get('TossThreshold'):
            self.face_object = [0, 0, img.width(), img.height()]
            self.has_face = False

        if self.config.get('TensorFlow'):
            if self.ml_model == None:
                self.ml_model = ml.Model('/rom/fomo_face_detection.tflite', postprocess=Fomo(threshold=self.config.get('FaceConfidence')))                
        
            for i, detection_list in enumerate(self.ml_model.predict([img])):
                if i == 0 or len(detection_list) == 0:
                    continue

                for (x, y, w, h), score in detection_list:
                    self.face_object = (x - w, y - h, w * 4, h * 4)
                    self.detector = "TensorFlow"
                    self.has_face = True
                    return

        if not self.config.get('TrackFace'):
            return

        self.face_angle = 0
        face_objects = img.find_features(self.face_cascade, threshold=self.config.get('FaceThreshold'), scale_factor=self.config.get('FaceScaleFactor'))

        if len(face_objects) == 0 and self.config.get('FaceAngles'):
            for angle in self.config.get('FaceAngles'):
                self.extra_fb.replace(img)
                self.extra_fb.rotation_corr(x_rotation=0.0, y_rotation=0.0, z_rotation=angle)

                face_objects = self.extra_fb.find_features(self.face_cascade, threshold=self.config.get('FaceThreshold'), scale_factor=self.config.get('FaceScaleFactor'))
                if face_objects:
                    self.face_angle = angle
                    break

        if face_objects:
            self.face_object = face_objects[0]
            self.detector = "HaarCascade"
            self.has_face = True

            if self.config.get('TrackEyes'):
                self.face_object = [self.face_object[0], self.face_object[1] + int(self.face_object[3] * 1/5), self.face_object[2], int(self.face_object[3] * 2/5)]

                if self.correct_angle:
                    eyes_x = self.face_object[0] + int(self.face_object[2] * 1/5)
                    eyes_y = self.face_object[1] + int(self.face_object[3] * 1/5)
                    eyes_width = self.face_object[2] - int(self.face_object[2] * 1/5)
                    eyes_height = int(self.face_object[3] * 2/5)
                    self.face_object = [eyes_x, eyes_y, eyes_width, eyes_height]
        
    def draw_region(self, img):
        if not self.config.get('DrawFaceRegion'):
            return

        if self.face_object[2] == img.width():
            return

        if self.detector == "TensorFlow":
            img.draw_rectangle(self.face_object, color=(70, 130, 180))

        if self.detector == "HaarCascade":
            img.draw_rectangle(self.face_object)

        if self.correct_angle:
            face_x = self.face_object[0]
            face_y = self.face_object[1]
            face_width = self.face_object[2]
            face_height = self.face_object[3]

            theta = math.radians(self.face_angle * -1)
            offset = (0, 0)

            rect = [
                (face_x, face_y),
                (face_x, face_y + face_height),
                (face_x + face_width, face_y + face_height),
                (face_x + face_width, face_y)
            ]

            rotated_rect = [self.translate(self.rotate(xy, theta), offset) for xy in rect]
            
            img.draw_line((rotated_rect[0][0], rotated_rect[0][1], rotated_rect[1][0], rotated_rect[1][1]), color=(220, 220, 0))    
            img.draw_line((rotated_rect[1][0], rotated_rect[1][1], rotated_rect[2][0], rotated_rect[2][1]), color=(220, 220, 0))    
            img.draw_line((rotated_rect[2][0], rotated_rect[2][1], rotated_rect[3][0], rotated_rect[3][1]), color=(220, 220, 0))    
            img.draw_line((rotated_rect[3][0], rotated_rect[3][1], rotated_rect[0][0], rotated_rect[0][1]), color=(220, 220, 0))
            img.draw_string(face_x, face_y + face_height - 10, str(self.face_angle) + "^", color=(70, 130, 180), mono_space=False)

    def rotate(self, xy, theta):
        cos_theta, sin_theta = math.cos(theta), math.sin(theta)

        return (
            int(xy[0] * cos_theta - xy[1] * sin_theta),
            int(xy[0] * sin_theta + xy[1] * cos_theta)
        )

    def translate(self, xy, offset):
        return xy[0] + offset[0], xy[1] + offset[1]
