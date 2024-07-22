# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# RTSP Video Server
#
# This example shows off how to stream video over RTSP with your OpenMV Cam.
#
# You can use a program like VLC to view the video stream by connecting to the
# OpenMV Cam's IP address. However, OpenMV IDE has an FFPLAY based RSTP Viewer built-in which
# you can use by going to Tools->Video Tools->Play RSTP Stream.

import network
import omv
import rtsp
import sensor
import time

# If you are using VLC on linux you may need to install the live555 library for RTSP support to
# work. If you are using Ubuntu then run the following command:
#
# sudo apt-get install livemedia-utils

# Regarding latency on programs like VLC, the default is typically set to buffer 1 second of video
# before playback. To reduce this you need to reduce the network caching which can be set using
# "show more options" when you open the network stream in VLC. You can reduce this to like 10ms
# to make the video real-time.

sensor.reset()

sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.VGA)

# Turn off the frame buffer connection to the IDE from the OpenMV Cam side.
#
# This needs to be done when manually compressing jpeg images at higher quality
# so that the OpenMV Cam does not try to stream them to the IDE using a fall back
# mechanism if the JPEG image is too large to fit in the IDE JPEG frame buffer on the OpenMV Cam.

omv.disable_fb(True)

# Setup Network Interface

network_if = network.LAN()
network_if.active(True)
network_if.ifconfig("dhcp")

# Setup RTSP Server

server = rtsp.rtsp_server(network_if)

# For the call back functions below:
#
# `pathname` is the name of the stream resource the client wants. You can ignore this if it's not
# needed. Otherwise, you can use it to determine what image object to return. By default the path
# name will be "/".
#
# `session` is random number that will change when a new connection is established. You can use
# session with a dictionary to differentiate different accesses to the same file name.

# Track the current FPS.
clock = time.clock()


def setup_callback(pathname, session):
    print('Opening "%s" in session %d' % (pathname, session))


def play_callback(pathname, session):
    clock.reset()
    clock.tick()
    print('Playing "%s" in session %d' % (pathname, session))


def pause_callback(pathname, session):  # VLC only pauses locally. This is never called.
    print('Pausing "%s" in session %d' % (pathname, session))


def teardown_callback(pathname, session):
    print('Closing "%s" in session %d' % (pathname, session))


server.register_setup_cb(setup_callback)
server.register_play_cb(play_callback)
server.register_pause_cb(pause_callback)
server.register_teardown_cb(teardown_callback)


# Called each time a new frame is needed.
def image_callback(pathname, session):
    img = sensor.snapshot()
    # Markup image and/or do various things.
    print(clock.fps())
    clock.tick()
    return img


# Stream does not return. It will call `image_callback` when it needs to get an image object to send
# to the remote rtsp client connecting to the server.

server.stream(image_callback, quality=70)
