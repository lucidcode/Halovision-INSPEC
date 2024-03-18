.. currentmodule:: display
.. _display.DisplayData:

class DisplayData -- Display Data
=================================

The `DisplayData` class is used for getting information about the attached DisplayPort/HDMI Display.

Constructors
------------

.. class:: display.DisplayData([cec=False, [ddc=False, [ddc_addr=0x50]]])

    ``cec`` Pass `True` to enable CEC communication to an external display (if possible).

    ``ddc`` Pass `True` to enable DDC communication to an external display (if possible).

    ``ddc_addr`` The I2C address to use to talk to the external display EEPROM.

Methods
-------

.. method:: display.DisplayData.display_id()

   Returns the external display EDID data as a bytes()
   object. Verifying the EDID headers, checksums, and concatenating all sections into one bytes()
   object is done for you. You may then parse this information by `following this guide <https://en.wikipedia.org/wiki/Extended_Display_Identification_Data>`__.

.. method:: DisplayData.send_frame(dst_addr, src_addr, bytes)

   Sends a packet on the HDMI-CEC bus to ``dst_addr`` with source ``src_addr`` and data ``bytes``.

.. method:: DisplayData.receive_frame(dst_addr, timeout=1000)

   Waits ``timeout`` milliseconds to receive an HDMI-CEC
   frame for address ``dst_addr``. Returns True if the received frame was for ``dst_addr`` and False
   if not. On timeout throws an `OSError` Exception.

.. method:: DisplayData.frame_callback(callback, dst_addr)

   Registers a ``callback`` which will be called on reception of an
   HDMI-CEC frame. The callback will receive two arguments of the frame src_addr as an int and
   payload as a `bytes()` object.

   ``dst_addr`` sets the filter address to listen to on the CEC bus.

   If you use this method do not call `DisplayData.receive_frame()` anymore until the callback is
   disabled by passing ``None`` as the callback for this method.
