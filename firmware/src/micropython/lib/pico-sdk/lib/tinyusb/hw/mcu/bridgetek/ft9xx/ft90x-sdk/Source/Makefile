#
#    @file
#
#    @brief
#    Main makefile for the FT900 HAL released along with FT90x toolchain package
#
#
# ============================================================================
# History
# =======
#
# Copyright (C) Bridgetek Pte Ltd
# ============================================================================
#
# This source code ("the Software") is provided by Bridgetek Pte Ltd
# ("Bridgetek") subject to the licence terms set out
# http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
# You must read the Licence Terms before downloading or using the Software.
# By installing or using the Software you agree to the Licence Terms. If you
# do not agree to the Licence Terms then do not download or use the Software.
#
# Without prejudice to the Licence Terms, here is a summary of some of the key
# terms of the Licence Terms (and in the event of any conflict between this
# summary and the Licence Terms then the text of the Licence Terms will
# prevail).
#
# The Software is provided "as is".
# There are no warranties (or similar) in relation to the quality of the
# Software. You use it at your own risk.
# The Software should not be used in, or for, any medical device, system or
# appliance. There are exclusions of Bridgetek liability for certain types of loss
# such as: special loss or damage; incidental loss or damage; indirect or
# consequential loss or damage; loss of income; loss of business; loss of
# profits; loss of revenue; loss of contracts; business interruption; loss of
# the use of money or anticipated savings; loss of information; loss of
# opportunity; loss of goodwill or reputation; and/or loss of, damage to or
# corruption of data.
# There is a monetary cap on Bridgetek's liability.
# The Software may have subsequently been amended by another user and then
# distributed by that other user ("Adapted Software").  If so that user may
# have additional licence terms that apply to those amendments. However, Bridgetek
# has no liability in relation to those amendments.
# ============================================================================

.PHONY := clean all 

# Determine what GCC version to use
ifndef PREFIX
PREFIX := ft32-elf-
endif

CC := $(PREFIX)gcc
LD := $(PREFIX)ld
AS := $(PREFIX)as
AR := $(PREFIX)ar
MKDIR := gmkdir -p

INCLUDE_DIR := ./include

# Set up flags and a destination directory....
CFLAGS += -c -I$(INCLUDE_DIR) 
CFLAGS += -fmessage-length=0 -ffunction-sections
CFLAGS += -Wall -Wextra
#extension for named address spaces is only supported by gcc for plain C only, not for C99
CFLAGS += -std=gnu99 #-std=c99

ifeq ($(CONFIG), Release)
# Release settings
CFLAGS += -Os
#CFLAGS += -ffunction-sections
#CFLAGS += -Wall -Wextra
LDFLAGS +=
DESTDIR := Release/$(TARGET)

else
ifeq ($(CONFIG), Debug)
# Debug settings
CFLAGS += -g3 
CFLAGS += -DDEBUG
CFLAGS += -Og
LDFLAGS +=
DESTDIR := Debug/$(TARGET)

else
$(error Please choose CONFIG=Debug or CONFIG=Release)
endif # ifeq ($(CONFIG), Debug)
endif # ifeq ($(CONFIG), Release)

ifeq ($(TARGET), ft900)
CFLAGS += -D__FT900__

SRCFILES := src/adc.c \
        src/cam.c \
        src/can.c \
        src/dac.c \
        src/delay.c \
        src/ethernet.c \
        src/gpio.c \
        src/i2cm.c \
        src/i2cs.c \
        src/i2s.c \
        src/interrupt.c \
        src/memctl.c \
        src/pwm.c \
        src/pwm_pcm.c \
        src/rtc.c \
        src/sdhost.c \
        src/spi.c \
        src/sys.c \
        src/timers.c \
        src/usbd.c \
        src/usbdx.c \
        src/usbd_hbw.c\
        src/usbd_dfu.c \
        src/usbd_rndis.c \
        src/usbd_startup_dfu.c \
        src/usbh.c \
        src/usbhx.c \
        src/usbh_boms.c \
        src/usbh_cdcacm.c \
        src/usbh_hid.c \
        src/usbh_aoa.c \
        src/uart_simple.c \
        src/wdt.c \
        src/dlog.c

else
ifeq ($(TARGET), ft930)
CFLAGS += -D__FT930__ -mcompress -mft32b

SRCFILES := src/adc.c \
		src/dac.c \
		src/delay.c \
		src/gpio.c \
		src/i2cm.c \
		src/i2cs.c \
		src/interrupt.c \
		src/memctl.c \
		src/pwm.c \
		src/pwm_pcm.c \
		src/rtc.c \
		src/sdhost.c \
		src/spi.c \
		src/sys.c \
		src/timers.c \
		src/usbd.c \
		src/usbdx.c \
		src/usbd_dfu.c \
		src/usbd_rndis.c \
		src/usbd_startup_dfu.c \
		src/uart_simple.c \
		src/wdt.c \
		src/dlog.c 

else
$(error Please choose TARGET=ft900 or TARGET=ft930)
endif # ifeq ($(TARGET), ft900)
endif # ifeq ($(TARGET), ft930)

OBJFILES := $(addprefix $(DESTDIR)/,$(notdir $(SRCFILES:.c=.o)))



all: $(DESTDIR)/lib$(TARGET).a

clean:
	-$(RM) $(OBJFILES)
	-$(RM) -r $(DESTDIR)


$(DESTDIR)/lib$(TARGET).a: $(OBJFILES) | $(DESTDIR) ; $(AR) rv $@ $^

$(DESTDIR)/%.o:        src/%.c     | $(DESTDIR) ; $(CC) $(CFLAGS) $^ -o $@

$(DESTDIR):                                     ; $(MKDIR) $@
