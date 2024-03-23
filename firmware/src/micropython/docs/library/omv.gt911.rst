:mod:`gt911` --- Touch Screen Driver
====================================

.. module:: gt911
   :synopsis: Touch Screen Driver

Basic polling mode example usage::

    import time
    from gt911 import GT911
    from machine import I2C
    # Note use pin numbers or names not Pin objects because the
    # driver needs to change pin directions to reset the controller.
    touch = GT911(I2C(1, freq=400_000), reset_pin="P1", irq_pin="P2", touch_points=5)
    while True:
       n, points = touch.read_points()
       for i in range(0, n):
          print(f"id {points[i][3]} x {points[i][0]} y {points[i][1]} size {points[i][2]}")
       time.sleep_ms(100)

Constructors
------------

.. class:: gt911.GT911(bus, reset_pin, irq_pin, [address=0x5D, [width=800, [height=480, [touch_points=1, [reserve_x=False, [reserve_y=False, [reverse_axis=True, [stio=True, [refresh_rate=240, [touch_callback=None]]]]]]]]]])

   Creates a touch screen controller object. You should initialize it according to the example above.

Methods
-------

.. method:: GT911._read_reg(reg, [size=1, [buf=None]])

   Reads a register value.

.. method:: GT911._write_reg(reg, val, [size=1])

   Writes a register value.

.. method:: GT911.read_id()

   Returns the ID of the gt911 chip.

.. method:: GT911.read_points()

   Returns a tuple containing the count of points an array of point tuples. Each point tuple has
   an x[0], y[1], size[2], and id[3]. x/y are the position on screen. Size is the amount of pressure
   applied. And id is a unique id per point which should correlate to the same point over reads.

.. method:: GT911.reset()

   Resets the gt911 chip.
