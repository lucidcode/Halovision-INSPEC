:mod:`mjpeg` --- mjpeg recording
================================

.. module:: mjpeg
   :synopsis: mjpeg recording

The ``mjpeg`` module is used for mjpeg recording.

class Mjpeg -- Mjpeg recorder
-----------------------------

You can use the mjpeg module to record large video clips. Note that mjpeg files save
compressed image data. So, they are best for recording long video clips that
you want to share. Use `gif` for short clips.

Example usage::

    import sensor, mjpeg, time

    # Setup camera.
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.skip_frames()
    c = time.clock()

    # Create the mjpeg object.
    m = mjpeg.Mjpeg("example.mjpeg")

    # Add frames.
    for i in range(100):
        c.tick()
        m.add_frame(sensor.snapshot())

    # Finalize.
    m.close(c.fps())

Constructors
~~~~~~~~~~~~

.. class:: Mjpeg(filename, [width, [height]])

   Create a Mjpeg object which you can add frames to. ``filename`` is the path to
   save the mjpeg recording to.

   ``width`` is automatically set equal to the image sensor horizontal resolution
   unless explicitly overridden.

   ``height`` is automatically set equal to the image sensor vertical resolution
   unless explicitly overridden.

   Methods
   ~~~~~~~

   .. method:: is_closed()

      Return True if the file was closed. You cannot write more data to a closed file.

   .. method:: width()

      Returns the width (horizontal resolution) for the mjpeg file.

   .. method:: height()

      Returns the height (vertical resolution) for the mjpeg file.

   .. method:: count()

      Returns the number of frames in the mjpeg file.

   .. method:: size()

      Returns the file size in bytes of the mjpeg so far. This value is updated after adding frames.

   .. method:: add_frame(image, [roi=None, [rgb_channel=-1, [alpha=256, [color_palette=None, [alpha_palette=None, [hint=0, [quality=90]]]]]]])

      Add an image to the mjpeg recording. The added image is automatically scaled up/down while
      preserving the aspect-ratio to the resolution specified when the mjpeg file was created.

      ``image`` can be any image format. Even PNG images or JPEG images at the wrong resolution.
      This method will automatically decompress, scale/convert, and re-compress images for the file.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image. This
      allows you to extract just the pixels in the ROI. By default this is the whole image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to render onto the destination. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and draw that in grayscale on the
      destination.

      ``alpha`` controls how much of the source image to blend into the destination. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination (which is a black background in this case). 0 results in a black image.

      ``color_palette`` if not ``-1`` can be `sensor.PALETTE_RAINBOW`, `sensor.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``-1`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.

      ``quality`` is the compression quality (0-100) (int) to be used for non-JPEG images.

      Returns the object.

   .. method:: write(image, [quality=90, [roi=None, [rgb_channel=-1, [alpha=256, [color_palette=None, [alpha_palette=None, [hint=0]]]]]]])

      Alias for `Mjpeg.add_frame()`.

   .. method:: sync(fps)

      Flushes the mjpeg file to disk but keeps the file open for writing more data. You should call
      flush periodically ensure that the file is saved to disk.

      ``fps`` is the frame rate the mjpeg was recorded at.

      Returns the object.

   .. method:: close(fps)

      Finalizes the mjpeg recording. This method must be called once the recording
      is complete to make the file viewable.

      ``fps`` is the frame rate the mjpeg was recorded at.

      Returns the object.
