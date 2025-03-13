import utime

class sleep_quality:
    def __init__(self, config):
        self.config = config
        self.indicator = 0
        self.last_movement = utime.ticks_ms()

    def measure(self, variance):
        now = utime.ticks_ms()

        if variance >= self.config.get('TossThreshold'):
            if now - self.last_movement > 1000 * 10:
                self.indicator = self.indicator - 1
                self.last_movement = now
        
        if now - self.last_movement > 1000 * 60:
            self.indicator = self.indicator + 1
            self.last_movement = now

        if self.indicator < 0:
            self.indicator = 0
            
        if self.indicator > 100:
            self.indicator = 100

        return self.indicator
