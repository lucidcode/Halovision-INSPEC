# Halovision-INSPEC

A night-vision smart camera that detects eye movements during REM sleep and triggers audio and visual alerts to promote lucidity.

![INSPEC](https://github.com/lucidcode/Halovision-INSPEC/raw/master/images/inspec.jpg?raw=true "INSPEC")

![INSPECs](https://github.com/lucidcode/Halovision-INSPEC/raw/master/images/inspecs.jpg?raw=true "INSPECs")

## Works with

- <a href="https://play.google.com/store/apps/details?id=com.lucidcode.lucidscribe">Lucid Scribe Android</a>

![Lucid Scribe Android](https://github.com/lucidcode/Halovision-INSPEC/raw/master/images/lucid_scribe_android.gif?raw=true "Lucid Scribe Android")

![Lucid Scribe Setting](https://github.com/lucidcode/Halovision-INSPEC/raw/master/images/lucid_scribe_settings.gif?raw=true "Lucid Scribe Setting")

- <a href="http://lucidcode.com/LucidScribe/">Lucid Scribe</a>

![Lucid Scribe REM sleep at 06:23](https://github.com/lucidcode/Halovision-INSPEC/raw/master/images/lucid_scribe_rem_0623.gif?raw=true "Lucid Scribe REM sleep at 06:23")

![Lucid Scribe REM sleep at 07:38](https://github.com/lucidcode/Halovision-INSPEC/raw/master/images/lucid_scribe_rem_0738.gif?raw=true "Lucid Scribe REM sleep at 07:38")

## Documentation

- [Setup](https://github.com/lucidcode/Halovision-INSPEC/wiki/Setup%E2%80%90en)

- [Settings](https://github.com/lucidcode/Halovision-INSPEC/wiki/Settings%E2%80%90en)

- [Interfaces](https://github.com/lucidcode/Halovision-INSPEC/wiki/Interfaces%E2%80%90en)

## Webhook API

### Start the webhook API server

```bash
cd api
python webhook_server.py
```

### Start the webhook demo

```bash
cd api
python webhook_demo.py
```

Refer to the [Interfaces](https://github.com/lucidcode/Halovision-INSPEC/wiki/Interfaces%E2%80%90en) documentation for API endpoints.

## Accessories

- [3D Printable Case with Flexible Tripod](https://makerworld.com/en/models/665460#profileId-592934) - simply print, assemble, insert the camera, and start using it immediately!

## Firmware build

### Install build dependencies
```bash
sudo apt-get update
sudo apt-get install git build-essential
```

### Install GNU ARM toolchain
```
TOOLCHAIN_PATH=${HOME}/cache/gcc
TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz"
mkdir -p ${TOOLCHAIN_PATH}
wget --no-check-certificate -O - ${TOOLCHAIN_URL} | tar --strip-components=1 -Jx -C ${TOOLCHAIN_PATH}
export PATH=${TOOLCHAIN_PATH}/bin:${PATH}
```

### Build the firmware
To build the firmware, run the following commands:
```bash
cd firmware
make -j$(nproc) -C lib/micropython/mpy-cross
make -j$(nproc) TARGET=LUCIDCODE_INSPEC
```
