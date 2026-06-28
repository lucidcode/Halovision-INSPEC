# This work is licensed under the MIT license.
# Copyright (c) 2026 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# This is an extension to the protocol module. Freeze this module
# in the board's manifest and CBORChannel will be importable from
# protocol directly.

import cbor2
from uprotocol import *  # noqa: F401


# SenML-compatible CBOR integer keys.
_N = 0        # name
_U = 1        # unit
_V = 2        # numeric value
_VD = 8       # data value

# 2D data extension keys.
_W = -20      # width
_H = -21      # height
_MIN = -23    # data min
_MAX = -24    # data max

# Widget keys.
_W_TYPE = -30  # widget type
_W_MIN = -31   # slider min
_W_MAX = -32   # slider max
_W_STEP = -33  # slider step
_W_OPTS = -34  # select options


class CBORChannel:
    """A protocol channel backend that serializes named fields to CBOR.

    Supports display widgets (label, depth) and interactive controls
    (toggle, slider, select) with on_read/on_write callbacks.
    Slider values can be set as (min, max, value) tuples.

    Usage::

        from protocol import CBORChannel
        import protocol

        def on_read(ch):
            ch["temp"] = read_temp()

        def on_write(ch, name, value):
            if name == "mirror":
                set_mirror(value)

        ch = CBORChannel(on_read=on_read, on_write=on_write)
        ch.add("temp", type="label", unit="Cel")
        ch.add("mirror", type="toggle")
        protocol.register(name="sensors", backend=ch)
    """

    def __init__(self, on_read=None, on_write=None):
        self._fields = []
        self._index = {}
        self._buf = b""
        self._on_read = on_read
        self._on_write = on_write
        self._dirty = False

    def add(self, name, type, value=None, unit=None,
            min=None, max=None, step=None, options=None,
            width=None, height=None):
        """Add a named field to the channel.

        Args:
            name: Display name (must be unique within this channel).
            type: Widget type - "label", "toggle", "slider", "select",
                  or "depth".
            value: Initial value. Defaults depend on type.
            unit: Unit string for label/slider (e.g. "Cel", "%RH").
            min: Minimum value (slider range or depth range).
            max: Maximum value (slider range or depth range).
            step: Step size (slider).
            options: List of option strings (select).
            width: Pixel width (depth).
            height: Pixel height (depth).
        """
        field = {_N: name, _W_TYPE: type}
        if type == "label":
            field[_V] = value if value is not None else 0
            if unit:
                field[_U] = unit
        elif type == "toggle":
            field[_V] = value if value is not None else False
        elif type == "slider":
            field[_V] = value if value is not None else (min if min is not None else 0)
            field[_W_MIN] = min if min is not None else 0
            field[_W_MAX] = max if max is not None else 100
            field[_W_STEP] = step if step is not None else 1
            if unit:
                field[_U] = unit
        elif type == "select":
            field[_V] = value if value is not None else ""
            field[_W_OPTS] = options if options is not None else []
        elif type == "depth":
            field[_VD] = b""
            field[_W] = width
            field[_H] = height
            field[_MIN] = min if min is not None else 0
            field[_MAX] = max if max is not None else 1000
        self._index[name] = len(self._fields)
        self._fields.append(field)
        self._serialize()

    def __getitem__(self, name):
        f = self._fields[self._index[name]]
        return f.get(_VD, f.get(_V))

    def __setitem__(self, name, value):
        f = self._fields[self._index[name]]
        if _VD in f:
            f[_VD] = value
        elif isinstance(value, tuple):
            if f[_W_TYPE] == "slider":
                f[_W_MIN] = value[0]
                f[_W_MAX] = value[1]
                f[_V] = value[2]
            else:
                f[_V] = list(value)
        else:
            f[_V] = value
        self._serialize()

    def _serialize(self):
        self._buf = cbor2.dumps(self._fields)
        self._dirty = True

    # -- Protocol backend interface --

    def poll(self):
        if self._on_read is not None:
            return len(self._fields) > 0
        return self._dirty

    def size(self):
        if self._on_read:
            self._on_read(self)
        self._dirty = False
        return len(self._buf)

    def read(self, offset, size):
        end = min(offset + size, len(self._buf))
        return bytes(self._buf[offset:end])

    def write(self, offset, data):
        updates = cbor2.loads(bytes(data))
        for rec in updates:
            name = rec.get(_N)
            value = rec.get(_V)
            if name is None or value is None:
                continue
            idx = self._index.get(name)
            if idx is None:
                continue
            self._fields[idx][_V] = value
            if self._on_write:
                self._on_write(self, name, value)
        return len(data)
