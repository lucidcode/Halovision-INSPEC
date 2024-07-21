# Set startup and system files for CMSIS Makefile.
SYSTEM  ?= st/system_stm32fxxx
STARTUP ?= st/startup_$(shell echo $(MCU) | tr '[:upper:]' '[:lower:]')

LDSCRIPT  ?= stm32fxxx

# Compiler Flags
CFLAGS += -std=gnu99 \
          -Wall \
          -Werror \
          -Warray-bounds \
          -nostartfiles \
          -fdata-sections \
          -ffunction-sections \
          -fno-inline-small-functions \
          -fsingle-precision-constant \
          -Wdouble-promotion \
          -mthumb \
          -mcpu=$(CPU) \
          -mtune=$(CPU) \
          -mfpu=$(FPU) \
          -mfloat-abi=hard \
          -D$(CFLAGS_MCU)

CFLAGS += -D$(MCU) \
          -D$(TARGET) \
          -DARM_NN_TRUNCATE \
          -D__FPU_PRESENT=1 \
          -D__VFP_FP__ \
          -DUSE_DEVICE_MODE \
          -DHSE_VALUE=$(OMV_HSE_VALUE)\
          -DVECT_TAB_OFFSET=$(VECT_TAB_OFFSET) \
          -DMAIN_APP_ADDR=$(MAIN_APP_ADDR) \
          -DSTM32_HAL_H=$(HAL_INC) \
          -DCMSIS_MCU_H=$(CMSIS_MCU_H) \
          -DUSE_FULL_LL_DRIVER \
          $(OMV_BOARD_EXTRA_CFLAGS)

# Linker Flags
LDFLAGS = -mthumb \
          -mcpu=$(CPU) \
          -mfpu=$(FPU) \
          -mfloat-abi=hard \
          -mabi=aapcs-linux \
          -Wl,--print-memory-usage \
          -Wl,--gc-sections \
          -Wl,-T$(BUILD)/$(LDSCRIPT).lds \
          -Wl,-Map=$(BUILD)/$(FIRMWARE).map

HAL_CFLAGS += -I$(TOP_DIR)/$(CMSIS_DIR)/include/
HAL_CFLAGS += -I$(TOP_DIR)/$(CMSIS_DIR)/include/st
HAL_CFLAGS += -I$(TOP_DIR)/$(HAL_DIR)/include/
HAL_CFLAGS += -I$(TOP_DIR)/$(HAL_DIR)/include/Legacy/

MPY_CFLAGS += -I$(MP_BOARD_CONFIG_DIR)
MPY_CFLAGS += -I$(BUILD)/$(MICROPY_DIR)/
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/py/
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/lib/oofatfs
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/lib/lwip/src/include/
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/lib/mbedtls/include
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/ports/stm32/
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/ports/stm32/usbdev/core/inc/
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/ports/stm32/usbdev/class/inc/
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/ports/stm32/lwip_inc/
MPY_CFLAGS += -I$(TOP_DIR)/$(MICROPY_DIR)/shared/runtime/
MPY_CFLAGS += -DMICROPY_PY_SSL=1 \
              -DMICROPY_SSL_MBEDTLS=1 \
              -DMICROPY_STREAMS_POSIX_API=1 \
              -DMICROPY_VFS_FAT=1

MICROPY_ARGS += STM32LIB_CMSIS_DIR=$(TOP_DIR)/$(CMSIS_DIR) \
                STM32LIB_HAL_DIR=$(TOP_DIR)/$(HAL_DIR) \
                MICROPY_PY_SSL=1 \
                MICROPY_SSL_MBEDTLS=1 \
                MICROPY_PY_BTREE=1\
                MICROPY_PY_OPENAMP=$(MICROPY_PY_OPENAMP)\
                MICROPY_PY_OPENAMP_REMOTEPROC=$(MICROPY_PY_OPENAMP_REMOTEPROC)

OMV_CFLAGS += -I$(OMV_BOARD_CONFIG_DIR)
OMV_CFLAGS += -I$(TOP_DIR)/$(OMV_DIR)/
OMV_CFLAGS += -I$(TOP_DIR)/$(OMV_DIR)/alloc/
OMV_CFLAGS += -I$(TOP_DIR)/$(OMV_DIR)/common/
OMV_CFLAGS += -I$(TOP_DIR)/$(OMV_DIR)/imlib/
OMV_CFLAGS += -I$(TOP_DIR)/$(OMV_DIR)/modules/
OMV_CFLAGS += -I$(TOP_DIR)/$(OMV_DIR)/sensors/
OMV_CFLAGS += -I$(TOP_DIR)/$(OMV_DIR)/templates/
OMV_CFLAGS += -I$(TOP_DIR)/$(OMV_DIR)/ports/$(PORT)/
OMV_CFLAGS += -I$(TOP_DIR)/$(OMV_DIR)/ports/$(PORT)/modules/

OMV_CFLAGS += -I$(TOP_DIR)/$(LEPTON_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(LSM6DS3_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(LSM6DSOX_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(VL53L5CX_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(WINC1500_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(MLX90621_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(MLX90640_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(MLX90641_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(PIXART_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(DISPLAY_DIR)/include/
OMV_CFLAGS += -I$(TOP_DIR)/$(LIBPDM_DIR)/
OMV_CFLAGS += -I$(BUILD)/$(TENSORFLOW_DIR)/

ifeq ($(OMV_ENABLE_BL), 1)
CFLAGS     += -DOMV_ENABLE_BOOTLOADER
BL_CFLAGS  := $(CFLAGS) $(HAL_CFLAGS)
BL_CFLAGS  += -I$(OMV_BOARD_CONFIG_DIR)
BL_CFLAGS  += -I$(TOP_DIR)/$(BOOTLDR_DIR)/include/
# Linker Flags
BL_LDFLAGS = -mcpu=$(CPU) \
             -mabi=aapcs-linux \
             -mthumb \
             -mfpu=$(FPU) \
             -mfloat-abi=hard \
             -Wl,--gc-sections \
             -Wl,-T$(BUILD)/$(BOOTLDR_DIR)/stm32fxxx.lds
endif

ifeq ($(OMV_ENABLE_UVC), 1)
UVC_CFLAGS := $(CFLAGS) $(HAL_CFLAGS)
UVC_CFLAGS += -I$(OMV_BOARD_CONFIG_DIR)
UVC_CFLAGS += -I$(TOP_DIR)/$(UVC_DIR)/include/
UVC_CFLAGS += $(OMV_CFLAGS) $(MPY_CFLAGS)
# Linker Flags
UVC_LDFLAGS = -mcpu=$(CPU) \
              -mabi=aapcs-linux \
              -mthumb \
              -mfpu=$(FPU) \
              -mfloat-abi=hard\
              -Wl,--gc-sections \
              -Wl,-T$(BUILD)/$(UVC_DIR)/stm32fxxx.lds
endif

CFLAGS += $(HAL_CFLAGS) $(MPY_CFLAGS) $(OMV_CFLAGS)

#------------- Libraries ----------------#
ifeq ($(MICROPY_PY_AUDIO), 1)
LIBS += $(TOP_DIR)/$(LIBPDM_DIR)/libPDMFilter_CM7_GCC_wc32.a
endif
LIBS += $(TOP_DIR)/$(TENSORFLOW_DIR)/libtflm/lib/libtflm-$(CPU)+fp-release.a

#------------- Firmware Objects ----------------#
FIRM_OBJ += $(wildcard $(BUILD)/$(CMSIS_DIR)/src/dsp/*/*.o)

FIRM_OBJ += $(wildcard $(BUILD)/$(HAL_DIR)/src/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(LEPTON_DIR)/src/*.o)
ifeq ($(MICROPY_PY_IMU), 1)
FIRM_OBJ += $(wildcard $(BUILD)/$(LSM6DS3_DIR)/src/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(LSM6DSOX_DIR)/src/*.o)
endif
ifeq ($(MICROPY_PY_WINC1500), 1)
FIRM_OBJ += $(wildcard $(BUILD)/$(WINC1500_DIR)/src/*.o)
endif
FIRM_OBJ += $(wildcard $(BUILD)/$(MLX90621_DIR)/src/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(MLX90640_DIR)/src/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(MLX90641_DIR)/src/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(VL53L5CX_DIR)/src/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(PIXART_DIR)/src/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(DISPLAY_DIR)/src/*.o)

#------------- OpenMV Objects ----------------#
FIRM_OBJ += $(addprefix $(BUILD)/$(CMSIS_DIR)/src/, \
	$(STARTUP).o                \
	$(SYSTEM).o                 \
	)

FIRM_OBJ += $(addprefix $(BUILD)/$(OMV_DIR)/alloc/, \
	xalloc.o                    \
	fb_alloc.o                  \
	umm_malloc.o                \
	dma_alloc.o                 \
	unaligned_memcpy.o          \
   )

FIRM_OBJ += $(addprefix $(BUILD)/$(OMV_DIR)/common/, \
	array.o                     \
	ini.o                       \
	ringbuf.o                   \
	trace.o                     \
	mutex.o                     \
	vospi.o                     \
	pendsv.o                    \
	usbdbg.o                    \
	file_utils.o                \
	mp_utils.o                  \
	sensor_utils.o              \
   )

FIRM_OBJ += $(addprefix $(BUILD)/$(OMV_DIR)/sensors/,   \
	ov2640.o                    \
	ov5640.o                    \
	ov7670.o                    \
	ov7690.o                    \
	ov7725.o                    \
	ov9650.o                    \
	mt9v0xx.o                   \
	mt9m114.o                   \
	lepton.o                    \
	hm01b0.o                    \
	hm0360.o                    \
	gc2145.o                    \
	pag7920.o                   \
	paj6100.o                   \
	frogeye2020.o               \
   )

FIRM_OBJ += $(addprefix $(BUILD)/$(OMV_DIR)/imlib/, \
	agast.o                     \
	apriltag.o                  \
	bayer.o                     \
	binary.o                    \
	blob.o                      \
	bmp.o                       \
	clahe.o                     \
	collections.o               \
	dmtx.o                      \
	draw.o                      \
	edge.o                      \
	eye.o                       \
	fast.o                      \
	fft.o                       \
	filter.o                    \
	fmath.o                     \
	font.o                      \
	framebuffer.o               \
	fsort.o                     \
	gif.o                       \
	haar.o                      \
	hog.o                       \
	hough.o                     \
	imlib.o                     \
	integral.o                  \
	integral_mw.o               \
	isp.o                       \
	jpegd.o                     \
	jpege.o                     \
	lodepng.o                   \
	png.o                       \
	kmeans.o                    \
	lab_tab.o                   \
	lbp.o                       \
	line.o                      \
	lsd.o                       \
	mathop.o                    \
	mjpeg.o                     \
	orb.o                       \
	phasecorrelation.o          \
	point.o                     \
	ppm.o                       \
	qrcode.o                    \
	qsort.o                     \
	rainbow_tab.o               \
	rectangle.o                 \
	selective_search.o          \
	sincos_tab.o                \
	stats.o                     \
	stereo.o                    \
	template.o                  \
	xyz_tab.o                   \
	yuv.o                       \
	zbar.o                      \
   )

FIRM_OBJ += $(wildcard $(BUILD)/$(TENSORFLOW_DIR)/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(OMV_DIR)/ports/$(PORT)/*.o)

#------------- MicroPy Objects -------------------#
FIRM_OBJ += $(wildcard $(BUILD)/$(MICROPY_DIR)/modules/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(MICROPY_DIR)/ports/$(PORT)/modules/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(MICROPY_DIR)/py/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(MICROPY_DIR)/boards/$(TARGET)/*.o)

FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/,\
	stm32_it.o              \
	usbd_conf.o             \
	usbd_desc.o             \
	usbd_cdc_interface.o    \
	usbd_hid_interface.o    \
	usbd_msc_interface.o    \
	bufhelper.o             \
	usb.o                   \
	usrsw.o                 \
	eth.o                   \
	eth_phy.o               \
	help.o                  \
	flash.o                 \
	flashbdev.o             \
	spibdev.o               \
	storage.o               \
	rtc.o                   \
	irq.o                   \
	adc.o                   \
	dac.o                   \
	dma.o                   \
	uart.o                  \
	systick.o               \
	powerctrl.o             \
	i2c.o                   \
	pyb_i2c.o               \
	spi.o                   \
	qspi.o                  \
	pyb_spi.o               \
	can.o                   \
	fdcan.o                 \
	pyb_can.o               \
	pin.o                   \
	pin_defs_stm32.o        \
	pin_named_pins.o        \
	pins_$(TARGET).o        \
	timer.o                 \
	servo.o                 \
	rng.o                   \
	led.o                   \
	mphalport.o             \
	sdcard.o                \
	sdram.o                 \
	fatfs_port.o            \
	extint.o                \
	modpyb.o                \
	modstm.o                \
	network_lan.o           \
	machine_i2c.o           \
	machine_spi.o           \
	machine_bitstream.o     \
	pybthread.o             \
	mpthreadport.o          \
	posix_helpers.o         \
	mbedtls/mbedtls_port.o  \
	frozen_content.o        \
	)

#------------- MicroPy Objects ----------------#
FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/shared/,\
	libc/printf.o               \
	libc/string0.o              \
	libc/abort_.o               \
	runtime/mpirq.o             \
	runtime/pyexec.o            \
	runtime/interrupt_char.o    \
	runtime/sys_stdio_mphal.o   \
	runtime/gchelper_thumb2.o   \
	runtime/gchelper_native.o   \
	runtime/stdout_helpers.o    \
	runtime/softtimer.o         \
	netutils/*.o                \
	timeutils/timeutils.o       \
	readline/readline.o         \
	)

FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/lib/,\
	berkeley-db-1.xx/btree/*.o  \
	berkeley-db-1.xx/mpool/*.o  \
	)

#------------- mbedtls -------------------#
FIRM_OBJ += $(wildcard $(BUILD)/$(MICROPY_DIR)/lib/mbedtls/library/*.o)
FIRM_OBJ += $(wildcard $(BUILD)/$(MICROPY_DIR)/lib/mbedtls_errors/*.o)

FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/usbdev/, \
	core/src/usbd_core.o                \
	core/src/usbd_ctlreq.o              \
	core/src/usbd_ioreq.o               \
	class/src/usbd_cdc_msc_hid.o        \
	class/src/usbd_msc_bot.o            \
	class/src/usbd_msc_scsi.o           \
	)

FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/extmod/,\
	machine_adc.o \
	machine_adc_block.o \
	machine_bitstream.o \
	machine_i2c.o \
	machine_i2s.o \
	machine_mem.o \
	machine_pinbase.o \
	machine_pulse.o \
	machine_pwm.o \
	machine_signal.o \
	machine_spi.o \
	machine_timer.o \
	machine_uart.o \
	machine_usb_device.o \
	machine_wdt.o \
	modasyncio.o \
	modbinascii.o \
	modbtree.o \
	modcryptolib.o \
	moddeflate.o \
	modframebuf.o \
	modhashlib.o \
	modheapq.o \
	modjson.o \
	modmachine.o \
	modnetwork.o \
	modonewire.o \
	modopenamp.o \
	modopenamp_remoteproc.o \
	modopenamp_remoteproc_store.o \
	modos.o \
	modplatform.o\
	modrandom.o \
	modre.o \
	modselect.o \
	modsocket.o \
	modtls_axtls.o \
	modtls_mbedtls.o \
	modtime.o \
	moductypes.o \
	modvfs.o \
	network_esp_hosted.o \
	network_ninaw10.o \
	network_wiznet5k.o \
	os_dupterm.o \
	vfs.o \
	vfs_blockdev.o \
	vfs_fat.o \
	vfs_fat_diskio.o \
	vfs_fat_file.o \
	vfs_lfs.o \
	vfs_posix.o \
	vfs_posix_file.o \
	vfs_reader.o \
	virtpin.o \
	)

FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/lib/oofatfs/,\
	ff.o                \
	ffunicode.o         \
	)

FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/drivers/,\
	bus/softspi.o       \
	dht/dht.o           \
	memory/spiflash.o   \
	)

ifeq ($(MICROPY_PY_ULAB), 1)
FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/modules/ulab/code/,\
	ndarray.o                       \
	ndarray_operators.o             \
	ndarray_properties.o            \
	numpy/approx.o                  \
	numpy/bitwise.o                 \
	numpy/carray/carray.o           \
	numpy/carray/carray_tools.o     \
	numpy/compare.o                 \
	numpy/create.o                  \
	numpy/fft/fft.o                 \
	numpy/fft/fft_tools.o           \
	numpy/filter.o                  \
	numpy/io/io.o                   \
	numpy/linalg/linalg.o           \
	numpy/linalg/linalg_tools.o     \
	numpy/ndarray/ndarray_iter.o    \
	numpy/numerical.o               \
	numpy/numpy.o                   \
	numpy/poly.o                    \
	numpy/random/random.o           \
	numpy/stats.o                   \
	numpy/transform.o               \
	numpy/vector.o                  \
	scipy/linalg/linalg.o           \
	scipy/optimize/optimize.o       \
	scipy/scipy.o                   \
	scipy/signal/signal.o           \
	scipy/special/special.o         \
	ulab.o                          \
	ulab_tools.o                    \
	user/user.o                     \
	utils/utils.o                   \
	)
endif

ifeq ($(MICROPY_PY_LWIP), 1)
FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/,\
	sdio.o                     \
	lib/lwip/src/core/*.o      \
	lib/lwip/src/core/*/*.o    \
	lib/lwip/src/netif/*.o     \
	lib/lwip/src/apps/*/*.o    \
	extmod/modlwip.o           \
	extmod/modwebsocket.o      \
	extmod/modwebrepl.o        \
	mpnetworkport.o            \
	extmod/network_lwip.o      \
	)
endif

ifeq ($(MICROPY_PY_NETWORK_CYW43), 1)
FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/,\
	lib/cyw43-driver/src/*.o    \
	extmod/network_cyw43.o      \
	)
FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/drivers/,\
	cyw43/cywbt.o               \
	)
endif

ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)
FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/lib/mynewt-nimble/,\
	ext/tinycrypt/src/*.o                   \
	nimble/host/services/gap/src/*.o        \
	nimble/host/services/gatt/src/*.o       \
	nimble/host/src/*.o                     \
	nimble/host/util/src/*.o                \
	nimble/transport/uart/src/*.o           \
	porting/nimble/src/*.o                  \
	)

FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/,\
	mpbthciport.o                               \
	mpnimbleport.o                              \
	extmod/nimble/modbluetooth_nimble.o         \
	extmod/nimble/nimble/nimble_npl_os.o        \
	extmod/nimble/hal/hal_uart.o                \
	extmod/modbluetooth.o                       \
	)
endif

ifeq ($(MICROPY_PY_OPENAMP),1)
FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/openamp/metal/,\
	device.o                        \
	dma.o                           \
	init.o                          \
	io.o                            \
	irq.o                           \
	log.o                           \
	shmem.o                         \
	softirq.o                       \
	version.o                       \
	system/micropython/condition.o  \
	system/micropython/device.o     \
	system/micropython/io.o         \
	system/micropython/irq.o        \
	system/micropython/shmem.o      \
	system/micropython/time.o       \
	)

FIRM_OBJ += $(addprefix $(BUILD)/$(MICROPY_DIR)/,\
	mpmetalport.o                                   \
	mpremoteprocport.o                              \
	lib/open-amp/lib/virtio/virtio.o                \
	lib/open-amp/lib/virtio/virtqueue.o             \
	lib/open-amp/lib/virtio_mmio/virtio_mmio_drv.o  \
	lib/open-amp/lib/rpmsg/rpmsg.o                  \
	lib/open-amp/lib/rpmsg/rpmsg_virtio.o           \
	lib/open-amp/lib/remoteproc/elf_loader.o        \
	lib/open-amp/lib/remoteproc/remoteproc.o        \
	lib/open-amp/lib/remoteproc/remoteproc_virtio.o \
	lib/open-amp/lib/remoteproc/rsc_table_parser.o  \
	)
endif

#------------- CubeAI Objects -------------------#
ifeq ($(CUBEAI), 1)
include $(TOP_DIR)/stm32cubeai/cube.mk
endif

ifeq ($(OMV_ENABLE_BL), 1)
BOOTLOADER = bootloader
# Bootloader object files
BOOT_OBJ += $(wildcard $(BUILD)/$(BOOTLDR_DIR)/src/*.o)
BOOT_OBJ += $(wildcard $(BUILD)/$(HAL_DIR)/src/*.o)
BOOT_OBJ += $(addprefix $(BUILD)/$(CMSIS_DIR)/src/,\
	$(STARTUP).o                                \
	$(SYSTEM).o                                 \
	)
endif

ifeq ($(OMV_ENABLE_UVC), 1)
UVC = uvc
# UVC object files
UVC_OBJ += $(BUILD)/$(MICROPY_DIR)/lib/libm/math.o
UVC_OBJ += $(wildcard $(BUILD)/$(UVC_DIR)/src/*.o)
UVC_OBJ += $(wildcard $(BUILD)/$(HAL_DIR)/src/*.o)
UVC_OBJ += $(addprefix $(BUILD)/$(CMSIS_DIR)/src/,\
	$(STARTUP).o                                \
	$(SYSTEM).o                                 \
	)

UVC_OBJ += $(addprefix $(BUILD)/$(OMV_DIR)/alloc/, \
	fb_alloc.o                              \
	dma_alloc.o                             \
	unaligned_memcpy.o                      \
	)

UVC_OBJ += $(addprefix $(BUILD)/$(OMV_DIR)/common/, \
	array.o                                 \
	trace.o                                 \
	mutex.o                                 \
	sensor_utils.o                          \
	vospi.o                                 \
	)

UVC_OBJ += $(addprefix $(BUILD)/$(OMV_DIR)/sensors/, \
	ov2640.o                                \
	ov5640.o                                \
	ov7690.o                                \
	ov7670.o                                \
	ov7725.o                                \
	ov9650.o                                \
	mt9v0xx.o                               \
	mt9m114.o                               \
	lepton.o                                \
	hm01b0.o                                \
	hm0360.o                                \
	gc2145.o                                \
	pag7920.o                               \
	paj6100.o                               \
	frogeye2020.o                           \
	)

UVC_OBJ += $(addprefix $(BUILD)/$(OMV_DIR)/imlib/,\
	bayer.o                                 \
	lab_tab.o                               \
	xyz_tab.o                               \
	rainbow_tab.o                           \
	jpege.o                                 \
	fmath.o                                 \
	imlib.o                                 \
	framebuffer.o                           \
	yuv.o                                   \
	)

UVC_OBJ += $(addprefix $(BUILD)/$(OMV_DIR)/ports/stm32/,\
	jpeg.o                                  \
	sensor.o                                \
	stm32fxxx_hal_msp.o                     \
	soft_i2c.o                              \
	ulpi.o                                  \
	dma_utils.o                             \
	omv_gpio.o                              \
	omv_i2c.o                               \
	omv_spi.o                               \
	)

UVC_OBJ += $(wildcard $(BUILD)/$(LEPTON_DIR)/src/*.o)
ifeq ($(MICROPY_PY_IMU), 1)
UVC_OBJ += $(wildcard $(BUILD)/$(LSM6DS3_DIR)/src/*.o)
UVC_OBJ += $(wildcard $(BUILD)/$(LSM6DSOX_DIR)/src/*.o)
endif
UVC_OBJ += $(wildcard $(BUILD)/$(MLX90621_DIR)/src/*.o)
UVC_OBJ += $(wildcard $(BUILD)/$(MLX90640_DIR)/src/*.o)
UVC_OBJ += $(wildcard $(BUILD)/$(MLX90641_DIR)/src/*.o)
UVC_OBJ += $(wildcard $(BUILD)/$(VL53L5CX_DIR)/src/*.o)
UVC_OBJ += $(wildcard $(BUILD)/$(PIXART_DIR)/src/*.o)
endif

###################################################
all: $(OPENMV)

$(BUILD):
	$(MKDIR) -p $@

$(FW_DIR):
	$(MKDIR) -p $@

FIRMWARE_OBJS: | $(BUILD) $(FW_DIR)
	$(MAKE)  -C $(CMSIS_DIR)                 BUILD=$(BUILD)/$(CMSIS_DIR)        CFLAGS="$(CFLAGS) -fno-strict-aliasing -MMD"
	$(MAKE)  -C $(HAL_DIR)                   BUILD=$(BUILD)/$(HAL_DIR)          CFLAGS="$(CFLAGS) -MMD"
	$(MAKE)  -C $(TENSORFLOW_DIR)            BUILD=$(BUILD)/$(TENSORFLOW_DIR)   CFLAGS="$(CFLAGS) -MMD" headers
	$(MAKE)  -C $(MICROPY_DIR)/ports/$(PORT) BUILD=$(BUILD)/$(MICROPY_DIR)      $(MICROPY_ARGS)
	$(MAKE)  -C $(LEPTON_DIR)                BUILD=$(BUILD)/$(LEPTON_DIR)       CFLAGS="$(CFLAGS) -MMD"
ifeq ($(MICROPY_PY_IMU), 1)
	$(MAKE)  -C $(LSM6DS3_DIR)               BUILD=$(BUILD)/$(LSM6DS3_DIR)      CFLAGS="$(CFLAGS) -MMD"
	$(MAKE)  -C $(LSM6DSOX_DIR)              BUILD=$(BUILD)/$(LSM6DSOX_DIR)     CFLAGS="$(CFLAGS) -MMD"
endif
ifeq ($(MICROPY_PY_WINC1500), 1)
	$(MAKE)  -C $(WINC1500_DIR)              BUILD=$(BUILD)/$(WINC1500_DIR)     CFLAGS="$(CFLAGS) -MMD"
endif
	$(MAKE)  -C $(MLX90621_DIR)              BUILD=$(BUILD)/$(MLX90621_DIR)     CFLAGS="$(CFLAGS) -MMD"
	$(MAKE)  -C $(MLX90640_DIR)              BUILD=$(BUILD)/$(MLX90640_DIR)     CFLAGS="$(CFLAGS) -MMD"
	$(MAKE)  -C $(MLX90641_DIR)              BUILD=$(BUILD)/$(MLX90641_DIR)     CFLAGS="$(CFLAGS) -MMD"
	$(MAKE)  -C $(VL53L5CX_DIR)              BUILD=$(BUILD)/$(VL53L5CX_DIR)     CFLAGS="$(CFLAGS) -MMD"
	$(MAKE)  -C $(PIXART_DIR)                BUILD=$(BUILD)/$(PIXART_DIR)       CFLAGS="$(CFLAGS) -MMD"
	$(MAKE)  -C $(DISPLAY_DIR)               BUILD=$(BUILD)/$(DISPLAY_DIR)      CFLAGS="$(CFLAGS) -MMD"
	$(MAKE)  -C $(TENSORFLOW_DIR)            BUILD=$(BUILD)/$(TENSORFLOW_DIR)   CFLAGS="$(CFLAGS) -MMD"
	$(MAKE)  -C $(OMV_DIR)                   BUILD=$(BUILD)/$(OMV_DIR)          CFLAGS="$(CFLAGS) -MMD"
ifeq ($(CUBEAI), 1)
	$(MAKE)  -C $(CUBEAI_DIR)                BUILD=$(BUILD)/$(CUBEAI_DIR)       CFLAGS="$(CFLAGS) -fno-strict-aliasing -MMD"
endif
ifeq ($(OMV_ENABLE_UVC), 1)
UVC_OBJS: FIRMWARE_OBJS
	$(MAKE)  -C $(UVC_DIR)                   BUILD=$(BUILD)/$(UVC_DIR)          CFLAGS="$(UVC_CFLAGS) -MMD"
endif
ifeq ($(OMV_ENABLE_BL), 1)
BOOTLOADER_OBJS: FIRMWARE_OBJS
	$(MAKE)  -C $(BOOTLDR_DIR)               BUILD=$(BUILD)/$(BOOTLDR_DIR)      CFLAGS="$(BL_CFLAGS) -MMD"
endif

# This target generates the main/app firmware image located at 0x08010000
$(FIRMWARE): FIRMWARE_OBJS
	$(CPP) -P -E  -I$(OMV_COMMON_DIR) -I$(OMV_BOARD_CONFIG_DIR) \
                    $(OMV_DIR)/ports/$(PORT)/$(LDSCRIPT).ld.S > $(BUILD)/$(LDSCRIPT).lds
	$(CC) $(LDFLAGS) $(FIRM_OBJ) -o $(FW_DIR)/$(FIRMWARE).elf $(LIBS) -lm
	$(OBJCOPY) -Obinary $(FW_DIR)/$(FIRMWARE).elf $(FW_DIR)/$(FIRMWARE).bin
	$(PYTHON) $(MKDFU) -D $(DFU_DEVICE) -b $(MAIN_APP_ADDR):$(FW_DIR)/$(FIRMWARE).bin $(FW_DIR)/$(FIRMWARE).dfu

ifeq ($(OMV_ENABLE_BL), 1)
# This target generates the bootloader.
$(BOOTLOADER): FIRMWARE_OBJS BOOTLOADER_OBJS
	$(CPP) -P -E -I$(OMV_COMMON_DIR) -I$(OMV_BOARD_CONFIG_DIR) \
                   $(BOOTLDR_DIR)/stm32fxxx.ld.S > $(BUILD)/$(BOOTLDR_DIR)/stm32fxxx.lds
	$(CC) $(BL_LDFLAGS) $(BOOT_OBJ) -o $(FW_DIR)/$(BOOTLOADER).elf
	$(OBJCOPY) -Obinary $(FW_DIR)/$(BOOTLOADER).elf $(FW_DIR)/$(BOOTLOADER).bin
	$(PYTHON) $(MKDFU) -D $(DFU_DEVICE) -b 0x08000000:$(FW_DIR)/$(BOOTLOADER).bin $(FW_DIR)/$(BOOTLOADER).dfu
endif

ifeq ($(OMV_ENABLE_UVC), 1)
# This target generates the UVC firmware.
$(UVC): FIRMWARE_OBJS UVC_OBJS
	$(CPP) -P -E -I$(OMV_COMMON_DIR) -I$(OMV_BOARD_CONFIG_DIR) \
                   $(UVC_DIR)/stm32fxxx.ld.S > $(BUILD)/$(UVC_DIR)/stm32fxxx.lds
	$(CC) $(UVC_LDFLAGS) $(UVC_OBJ) -o $(FW_DIR)/$(UVC).elf -lgcc
	$(OBJCOPY) -Obinary $(FW_DIR)/$(UVC).elf $(FW_DIR)/$(UVC).bin
	$(PYTHON) $(MKDFU) -D $(DFU_DEVICE) -b $(MAIN_APP_ADDR):$(FW_DIR)/$(UVC).bin $(FW_DIR)/$(UVC).dfu
endif

# This target generates the uvc, bootloader and firmware images.
$(OPENMV): $(BOOTLOADER) $(UVC) $(FIRMWARE)
ifeq ($(OMV_ENABLE_BL), 1)
	# Generate a contiguous firmware image for factory programming
	$(OBJCOPY) -Obinary --pad-to=$(MAIN_APP_ADDR) --gap-fill=0xFF $(FW_DIR)/$(BOOTLOADER).elf $(FW_DIR)/$(BOOTLOADER)_padded.bin
	$(CAT) $(FW_DIR)/$(BOOTLOADER)_padded.bin $(FW_DIR)/$(FIRMWARE).bin > $(FW_DIR)/$(OPENMV).bin
	$(RM) $(FW_DIR)/$(BOOTLOADER)_padded.bin
	$(SIZE) $(FW_DIR)/$(BOOTLOADER).elf
endif
	$(SIZE) $(FW_DIR)/$(FIRMWARE).elf

size:
ifeq ($(OMV_ENABLE_BL), 1)
	$(SIZE) --format=SysV $(FW_DIR)/$(BOOTLOADER).elf
endif
	$(SIZE) --format=SysV $(FW_DIR)/$(FIRMWARE).elf

# Flash the bootloader
flash_boot::
	$(PYDFU) -u $(FW_DIR)/$(BOOTLOADER).dfu

# Flash the main firmware image
flash_image::
	$(PYDFU) -u $(FW_DIR)/$(FIRMWARE).dfu

# Flash the bootloader using dfu_util
flash_boot_dfu_util::
	dfu-util -a 0 -d $(DFU_DEVICE) -D $(FW_DIR)/$(BOOTLOADER).dfu

# Flash the main firmware image using dfu_util
flash_image_dfu_util::
	dfu-util -a 0 -d $(DFU_DEVICE) -D $(FW_DIR)/$(FIRMWARE).dfu
