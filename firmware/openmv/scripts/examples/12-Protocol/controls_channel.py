# This work is licensed under the MIT license.
# Copyright (c) 2026 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# FLIR Lepton Controls Channel Example
#
# This example streams the FLIR Lepton thermal camera while
# exposing its controls (measurement mode, temperature range,
# mirror/flip) and live readings (FPA/AUX temps) as an
# interactive channel in the IDE.

import csi
from protocol import CBORChannel
import protocol

csi0 = csi.CSI()
csi0.reset()
csi0.pixformat(csi.RGB565)
csi0.framesize(csi.QQVGA)

# Enable measurement mode.
csi0.ioctl(csi.IOCTL_LEPTON_SET_MODE, True)
csi0.ioctl(csi.IOCTL_LEPTON_SET_RANGE, 20.0, 40.0)


def on_read(ch):
    """Read live sensor state into the channel."""
    ch["FPA Temp"] = round(csi0.ioctl(csi.IOCTL_LEPTON_GET_FPA_TEMP), 1)
    ch["AUX Temp"] = round(csi0.ioctl(csi.IOCTL_LEPTON_GET_AUX_TEMP), 1)
    ch["Measurement"], ch["High Temp"] = csi0.ioctl(csi.IOCTL_LEPTON_GET_MODE)
    t_min, t_max = csi0.ioctl(csi.IOCTL_LEPTON_GET_RANGE)
    r_max = 600.0 if ch["High Temp"] else 140.0
    ch["Temp Min"] = (-10.0, r_max, round(t_min, 1))
    ch["Temp Max"] = (-10.0, r_max, round(t_max, 1))


def on_write(ch, name, value):
    """Apply control updates from the IDE."""
    if name == "Measurement":
        mode = csi0.ioctl(csi.IOCTL_LEPTON_GET_MODE)
        csi0.ioctl(csi.IOCTL_LEPTON_SET_MODE, value, mode[1])
    elif name == "High Temp":
        mode = csi0.ioctl(csi.IOCTL_LEPTON_GET_MODE)
        csi0.ioctl(csi.IOCTL_LEPTON_SET_MODE, mode[0], value)
    elif name == "Temp Min":
        _, cur_max = csi0.ioctl(csi.IOCTL_LEPTON_GET_RANGE)
        csi0.ioctl(csi.IOCTL_LEPTON_SET_RANGE, value, cur_max)
    elif name == "Temp Max":
        cur_min, _ = csi0.ioctl(csi.IOCTL_LEPTON_GET_RANGE)
        csi0.ioctl(csi.IOCTL_LEPTON_SET_RANGE, cur_min, value)
    elif name == "H-Mirror":
        csi0.hmirror(value)
    elif name == "V-Flip":
        csi0.vflip(value)


# Build the controls channel.
ch = CBORChannel(on_read=on_read, on_write=on_write)
ch.add("FPA Temp", type="label", unit="Cel")
ch.add("AUX Temp", type="label", unit="Cel")
ch.add("Measurement", type="toggle")
ch.add("High Temp", type="toggle")
ch.add("Temp Min", type="slider", min=-10.0, max=140.0, step=5.0, unit="Cel")
ch.add("Temp Max", type="slider", min=-10.0, max=140.0, step=5.0, unit="Cel")
ch.add("H-Mirror", type="toggle")
ch.add("V-Flip", type="toggle")
protocol.register(name="lepton", backend=ch)

while True:
    img = csi0.snapshot()
