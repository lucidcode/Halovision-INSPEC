from machine import LED
import utime

class lights:
    def __init__(self, config):
        self.config = config
        self.led_red = LED("LED_RED")
        self.led_red.off()
        self.led_green = LED("LED_GREEN")
        self.led_green.off()
        self.led_blue = LED("LED_BLUE")
        self.led_blue.off()
        self.processing = False

    def blink(self):
        self.leds = self.config.config['LEDs']
        
        if self.leds == "":
            return
        
        self.flashes = self.config.config['LEDFlashes']
        if self.flashes == 0:
            return

        self.interval = self.config.config['LEDInterval']
        self.iterations = 0
        self.state = 0
        self.last_flash = utime.ticks_ms() - self.interval
        self.processing = True

    def process(self):
        if not self.processing:
            return

        now = utime.ticks_ms()

        if (now - self.last_flash < self.interval):
            return

        if not self.state:
            self.state = True
            self.iterations = self.iterations + 1

            if "R" in self.leds:
                self.led_red.on()
            if "G" in self.leds:
                self.led_green.on()
            if "B" in self.leds:
                self.led_blue.on()
        else:
            self.state = False

            if "R" in self.leds:
                self.led_red.off()
            if "G" in self.leds:
                self.led_green.off()
            if "B" in self.leds:
                self.led_blue.off()

            if self.iterations >= self.flashes :
                self.processing = False

        self.last_flash = now
