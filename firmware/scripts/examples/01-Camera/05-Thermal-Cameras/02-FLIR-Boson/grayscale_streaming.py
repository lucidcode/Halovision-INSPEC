# This work is licensed under the MIT license.
# Copyright (c) 2013-2024 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# This example shows off streaming the default grayscale 8-bit AGC image from the FLIR Boson.

import sensor
import time

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)  # Must always be grayscale.
sensor.set_framesize(sensor.VGA)  # Must always be VGA or QVGA.

clock = time.clock()

while True:
    clock.tick()

    img = sensor.snapshot()

    print(clock.fps())
