from .se05x import SE05X # noqa
from .iso7816 import SmartCard # noqa
from micropython import const

# Secure Object Types.
EC_KEY = const(0x01)
AES_KEY = const(0x03)
DES_KEY = const(0x04)
HMAC_KEY = const(0x05)
BINARY = const(0x06)
USERID = const(0x07)
CURVE = const(0x0B)
SIGNATURE = const(0x0C)
MAC = const(0x0D)
CIPHER = const(0x0E)

# Supported EC curves.
EC_CURVE_NIST_P192 = const(0x01)
EC_CURVE_NIST_P224 = const(0x02)
EC_CURVE_NIST_P256 = const(0x03)
EC_CURVE_NIST_P384 = const(0x04)
EC_CURVE_NIST_P521 = const(0x05)
