#!/bin/sh
BASE_DIR="$(dirname "${BASH_SOURCE[0]}")"
CFG_PATH="${BASE_DIR}/uncrustify.cfg"
SDK_VERSION="$(cat "${BASE_DIR}/../SDK_VERSION")"
SDK_DIR="${HOME}/openmv-sdk-${SDK_VERSION}"

if [ $# -lt 1 ]; then
    echo "usage `basename $0` file.(h/c)"
    exit 1
fi

for file in "$@"; do
    ${SDK_DIR}/bin/uncrustify -c ${CFG_PATH} --no-backup $file
done
