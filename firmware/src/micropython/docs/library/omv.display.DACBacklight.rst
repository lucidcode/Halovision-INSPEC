.. currentmodule:: display
.. _display.DACBacklight:

class DACBacklight -- DAC Backlight
===================================

The `DACBacklight` class is used to control a screen backlight.

Constructors
------------

.. class:: display.DACBacklight(channel, [bits=8])

   Creates a backlight object to initialize the display backlight. This class should be passed as
   the ``backlight`` argument to any display object constructor which can use a backlight controller.

   ``channel`` specifies the DAC channel to use. This can be the GPIO pin also. For STM32 based
   OpenMV Cams this is ``P5``.

   ``bits`` specifies the resolution of the DAC. The default value of 8-bits should be good enough.

Methods
-------

.. method:: DACBacklight.deinit()

   Deinitializes the backlight controller.

.. method:: DACBacklight.backlight([value])

   Sets the backlight strength from 0-100. Note that a linear voltage on the backlight output
   will not necessary result in a linear brightness change on the screen. Typically there's
   a small region where the screen brightness will change drastically.
