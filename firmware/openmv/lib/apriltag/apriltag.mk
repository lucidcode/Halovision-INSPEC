# SPDX-License-Identifier: MIT
#
# Copyright (C) 2025 OpenMV, LLC.
#
# AprilTag v3.4.5 Makefile

APRILTAG_DIR = lib/apriltag

APRILTAG_SRC_C += \
    apriltag.c \
    apriltag_quad_thresh.c \
    tag16h5.c \
    tag25h9.c \
    tag36h10.c \
    tag36h11.c \
    tagCircle21h7.c \
    tagCircle49h12.c \
    tagCustom48h12.c \
    tagStandard41h12.c \
    tagStandard52h13.c \
    common/zarray.c \
    common/matd.c \
    common/homography.c \
    common/g2d.c \
    common/svd22.c \
    common/unionfind.c \
    common/zmaxheap.c \
    common/image_u8.c \
    common/image_u8_parallel.c \
    common/workerpool.c \

CFLAGS += -I$(TOP_DIR)/$(APRILTAG_DIR) -I$(TOP_DIR)/$(APRILTAG_DIR)/common

$(BUILD)/$(APRILTAG_DIR)/%.o: override CFLAGS += -DAPRILTAG_HAVE_CONFIG

# Suppress warnings in upstream code.
$(BUILD)/$(APRILTAG_DIR)/%.o: override CFLAGS += \
    -Wno-unused-variable \
    -Wno-unused-but-set-variable \
    -Wno-sign-compare \
    -Wno-missing-braces \
    -Wno-unused-function \
    -Wno-format \
    -Wno-double-promotion \

OMV_FIRM_OBJ += $(addprefix $(BUILD)/$(APRILTAG_DIR)/, $(APRILTAG_SRC_C:.c=.o))
