# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Linear Regression Example
#
# This example shows off how to use the get_regression() method on your OpenMV Cam
# to get the linear regression of a ROI. Using this method you can easily build
# a robot which can track lines which all point in the same general direction
# but are not actually connected. Use find_blobs() on lines that are nicely
# connected for better filtering options and control.
#
# get_regression() runs in O(N^2) time on the image. To keep this manageable it
# area-scales the source ROI down to a temporary buffer before fitting the line;
# the buffer size is configurable via the target_size=(w, h) keyword argument
# (default (80, 60)). Pass a larger tuple to operate at higher resolution.

import csi
import time

THRESHOLD = (0, 100)  # Grayscale threshold for dark things.
BINARY_VISIBLE = True  # Binary pass first to see what linear regression is running on.

csi0 = csi.CSI()
csi0.reset()
csi0.pixformat(csi.GRAYSCALE)
csi0.framesize(csi.QVGA)
csi0.snapshot(time=2000)

clock = time.clock()  # to process a frame sometimes.

while True:
    clock.tick()
    img = csi0.snapshot().binary([THRESHOLD]) if BINARY_VISIBLE else csi0.snapshot()

    # Returns a line object similar to line objects returned by find_lines() and
    # find_line_segments(). You have x1, y1, x2, y2, length,
    # theta (rotation in degrees), rho, and magnitude attributes.
    #
    # magnitude represents how well the linear regression worked. In general,
    # the larger the value the better...
    line = img.get_regression([(255, 255) if BINARY_VISIBLE else THRESHOLD])

    if line:
        img.draw_line(line, color=127)
    print(
        "FPS %f, mag = %s" % (clock.fps(), str(line.magnitude) if (line) else "N/A")
    )

# About negative rho values:
#
# A [theta+0:-rho] tuple is the same as [theta+180:+rho].
