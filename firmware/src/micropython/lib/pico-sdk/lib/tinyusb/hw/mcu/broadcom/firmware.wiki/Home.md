## Welcome to the Raspberry Pi Firmware Wiki

This Wiki covers the firmware used on the Video GPU used in all Raspberry Pi models.

Note that the firmware for the Videocore is closed source, so there is no source code for it available in this repository. The Code section contains precompiled binaries only.

### Overview
This wiki describes the interface to the Raspberry Pi firmware from the point of view of a developer working on kernel (or "bare metal") code running on the ARM.

### Mailboxes
Mailboxes are the primary means of communication between the ARM and the VideoCore firmware running on the GPU. For a list of the available mailboxes, see [here](wiki/Mailboxes).
