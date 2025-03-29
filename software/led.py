from machine import LED
import utime
import time

class lights:
    def __init__(self, config):
        self.config = config
        self.led_red = LED("LED_RED")
        time.sleep_ms(256)
        self.led_red.off()
        self.led_green = LED("LED_GREEN")
        time.sleep_ms(256)
        self.led_green.off()
        self.led_blue = LED("LED_BLUE")
        time.sleep_ms(256)
        self.led_blue.off()
        self.processing = False

    def flash(self):
        self.leds = self.config.get('LEDs')
        
        if self.leds == "":
            return
        
        self.flashes = self.config.get('LEDFlashes')
        if self.flashes == 0:
            return

        self.interval = self.config.get('LEDInterval')
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
