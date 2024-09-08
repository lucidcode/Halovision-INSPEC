:mod:`display` --- display driver
=================================

.. module:: display
   :synopsis: display driver

The ``display`` module is used for driving SPI LCDs, 24-bit parallel LCDs, MIPI DSI LCDs, HDMI output, and Display Port output.

Classes
-------

.. toctree::
   :maxdepth: 1

   omv.display.spidisplay.rst
   omv.display.rgbdisplay.rst
   omv.display.dsidisplay.rst
   omv.display.displaydata.rst
   omv.display.ST7701.rst
   omv.display.DACBacklight.rst
   omv.display.PWMBacklight.rst

Constants
---------

.. data:: QVGA
   :type: int

   320x240 resolution for framesize.

.. data:: TQVGA
   :type: int

   240x320 resolution for framesize.

.. data:: FHVGA
   :type: int

   480x272 resolution for framesize.

.. data:: FHVGA2
   :type: int

   480x128 resolution for framesize.

.. data:: VGA
   :type: int

   640x480 resolution for framesize.

.. data:: THVGA
   :type: int

   320x480 resolution for framesize.

.. data:: FWVGA
   :type: int

   800x480 resolution for framesize.

.. data:: FWVGA2
   :type: int

   800x320 resolution for framesize.

.. data:: TFWVGA
   :type: int

   480x800 resolution for framesize.

.. data:: TFWVGA2
   :type: int

   480x480 resolution for framesize.

.. data:: SVGA
   :type: int

   800x600 resolution for framesize.

.. data:: WSVGA
   :type: int

   1024x600 resolution for framesize.

.. data:: XGA
   :type: int

   1024x768 resolution for framesize.

.. data:: SXGA
   :type: int

   1280x1024 resolution for framesize.

.. data:: SXGA2
   :type: int

   1280x400 resolution for framesize.

.. data:: UXGA
   :type: int

   1600x1200 resolution for framesize.

.. data:: HD
   :type: int

   1280x720 resolution for framesize.

.. data:: FHD
   :type: int

   1920x1080 resolution for framesize.

   .. note::

      Use a ``refresh`` of 30 Hz with this setting. The STM32H7 is not capable of
      driving 1080p at 60 Hz.
