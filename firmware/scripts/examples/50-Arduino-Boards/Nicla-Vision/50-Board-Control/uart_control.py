# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# UART Control
#
# This example shows how to use the serial port on your OpenMV Cam.

import time
from pyb import UART

# Init UART object.
uart = UART("LP1", 19200, timeout_char=200)

while True:
    uart.write("Hello World!\r")
    time.sleep_ms(1000)
