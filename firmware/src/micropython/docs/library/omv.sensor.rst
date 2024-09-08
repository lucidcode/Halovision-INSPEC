:mod:`sensor` --- camera sensor
===============================

.. module:: sensor
   :synopsis: camera sensor

The ``sensor`` module is used for taking pictures.

Example usage::

    import sensor

    # Setup camera.
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.skip_frames()

    # Take pictures.
    while(True):
        sensor.snapshot()

Functions
---------

.. function:: reset() -> None

   Initializes the camera sensor.

.. function:: sleep(enable:bool) -> None

   Puts the camera to sleep if enable is True. Otherwise, wakes it back up.

.. function:: shutdown(enable:bool) -> None

   Puts the camera into a lower power mode than sleep (but the camera must be reset on being woken up).

.. function:: flush() -> None

   Copies whatever was in the frame buffer to the IDE. You should call this
   method to display the last image your OpenMV Cam takes if it's not running
   a script with an infinite loop. Note that you'll need to add a delay time
   of about a second after your script finishes for the IDE to grab the image
   from your camera. Otherwise, this method will have no effect.

.. function:: snapshot() -> image.Image

   Takes a picture using the camera and returns an ``image`` object.

   The OpenMV Cam has two memory areas for images. The classical stack/heap
   area used for normal MicroPython processing can store small images within
   it's heap. However, the MicroPython heap is only about ~100 KB which is not
   enough to store larger images. So, your OpenMV Cam has a secondary frame
   buffer memory area that stores images taken by `sensor.snapshot()`. Images
   are stored on the bottom of this memory area. Any memory that's left
   over is then available for use by the frame buffer stack which your OpenMV
   Cam's firmware uses to hold large temporary data structures for image
   processing algorithms.

   If you need room to hold multiple frames you may "steal" frame buffer space
   by calling `sensor.alloc_extra_fb()`.

   If `sensor.set_auto_rotation()` is enabled this method will return a new
   already rotated `image` object.

   .. note::

      `sensor.snapshot()` may apply cropping parameters to fit the snapshot in the available
      RAM the pixformat, framesize, windowing, and framebuffers. The cropping parameters will be applied
      to maintain the aspect ratio and will stay until `sensor.set_framesize()` or `sensor.set_windowing()` are called.

.. function:: skip_frames(n:Optional[int]=None, time:Optional[int]=None) -> None

   Takes ``n`` number of snapshots to let the camera image stabilize after
   changing camera settings. ``n`` is passed as normal argument, e.g.
   ``skip_frames(10)`` to skip 10 frames. You should call this function after
   changing camera settings.

   Alternatively, you can pass the keyword argument ``time`` to skip frames
   for some number of milliseconds, e.g. ``skip_frames(time = 2000)`` to skip
   frames for 2000 milliseconds.

   If neither ``n`` nor ``time`` is specified this method skips frames for
   300 milliseconds.

   If both are specified this method skips ``n`` number of frames but will
   timeout after ``time`` milliseconds.

   .. note::

      `sensor.snapshot()` may apply cropping parameters to fit the snapshot in the available
      RAM given the pixformat, framesize, windowing, and framebuffers. The cropping parameters will be applied
      to maintain the aspect ratio and will stay until `sensor.set_framesize()` or `sensor.set_windowing()` are called.

.. function:: width() -> int

   Returns the sensor resolution width.

.. function:: height() -> int

   Returns the sensor resolution height.

.. function:: get_fb() -> Optional[image.Image]

   (Get Frame Buffer) Returns the image object returned by a previous call of
   `sensor.snapshot()`. If `sensor.snapshot()` had not been called before
   then ``None`` is returned.

.. function:: get_id() -> int

   Returns the camera module ID.

      * `sensor.OV9650`: First gen OpenMV Cam sensor - never released.
      * `sensor.OV2640`: Second gen OpenMV Cam sensor - never released.
      * `sensor.OV5640`: High-res OpenMV Cam H7 sensor.
      * `sensor.OV7725`: Rolling shutter sensor module.
      * `sensor.OV7690`: OpenMV Cam Micro sensor module.
      * `sensor.MT9V034`: Global shutter sensor module.
      * `sensor.MT9M114`: New Rolling shutter sensor module.
      * `sensor.LEPTON`: Lepton1/2/3 sensor module.
      * `sensor.HM01B0`: Arduino Portenta H7 sensor module.
      * `sensor.GC2145`: Arduino Nicla Vision H7 sensor module.
      * `sensor.PAJ6100`: PixArt Imaging sensor Module.

.. function:: alloc_extra_fb(width:int, height:int, pixformat:int) -> image.Image

   Allocates another frame buffer for image storage from the frame buffer stack
   and returns an ``image`` object of ``width``, ``height``, and ``pixformat``.

   You may call this function as many times as you like as long as there's
   memory available to allocate any number of extra frame buffers.

   If ``pixformat`` is a number >= 4 then this will allocate a JPEG image. You
   can then do `Image.bytearray()` to get byte level read/write access to the JPEG image.

   .. note::

      Creating secondary images normally requires creating them on the heap which
      has a limited amount of RAM... but, also gets fragmented making it hard to
      grab a large contiguous memory array to store an image in. With this method
      you are able to allocate a very large memory array for an image instantly
      by taking space away from our frame buffer stack memory which we use for
      computer vision algorithms. That said, this also means you'll run out of
      memory more easily if you try to execute more memory intensive machine
      vision algorithms like `Image.find_apriltags()`.

.. function:: dealloc_extra_fb() -> None

   Deallocates the last previously allocated extra frame buffer. Extra frame
   buffers are stored in a stack like structure.

   .. note::

      Your OpenMV Cam has two memory areas. First, you have your classical
      .data/.bss/heap/stack memory area. The .data/.bss/heap regions are
      fixed by firmware. The stack then grows down until it hits the heap.
      Next, frame buffers are stored in a secondary memory region. Memory is
      liad out with the main frame buffer on the bottom and the frame buffer
      stack on the top. When `sensor.snapshot()` is called it fills the frame buffer
      from the bottom. The frame buffer stack is then able to use whatever is
      left over. This memory allocation method is extremely efficient for computer
      vision on microcontrollers.

.. function:: set_pixformat(pixformat:int) -> None

   Sets the pixel format for the camera module.

      * `sensor.GRAYSCALE`: 8-bits per pixel.
      * `sensor.RGB565`: 16-bits per pixel.
      * `sensor.BAYER`: 8-bits per pixel bayer pattern.
      * `sensor.YUV422`: 16-bits per pixel (8-bits Y1, 8-bits U, 8-bits Y2, 8-bits V, etc.)
      * `sensor.JPEG`: Compressed JPEG data. Only for the OV2640/OV5640.

   If you are trying to take JPEG images with the OV2640 or OV5640 camera modules at high
   resolutions you should set the pixformat to `sensor.JPEG`. You can control the image
   quality then with `sensor.set_quality()`.

.. function:: get_pixformat() -> int

   Returns the pixformat for the camera module.

.. function:: set_framesize(framesize:int) -> None

   Sets the frame size for the camera module.

      * `sensor.QQCIF`: 88x72
      * `sensor.QCIF`: 176x144
      * `sensor.CIF`: 352x288
      * `sensor.QQSIF`: 88x60
      * `sensor.QSIF`: 176x120
      * `sensor.SIF`: 352x240
      * `sensor.QQQQVGA`: 40x30
      * `sensor.QQQVGA`: 80x60
      * `sensor.QQVGA`: 160x120
      * `sensor.QVGA`: 320x240
      * `sensor.VGA`: 640x480
      * `sensor.HQQQQVGA`: 30x20
      * `sensor.HQQQVGA`: 60x40
      * `sensor.HQQVGA`: 120x80
      * `sensor.HQVGA`: 240x160
      * `sensor.HVGA`: 480x320
      * `sensor.B64X32`: 64x32 (for use with `Image.find_displacement()`)
      * `sensor.B64X64`: 64x64 (for use with `Image.find_displacement()`)
      * `sensor.B128X64`: 128x64 (for use with `Image.find_displacement()`)
      * `sensor.B128X128`: 128x128 (for use with `Image.find_displacement()`)
      * `sensor.B160X160`: 160x160 (for the HM01B0)
      * `sensor.B320X320`: 320x320 (for the HM01B0)
      * `sensor.LCD`: 128x160 (for use with the lcd shield)
      * `sensor.QQVGA2`: 128x160 (for use with the lcd shield)
      * `sensor.WVGA`: 720x480 (for the MT9V034)
      * `sensor.WVGA2`:752x480 (for the MT9V034)
      * `sensor.SVGA`: 800x600 (only for the OV2640/OV5640 sensor)
      * `sensor.XGA`: 1024x768 (only for the OV2640/OV5640 sensor)
      * `sensor.WXGA`: 1280x768 (for the MT9M114)
      * `sensor.SXGA`: 1280x1024 (only for the OV2640/OV5640 sensor)
      * `sensor.SXGAM`: 1280x960 (for the MT9M114)
      * `sensor.UXGA`: 1600x1200 (only for the OV2640/OV5640 sensor)
      * `sensor.HD`: 1280x720 (only for the OV2640/OV5640 sensor)
      * `sensor.FHD`: 1920x1080 (only for the OV5640 sensor)
      * `sensor.QHD`: 2560x1440 (only for the OV5640 sensor)
      * `sensor.QXGA`: 2048x1536 (only for the OV5640 sensor)
      * `sensor.WQXGA`: 2560x1600 (only for the OV5640 sensor)
      * `sensor.WQXGA2`: 2592x1944 (only for the OV5640 sensor)

.. function:: get_framesize() -> int

   Returns the frame size for the camera module.

.. function:: set_framerate(rate:int) -> None

   Sets the frame rate in hz for the camera module.

   .. note::

      `set_framerate` works by dropping frames received by the camera module to keep the frame rate
      equal to (or below) the rate you specify. By default the camera will run at the maximum frame
      rate. If implemented for the particular camera sensor then `set_framerate` will also reduce
      the camera sensor frame rate internally to save power and improve image quality by increasing
      the sensor exposure. `set_framerate` may conflict with `set_auto_exposure` on some cameras.

.. function:: get_framerate() -> int

   Returns the frame rate in hz for the camera module.

.. function:: set_windowing(roi:Union[Tuple[int,int],Tuple[int,int,int,int]]) -> None

   Sets the resolution of the camera to a sub resolution inside of the current
   resolution. For example, setting the resolution to `sensor.VGA` and then
   the windowing to (120, 140, 200, 200) sets `sensor.snapshot()` to capture
   the 200x200 center pixels of the VGA resolution outputted by the camera
   sensor. You can use windowing to get custom resolutions. Also, when using
   windowing on a larger resolution you effectively are digital zooming.

   ``roi`` is a rect tuple (x, y, w, h). However, you may just pass (w, h) and
   the ``roi`` will be centered on the frame. You may also pass roi not in parens.

   This function will automatically handle cropping the passed roi to the framesize.

.. function:: get_windowing() -> Tuple[int, int, int, int]

   Returns the ``roi`` tuple (x, y, w, h) previously set with `sensor.set_windowing()`.

.. function:: set_gainceiling(gainceiling:int) -> None

   Set the camera image gainceiling. 2, 4, 8, 16, 32, 64, or 128.

.. function:: set_contrast(contrast:int) -> None

   Set the camera image contrast. -3 to +3.

.. function:: set_brightness(brightness:int) -> None

   Set the camera image brightness. -3 to +3.

.. function:: set_saturation(saturation:int) -> None

   Set the camera image saturation. -3 to +3.

.. function:: set_quality(quality:int) -> None

   Set the camera image JPEG compression quality. 0 - 100.

   .. note::

      Only for the OV2640/OV5640 cameras.

.. function:: set_colorbar(enable:bool) -> None

   Turns color bar mode on (True) or off (False). Defaults to off.

.. function:: set_auto_gain(enable:bool, gain_db=-1, gain_db_ceiling:Optional[int]=None) -> None

   ``enable`` turns auto gain control on (True) or off (False).
   The camera will startup with auto gain control on.

   If ``enable`` is False you may set a fixed gain in decibels with ``gain_db``.

   If ``enable`` is True you may set the maximum gain ceiling in decibels with
   ``gain_db_ceiling`` for the automatic gain control algorithm.

   .. note::

      You need to turn off white balance too if you want to track colors.

.. function:: get_gain_db() -> float

   Returns the current camera gain value in decibels (float).

.. function:: set_auto_exposure(enable:bool, exposure_us:Optional[int]=None) -> None

   ``enable`` turns auto exposure control on (True) or off (False).
   The camera will startup with auto exposure control on.

   If ``enable`` is False you may set a fixed exposure time in microseconds
   with ``exposure_us``.

   .. note::

      Camera auto exposure algorithms are pretty conservative about how much
      they adjust the exposure value by and will generally avoid changing the
      exposure value by much. Instead, they change the gain value a lot to deal
      with changing lighting.

.. function:: get_exposure_us() -> int

   Returns the current camera exposure value in microseconds (int).

.. function:: set_auto_whitebal(enable:bool, rgb_gain_db:Optional[Tuple[float,float,float]]=None) -> None

   ``enable`` turns auto white balance on (True) or off (False).
   The camera will startup with auto white balance on.

   If ``enable`` is False you may set a fixed gain in decibels for the red, green,
   and blue channels respectively with ``rgb_gain_db``.

   .. note::

      You need to turn off gain control too if you want to track colors.

.. function:: get_rgb_gain_db() -> Tuple[float, float, float]

   Returns a tuple with the current camera red, green, and blue gain values in
   decibels ((float, float, float)).

.. function:: set_auto_blc(enable:bool, regs:Optional[Any]=None)

   Sets the auto black line calibration (blc) control on the camera.

   ``enable`` pass `True` or `False` to turn BLC on or off. You typically always want this on.

   ``regs`` if disabled then you can manually set the blc register values via the values you
   got previously from `get_blc_regs()`.

.. function:: get_blc_regs() -> Any

   Returns the sensor blc registers as an opaque tuple of integers. For use with `set_auto_blc`.

.. function:: set_hmirror(enable:bool) -> None

   Turns horizontal mirror mode on (True) or off (False). Defaults to off.

.. function:: get_hmirror() -> bool

   Returns if horizontal mirror mode is enabled.

.. function:: set_vflip(enable:bool) -> None

   Turns vertical flip mode on (True) or off (False). Defaults to off.

.. function:: get_vflip() -> bool

   Returns if vertical flip mode is enabled.

.. function:: set_transpose(enable:bool) -> None

   Turns transpose mode on (True) or off (False). Defaults to off.

      * vflip=False, hmirror=False, transpose=False -> 0 degree rotation
      * vflip=True,  hmirror=False, transpose=True  -> 90 degree rotation
      * vflip=True,  hmirror=True,  transpose=False -> 180 degree rotation
      * vflip=False, hmirror=True,  transpose=True  -> 270 degree rotation

.. function:: get_transpose() -> bool

   Returns if transpose mode is enabled.

.. function:: set_auto_rotation(enable:bool) -> None

   Turns auto rotation mode on (True) or off (False). Defaults to off.

   .. note::

      This function only works when the OpenMV Cam has an `imu` installed and is enabled automatically.

.. function:: get_auto_rotation() -> bool

   Returns if auto rotation mode is enabled.

   .. note::

      This function only works when the OpenMV Cam has an `imu` installed and is enabled automatically.

.. function:: set_framebuffers(count:int) -> None

   Sets the number of frame buffers used to receive image data. By default your OpenMV Cam will
   automatically try to allocate the maximum number of frame buffers it can possibly allocate
   without using more than 1/2 of the available frame buffer RAM at the time of allocation to
   ensure the best performance. Automatic reallocation of frame buffers occurs whenever you
   call `sensor.set_pixformat()`, `sensor.set_framesize()`, and `sensor.set_windowing()`.

   `sensor.snapshot()` will automatically handle switching active frame buffers in the background.
   From your code's perspective there is only ever 1 active frame buffer even though there might
   be more than 1 frame buffer on the system and another frame buffer receiving data in the background.

   If count is:

      1 - Single Buffer Mode (you may also pass `sensor.SINGLE_BUFFER`)
          In single buffer mode your OpenMV Cam will allocate one frame buffer for receiving images.
          When you call `sensor.snapshot()` that framebuffer will be used to receive the image and
          the camera driver will continue to run. In the advent you call `sensor.snapshot()` again
          before the first line of the next frame is received your code will execute at the frame rate
          of the camera. Otherwise, the image will be dropped.

      2 - Double Buffer Mode (you may also pass `sensor.DOUBLE_BUFFER`)
          In double buffer mode your OpenMV Cam will allocate two frame buffers for receiving images.
          When you call `sensor.snapshot()` one framebuffer will be used to receive the image and
          the camera driver will continue to run. When the next frame is received it will be stored
          in the other frame buffer. In the advent you call `sensor.snapshot()` again
          before the first line of the next frame after is received your code will execute at the frame rate
          of the camera. Otherwise, the image will be dropped.

      3 - Triple Buffer Mode (you may also pass `sensor.TRIPLE_BUFFER`)
          In triple buffer mode your OpenMV Cam will allocate three buffers for receiving images.
          In this mode there is always a frame buffer to store the received image to in the background
          resulting in the highest performance and lowest latency for reading the latest received frame.
          No frames are ever dropped in this mode. The next frame read by `sensor.snapshot()` is the
          last captured frame by the sensor driver (e.g. if you are reading slower than the camera
          frame rate then the older frame in the possible frames available is skipped).

   Regarding the reallocation above, triple buffering is tried first, then double buffering, and if
   these both fail to fit in 1/2 of the available frame buffer RAM then single buffer mode is used.

   You may pass a value of 4 or greater to put the sensor driver into video FIFO mode where received
   images are stored in a frame buffer FIFO with ``count`` buffers. This is useful for video recording
   to an SD card which may randomly block your code from writing data when the SD card is performing
   house-keeping tasks like pre-erasing blocks to write data to.

   .. note::

      On frame drop (no buffers available to receive the next frame) all frame buffers are automatically
      cleared except the active frame buffer. This is done to ensure `sensor.snapshot()` returns current
      frames and not frames from long ago.

   Fun fact, you can pass a value of 100 or so on OpenMV Cam's with SDRAM for a huge video fifo. If
   you then call snapshot slower than the camera frame rate (by adding `machine.sleep()`) you'll get
   slow-mo effects in OpenMV IDE. However, you will also see the above policy effect of resetting
   the frame buffer on a frame drop to ensure that frames do not get too old. If you want to record
   slow-mo video just record video normally to the SD card and then play the video back on a desktop
   machine slower than it was recorded.

.. function:: get_framebuffers() -> int

   Returns the current number of frame buffers allocated.

.. function:: disable_delays(disable:Optional[bool]=None) -> bool

   If ``disable`` is ``True`` then disable all settling time delays in the sensor module.
   Whenever you reset the camera module, change modes, etc. the sensor driver delays to prevent
   you can from calling `snapshot` to quickly afterwards and receiving corrupt frames from the
   camera module. By disabling delays you can quickly update the camera module settings in bulk
   via multiple function calls before delaying at the end and calling `snapshot`.

   If this function is called with no arguments it returns if delays are disabled.

.. function:: disable_full_flush(disable:Optional[bool]=None) -> bool

   If ``disable`` is ``True`` then automatic framebuffer flushing mentioned in `set_framebuffers`
   is disabled. This removes any time limit on frames in the frame buffer fifo. For example, if
   you set the number of frame buffers to 30 and set the frame rate to 30 you can now precisely
   record 1 second of video from the camera without risk of frame loss.

   If this function is called with no arguments it returns if automatic flushing is disabled. By
   default automatic flushing on frame drop is enabled to clear out stale frames.

   .. note::

      `snapshot` starts the frame capture process which will continue to capture frames until
      there is no space to hold a frame at which point the frame capture process stops. The
      process always stops when there is no space to hold the next frame.

.. function:: set_lens_correction(enable:bool, radi:int, coef:int) -> None

   ``enable`` True to enable and False to disable (bool).
   ``radi`` integer radius of pixels to correct (int).
   ``coef`` power of correction (int).

.. function:: set_vsync_callback(cb) -> None

   Registers callback ``cb`` to be executed (in interrupt context) whenever the camera module
   generates a new frame (but, before the frame is received).

   ``cb`` takes one argument and is passed the current state of the vsync pin after changing.

.. function:: set_frame_callback(cb) -> None

   Registers callback ``cb`` to be executed (in interrupt context) whenever the camera module
   generates a new frame and the frame is ready to be read via `sensor.snapshot()`.

   ``cb`` takes no arguments.

   Use this to get an interrupt to schedule reading a frame later with `micropython.schedule()`.

.. function:: get_frame_available() -> bool

   Returns True if a frame is available to read by calling `sensor.snapshot()`.

.. function:: ioctl(*args, **kwargs) -> Any

   Executes a sensor specific method:

   * `sensor.IOCTL_SET_READOUT_WINDOW` - Pass this enum followed by a rect tuple (x, y, w, h) or a size tuple (w, h).
      * This IOCTL allows you to control the readout window of the camera sensor which dramatically improves the frame rate at the cost of field-of-view.
      * If you pass a rect tuple (x, y, w, h) the readout window will be positioned on that rect tuple. The rect tuple's x/y position will be adjusted so the size w/h fits. Additionally, the size w/h will be adjusted to not be smaller than the ``framesize``.
      * If you pass a size tuple (w, h) the readout window will be centered given the w/h. Additionally, the size w/h will be adjusted to not be smaller than the ``framesize``.
      * This IOCTL is extremely helpful for increasing the frame rate on higher resolution cameras like the OV2640/OV5640.
   * `sensor.IOCTL_GET_READOUT_WINDOW` - Pass this enum for `sensor.ioctl` to return the current readout window rect tuple (x, y, w, h). By default this is (0, 0, maximum_camera_sensor_pixel_width, maximum_camera_sensor_pixel_height).
   * `sensor.IOCTL_SET_TRIGGERED_MODE` - Pass this enum followed by True or False set triggered mode for the MT9V034 sensor.
   * `sensor.IOCTL_GET_TRIGGERED_MODE` - Pass this enum for `sensor.ioctl` to return the current triggered mode state.
   * `sensor.IOCTL_SET_FOV_WIDE` - Pass this enum followed by True or False enable `sensor.set_framesize()` to optimize for the field-of-view over FPS.
   * `sensor.IOCTL_GET_FOV_WIDE` - Pass this enum for `sensor.ioctl` to return the current field-of-view over fps optimization state.
   * `sensor.IOCTL_TRIGGER_AUTO_FOCUS` - Pass this enum for `sensor.ioctl` to trigger auto focus on the OV5640 FPC camera module.
   * `sensor.IOCTL_PAUSE_AUTO_FOCUS` - Pass this enum for `sensor.ioctl` to pause auto focus (after triggering) on the OV5640 FPC camera module.
   * `sensor.IOCTL_RESET_AUTO_FOCUS` - Pass this enum for `sensor.ioctl` to reset auto focus (after triggering) on the OV5640 FPC camera module.
   * `sensor.IOCTL_WAIT_ON_AUTO_FOCUS` - Pass this enum for `sensor.ioctl` to wait for auto focus (after triggering) to finish on the OV5640 FPC camera module. You may pass a second argument of the timeout in milliseconds. The default is 5000 ms.
   * `sensor.IOCTL_SET_NIGHT_MODE` - Pass this enum followed by True or False set nightmode the OV7725 and OV5640 sensors.
   * `sensor.IOCTL_GET_NIGHT_MODE` - Pass this enum for `sensor.ioctl` to return the current night mode state.
   * `sensor.IOCTL_LEPTON_GET_WIDTH` - Pass this enum to get the FLIR Lepton image width in pixels.
   * `sensor.IOCTL_LEPTON_GET_HEIGHT` - Pass this enum to get the FLIR Lepton image height in pixels.
   * `sensor.IOCTL_LEPTON_GET_RADIOMETRY` - Pass this enum to get the FLIR Lepton type (radiometric or not).
   * `sensor.IOCTL_LEPTON_GET_REFRESH` - Pass this enum to get the FLIR Lepton refresh rate in hertz.
   * `sensor.IOCTL_LEPTON_GET_RESOLUTION` - Pass this enum to get the FLIR Lepton ADC resolution in bits.
   * `sensor.IOCTL_LEPTON_RUN_COMMAND` - Pass this enum to execute a FLIR Lepton SDK command. You need to pass an additional 16-bit value after the enum as the command to execute.
   * `sensor.IOCTL_LEPTON_SET_ATTRIBUTE` - Pass this enum to set a FLIR Lepton SDK attribute.
      * The first argument is the 16-bit attribute ID to set (set the FLIR Lepton SDK).
      * The second argument is a MicroPython byte array of bytes to write (should be a multiple of 16-bits). Create the byte array using ``struct`` following the FLIR Lepton SDK.
   * `sensor.IOCTL_LEPTON_GET_ATTRIBUTE` - Pass this enum to get a FLIR Lepton SDK attribute.
      * The first argument is the 16-bit attribute ID to set (set the FLIR Lepton SDK).
      * Returns a MicroPython byte array of the attribute. Use ``struct`` to deserialize the byte array following the FLIR Lepton SDK.
   * `sensor.IOCTL_LEPTON_GET_FPA_TEMPERATURE` - Pass this enum to get the FLIR Lepton FPA Temp in celsius.
   * `sensor.IOCTL_LEPTON_GET_AUX_TEMPERATURE` - Pass this enum to get the FLIR Lepton AUX Temp in celsius.
   * `sensor.IOCTL_LEPTON_SET_MEASUREMENT_MODE` - Pass this followed by True or False to turn off automatic gain control on the FLIR Lepton and force it to output an image where each pixel value represents an exact temperature value in celsius. A second True enables high temperature mode enabling measurements up to 500C on the Lepton 3.5, False is the default low temperature mode.
   * `sensor.IOCTL_LEPTON_GET_MEASUREMENT_MODE` - Pass this to get a tuple for (measurement-mode-enabled, high-temp-enabled).
   * `sensor.IOCTL_LEPTON_SET_MEASUREMENT_RANGE` - Pass this when measurement mode is enabled to set the temperature range in celsius for the mapping operation. The temperature image returned by the FLIR Lepton will then be clamped between these min and max values and then scaled to values between 0 to 255. To map a pixel value back to a temperature (on a grayscale image) do: ((pixel * (max_temp_in_celsius - min_temp_in_celsius)) / 255.0) + min_temp_in_celsius.
      * The first argument should be the min temperature in celsius.
      * The second argument should be the max temperature in celsius. If the arguments are reversed the library will automatically swap them for you.
   * `sensor.IOCTL_LEPTON_GET_MEASUREMENT_RANGE` - Pass this to return the sorted (min, max) 2 value temperature range tuple. The default is -10C to 40C if not set yet.
   * `sensor.IOCTL_HIMAX_MD_ENABLE` - Pass this enum followed by ``True``/``False`` to enable/disable motion detection on the HM01B0. You should also enable the I/O pin (PC15 on the Arduino Portenta) attached the HM01B0 motion detection line to receive an interrupt.
   * `sensor.IOCTL_HIMAX_MD_CLEAR` - Pass this enum to clear the motion detection interrupt on the HM01B0.
   * `sensor.IOCTL_HIMAX_MD_WINDOW` - Pass this enum followed by (x1, y1, x2, y2) to set the motion detection window on the HM01B0.
   * `sensor.IOCTL_HIMAX_MD_THRESHOLD` - Pass this enum followed by a threshold value (0-255) to set the motion detection threshold on the HM01B0.
   * `sensor.IOCTL_HIMAX_OSC_ENABLE` - Pass this enum followed by ``True``/``False`` to enable/disable the oscillator HM01B0 to save power.

.. function:: set_color_palette(palette:int) -> None

   Sets the color palette to use for FLIR Lepton grayscale to RGB565 conversion.

.. function:: get_color_palette() -> int

   Returns the current color palette setting. Defaults to `image.PALETTE_RAINBOW`.

.. function:: __write_reg(address:int, value:int) -> None

   Write ``value`` (int) to camera register at ``address`` (int).

   .. note:: See the camera data sheet for register info.

.. function:: __read_reg(address:int) -> int

   Read camera register at ``address`` (int).

   .. note:: See the camera data sheet for register info.

Constants
---------

.. data:: BINARY
   :type: int

   BINARY (bitmap) pixel format. Each pixel is 1-bit.

   This format is usful for mask storage. Can be used with `Image()` and
   `sensor.alloc_extra_fb()`.

.. data:: GRAYSCALE
   :type: int

   GRAYSCALE pixel format (Y from YUV422). Each pixel is 8-bits, 1-byte.

   All of our computer vision algorithms run faster on grayscale images than
   RGB565 images.

.. data:: RGB565
   :type: int

   RGB565 pixel format. Each pixel is 16-bits, 2-bytes. 5-bits are used for red,
   6-bits are used for green, and 5-bits are used for blue.

   All of our computer vision algorithms run slower on RGB565 images than
   grayscale images.

.. data:: BAYER
   :type: int

   RAW BAYER image pixel format. If you try to make the frame size too big
   to fit in the frame buffer your OpenMV Cam will set the pixel format
   to BAYER so that you can capture images but only some image processing methods
   will be operational.

.. data:: YUV422
   :type: int

   A pixel format that is very easy to jpeg compress. Each pixel is stored as a grayscale
   8-bit Y value followed by alternating 8-bit U/V color values that are shared between two
   Y values (8-bits Y1, 8-bits U, 8-bits Y2, 8-bits V, etc.). Only some image processing
   methods work with YUV422.

.. data:: JPEG
   :type: int

   JPEG mode. The camera module outputs compressed jpeg images.
   Use `sensor.set_quality()` to control the jpeg quality.
   Only works for the OV2640/OV5640 cameras.

.. data:: OV2640
   :type: int

   `sensor.get_id()` returns this for the OV2640 camera.

.. data:: OV5640
   :type: int

   `sensor.get_id()` returns this for the OV5640 camera.

.. data:: OV7690
   :type: int

   `sensor.get_id()` returns this for the OV7690 camera.

.. data:: OV7725
   :type: int

   `sensor.get_id()` returns this for the OV7725 camera.

.. data:: OV9650
   :type: int

   `sensor.get_id()` returns this for the OV9650 camera.

.. data:: MT9V022
   :type: int

   `sensor.get_id()` returns this for the MT9V022 camera.

.. data:: MT9V024
   :type: int

   `sensor.get_id()` returns this for the MT9V024 camera.

.. data:: MT9V032
   :type: int

   `sensor.get_id()` returns this for the MT9V032 camera.

.. data:: MT9V034
   :type: int

   `sensor.get_id()` returns this for the MT9V034 camera.

.. data:: MT9M114
   :type: int

   `sensor.get_id()` returns this for the MT9M114 camera.

.. data:: LEPTON
   :type: int

   `sensor.get_id()` returns this for the LEPTON1/2/3 cameras.

.. data:: HM01B0
   :type: int

   `sensor.get_id()` returns this for the HM01B0 camera.

.. data:: HM0360
   :type: int

   `sensor.get_id()` returns this for the HM01B0 camera.

.. data:: GC2145
   :type: int

   `sensor.get_id()` returns this for the GC2145 camera.

.. data:: PAG7920
   :type: int

   `sensor.get_id()` returns this for the PAG7920 camera.

.. data:: PAJ6100
   :type: int

   `sensor.get_id()` returns this for the PAJ6100 camera.

.. data:: FROGEYE2020
   :type: int

   `sensor.get_id()` returns this for the FROGEYE2020 camera.

.. data:: QQCIF
   :type: int

   88x72 resolution for the camera sensor.

.. data:: QCIF
   :type: int

   176x144 resolution for the camera sensor.

.. data:: CIF
   :type: int

   352x288 resolution for the camera sensor.

.. data:: QQSIF
   :type: int

   88x60 resolution for the camera sensor.

.. data:: QSIF
   :type: int

   176x120 resolution for the camera sensor.

.. data:: SIF
   :type: int

   352x240 resolution for the camera sensor.

.. data:: QQQQVGA
   :type: int

   40x30 resolution for the camera sensor.

.. data:: QQQVGA
   :type: int

   80x60 resolution for the camera sensor.

.. data:: QQVGA
   :type: int

   160x120 resolution for the camera sensor.

.. data:: QVGA
   :type: int

   320x240 resolution for the camera sensor.

.. data:: VGA
   :type: int

   640x480 resolution for the camera sensor.

.. data:: HQQQQVGA
   :type: int

   30x20 resolution for the camera sensor.

.. data:: HQQQVGA
   :type: int

   60x40 resolution for the camera sensor.

.. data:: HQQVGA
   :type: int

   120x80 resolution for the camera sensor.

.. data:: HQVGA
   :type: int

   240x160 resolution for the camera sensor.

.. data:: HVGA
   :type: int

   480x320 resolution for the camera sensor.

.. data:: B64X32
   :type: int

   64x32 resolution for the camera sensor.

   For use with `Image.find_displacement()` and any other FFT based algorithm.

.. data:: B64X64
   :type: int

   64x64 resolution for the camera sensor.

   For use with `Image.find_displacement()` and any other FFT based algorithm.

.. data:: B128X64
   :type: int

   128x64 resolution for the camera sensor.

   For use with `Image.find_displacement()` and any other FFT based algorithm.

.. data:: B128X128
   :type: int

   128x128 resolution for the camera sensor.

   For use with `Image.find_displacement()` and any other FFT based algorithm.

.. data:: B160X160
   :type: int

   160x160 resolution for the HM01B0 camera sensor.

.. data:: B320X320
   :type: int

   320x320 resolution for the HM01B0 camera sensor.

.. data:: LCD
   :type: int

   128x160 resolution for the camera sensor (for use with the lcd shield).

.. data:: QQVGA2
   :type: int

   128x160 resolution for the camera sensor (for use with the lcd shield).

.. data:: WVGA
   :type: int

   720x480 resolution for the MT9V034 camera sensor.

.. data:: WVGA2
   :type: int

   752x480 resolution for the MT9V034 camera sensor.

.. data:: SVGA
   :type: int

   800x600 resolution for the camera sensor.

.. data:: XGA
   :type: int

   1024x768 resolution for the camera sensor.

.. data:: WXGA
   :type: int

   1280x768 resolution for the MT9M114 camera sensor.

.. data:: SXGA
   :type: int

   1280x1024 resolution for the camera sensor. Only works for the OV2640/OV5640 cameras.

.. data:: SXGAM
   :type: int

   1280x960 resolution for the MT9M114 camera sensor.

.. data:: UXGA
   :type: int

   1600x1200 resolution for the camera sensor. Only works for the OV2640/OV5640 cameras.

.. data:: HD
   :type: int

   1280x720 resolution for the camera sensor.

.. data:: FHD
   :type: int

   1920x1080 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: QHD
   :type: int

   2560x1440 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: QXGA
   :type: int

   2048x1536 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: WQXGA
   :type: int

   2560x1600 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: WQXGA2
   :type: int

   2592x1944 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: IOCTL_SET_READOUT_WINDOW
   :type: int

   Lets you set the readout window for the OV5640.

.. data:: IOCTL_GET_READOUT_WINDOW
   :type: int

   Lets you get the readout window for the OV5640.

.. data:: IOCTL_SET_TRIGGERED_MODE
   :type: int

   Lets you set the triggered mode for the MT9V034.

.. data:: IOCTL_GET_TRIGGERED_MODE
   :type: int

   Lets you get the triggered mode for the MT9V034.

.. data:: IOCTL_SET_FOV_WIDE
   :type: int

   Enable `sensor.set_framesize()` to optimize for the field-of-view over FPS.

.. data:: IOCTL_GET_FOV_WIDE
   :type: int

   Return if `sensor.set_framesize()` is optimizing for field-of-view over FPS.

.. data:: IOCTL_TRIGGER_AUTO_FOCUS
   :type: int

   Used to trigger auto focus for the OV5640 FPC camera module.

.. data:: IOCTL_PAUSE_AUTO_FOCUS
   :type: int

   Used to pause auto focus (while running) for the OV5640 FPC camera module.

.. data:: IOCTL_RESET_AUTO_FOCUS
   :type: int

   Used to reset auto focus back to the default for the OV5640 FPC camera module.

.. data:: IOCTL_WAIT_ON_AUTO_FOCUS
   :type: int

   Used to wait on auto focus to finish after being triggered for the OV5640 FPC camera module.

.. data:: IOCTL_SET_NIGHT_MODE
   :type: int

   Used to turn night mode on or off on a sensor. Nightmode reduces the frame rate to increase exposure dynamically.

.. data:: IOCTL_GET_NIGHT_MODE
   :type: int

   Gets the current value of if night mode is enabled or disabled for your sensor.

.. data:: IOCTL_LEPTON_GET_WIDTH
   :type: int

   Lets you get the FLIR Lepton image resolution width in pixels.

.. data:: IOCTL_LEPTON_GET_HEIGHT
   :type: int

   Lets you get the FLIR Lepton image resolution height in pixels.

.. data:: IOCTL_LEPTON_GET_RADIOMETRY
   :type: int

   Lets you get the FLIR Lepton type (radiometric or not).

.. data:: IOCTL_LEPTON_GET_REFRESH
   :type: int

   Lets you get the FLIR Lepton refresh rate in hertz.

.. data:: IOCTL_LEPTON_GET_RESOLUTION
   :type: int

   Lets you get the FLIR Lepton ADC resolution in bits.

.. data:: IOCTL_LEPTON_RUN_COMMAND
   :type: int

   Executes a 16-bit command given the FLIR Lepton SDK.

.. data:: IOCTL_LEPTON_SET_ATTRIBUTE
   :type: int

   Sets a FLIR Lepton Attribute given the FLIR Lepton SDK.

.. data:: IOCTL_LEPTON_GET_ATTRIBUTE
   :type: int

   Gets a FLIR Lepton Attribute given the FLIR Lepton SDK.

.. data:: IOCTL_LEPTON_GET_FPA_TEMPERATURE
   :type: int

   Gets the FLIR Lepton FPA temp in celsius.

.. data:: IOCTL_LEPTON_GET_AUX_TEMPERATURE
   :type: int

   Gets the FLIR Lepton AUX temp in celsius.

.. data:: IOCTL_LEPTON_SET_MEASUREMENT_MODE
   :type: int

   Lets you set the FLIR Lepton driver into a mode where you can get a valid temperature value per pixel. See `sensor.ioctl()` for more information.

.. data:: IOCTL_LEPTON_GET_MEASUREMENT_MODE
   :type: int

   Lets you get if measurement mode is enabled or not for the FLIR Lepton sensor. See `sensor.ioctl()` for more information.

.. data:: IOCTL_LEPTON_SET_MEASUREMENT_RANGE
   :type: int

   Lets you set the temperature range you want to map pixels in the image to when in measurement mode. See `sensor.ioctl()` for more information.

.. data:: IOCTL_LEPTON_GET_MEASUREMENT_RANGE
   :type: int

   Lets you get the temperature range used for measurement mode. See `sensor.ioctl()` for more information.

.. data:: IOCTL_HIMAX_MD_ENABLE
   :type: int

   Lets you control the motion detection interrupt on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: IOCTL_HIMAX_MD_CLEAR
   :type: int

   Lets you control the motion detection interrupt on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: IOCTL_HIMAX_MD_WINDOW
   :type: int

   Lets you control the motion detection interrupt on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: IOCTL_HIMAX_MD_THRESHOLD
   :type: int

   Lets you control the motion detection interrupt on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: IOCTL_HIMAX_OSC_ENABLE
   :type: int

   Lets you control the internal oscillator on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: SINGLE_BUFFER
   :type: int

   Pass to `sensor.set_framebuffers()` to set single buffer mode (1 buffer).

.. data:: DOUBLE_BUFFER
   :type: int

   Pass to `sensor.set_framebuffers()` to set double buffer mode (2 buffers).

.. data:: TRIPLE_BUFFER
   :type: int

   Pass to `sensor.set_framebuffers()` to set triple buffer mode (3 buffers).

.. data:: VIDEO_FIFO
   :type: int

   Pass to `sensor.set_framebuffers()` to set video FIFO mode (4 buffers).
