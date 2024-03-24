import random
import struct
import time
from machine import Pin
from micropython import const
from ble import inspec_comms
from config import inspec_config
from inspec import inspec_sensor

comms = inspec_comms()

config = inspec_config()

inspec = inspec_sensor(config)

while True:
    time.sleep_ms(128)

    img = inspec.snapshot()
    diff = inspec.variance()
    comms.send_diff(str(diff))

    if comms.requested_image:
        comms.send_image(img)

    if comms.sending_image:
        comms.process_image()
