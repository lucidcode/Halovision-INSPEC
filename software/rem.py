import utime

class rapid_eye_movement:
    def __init__(self, config, face):
        self.config = config
        self.face = face
        self.eye_movements = 0

        now = utime.ticks_ms()
        self.last_eye_movement = now

    def detect(self, variance):
        if variance >= self.config.config['TossThreshold']:
            self.eye_movements = 0
            print("eye_movement", self.eye_movements)

        now = utime.ticks_ms()
        if now - self.last_eye_movement > 1000 * 60:
            if self.eye_movements > 0:
                self.eye_movements = self.eye_movements - 1
                print("eye_movement", self.eye_movements)

        if self.config.config['TrackFace'] and not self.face.has_face:
            return self.eye_movements

        if variance >= self.config.config['TriggerThreshold']:
            if now - self.last_eye_movement > 1000:
                if self.eye_movements < 8:
                    self.eye_movements = self.eye_movements + 1
                    self.last_eye_movement = now
                    print("eye_movement", self.eye_movements, "face", self.face.has_face)

        return self.eye_movements
