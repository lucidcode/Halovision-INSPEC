# OpenMV library
add_library("openmv-lib", "$(OMV_LIB_DIR)")

# Drivers
require("onewire")
require("ds18x20")
require("dht")
require("neopixel")
freeze ("$(OMV_LIB_DIR)/", "modbus.py")
freeze ("$(OMV_LIB_DIR)/", "pid.py")
freeze ("$(OMV_LIB_DIR)/", "bno055.py")
freeze ("$(OMV_LIB_DIR)/", "ssd1306.py")
freeze ("$(OMV_LIB_DIR)/", "ssd1351.py")
freeze ("$(OMV_LIB_DIR)/", "pca9674a.py")
freeze ("$(OMV_LIB_DIR)/", "tb6612.py")
freeze ("$(OMV_LIB_DIR)/", "vl53l1x.py")
freeze ("$(OMV_LIB_DIR)/", "machine.py")
freeze ("$(OMV_LIB_DIR)/", "display.py")

# Bluetooth
require("aioble")
freeze ("$(OMV_LIB_DIR)/", "ble_advertising.py")

# Networking
require("ssl")
require("ntptime")
require("webrepl")
freeze ("$(OMV_LIB_DIR)/", "rpc.py")
freeze ("$(OMV_LIB_DIR)/", "rtsp.py")
freeze ("$(OMV_LIB_DIR)/", "mqtt.py")
freeze ("$(OMV_LIB_DIR)/", "requests.py")
require("microdot-lib")

# Utils
require("time")
require("senml")
require("logging")
freeze ("$(OMV_LIB_DIR)/", "mutex.py")

# Libraries
require("ml", library="openmv-lib")
require("protocol", library="openmv-lib")
include("$(MPY_DIR)/extmod/asyncio")

# Boot script
freeze ("$(OMV_LIB_DIR)/", "_boot.py")

# INSPEC
freeze ("$(TOP_DIR)/../../../../software", "ble.py")
freeze ("$(TOP_DIR)/../../../../software", "face.py")
freeze ("$(TOP_DIR)/../../../../software", "config.py")
freeze ("$(TOP_DIR)/../../../../software", "inspec.py")
freeze ("$(TOP_DIR)/../../../../software", "wifi.py")
freeze ("$(TOP_DIR)/../../../../software", "rem.py")
freeze ("$(TOP_DIR)/../../../../software", "nrem.py")
freeze ("$(TOP_DIR)/../../../../software", "quality.py")
freeze ("$(TOP_DIR)/../../../../software", "led.py")
freeze ("$(TOP_DIR)/../../../../software", "lsd.py")
freeze ("$(TOP_DIR)/../../../../software", "version.py")
