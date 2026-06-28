# This work is licensed under the MIT license.
# Copyright (c) 2013-2026 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# This example shows off using the the FLIR Lepton overlayed
# on top of a color sensor using an image transform. This
# only works on the OpenMV Cam N6 which can use it's GPU to
# perform the image transform.

import time
import csi
import image
from ulab import numpy as np
import math

# To get the transform matrix for overlaying thermal video on your color video use the tool here:
# https://github.com/openmv/openmv-projects/tree/master/tools/thermal-overlay-calibration
m = np.array([
    [3.704807, 0.257018, 37.260564],
    [0.052147, 3.609977, -7.831831],
    [0.000294, 0.000552, 1.000000],
])

alpha_pal = image.Image(256, 1, image.GRAYSCALE)
for i in range(256):
    alpha_pal[i] = int(math.pow((i / 255), 2) * 255)

# Setup the color camera sensor.
csi0 = csi.CSI()
csi0.reset(hard=True)  # force hardware reset.
csi0.pixformat(csi.RGB565)
csi0.framesize(csi.VGA)

csi1 = csi.CSI(cid=csi.LEPTON)
csi1.reset(hard=False)  # no hardware reset - just configure lepton
csi1.pixformat(csi.GRAYSCALE)
csi1.framesize(csi.QQVGA)

# Optional temperature range controls for the LEPTON.
# csi1.ioctl(csi.IOCTL_LEPTON_SET_MODE, True, False)
# csi1.ioctl(csi.IOCTL_LEPTON_SET_RANGE, 20.0, 40.0)

clock = time.clock()

img1 = image.Image(csi1.width(), csi1.height(), csi1.pixformat())

while True:
    clock.tick()
    img0 = csi0.snapshot()
    csi1.snapshot(blocking=False, image=img1)
    img0.draw_image(img1, 0, 0, color_palette=image.PALETTE_IRONBOW,
                    alpha_palette=alpha_pal,
                    hint=image.BILINEAR,
                    transform=m)
    print(clock.fps())
