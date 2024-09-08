.. currentmodule:: display
.. _display.SPIDisplay:

class SPIDisplay -- SPI Display Driver
======================================

The `SPIDisplay` class is used for driving SPI LCDs.

Example usage for driving the 128x160 LCD shield::

    import sensor, display

    # Setup camera.
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.LCD)
    sensor.skip_frames()
    lcd = display.SPIDisplay()

    # Show image.
    while(True):
        lcd.write(sensor.snapshot())

Constructors
------------

.. class:: SPIDisplay(width=128, height=160, refresh=60, bgr=False, byte_swap=False, triple_buffer, controller, backlight)

    ``width`` SPI LCD width. By default this is 128 to match the OpenMV 128x160 LCD shield.

    ``height`` SPI LCD height. By default this is 160 to match the OpenMV 128x160 LCD shield.

    ``refresh`` Sets the LCD refresh rate in hertz. This controls the SPI LCD shield clock.

    ``bgr`` set to True to swap the red and blue channels.
    This argument allows you to use our driver with more types of displays.

    ``byte_swap`` set to True to swap RGB565 pixel bytes sent to the LCD.
    This argument allows you to use our driver with more types of displays.

    ``triple_buffer`` If True then makes updates to the screen non-blocking at the cost of 3X the
    display size in RAM. This is on by default for OpenMV Cam boards with SDRAM.

    ``controller`` Pass the controller chip class here to initialize it along with the display.

    ``backlight`` specify a backlight controller module to use. By default the backlight will be
    controlled via a GPIO pin.

    .. note::

        Uses pins P0, P2, P3, P6, P7, and P8.

Methods
-------

.. method:: SPIDisplay.deinit() -> None

   Releases the I/O pins and RAM used by the class. This is called automatically on destruction.

.. method:: SPIDisplay.width() -> int

   Returns the width of the screen.

.. method:: SPIDisplay.height() -> int

   Returns the height of the screen.

.. method:: SPIDisplay.refresh() -> int

   Returns the refresh rate.

.. method:: SPIDisplay.bgr() -> bool

   Returns if the red and blue channels are swapped.

.. method:: SPIDisplay.byte_swap() -> bool

   Returns if the RGB565 pixels are displayed byte reversed.

.. method:: SPIDisplay.triple_buffer() -> bool

   Returns if triple buffering is enabled.

.. method:: SPIDisplay.write(image:image.Image, x=0, y=0, x_scale=1.0, y_scale=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel=-1, alpha=256, color_palette=None, alpha_palette=None, hint=0)

   Displays an ``image`` whose top-left corner starts at location x, y.

   You may also pass a path instead of an image object for this method to automatically load the image
   from disk and draw it in one step. E.g. ``write("test.jpg")``.

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

.. method:: SPIDisplay.clear(display_off=False) -> None

   Clears the lcd screen to black.

   ``display_off`` if True instead turns off the display logic versus clearing the frame LCD
   frame buffer to black. You should also turn off the backlight too after this to ensure the
   screen goes to black as many displays are white when only the backlight is on.

.. method:: SPIDisplay.backlight(value:Optional[int]=None) -> int

   Sets the lcd backlight dimming value. 0 (off) to 100 (on).

   Note that unless you pass `DACBacklight` or `PWMBacklight` the backlight will be controlled
   as a GPIO pin and will only go from 0 (off) to !0 (on).

   Pass no arguments to get the state of the backlight value.

.. method:: SPIDisplay.bus_write(cmd:int, args=None) -> None

   Send the SPI Display ``cmd`` with ``args``.
