import utime

class rapid_eye_movement:
    def __init__(self, config):
        self.config = config
        self.eye_movements = 0

        now = utime.ticks_ms()
        self.last_eye_movement = now

    def dreaming(self, variance, trigger_threshold, toss_threshold):
        now = utime.ticks_ms()

        if variance >= trigger_threshold:
            if now - self.last_eye_movement > 1000:
                if self.eye_movements < 8:
                    self.eye_movements = self.eye_movements + 1
                    self.last_eye_movement = now
                    print("eye_movement", self.eye_movements)
        
        if variance >= toss_threshold:
            self.eye_movements = 0
            print("eye_movement", self.eye_movements)
            
        if now - self.last_eye_movement > 1000 * 60:
            if self.eye_movements > 0:
                self.eye_movements = self.eye_movements - 1
                print("eye_movement", self.eye_movements)

        return self.eye_movements
