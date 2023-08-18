# Introduction
The bootcode.bin does have some debug output available which can be used to debug the various boot modes.  Specifically the two boot modes that have significant debug in are USB mass storage and network booting.

Click [bootcode.bin](https://github.com/raspberrypi/firmware/files/1466884/bootcode.bin.gz), for the latest build of the second stage with latest debug...

## Connecting the UART serial connection
To do this I would suggest using a simple three wire serial dongle such as the [Adafruit USB serial dongle](https://www.adafruit.com/product/954) although it is also simple enough to do with a second Raspberry Pi.  Just use pins 6, 8 and 10 on the GPIO connector.

Then on the host computer run putty (on a windows PC) or screen on a linux based computer or a MAC.

### Windows 10

* Install putty
* Search for "device manager" and open it
* In the Device Manager find the "Ports (COM and LPT)" section and open it, the serial device will be listed with something like COM5 next to it...
* Open putty click on "Session"
* Change "Serial line" to match the one from the device manager (COM5 for me)
* Change speed to 115200
* If required change any extra settings under the "Serial" tree node, specifically you'll probably need to disable hardware flow control
* Then click 'open'

### Linux (Debian)

* Plug the serial dongle into the Linux computer
* type `ls /dev/tty*` this should find something like /dev/ttyUSB0
* now type `screen /dev/ttyUSB0 115200` to connect to the serial

### MAC

* Plug the serial dongle into the MAC
* type `ls /dev/tty*` this should find something like /dev/tty.usbserial-FTHIH75J
* Now type `screen /dev/tty.usbserial-FTHIH75J 115200` to connect to the serial

### Common - Testing the USB serial

Using a standard Raspbian build, boot the Raspberry Pi device you are trying to debug.  Log in and open the raspi-config application by opening a terminal (Ctrl-Alt-T) and typing `sudo raspi-config`

Now, go to `Interfacing options` and `serial` and enable the serial login shell. Now type `sudo reboot`

When the Pi reboots, it should output serial at 115200 baud on pin 8 (6 is ground).  You should check it is correctly configured before proceeding.

## Activating the debug

There are two different ways that bootcode.bin can be used and therefore two different ways that it can be debugged.  The two methods are characterised as follows:

1) Read bootcode.bin from source (SD card, USB MSD or via ethernet) then read start.elf, kernel.img from the same source.
2) Read bootcode.bin from SD card (specifically with no config.txt or start.elf on the SD card) then try to boot from USB MSD or ethernet to find the other files (start.elf, config.txt, kernel.img etc)

The reason we use 2) above is when we have problems with the standard boot mode and require some debug to get any further (i.e. why can't it read from the USB MSD or network)

To enable debug output in 1) above you need to add `uart_2ndstage=1` to config.txt

To enable debug output in 2) above you need to add a file named `UART` to the filesystem alongside the bootcode.bin file

Once you have done this you should get boot output during the boot phase, this will start with the following:

`Raspberry Pi Bootcode`
