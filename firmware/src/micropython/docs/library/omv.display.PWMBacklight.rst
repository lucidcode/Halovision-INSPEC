.. currentmodule:: display
.. _display.PWMBacklight:

class PWMBacklight -- PWM Backlight
===================================

The `PWMBacklight` class is used to control a screen backlight.

Constructors
------------

.. class:: display.PWMBacklight(pin, [timer=3, [channel=3, [frequency=200]]])

   Creates a backlight object to initialize the display backlight. This class should be passed as
   the ``backlight`` argument to any display object constructor which can use a backlight controller.

   ``pin`` specifies the Pin to use.

   ``timer`` specifies the Timer number to use.

   ``channel`` specifies the Timer channel to use.

   ``frequency`` specifies the PWM frequency.

Methods
-------

.. method:: PWMBacklight.deinit()

   Deinitializes the backlight controller.

.. method:: PWMBacklight.backlight([value])

   Sets the backlight strength from 0-100. Note that a linear pwm duty cycle on the backlight output
   will not necessary result in a linear brightness change on the screen. Typically there's
   a small region where the screen brightness will change drastically.
