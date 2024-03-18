:mod:`tv` --- tv shield driver
==============================

.. module:: tv
   :synopsis: tv shield driver

The ``tv`` module is used for controlling the tv shield.

Example usage::

    import sensor, tv

    # Setup camera.
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.SIF)
    sensor.skip_frames()
    tv.init()

    # Show image.
    while(True):
        tv.display(sensor.snapshot())

Functions
---------

.. function:: init([type=tv.TV_SHIELD, [triple_buffer=False]])

   Initializes an attached tv output module.

   ``type`` indicates how the lcd module should be initialized:

      * `tv.TV_NONE`: Do nothing.
      * `tv.TV_SHIELD`: Initialize a TV output module. Uses pins P0, P1, P2, and P3.

    ``triple_buffer`` If True then makes updates to the screen non-blocking in `tv.TV_SHIELD`
    mode at the cost of 3X the display RAM (495 KB).

.. function:: deinit()

   Deinitializes the tv module, internal/external hardware, and I/O pins.

.. function:: width()

   Returns 352 pixels. This is the `sensor.SIF` resolution.

.. function:: height()

   Returns 240 pixels. This is the `sensor.SIF` resolution.

.. function:: type()

   Returns the type of the screen that was set during `tv.init()`.

.. function:: triple_buffer()

   Returns if triple buffering is enabled that was set during `tv.init()`.

.. function:: refresh()

   Returns 60 Hz.

.. function:: channel([channel])

   For the wireless TV shield this sets the broadcast channel between 1-8. If passed without a channel
   argument then this method returns the previously set channel (1-8). Default is channel 8.

.. function:: display(image, [x=0, [y=0, [x_scale=1.0, [y_scale=1.0, [roi=None, [rgb_channel=-1, [alpha=256, [color_palette=None, [alpha_palette=None, [hint=0]]]]]]]]]])

   Displays an ``image`` whose top-left corner starts at location x, y.

   ``x_scale`` controls how much the displayed image is scaled by in the x direction (float). If this
   value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
   then it will match ``x_scale`` to maintain the aspect ratio.

   ``y_scale`` controls how much the displayed image is scaled by in the y direction (float). If this
   value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
   then it will match ``x_scale`` to maintain the aspect ratio.

   ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to display. This
   allows you to extract just the pixels in the ROI to scale.

   ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
   and to render on the display. For example, if you pass ``rgb_channel=1`` this will
   extract the green channel of the RGB565 image and display that in grayscale.

   ``alpha`` controls how opaque the image is. A value of 256 displays an opaque image while a
   value lower than 256 produces a black transparent image. 0 results in a perfectly black image.

   ``color_palette`` if not ``-1`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
   a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
   whatever the input image is. This is applied after ``rgb_channel`` extraction if used.

   ``alpha_palette`` if not ``-1`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
   palette which modulates the ``alpha`` value of the input image being displayed at a pixel pixel
   level allowing you to precisely control the alpha value of pixels based on their grayscale value.
   A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
   more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

   ``hint`` can be a logical OR of the flags:

      * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
      * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
      * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
      * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
      * `image.HMIRROR`: Horizontally mirror the image.
      * `image.VFLIP`: Vertically flip the image.
      * `image.TRANSPOSE`: Transpose the image (swap x/y).
      * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
      * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
      * `image.SCALE_ASPECT_KEEP`: Scale the image being drawn to fit inside the display.
      * `image.SCALE_ASPECT_EXPAND`: Scale the image being drawn to fill the display (results in cropping)
      * `image.SCALE_ASPECT_IGNORE`: Scale the image being drawn to fill the display (results in stretching).
      * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
      * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
      * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

Constants
---------

.. data:: TV_NONE

   Returned by `tv.type()` when the this module is not initialized.

.. data:: TV_SHIELD

   Used to initialize the TV module.
