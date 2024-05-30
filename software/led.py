from machine import LED
import time

class lights:
    def __init__(self, config):
        self.config = config
        self.led_red = LED("LED_RED")
        self.led_green = LED("LED_GREEN")
        self.led_blue = LED("LED_BLUE")
        self.led_red.off()
        self.led_green.off()
        self.led_blue.off()

    def blink(self):
        leds = self.config.config['LEDs']
        if leds == "":
            return
        
        flashes = self.config.config['LEDFlashes']
        if flashes == 0:
            return

        interval = self.config.config['LEDInterval']

        for i in range(flashes):
            if "R" in leds:
                self.led_red.on()
            if "G" in leds:
                self.led_green.on()
            if "B" in leds:
                self.led_blue.on()

            time.sleep_ms(interval)

            if "R" in leds:
                self.led_red.off()
            if "G" in leds:
                self.led_green.off()
            if "B" in leds:
                self.led_blue.off()

            time.sleep_ms(interval)
