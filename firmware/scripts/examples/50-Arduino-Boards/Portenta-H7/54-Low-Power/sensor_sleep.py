# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Sensor Sleep Mode Example.
# This example demonstrates the sensor sleep mode. The sleep mode saves around
# 40mA when enabled and it's automatically cleared when calling sensor reset().

import sensor

sensor.reset()  # Reset and initialize the sensor.
sensor.set_pixformat(sensor.GRAYSCALE)  # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)  # Set frame size to QVGA (320x240)
sensor.skip_frames(time=3000)  # Capture frames for 3000ms.
sensor.sleep(True)  # Enable sensor sleep mode (saves about 40mA).
