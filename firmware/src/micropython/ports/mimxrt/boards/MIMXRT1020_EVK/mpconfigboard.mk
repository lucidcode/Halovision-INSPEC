MCU_SERIES = MIMXRT1021
MCU_VARIANT = MIMXRT1021DAG5A

MICROPY_FLOAT_IMPL = double

SRC_C += \
	hal/flexspi_nor_flash.c \

JLINK_PATH ?= /media/RT1020-EVK/
JLINK_COMMANDER_SCRIPT = $(BUILD)/script.jlink


ifdef JLINK_IP
JLINK_CONNECTION_SETTINGS = -IP $(JLINK_IP)
else
JLINK_CONNECTION_SETTINGS =
endif


deploy_jlink: $(BUILD)/firmware.hex
	$(ECHO) "ExitOnError 1" > $(JLINK_COMMANDER_SCRIPT)
	$(ECHO) "speed auto" >> $(JLINK_COMMANDER_SCRIPT)
	$(ECHO) "r" >> $(JLINK_COMMANDER_SCRIPT)
	$(ECHO) "st" >> $(JLINK_COMMANDER_SCRIPT)
	$(ECHO) "loadfile \"$(realpath $(BUILD)/firmware.hex)\"" >> $(JLINK_COMMANDER_SCRIPT)
	$(ECHO) "qc" >> $(JLINK_COMMANDER_SCRIPT)
	$(JLINK_PATH)JLinkExe -device $(MCU_VARIANT) -if SWD $(JLINK_CONNECTION_SETTINGS) -CommanderScript $(JLINK_COMMANDER_SCRIPT)

deploy: $(BUILD)/firmware.bin
	cp $< $(JLINK_PATH)