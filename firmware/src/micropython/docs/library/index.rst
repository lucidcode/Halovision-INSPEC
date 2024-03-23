.. _micropython_lib:

MicroPython libraries
=====================

.. warning::

   Important summary of this section

   * MicroPython provides built-in modules that mirror the functionality of the
     :ref:`Python standard library <micropython_lib_python>` (e.g. :mod:`os`,
     :mod:`time`), as well as :ref:`MicroPython-specific modules <micropython_lib_micropython>`
     (e.g. :mod:`bluetooth`, :mod:`machine`).
   * Most Python standard library modules implement a subset of the
     functionality of the equivalent Python module, and in a few cases provide
     some MicroPython-specific extensions (e.g. :mod:`array`, :mod:`os`)
   * Due to resource constraints or other limitations, some ports or firmware
     versions may not include all the functionality documented here.
   * To allow for extensibility, some built-in modules can be
     :ref:`extended from Python code <micropython_lib_extending>` loaded onto
     the device filesystem.

This chapter describes modules (function and class libraries) which are built
into MicroPython. This documentation in general aspires to describe all modules
and functions/classes which are implemented in the MicroPython project.
However, MicroPython is highly configurable, and each port to a particular
board/embedded system may include only a subset of the available MicroPython
libraries.

With that in mind, please be warned that some functions/classes in a module (or
even the entire module) described in this documentation **may be unavailable**
in a particular build of MicroPython on a particular system. The best place to
find general information of the availability/non-availability of a particular
feature is the "General Information" section which contains information
pertaining to a specific :term:`MicroPython port`.

On some ports you are able to discover the available, built-in libraries that
can be imported by entering the following at the :term:`REPL`::

    help('modules')

Beyond the built-in libraries described in this documentation, many more
modules from the Python standard library, as well as further MicroPython
extensions to it, can be found in :term:`micropython-lib`.

.. _micropython_lib_python:

Python standard libraries and micro-libraries
---------------------------------------------

The following standard Python libraries have been "micro-ified" to fit in with
the philosophy of MicroPython.  They provide the core functionality of that
module and are intended to be a drop-in replacement for the standard Python
library.

.. toctree::
   :maxdepth: 1

   array.rst
   asyncio.rst
   binascii.rst
   builtins.rst
   cmath.rst
   collections.rst
   errno.rst
   gc.rst
   gzip.rst
   hashlib.rst
   heapq.rst
   io.rst
   json.rst
   math.rst
   os.rst
   platform.rst
   random.rst
   re.rst
   select.rst
   socket.rst
   ssl.rst
   struct.rst
   sys.rst
   time.rst
   zlib.rst
   _thread.rst

.. _micropython_lib_micropython:

MicroPython-specific libraries
------------------------------

Functionality specific to the MicroPython implementation is available in
the following libraries.

.. toctree::
   :maxdepth: 1

   bluetooth.rst
   btree.rst
   cryptolib.rst
   deflate.rst
   framebuf.rst
   machine.rst
   micropython.rst
   neopixel.rst
   network.rst
   uctypes.rst
   mutex.rst
   uping.rst
   urequests.rst

Libraries specific to the OpenMV Cam
------------------------------------

The following libraries are specific to the OpenMV Cam.

.. toctree::
   :maxdepth: 1

   pyb.rst
   stm.rst
   omv.sensor.rst
   omv.image.rst
   omv.tf.rst
   omv.gif.rst
   omv.mjpeg.rst
   omv.audio.rst
   omv.micro_speech.rst
   omv.display.rst
   omv.fir.rst
   omv.tv.rst
   omv.cpufreq.rst
   omv.buzzer.rst
   omv.imu.rst
   omv.rpc.rst
   omv.rtsp.rst
   omv.omv.rst
   omv.gt911.rst
   omv.ft5x06.rst
   omv.tfp410.rst

Third-party libraries on the OpenMV Cam
---------------------------------------

The following third-party libraries are built-in to your OpenMV Cam's firmware:

:mod:`ulab` --- `numpy-like array manipulation library <https://micropython-ulab.readthedocs.io/en/latest/>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: ulab
   :synopsis: numpy-like array manipulation library

:mod:`pid` --- `Proportional/Integral/Derivative Control <https://github.com/openmv/openmv/blob/master/scripts/libraries/pid.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: pid
   :synopsis: Proportional/Integral/Derivative Control

:mod:`bno055` --- `IMU Driver <https://github.com/openmv/openmv/blob/master/scripts/libraries/bno055.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: bno055
   :synopsis: IMU Driver

Examples scripts are located in OpenMV IDE under the ``IMU Shield`` examples folder.

:mod:`ssd1306` --- `OLED Driver <https://github.com/openmv/openmv/blob/master/scripts/libraries/ssd1306.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: ssd1306
   :synopsis: OLED Driver

:mod:`tb6612` --- `Stepper Motor Driver <https://github.com/openmv/openmv/blob/master/scripts/libraries/tb6612.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: tb6612
   :synopsis: Stepper Motor Driver

Examples scripts are located in OpenMV IDE under the ``Motor Shield`` examples folder.

:mod:`lsm6dsox` --- `lsm6dsox Driver <https://github.com/openmv/openmv/blob/master/scripts/libraries/lsm6dsox.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: lsm6dsox
   :synopsis: lsm6dsox Driver

:mod:`modbus` --- `modbus protocol library <https://github.com/openmv/openmv/blob/master/scripts/libraries/modbus.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: modbus
   :synopsis: modbus protocol library

Examples scripts are located in OpenMV IDE under the ``Modbus`` examples folder.

:mod:`mqtt` --- `mqtt protocol library <https://github.com/openmv/openmv/blob/master/scripts/libraries/mqtt.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: mqtt
   :synopsis: mqtt protocol library

Examples scripts are located in OpenMV IDE under the ``WiFi Shield`` examples folder.

:mod:`vl53l1x` --- `ToF Distance Sensor Driver <https://github.com/openmv/openmv/blob/master/scripts/libraries/vl53l1x.py>`_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. module:: vl53l1x
   :synopsis: ToF Distance Sensor Driver

Examples scripts are located in OpenMV IDE under the ``Distance Shield`` examples folder.

.. _micropython_lib_extending:

Extending built-in libraries from Python
----------------------------------------

A subset of the built-in modules are able to be extended by Python code by
providing a module of the same name in the filesystem. This extensibility
applies to the following Python standard library modules which are built-in to
the firmware: ``array``, ``binascii``, ``collections``, ``errno``, ``gzip``,
``hashlib``, ``heapq``, ``io``, ``json``, ``os``, ``platform``, ``random``,
``re``, ``select``, ``socket``, ``ssl``, ``struct``, ``time`` ``zlib``, as well
as the MicroPython-specific ``machine`` module. All other built-in modules
cannot be extended from the filesystem.

This allows the user to provide an extended implementation of a built-in library
(perhaps to provide additional CPython compatibility or missing functionality).
This is used extensively in :term:`micropython-lib`, see :ref:`packages` for
more information. The filesystem module will typically do a wildcard import of
the built-in module in order to inherit all the globals (classes, functions and
variables) from the built-in.

In MicroPython v1.21.0 and higher, to prevent the filesystem module from
importing itself, it can force an import of the built-in module it by
temporarily clearing ``sys.path`` during the import. For example, to extend the
``time`` module from Python, a file named ``time.py`` on the filesystem would
do the following::

  _path = sys.path
  sys.path = ()
  try:
    from time import *
  finally:
    sys.path = _path
    del _path

  def extra_method():
    pass

The result is that ``time.py`` contains all the globals of the built-in ``time``
module, but adds ``extra_method``.

In earlier versions of MicroPython, you can force an import of a built-in module
by appending a ``u`` to the start of its name. For example, ``import utime``
instead of ``import time``. For example, ``time.py`` on the filesystem could
look like::

  from utime import *

  def extra_method():
    pass

This way is still supported, but the ``sys.path`` method described above is now
preferred as the ``u``-prefix will be removed from the names of built-in
modules in a future version of MicroPython.

*Other than when it specifically needs to force the use of the built-in module,
code should always use* ``import module`` *rather than* ``import umodule``.
