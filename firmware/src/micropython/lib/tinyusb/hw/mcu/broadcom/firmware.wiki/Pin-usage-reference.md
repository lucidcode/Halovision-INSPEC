Since ARM and VideoCore share the same pin space, the concurrent access on the same pins must be avoided. The [Firmware DT blob](https://github.com/raspberrypi/firmware/blob/master/extra/dt-blob.dts) already describe the pins which are used by the GPU firmware, but it's not very practical. So here is a reference table:

| Pin | RPi 1 A+ | RPi 1 B r1 | RPi 1 B r2 | RPi 1 B+ | RPi CM1 | RPi 0 | RPi 0 W | RPi 2 B | RPi 3 B | RPi CM3 |
|-----|----------|------------|------------|----------|---------|-------|---------|---------|---------|---------|
| 0   |       |       |  CAM  |       |       |       |       |       |       |       |
| 1   |       |       |  CAM  |       |       |       |       |       |       |       |
| 2   |       |  CAM  |       |       |       |       |       |       |       |       |
| 3   |       |  CAM  |       |       |       |       |       |       |       |       |
| 4   |       |       |       |       |       |       |       |       |       |       |
| 5   |       |  CAM  |  CAM  |       |       |       |       |       |       |       |
| 6   |       |       |       |       |       |       |       |       |       |       |
| 7   |       |       |       |       |       |       |       |       |       |       |
| 8   |       |       |       |       |       |       |       |       |       |       |
| 9   |       |       |       |       |       |       |       |       |       |       |
| 10  |       |       |       |       |       |       |       |       |       |       |
| 11  |       |       |       |       |       |       |       |       |       |       |
| 12  |       |       |       |       |       |       |       |       |       |       |
| 13  |       |       |       |       |       |       |       |       |       |       |
| 14  |       |       |       |       |       |       |       |       |       |       |
| 15  |       |       |       |       |       |       |       |       |       |       |
| 16  |       |       |       |       |       |       |       |       |       |       |
| 17  |       |       |       |       |       |       |       |       |       |       |
| 18  |       |       |       |       |       |       |       |       |       |       |
| 19  |       |       |       |       |       |       |       |       |       |       |
| 20  |       |       |       |       |       |       |       |       |       |       |
| 21  |       |       |  CAM  |       |       |       |       |       |       |       |
| 22  |       |       |       |       |       |       |       |       |       |       |
| 23  |       |       |       |       |       |       |       |       |       |       |
| 24  |       |       |       |       |       |       |       |       |       |       |
| 25  |       |       |       |       |       |       |       |       |       |       |
| 26  |       |       |       |       |       |       |       |       |       |       |
| 27  |       |  CAM  |       |       |       |       |       |       |       |       |
| 28  |  CAM  |       |       |  CAM  |       |  CAM  |  CAM  |  CAM  |       |       |
| 29  |  CAM  |       |       |  CAM  |       |  CAM  |  CAM  |  CAM  |       |       |
| 30  |       |       |       |       |       |       |       |       |       |       |
| 31  |       |       |       |       |       |       |       |       |       |       |
| 32  |  CAM  |       |       |  CAM  |       |  CAM  |       |  CAM  |       |       |
| 33  |       |       |       |       |       |       |       |       |       |       |
| 34  |       |       |       |       |       |       |       |       |       |       |
| 35  |       |       |       |       |       |       |       |       |       |       |
| 36  |       |       |       |       |       |       |       |       |       |       |
| 37  |       |       |       |       |       |       |       |       |       |       |
| 38  |       |       |       |       |       |       |       |       |       |       |
| 39  |       |       |       |       |       |       |       |       |       |       |
| 40  |  AUD  |  AUD  |  AUD  |  AUD  |       |       |  CAM  |  AUD  |  AUD  |       |
| 41  |  CAM  |       |       |  CAM  |       |  CAM  |       |  CAM  |  AUD  |       |
| 42  |       |       |       |       |       |       |       |  SMPS |       |       |
| 43  |       |       |       |       |       |       |       |  SMPS |       |       |
| 44  |       |       |       |       |       |       |  CAM  |       |  CAM  |       |
| 45  |  AUD  |  AUD  |  AUD  |  AUD  |       |       |       |  AUD  |  CAM  |       |
| 46  |       |       |       |       |       |       |       |       |  SMPS |  SMPS |
| 47  |       |       |       |       |       |       |       |       |  SMPS |  SMPS |
| 48  |       |       |       |       |       |       |       |       |       |       |
| 49  |       |       |       |       |       |       |       |       |       |       |
| 50  |       |       |       |       |       |       |       |       |       |       |
| 51  |       |       |       |       |       |       |       |       |       |       |
| 52  |       |       |       |       |       |       |       |       |       |       |
| 53  |       |       |       |       |       |       |       |       |       |       |