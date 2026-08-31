import csi
import image
import math
import ml
import random
from ml.postprocessing.edgeimpulse import Fomo
from ml.postprocessing.mediapipe import BlazeFace
from ml.utils import NMS
from ulab import numpy as np

AJNA = True


def _build_blazeface_anchors():
    anchor_grid = [(16, 2), (8, 6)]
    count = sum((g * g) * d for g, d in anchor_grid)
    a = np.empty((count, 2))
    idx = 0
    for grid_size, scales in anchor_grid:
        for gy in range(grid_size):
            cy = (gy + 0.5) / grid_size
            for gx in range(grid_size):
                cx = (gx + 0.5) / grid_size
                for _ in range(scales):
                    a[idx, 0] = cx
                    a[idx, 1] = cy
                    idx += 1
    return a


_BLAZEFACE_ANCHORS = _build_blazeface_anchors()

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
        self.blazeface_model = None
        self.blazeface_skip_counter = 0
        self.eye_centers = [(0, 0), (0, 0)]
        self.ipd = 0
        self.detector = ""

        self.halo_mid_x = 0
        self.halo_mid_y = 0
        self.halo_angle = 0
        self.halo_rx_inner = 10
        self.halo_ry_inner = 6
        self.halo_rx_outer = 12
        self.halo_ry_outer = 7
        self.halo_cos_f = 1.0
        self.halo_sin_f = 0.0
        self.halo_top_x = 0
        self.halo_top_y = 0
        self.halo_verts = [(0, 0), (0, 0), (0, 0)]
        self.third_eye_rx = 3
        self.third_eye_ry = 2
        self.third_eye_pupil_r = 1

        self.extra_fb = image.Image(self.sensor.width(), self.sensor.height(), csi.GRAYSCALE)

    def detect(self, img, global_variance):
        if not self.config.get('TrackFace') and not self.config.get('TensorFlow') and not self.config.get('BlazeFace'):
            self.face_object = [0, 0, img.width(), img.height()]
            self.has_face = False
            return

        is_toss = global_variance >= self.config.get('TossThreshold')

        if self.config.get('BlazeFace'):
            if is_toss and self.blazeface_skip_counter > 8:
                self.blazeface_skip_counter = 8

            if self.blazeface_skip_counter > 0 and self.detector == "BlazeFace" and self.has_face:
                self.blazeface_skip_counter -= 1
                return

        if is_toss:
            self.face_object = [0, 0, img.width(), img.height()]

        self.has_face = False

        if self.config.get('BlazeFace'):
            if self.blazeface_model is None:
                self.blazeface_model = ml.Model('/rom/blazeface_front_128.tflite', postprocess=BlazeFace(threshold=self.config.get('BlazeFaceConfidence'), anchors=_BLAZEFACE_ANCHORS))

            for r, score, keypoints in self.blazeface_model.predict([img]):
                right_eye = (int(keypoints[0][0]), int(keypoints[0][1]))
                left_eye = (int(keypoints[1][0]), int(keypoints[1][1]))
                self.eye_centers = [right_eye, left_eye]

                dx = left_eye[0] - right_eye[0]
                dy = left_eye[1] - right_eye[1]
                ipd_f = math.sqrt(dx * dx + dy * dy)
                self.ipd = int(ipd_f)
                self.face_angle = math.degrees(math.atan2(dy, dx))

                if ipd_f > 0:
                    abs_cos = abs(dx) / ipd_f
                    abs_sin = abs(dy) / ipd_f
                else:
                    abs_cos, abs_sin = 1.0, 0.0

                eye_cx = (right_eye[0] + left_eye[0]) // 2
                eye_cy = (right_eye[1] + left_eye[1]) // 2
                eye_w_aligned = ipd_f * 2.5
                eye_h_aligned = ipd_f * 1.0
                eye_w = max(int(eye_w_aligned * abs_cos + eye_h_aligned * abs_sin), 1)
                eye_h = max(int(eye_w_aligned * abs_sin + eye_h_aligned * abs_cos), 1)
                eye_x = max(eye_cx - eye_w // 2, 0)
                eye_y = max(eye_cy - eye_h // 2, 0)
                self.face_object = (eye_x, eye_y, eye_w, eye_h)

                self.detector = "BlazeFace"
                self.has_face = True
                self.blazeface_skip_counter = max(128, 0)
                self.compute_halo_geometry()
                return

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
        face_objects = img.find_features(self.face_cascade, threshold=self.config.get('FaceThreshold'), scale=self.config.get('FaceScaleFactor'))

        if len(face_objects) == 0 and self.config.get('FaceAngles'):
            for angle in self.config.get('FaceAngles'):
                self.extra_fb.replace(img)
                self.extra_fb.rotation_corr(x_rotation=0.0, y_rotation=0.0, z_rotation=angle)

                face_objects = self.extra_fb.find_features(self.face_cascade, threshold=self.config.get('FaceThreshold'), scale=self.config.get('FaceScaleFactor'))
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
        
    def compute_halo_geometry(self):
        right_eye, left_eye = self.eye_centers
        self.halo_mid_x = (right_eye[0] + left_eye[0]) // 2
        self.halo_mid_y = (right_eye[1] + left_eye[1]) // 2
        self.halo_angle = int(self.face_angle)
        self.halo_rx_inner = max(int(self.ipd * 1.00), 10)
        self.halo_ry_inner = max(int(self.ipd * 0.55), 6)
        self.halo_rx_outer = max(int(self.ipd * 1.10), 12)
        self.halo_ry_outer = max(int(self.ipd * 0.62), 7)

        face_rad = math.radians(self.face_angle)
        self.halo_cos_f = math.cos(face_rad)
        self.halo_sin_f = math.sin(face_rad)

        top_x_f = self.halo_mid_x + self.halo_ry_outer * self.halo_sin_f
        top_y_f = self.halo_mid_y - self.halo_ry_outer * self.halo_cos_f
        self.halo_top_x = int(top_x_f)
        self.halo_top_y = int(top_y_f)

        if AJNA:
            self.third_eye_rx = max(int(self.ipd * 0.10), 3)
            self.third_eye_ry = max(int(self.ipd * 0.05), 2)
            self.third_eye_pupil_r = max(int(self.ipd * 0.025), 1)
        else:
            tri_size = max(int(self.ipd * 0.06), 2)
            verts_aligned = [
                (0, -tri_size),
                (-tri_size * 0.87, tri_size * 0.5),
                (tri_size * 0.87, tri_size * 0.5),
            ]
            self.halo_verts = []
            for vx, vy in verts_aligned:
                rx = vx * self.halo_cos_f - vy * self.halo_sin_f
                ry = vx * self.halo_sin_f + vy * self.halo_cos_f
                self.halo_verts.append((int(top_x_f + rx), int(top_y_f + ry)))

    def draw_region(self, img):
        if not self.config.get('DrawFaceRegion'):
            return

        if self.face_object[2] == img.width():
            return

        if self.detector == "BlazeFace" and self.ipd > 0 and self.has_face:
            img.draw_ellipse((self.halo_mid_x, self.halo_mid_y, self.halo_rx_inner, self.halo_ry_inner, self.halo_angle), color=140, thickness=1)
            img.draw_ellipse((self.halo_mid_x, self.halo_mid_y, self.halo_rx_outer, self.halo_ry_outer, self.halo_angle), color=180, thickness=1)

            if AJNA:
                img.draw_ellipse((self.halo_top_x, self.halo_top_y, self.third_eye_rx, self.third_eye_ry, self.halo_angle), color=140, thickness=1)
                img.draw_circle((self.halo_top_x, self.halo_top_y, self.third_eye_pupil_r), color=140, thickness=1, fill=True)
            else:
                v = self.halo_verts
                img.draw_line((v[0][0], v[0][1], v[1][0], v[1][1]), color=140, thickness=1)
                img.draw_line((v[1][0], v[1][1], v[2][0], v[2][1]), color=140, thickness=1)
                img.draw_line((v[2][0], v[2][1], v[0][0], v[0][1]), color=140, thickness=1)

            for _ in range(0):
                theta = random.uniform(0, 2 * math.pi)
                ex = self.halo_rx_outer * math.cos(theta)
                ey = self.halo_ry_outer * math.sin(theta)
                px = int(self.halo_mid_x + ex * self.halo_cos_f - ey * self.halo_sin_f)
                py = int(self.halo_mid_y + ex * self.halo_sin_f + ey * self.halo_cos_f)
                img.set_pixel((px, py), random.randint(120, 255))

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
            img.draw_string((face_x, face_y + face_height - 10), str(self.face_angle) + "^", color=(70, 130, 180), mono_space=False)

    def rotate(self, xy, theta):
        cos_theta, sin_theta = math.cos(theta), math.sin(theta)

        return (
            int(xy[0] * cos_theta - xy[1] * sin_theta),
            int(xy[0] * sin_theta + xy[1] * cos_theta)
        )

    def translate(self, xy, offset):
        return xy[0] + offset[0], xy[1] + offset[1]
