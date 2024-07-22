# This file is part of the OpenMV project.
#
# Copyright (c) 2024 Ibrahim Abdelkader <iabdalkader@openmv.io>
# Copyright (c) 2024 Kwabena W. Agyeman <kwagyeman@openmv.io>
#
# This work is licensed under the MIT license, see the file LICENSE for details.

import image
from ulab import numpy as np


class Normalization:
    def __init__(
        self,
        scale=(0.0, 1.0),
        mean=(0.0, 0.0, 0.0),
        stdev=(1.0, 1.0, 1.0),
        roi=None,
    ):
        self.scale = scale
        self.mean = mean
        self.stdev = stdev
        self.roi = roi
        self._image = None

    def __call__(self, *args):
        if len(args) == 1:
            img = args[0]
            if not isinstance(img, image.Image):
                raise ValueError("Expected an image input")
            if self.roi is None:
                self.roi = (0, 0, img.width(), img.height())
            n = Normalization(self.scale, self.mean, self.stdev, self.roi)
            n._image = img
            return n

        buffer, shape, dtype = args

        # Create an image using the input tensor as buffer.
        if len(shape) != 4:
            raise ValueError("Expected input tensor with shape: (1, H, W, C)")
        b, h, w, c = shape
        if b != 1:
            raise ValueError("Expected batches to be 1")
        if c != 1 and c != 3:
            raise ValueError("Expected channels to be 1 or 3")

        # Place the image buffer at the end of the input buffer so we can convert it in-place.
        pixfmt = image.GRAYSCALE if c == 1 else image.RGB565
        offset = len(buffer) - (w * h * (1 if c == 1 else 2))
        img = image.Image(w, h, pixfmt, buffer=memoryview(buffer)[offset:])

        # Copy and scale (if needed) the input image to the input buffer.
        hints = image.BILINEAR | image.CENTER | image.SCALE_ASPECT_EXPAND | image.BLACK_BACKGROUND
        img.draw_image(self._image, 0, 0, roi=self.roi, hint=hints)

        # Convert the image in-place into an ndarray input tensor.
        array = img.to_ndarray(dtype, buffer=buffer)

        # Normalize the input tensor.
        if dtype == ord('f'):
            fscale = (self.scale[1] - self.scale[0]) / 255.0
            fadd = self.scale[0]

            def grayscale(x):
                return (x[0] * 0.299) + (x[1] * 0.587) + (x[2] * 0.114)

            if c == 1:
                fadd = (fadd - grayscale(self.mean)) / grayscale(self.stdev)
                fscale = fscale / grayscale(self.stdev)
            else:
                fadd = (fadd - np.array(self.mean)) / np.array(self.stdev)
                fscale = fscale / np.array(self.stdev)

            array = (array * fscale) + fadd
