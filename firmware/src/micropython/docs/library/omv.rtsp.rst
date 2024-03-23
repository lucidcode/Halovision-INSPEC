:mod:`rtsp` --- rtsp library
============================

.. module:: rtsp
   :synopsis: rtsp library

The ``rtsp`` module on the OpenMV Cam allows you to stream video from your OpenMV Cam to any
compatible RTSP client (like `VLC <https://www.videolan.org/vlc/index.html>`_).

How to use the Library
----------------------

Please checkout the example scripts in OpenMV IDE under ``Web Servers``.

The `rtsp_server` is very easy to use. After being created you just need to call the `rtsp_server.stream()`
method with a call back function to generate image objects. For example::

    network_if = network.WLAN(network.STA_IF)
    network_if.active(True)
    network_if.connect('your-ssid', 'your-password')
    while not network_if.isconnected():
        print("Trying to connect. Note this may take a while...")
        time.sleep_ms(1000)

    server = rtsp.rtsp_server(network_if)
    server.stream(lambda pathname, session: sensor.snapshot())

Note that not all RTSP clients can decode all types of JPEG images streamed. For best results please
use the OV2640/OV5640 camera modules `sensor.JPEG` mode for streaming RTSP JPEG video.

class rtsp_server - rtsp_server class
-------------------------------------

The `rtsp_server` class creates a single connection RTSP web server on your OpenMV Cam.

Constructors
~~~~~~~~~~~~

.. class:: rtsp_server(network_if, port=554)

   Creates a WiFi ``rtsp`` server.

   ``network_if`` is the network module interface created from ``network.LAN()``, ``network.WLAN()``, or etc.

   ``port`` is the port number to use. The default RTSP port is 554.

   Methods
   ~~~~~~~

   .. method:: register_setup_cb(cb)

      Bind a call back (``cb``) to be executed when a client sets up a RTSP connection with the `rtsp_server`.

      Registering a call back is not required for the `rtsp_server` to work.

      The call back should accept two arguments:

      ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
      needed. Otherwise, you can use it to determine what image object to return. By default the
      ``pathname`` will be "/".

      ``session`` is random number that will change when a new connection is established. You can use
      ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.

   .. method:: register_play_cb(cb)

      Bind a call back (``cb``) to be executed when a client wants to start streaming.

      Registering a call back is not required for the `rtsp_server` to work.

      The call back should accept two arguments:

      ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
      needed. Otherwise, you can use it to determine what image object to return. By default the
      ``pathname`` will be "/".

      ``session`` is random number that will change when a new connection is established. You can use
      ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.

   .. method:: register_pause_cb(cb)

      Bind a call back (``cb``) to be executed when a client wants to pause streaming.

      Registering a call back is not required for the `rtsp_server` to work.

      NOTE: When you click the pause button on `VLC <https://www.videolan.org/vlc/index.html>`_ in does not tell the server to pause.

      The call back should accept two arguments:

      ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
      needed. Otherwise, you can use it to determine what image object to return. By default the
      ``pathname`` will be "/".

      ``session`` is random number that will change when a new connection is established. You can use
      ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.

   .. method:: register_teardown_cb(cb)

      Bind a call back (``cb``) to be executed when a client wants tear down a RTSP connection with the `rtsp_server`.

      Registering a call back is not required for the `rtsp_server` to work.

      The call back should accept two arguments:

      ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
      needed. Otherwise, you can use it to determine what image object to return. By default the
      ``pathname`` will be "/".

      ``session`` is random number that will change when a new connection is established. You can use
      ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.

   .. method:: stream(cb, quality=90)

      Starts running the `rtsp_server` logic and does not return. Make sure to setup everything you
      want to first before calling this method. Once called the `rtsp_server` will start accepting
      connections and streaming video data.

      ``cb`` should be a call back that returns an `Image` object which the RTSP library will jpeg
      compress and stream to the remote client. You are free to modify a `sensor.snapshot()` image
      as much as you like before returning the image object to be sent.

      ``quality`` is the JPEG compression quality to use while streaming.

      The call back should accept two arguments:

      ``pathname`` is the name of the stream resource the client wants. You can ignore this if it's not
      needed. Otherwise, you can use it to determine what image object to return. By default the
      ``pathname`` will be "/".

      ``session`` is random number that will change when a new connection is established. You can use
      ``session`` with a dictionary to differentiate different accesses to the same ``pathname``.
