.. currentmodule:: display
.. _display.ST7701:

class ST7701 -- Display Controller
==================================

The `ST7701` class is used to initialize the LCD screen controller.

Constructors
------------

.. class:: display.ST7701()

   Creates a controller object to initialize the ST7701 display controller which typically powers
   MIPI DSI displays. This class should be passed as the ``cotnroller`` argument to the `DSIDisplay()`
   class constructor which will take care of calling the `ST7701.init()` method for you.

Methods
-------

.. method:: ST7701.init(display_controller)

   Initializes the display using the display controller which must provide `display.DSIDisplay.bus_write()` and
   `display.DSIDisplay.bus_read()` methods.

.. method:: ST7701.read_id()

   Returns the display id.
