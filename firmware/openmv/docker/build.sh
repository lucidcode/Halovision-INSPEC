#!/bin/bash
set -e -x

BUILD_DIR=/workspace/build/${TARGET}

# Update submodules.
git submodule update --init --depth=1

# Run the per-target clean separately (build-dev.sh skips this for
# incremental builds), then delegate to the shared build sequence.
make -j$(nproc) BUILD=${BUILD_DIR} clean
BUILD_OPTS="BUILD=${BUILD_DIR}" source "$(dirname "$0")/build-common.sh"

# Fix permissions.
chown -R ${HOST_UID:-1000}:${HOST_GID:-1000} /workspace/build
