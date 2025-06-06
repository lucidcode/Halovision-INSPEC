import utime

class non_rapid_eye_movement:
    def __init__(self, config, face):
        self.config = config
        self.face = face
        self.nrem1 = 0
        self.last_movement = utime.ticks_ms()

    def detect(self, variance):
        now = utime.ticks_ms()
        if variance >= self.config.get('TriggerThreshold'):
            self.last_movement = now
            self.nrem1 = 0

        delay_step = self.config.get('NREM1Delay') / 8
        if now - self.last_movement > delay_step:
            if self.nrem1 < 8:
                self.nrem1 = self.nrem1 + 1

            self.last_movement = now

        return self.nrem1
