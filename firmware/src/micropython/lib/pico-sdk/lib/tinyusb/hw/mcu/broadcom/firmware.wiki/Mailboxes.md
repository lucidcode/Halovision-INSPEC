### Overview
Mailboxes facilitate communication between the ARM and the VideoCore. This page lists the available mailboxes/channels.  Each mailbox is an 8-deep FIFO of 32-bit words, which can be read (popped)/written (pushed) by the ARM and VC.  Only mailbox 0's status can trigger interrupts on the ARM, so MB 0 is always for communication from VC to ARM and MB 1 is for ARM to VC.  The ARM should never write MB 0 or read MB 1.

### Channels
The following lists the currently defined mailbox channels, with links to pages describing the format of the messages.

Mailbox 0 defines the following channels:

0: Power management  
1: [Framebuffer](Mailbox-framebuffer-interface)  
2: Virtual UART  
3: VCHIQ  
4: LEDs  
5: Buttons  
6: Touch screen  
7:  
8: [Property tags (ARM -> VC)](Mailbox-property-interface)  
9: Property tags (VC -> ARM)  

### Mailbox registers
The following table shows the register offsets for the different mailboxes. For a description of the procedure for using these registers to access a mailbox, see [here](Accessing-mailboxes).

    Mailbox Read/Write Peek  Sender  Status    Config
       0    0x00       0x10  0x14    0x18      0x1c
       1    0x20       0x30  0x34    0x38      0x3c
