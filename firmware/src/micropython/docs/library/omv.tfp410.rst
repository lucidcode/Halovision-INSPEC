:mod:`tfp410` --- DVI/HDMI Controller
=====================================

.. module:: TFP410
   :synopsis: DVI/HDMI Controller

DVI/HDMI Controller for the OpenMV Pure Thermal.

.. note::

   This will be refactored to be under the display module soon.

Constructors
------------

.. class:: tfp410.TFP410(i2c_addr=0x3F)

   Initializes the TFP410 DVI/HDMI controller chip to drive an external DVI/HDMI display via
   a 24-bit parallel LCD bus. You just need to create this object to initialize the display.

Methods
-------

.. method:: TFP410.isconnected() -> bool

   Returns if an external display is connected.

.. method:: TFP410.hotplug_callback(callback) -> None

   Registers a ``callback`` function that be called whenever the state
   of an external display being connected changes. The new state will be passed as an argument.

   If you use this method do not call `TFP410.isconnected()` anymore until the callback is
   disabled by pass ``None`` as the callback for this method.
