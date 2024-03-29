# Halovision-INSPEC

A night-vision smart camera that detects eye movements during REM sleep and triggers audio and visual alerts to promote lucidity.

![INSPEC](https://github.com/lucidcode/Halovision-INSPEC/raw/main/images/inspec.jpg?raw=true "INSPEC")

![INSPECs](https://github.com/lucidcode/Halovision-INSPEC/raw/main/images/inspecs.jpg?raw=true "INSPECs")

## Works with

<a href="http://lucidcode.com/LucidScribe/">Lucid Scribe</a>

![Lucid Scribe REM sleep at 06:23](https://github.com/lucidcode/Halovision-INSPEC/raw/main/images/lucid_scribe_rem_0623.gif?raw=true "Lucid Scribe REM sleep at 06:23")

![Lucid Scribe REM sleep at 07:38](https://github.com/lucidcode/Halovision-INSPEC/raw/main/images/lucid_scribe_rem_0738.gif?raw=true "Lucid Scribe REM sleep at 07:38")

<a href="https://play.google.com/store/apps/details?id=com.lucidcode.lucidscribe">Lucid Scribe Android</a>

![Lucid Scribe Android](https://github.com/lucidcode/Halovision-INSPEC/raw/main/images/lucid_scribe_android.gif?raw=true "Lucid Scribe Android")

## Firmware build
`cd firmware/src`
`cd micropython/mpy-cross`
`make`
`cd ../..`
`make TARGET=INSPEC`