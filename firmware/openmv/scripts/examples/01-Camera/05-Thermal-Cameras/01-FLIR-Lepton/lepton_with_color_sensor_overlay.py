# This work is licensed under the MIT license.
# Copyright (c) 2013-2026 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# This example shows off using the the FLIR Lepton overlayed
# on top of a color sensor.

import time
import csi
import image
import math

alpha_pal = image.Image(256, 1, image.GRAYSCALE)
for i in range(256):
    alpha_pal[i] = int(math.pow((i / 255), 2) * 255)

# Setup the color camera sensor.
csi0 = csi.CSI()
csi0.reset(hard=True)  # force hardware reset.
csi0.pixformat(csi.RGB565)
csi0.framesize(csi.QVGA)

csi1 = csi.CSI(cid=csi.LEPTON)
csi1.reset(hard=False)  # no hardware reset - just configure lepton
csi1.pixformat(csi.GRAYSCALE)
csi1.framesize(csi.QVGA)

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
                    hint=image.BILINEAR)
    print(clock.fps())
