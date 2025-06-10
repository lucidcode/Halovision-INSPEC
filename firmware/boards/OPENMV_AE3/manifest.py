include("$(MPY_DIR)/extmod/asyncio")

# Drivers
require("lsm6dsox")
#freeze ("$(OMV_LIB_DIR)/", "modbus.py")
#freeze ("$(OMV_LIB_DIR)/", "pid.py")
#freeze ("$(OMV_LIB_DIR)/", "bno055.py")
#freeze ("$(OMV_LIB_DIR)/", "ssd1306.py")
#freeze ("$(OMV_LIB_DIR)/", "tb6612.py")
freeze ("$(OMV_LIB_DIR)/", "vl53l1x.py")
freeze ("$(OMV_LIB_DIR)/", "machine.py")
#freeze ("$(OMV_LIB_DIR)/", "display.py")
freeze ("$(OMV_LIB_DIR)/", "openamp.py")
freeze ("$(OMV_LIB_DIR)/ml")
freeze ("$(OMV_LIB_DIR)/", "romfs.py")

# Networking
require("ssl")
require("ntptime")
require("webrepl")
#freeze ("$(OMV_LIB_DIR)/", "rpc.py")
freeze ("$(OMV_LIB_DIR)/", "rtsp.py")
freeze ("$(OMV_LIB_DIR)/", "mqtt.py")
freeze ("$(OMV_LIB_DIR)/", "requests.py")

# Utils
require("time")
require("logging")
require("collections-defaultdict")
require("types")

# Bluetooth
require("aioble")

# Custom boot script.
freeze("$(OMV_LIB_DIR)/alif/$(MCU_CORE)", "_boot.py")
