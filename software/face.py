import sensor
import image
import math
import ml
from ml.utils import NMS

class face_detection:
    def __init__(self, config, comms):
        self.config = config
        self.comms = comms
        self.has_face = False
        self.face_cascade = image.HaarCascade("frontalface", stages=self.config.get('FaceStages'))
        self.face_object = [0, 0, 1, 1]
        self.face_angle = 0
        self.correct_angle = False
        self.ml_model = None
        self.min_confidence = 0.4
        self.threshold_list = [(math.ceil(self.min_confidence * 255), 255)]
        
        self.extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.GRAYSCALE)

    def detect(self, img, global_variance):
        if not self.config.get('TrackFace'):
            self.face_object = [0, 0, img.width(), img.height()]
            self.has_face = False
            return

        if global_variance >= self.config.get('TossThreshold'):
            self.face_object = [0, 0, img.width(), img.height()]
            self.has_face = False

        if self.config.get('TensorFlow'):
            if self.ml_model == None:
                self.ml_model = ml.Model('fomo_face_detection')                
        
            for i, detection_list in enumerate(self.ml_model.predict([img], callback=self.post_process)):
                if i == 0 or len(detection_list) == 0:
                    continue

                for (x, y, w, h), score in detection_list:
                    self.face_object = (x - w, y - h, w * 4, h * 4)
                    self.has_face = True
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

        img.draw_rectangle(self.face_object, color=(70, 130, 180))

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

            rotated_rect = [translate(rotate(xy, theta), offset) for xy in rect]
            
            img.draw_line((rotated_rect[0][0], rotated_rect[0][1], rotated_rect[1][0], rotated_rect[1][1]), color=(220, 220, 0))    
            img.draw_line((rotated_rect[1][0], rotated_rect[1][1], rotated_rect[2][0], rotated_rect[2][1]), color=(220, 220, 0))    
            img.draw_line((rotated_rect[2][0], rotated_rect[2][1], rotated_rect[3][0], rotated_rect[3][1]), color=(220, 220, 0))    
            img.draw_line((rotated_rect[3][0], rotated_rect[3][1], rotated_rect[0][0], rotated_rect[0][1]), color=(220, 220, 0))
            img.draw_string(face_x, face_y + face_height - 10, str(self.face_angle) + "^", color=(70, 130, 180), mono_space=False)

    def post_process(self, model, inputs, outputs):
        n, oh, ow, oc = model.output_shape[0]
        nms = NMS(ow, oh, inputs[0].roi)
        for i in range(oc):
            img = image.Image(outputs[0][0, :, :, i] * 255)
            blobs = img.find_blobs(
                self.threshold_list, x_stride=1, area_threshold=1, pixels_threshold=1
            )
            for b in blobs:
                rect = b.rect()
                x, y, w, h = rect
                score = (
                    img.get_statistics(thresholds=self.threshold_list, roi=rect).l_mean() / 255.0
                )
                nms.add_bounding_box(x, y, x + w, y + h, score, i)
        return nms.get_bounding_boxes()

    def rotate(xy, theta):
        cos_theta, sin_theta = math.cos(theta), math.sin(theta)

        return (
            int(xy[0] * cos_theta - xy[1] * sin_theta),
            int(xy[0] * sin_theta + xy[1] * cos_theta)
        )

    def translate(xy, offset):
        return xy[0] + offset[0], xy[1] + offset[1]
