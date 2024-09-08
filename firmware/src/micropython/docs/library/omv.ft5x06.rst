:mod:`ft5x06` --- Touch Screen Driver
=====================================

.. module:: ft5x06
   :synopsis: Touch Screen Driver

Touch Screen Driver for the OpenMV Pure Thermal.

.. note::

   This will be refactored to be under the display module soon.

Constructors
------------

.. class:: ft5x06.FT5X06(i2c_addr=0x38)

   Creates a touch screen controller object

Methods
-------

.. method:: FT5X06.get_gesture()

   This is one of LCD_GESTURE_*.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. method:: FT5X06.get_points()

   This returns the current number of touch points (0-5).

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. method:: FT5X06.get_point_flag(index)

   This returns the current touch point state of the point at ``index``.

   This is one of LCD_FLAG_*.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. method:: FT5X06.get_point_id(index)

   This returns the current touch point ``id`` of the point at ``index``.

   The touch point ``id`` is a numeric value that allows you to track a touch point as it may move
   around in list of touch points returned as points are added and removed.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. method:: FT5X06.get_point_x(index)

   This returns the current touch point x position of the point at ``index``.

   This is the x pixel position of the touch point on the screen.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. method:: FT5X06.get_point_y(index)

   This returns the current touch point y position of the point at ``index``.

   This is the y pixel position of the touch point on the screen.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. method:: FT5X06.touch_callback(callback)

   This method registers a callback which will receive the number of touch
   points (0-5) when a touch event happens.

   If you use this method do not call `FT5X06.update_points()` anymore until the callback is
   disabled by pass ``None`` as the callback for this method.

.. method:: FT5X06.update_points()

   This function reads the touch screen state and returns the number of touch points (0-5).

Constants
---------

.. data:: LCD_GESTURE_MOVE_UP
   :type: int

   Touch screen move up gesture.

.. data:: LCD_GESTURE_MOVE_LEFT
   :type: int

   Touch screen move left gesture.

.. data:: LCD_GESTURE_MOVE_DOWN
   :type: int

   Touch screen move down gesture.

.. data:: LCD_GESTURE_MOVE_RIGHT
   :type: int

   Touch screen move right gesture.

.. data:: LCD_GESTURE_ZOOM_IN
   :type: int

   Touch screen zoom in gesture.

.. data:: LCD_GESTURE_ZOOM_OUT
   :type: int

   Touch screen zoom out gesture.

.. data:: LCD_GESTURE_NONE
   :type: int

   Touch screen no gesture.

.. data:: LCD_FLAG_PRESSED
   :type: int

   Touch point is pressed.

.. data:: LCD_FLAG_RELEASED
   :type: int

   Touch point is released.

.. data:: LCD_FLAG_MOVED
   :type: int

   Touch point is moved.
