#!/bin/bash
# Local development build script.
#
# Used by `make build-firmware-dev`. Differs from build.sh:
#   - No `make clean` before building, so rebuilds are incremental.
#   - PATH is re-derived from runtime $SDK_DIR (the Dockerfile bakes a
#     /workspace/sdk PATH at image build time which doesn't apply when the
#     SDK is mounted at its host path).
#   - Container runs as the host user (set via docker --user in the
#     Makefile), so files written end up host-owned and no chown is needed.
#   - Submodule init scopes protocol.file.allow=always to the single
#     command (worktree alternates use file://; CVE-2022-39253).
#
# The shared firmware build sequence lives in build-common.sh.
set -e -x

if [ -z "${SDK_DIR}" ]; then
    echo "SDK_DIR is not set" >&2
    exit 1
fi
if [ -z "${TARGET}" ]; then
    echo "TARGET is not set" >&2
    exit 1
fi

export PATH="${SDK_DIR}/gcc/bin:${SDK_DIR}/llvm/bin:${SDK_DIR}/make:${SDK_DIR}/cmake/bin:${SDK_DIR}/python/bin:${SDK_DIR}/stcubeprog/bin:${PATH}"

git -c protocol.file.allow=always submodule update --init --depth=1
source "$(dirname "$0")/build-common.sh"
