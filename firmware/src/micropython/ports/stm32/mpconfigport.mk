# Enable/disable extra modules

# wiznet5k module for ethernet support; valid values are:
#   0    : no Wiznet support
#   5200 : support for W5200 module
#   5500 : support for W5500 module
MICROPY_PY_WIZNET5K ?= 0

# cc3k module for wifi support
MICROPY_PY_CC3K ?= 0

MICROPY_PY_BTREE = 1

MICROPY_PY_USSL = 1

MICROPY_SSL_MBEDTLS = 1

# VFS FAT FS support
MICROPY_VFS_FAT ?= 1
