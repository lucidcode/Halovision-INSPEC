#!/bin/bash
# This file is part of the OpenMV project.
#
# Copyright (C) 2026 OpenMV, LLC.
#
# This work is licensed under the MIT license, see the file LICENSE for details.
#
# Run a single unit test (or benchmark) on an OpenMV target via mpremote.
# Supports QEMU (auto-detected tty) and FVP (socket via Docker).
# Usage: run_test.sh <test_name.py>

SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MPREMOTE="$SCRIPT_DIR/lib/micropython/tools/mpremote/mpremote.py"
UNITTEST_DIR="$SCRIPT_DIR/scripts/unittest"

PORT=""
TEST_ARG=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -p|--port)
            PORT="$2"
            shift 2
            ;;
        -*)
            echo "Unknown option $1" >&2
            exit 1
            ;;
        *)
            if [ -n "$TEST_ARG" ]; then
                echo "Error: Multiple test files specified." >&2
                exit 1
            fi
            TEST_ARG="$1"
            shift
            ;;
    esac
done

if [ -z "$TEST_ARG" ]; then
    echo "Usage: $(basename "$0") [-p|--port <port>] <path/to/test.py>" >&2
    echo "  Runs a single test on the target via mpremote" >&2
    echo "  Scripts outside scripts/unittest/tests/ are run standalone" >&2
    exit 1
fi

TEST_PATH="$(realpath "$TEST_ARG")"
TEST="$(basename "$TEST_PATH")"

if [ ! -f "$TEST_PATH" ]; then
    echo "Error: test file not found: $TEST_PATH" >&2
    exit 1
fi

TIMEOUT_CMD=()
if [ -z "$PORT" ]; then
    # Try QEMU tty first, fall back to FVP socket
    echo "Trying QEMU..."
    PORT=$(python3 -c "
import subprocess, os
who = subprocess.check_output(['who'], text=True)
sessions = {'/dev/' + p for line in who.splitlines() for p in line.split() if p.startswith('ttys')}
devs = [f'/dev/ttys{i:03d}' for i in range(100) if os.path.exists(f'/dev/ttys{i:03d}')]
candidates = [d for d in devs if d not in sessions]
if candidates:
    print(candidates[0])
")

    if [ -z "$PORT" ]; then
        echo "Trying FVP..."
        PORT="socket://localhost:5555"
        TIMEOUT_CMD=(timeout 600)
    fi
fi

echo "Using port: $PORT"

if [[ "$TEST_PATH" != "$UNITTEST_DIR/tests/"* ]]; then
    exec "${TIMEOUT_CMD[@]}" python3 "$MPREMOTE" connect "$PORT" mount "$UNITTEST_DIR" run "$TEST_PATH"
fi
exec "${TIMEOUT_CMD[@]}" python3 "$MPREMOTE" connect "$PORT" mount "$UNITTEST_DIR" run /dev/stdin << PYEOF
import os, time, gc

DATA_PATH = "/rom"
TEMP_PATH = "/remote/temp"
TEST_DIR = "/remote/tests"

test_name = "${TEST}"

if "temp" not in os.listdir("/remote"):
    os.mkdir("/remote/temp")

start_ms = time.ticks_ms()
try:
    with open("/".join((TEST_DIR, test_name))) as f:
        buf = f.read()
    exec(buf)
    ret = unittest(DATA_PATH, TEMP_PATH)
    if ret == "skip":
        result = "SKIPPED"
    elif ret is False:
        result = "FAILED"
    else:
        result = "PASSED"
except Exception as e:
    print(e)
    if "SKIPPED" in str(e):
        result = "SKIPPED"
    else:
        result = "FAILED"
        import sys
        sys.print_exception(e)

time_ms = time.ticks_diff(time.ticks_ms(), start_ms)

colors = {"PASSED": "\033[92m", "FAILED": "\033[91m", "SKIPPED": "\033[33m"}
reset = "\033[0m"
s = "Unittest (%s)" % test_name
padding = "." * (60 - len(s))
print(s + padding + colors[result] + result + reset + " (%dms)" % time_ms)
PYEOF
