import random
import struct
import time
import sensor
from machine import Pin
from micropython import const
from ble import inspec_comms

comms = inspec_comms()

sensor.reset()
sensor.set_pixformat(sensor.RGB565)  # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)  # Set frame size to QVGA (320x240)

extra_fb = sensor.alloc_extra_fb(sensor.width(), sensor.height(), sensor.RGB565)

img = sensor.snapshot()
extra_fb.replace(img)
diff = img.variance(extra_fb, 128) / 100000

while True:
    comms.send_diff(str(diff))

    time.sleep_ms(128)
    img = sensor.snapshot()
    diff = img.variance(extra_fb, 128) / 100000
    extra_fb.replace(img)

    if comms.requested_image:
        comms.send_image(img)

    if comms.sending_image:
        comms.process_image()
