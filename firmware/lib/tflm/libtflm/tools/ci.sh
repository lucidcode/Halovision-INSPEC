#!/bin/bash
# This file is part of the OpenMV project.
#
# Copyright (c) 2024 Ibrahim Abdelkader <iabdalkader@openmv.io>
# Copyright (c) 2024 Kwabena W. Agyeman <kwagyeman@openmv.io>
#
# This work is licensed under the MIT license, see the file LICENSE for details.

# Install ARM GCC.
########################################################################################
TFLM_REPO_PATH=tflite-micro
TOOLCHAIN_PATH=${HOME}/cache/gcc
export PATH=${TOOLCHAIN_PATH}/bin:${PATH}
TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz"

ci_install_arm_gcc() {
    mkdir -p ${TOOLCHAIN_PATH}
    wget --no-check-certificate -O - ${TOOLCHAIN_URL} | tar --strip-components=1 -Jx -C ${TOOLCHAIN_PATH}
    export PATH=${TOOLCHAIN_PATH}/bin:${PATH}
    arm-none-eabi-gcc --version
}

# Update Submodules.
########################################################################################
ci_update_submodules() {
    git submodule update --init --depth=1 --no-single-branch
}

# Build Targets.
########################################################################################
ci_build_target() {
    TARGET_NAME=${1}
    TARGET_ARCH=${2}
    TARGET_KERNEL=${3}
    TARGET_COPROC=${4}
    TARGET_ARGS="${5}"
    TARGET_BUILD=${6}

    ETHOSU_CONFIG="-DNPU_QCONFIG=0 -DNPU_REGIONCFG_0=0 -DAXI_LIMIT0_MAX_BEATS_BYTES=1 -DAXI_LIMIT0_MEM_TYPE=2 -DAXI_LIMIT1_MAX_BEATS_BYTES=1 -DAXI_LIMIT1_MEM_TYPE=2 -DAXI_LIMIT2_MAX_BEATS_BYTES=1 -DAXI_LIMIT2_MEM_TYPE=2 -DAXI_LIMIT3_MAX_BEATS_BYTES=1 -DAXI_LIMIT3_MEM_TYPE=2"

    pushd ${TFLM_REPO_PATH}
echo "make -j$(nproc) -f tensorflow/lite/micro/tools/make/Makefile \
TARGET=cortex_m_generic TARGET_ARCH=${TARGET_ARCH} CO_PROCESSOR=${TARGET_COPROC} \
${TARGET_ARGS} OPTIMIZED_KERNEL_DIR=${TARGET_KERNEL} CORE_OPTIMIZATION_LEVEL=-O2 \
KERNEL_OPTIMIZATION_LEVEL=-O2 THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=-O2 \
TARGET_TOOLCHAIN_ROOT=${TOOLCHAIN_PATH}/bin/ TARGET_TOOLCHAIN_PREFIX=arm-none-eabi- \
ETHOSU_EXTRA_FLAGS=${ETHOSU_CONFIG} BUILD_TYPE=${TARGET_BUILD} MICROLITE_LIB_NAME=libtflm.a microlite"
    
    make -j$(nproc) -f tensorflow/lite/micro/tools/make/Makefile \
         TARGET=cortex_m_generic TARGET_ARCH=${TARGET_ARCH} CO_PROCESSOR=${TARGET_COPROC} \
         ${TARGET_ARGS} OPTIMIZED_KERNEL_DIR=${TARGET_KERNEL} CORE_OPTIMIZATION_LEVEL=-O2 \
         KERNEL_OPTIMIZATION_LEVEL=-O2 THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=-O2 \
         TARGET_TOOLCHAIN_ROOT=${TOOLCHAIN_PATH}/bin/ TARGET_TOOLCHAIN_PREFIX=arm-none-eabi- \
         ETHOSU_EXTRA_FLAGS="${ETHOSU_CONFIG}" BUILD_TYPE=${TARGET_BUILD} MICROLITE_LIB_NAME=libtflm.a microlite
    popd
    mv ${TFLM_REPO_PATH}/gen/*/lib/libtflm.a libtflm-${TARGET_NAME}-${TARGET_BUILD}.a
}

# Build Headers.
########################################################################################
ci_build_headers() {
    TARGET_NAME=${1}
    TARGET_ARCH=${2}
    TARGET_KERNEL=${3}
    TARGET_COPROC=${4}
    TARGET_ARGS="${5}"
    TARGET_BUILD=${6}

    pushd ${TFLM_REPO_PATH}
    python3 tensorflow/lite/micro/tools/project_generation/create_tflm_tree.py --makefile_options="TARGET=cortex_m_generic TARGET_ARCH=${TARGET_ARCH} CO_PROCESSOR=${TARGET_COPROC} ${TARGET_ARGS} OPTIMIZED_KERNEL_DIR=${TARGET_KERNEL} TARGET_TOOLCHAIN_ROOT=${TOOLCHAIN_PATH}/bin/ TARGET_TOOLCHAIN_PREFIX=arm-none-eabi- BUILD_TYPE=${TARGET_BUILD}" headers
    popd
    mv ${TFLM_REPO_PATH}/headers ${TARGET_NAME}-headers
}
