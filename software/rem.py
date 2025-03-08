import utime

class rapid_eye_movement:
    def __init__(self, config, face):
        self.config = config
        self.face = face
        self.eye_movements = 0
        self.last_eye_movement = utime.ticks_ms()

    def detect(self, variance, global_variance):
        if global_variance >= self.config.get('TossThreshold'):
            self.eye_movements = 0

        now = utime.ticks_ms()
        if now - self.last_eye_movement > 1000 * 60:
            if self.eye_movements > 0:
                self.eye_movements = self.eye_movements - 1

        if self.config.get('TrackFace') and not self.face.has_face:
            return self.eye_movements

        if variance >= self.config.get('TriggerThreshold'):
            if now - self.last_eye_movement > 1000:
                if self.eye_movements < 8:
                    self.eye_movements = self.eye_movements + 1
                    self.last_eye_movement = now

        return self.eye_movements
