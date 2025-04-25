import utime

class rapid_eye_movement:
    def __init__(self, config, face):
        self.config = config
        self.face = face
        self.eye_movements = 0
        self.last_eye_movement = utime.ticks_ms()

    def detect(self, variance, global_variance):
        now = utime.ticks_ms()

        if global_variance >= self.config.get('TossThreshold'):
            self.eye_movements = 0
            self.last_eye_movement = now + 1000 * self.config.get('TossCooldown')

        if now - self.last_eye_movement > 1000 * 60:
            if self.eye_movements > 0:
                self.eye_movements = self.eye_movements - 1
                self.last_eye_movement = now - 1000 * 58

        if (self.config.get('TrackFace') or self.config.get('TensorFlow')) and not self.face.has_face:
            return self.eye_movements

        if variance >= self.config.get('TriggerThreshold'):
            if now - self.last_eye_movement > 1000:
                self.last_eye_movement = now
                if self.eye_movements < 8:
                    self.eye_movements = self.eye_movements + 1

        return self.eye_movements
