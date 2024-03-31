import random
import struct
import time
from machine import Pin
from micropython import const
from inspec import inspec_sensor

inspec = inspec_sensor()

while True:
    time.sleep_ms(128)
    inspec.monitor()
