/**
  @file
  @brief    CRT0 for FT32 MCU FreeRTOS Port.
  @version  2.0
  @date     23-Jan-17
*/
/*
 * =============================================================================
 * Copyright (C) Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of FTDI liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on FTDI's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, FTDI
 * has no liability in relation to those amendments.
 * ============================================================================
 * Modification History
 * v1.0 @ 02-Sep-15
 * 1. Creation.
 * 2. Updated for FT930
 */
.equ SYS_REGMSC0CFG_B3  , 0x1001b
.equ SYS_REGIRQCTL_B3   , 0x100e3
.equ MAILBOX_MEMORY     , 0x13000

.equ	IS_IMG_SDBL_PRESENT, 0
.equ	IS_IMG_D2XX_PRESENT, 1
.equ	IS_IMG_DLOG_PRESENT, 0

.section .crt0
.global _start

_start:
# START Interrupt Vector Table [[
        jmp     __PMSIZE-4         # RESET Vector
        jmp     interrupt_33	   # Watchdog reset vector
        jmp     interrupt_0
        jmp     interrupt_1
        jmp     interrupt_2
        jmp     interrupt_3
        jmp     interrupt_4
        jmp     interrupt_5
        jmp     interrupt_6
        jmp     interrupt_7
        jmp     interrupt_8
        jmp     interrupt_9
        jmp     interrupt_10
        jmp     interrupt_11
        jmp     interrupt_12
        jmp     interrupt_13
        jmp     interrupt_14
        jmp     interrupt_15
        jmp     interrupt_16
        jmp     interrupt_17
        jmp     interrupt_18
        jmp     interrupt_19
        jmp     interrupt_20
        jmp     interrupt_21
        jmp     interrupt_22
        jmp     interrupt_23
        jmp     interrupt_24
        jmp     interrupt_25
        jmp     interrupt_26
        jmp     interrupt_27
        jmp     interrupt_28
        jmp     interrupt_29
        jmp     interrupt_30
        jmp     interrupt_31
        jmp     __PMSIZE-8				#Interrupt vector 32 (NMI)
# ]] END Interrupt Vector Table

codestart:
        jmp    init

        
.global _exithook
_exithook:               # Debugger uses '_exithook' at 0x90 to catch program exit
        return
        
init:
        # Disable all interrupts
        ldk     $r0,0x80
.ifdef __FT930__
        sta.b   0x10123, $r0
.else
        sta.b   0x100e3,$r0
.endif
        
        # Initialize DATA by copying from program memory
        ldk.l   $r0,__data_load_start
        ldk.l   $r1,__data_load_end
        ldk.l   $r2,0   # Will use __data after binutils patch

        jmp     .dscopy
.dsloop:
        # Copy PM[$r0] to RAM $r2
        lpmi.l  $r3,$r0,0
        sti.l   $r2,0,$r3
        add.l   $r0,$r0,4
        add.l   $r2,$r2,4
.dscopy:
        cmp.l   $r0,$r1
        jmpc    lt,.dsloop

        # Zero BSS
        ldk.l   $r0,_bss_start
        ldk.l   $r2,_end
        sub.l   $r2,$r2,$r0
        ldk.l   $r1,0
        ldk    $r3,32764
1:
        cmp    $r2,$r3
        jmpc   lt,2f
        memset $r0,$r1,$r3
        add    $r0,$r0,$r3
        sub    $r2,$r2,$r3
        jmp    1b
2:
        memset $r0,$r1,$r2
.ifdef __FT930__
/*##############################################################*/
		# copy UserConfig DATA from flash to mailbox memory
/*##############################################################*/
        ldk.l   $r0,D2XX_Struct_start  /*start of d2xx config in PM memory */
        ldk.l   $r1,D2XX_Struct_end /*end of d2xx config in PM memory */
        ldk.l   $r2,D2XXTEST_UserD2xxConfig /* RAM cache where the d2xx config from PM to be copied*/
        jmp     .configcopy

.configloop:
        # Copy PM[$r0] to RAM[$r2]
        lpmi.l  $r3,$r0,0
        sti.l   $r2,0,$r3
        # Increment
        add.l   $r0,$r0,4
        add.l   $r2,$r2,4
.configcopy:
        cmp.l   $r0,$r1
        jmpc    lt,.configloop

        ldk.l   $r1,D2XX_Struct_start
        ldk.l   $r2,D2XX_Struct_end
        #compute size
        sub.l   $r2,$r2,$r1
        ldk.l   $r1,D2XXTEST_UserD2xxConfig
        ldk.l   $r0,MAILBOX_MEMORY		/* D2xx config from RAM cache to be copied to Mailbox memory */
        # Copy RAM[$r1] to Mailbox $r0, for $r2 bytes
        streamouti.b   $r0,$r1,$r2
/*############################################################*/
.endif
        sub.l   $sp,$sp,24  # Space for the caller argument frame
        jmp		main

.equ EXITEXIT    , 0x1fffc

.global _exit
_exit:
        sta.l   EXITEXIT,$r0    # simulator end of test
        jmp     _exithook

_watchdog_timeout:
        ldk     $sp,__RAMSIZE
        lda     $r0,(vector_table + 4 * 33)
        jmpi    $r0

# Macro to construct the interrupt stub code.
# it just saves r0, loads r0 with the int vector
# and branches to interrupt_common.

.macro  inth i=0
interrupt_\i:
        push    $cc     # { See portasm.S for reason why $cc is saved first
        push    $r0     # {
        lda     $r0,(vector_table + 4 * \i)
        jmp     interrupt_common
.endm

        inth    0
        inth    1       # interrupt_usb_host
        inth    2       # interrupt_usb_device
        inth    3       # interrupt_ethernet
        inth    4       # interrupt_sd_card
        inth    5       # interrupt_can0
        inth    6       # interrupt_can1
        inth    7       # interrupt_camera
        inth    8       # interrupt_spim
        inth    9       # interrupt_spis0
        inth    10      # interrupt_spis1
        inth    11      # interrupt_i2cm
        inth    12      # interrupt_i2cs
        inth    13      # interrupt_uart0
        inth    14
        inth    15
        inth    16
        inth    17
        inth    18
        inth    19
        inth    20
        inth    21
        inth    22
        inth    23
        inth    24
        inth    25
        inth    26
        inth    27
        inth    28
        inth    29
        inth    30
        inth    31
        inth    32
        inth    33

# On entry: $cc and $r0 already saved; $r0 holds the handler function
interrupt_common:
        push    $r1     # {
        push    $r2     # {
        push    $r3     # {
        push    $r4     # {
        push    $r5     # {
        push    $r6     # {
        push    $r7     # {
        push    $r8     # {
        push    $r9     # {
        push    $r10    # {
        push    $r11    # {
        push    $r12    # {
        push    $r13    # {

# See tasks.c
.extern pxCurrentTCB
        # Save current context pointer.
        lda.l   $r13, pxCurrentTCB

        # We use system specific stack for ISR.
        # This stack is located at the top-most of memory map.
        # This region is unused once FreeRTOS scheduler starts.
        # This way, we can also reduce the stack size of each task.
        ### NOTE: with delayed push/pop code, we cannot do this here w/o
        ###         addiitional instructions (pre-calculate actual sp, store it, restore it.)
        #ldk.l   $sp, 0

        # Call IRQ handler
        calli   $r0

        # load new context pointer. 
        lda.l   $r0, pxCurrentTCB

        # check if different
        cmp.l   $r0,$r13
        jmpc    z,exit_isr


        # Context has changed, continue pushing remaining registers.
        push    $r14    # {
        push    $r15    # {
        push    $r16    # {
        push    $r17    # {
        push    $r18    # {
        push    $r19    # {
        push    $r20    # {
        push    $r21    # {
        push    $r22    # {
        push    $r23    # {
        push    $r24    # {
        push    $r25    # {
        push    $r26    # {
        push    $r27    # {
        push    $r28    # {
        push    $r29    # {

        #save interrupt context.
        #call    vPortGetInterrupts
.ifdef __FT930__
        lda.b   $r1,0x10123
.else
        lda.b   $r1,0x100e3
.endif
        push.l  $r1

        # Save stack to prev context
        sti.l   $r13, 0, $sp

        # Restore stack from new context.
        ldi.l   $sp, $r0, 0


        # Restore interrupt context
        pop.l   $r0
        call    vPortSetInterrupts

        # finish poping registers for context switch.
        pop     $r29    # }
        pop     $r28    # }
        pop     $r27    # }
        pop     $r26    # }
        pop     $r25    # }
        pop     $r24    # }
        pop     $r23    # }
        pop     $r22    # }
        pop     $r21    # }
        pop     $r20    # }
        pop     $r19    # }
        pop     $r18    # }
        pop     $r17    # }
        pop     $r16    # }
        pop     $r15    # }
        pop     $r14    # }

exit_isr:
        # finish polling registers always pushed
        pop     $r13    # }
        pop     $r12    # }
        pop     $r11    # }
        pop     $r10    # }
        pop     $r9     # }
        pop     $r8     # }
        pop     $r7     # }
        pop     $r6     # }
        pop     $r5     # }
        pop     $r4     # }
        pop     $r3     # }
        pop     $r2     # }
        pop     $r1     # }
        pop     $r0     # }
        pop     $cc     # } matching push in interrupt_0-31 above
        reti

        ######################################
        # End-of-Context-Switching Operation
        ######################################

# Null function for unassigned interrupt to point at
.global     nullvector
nullvector:
        return

.section .data
.align 4
.global vector_table
vector_table:
        .rept 34
                .long   nullvector
        .endr

.section .text
.global __gxx_personality_sj0
__gxx_personality_sj0:
	.section ._flash_d2xx_config
.global __pD2XXDefaultConfiguration
	.align (10)

D2XX_partition_start = .

.if IS_IMG_D2XX_PRESENT
.ifdef __FT930__
.include "ft930_d2xx_default_config.inc"
.else
.include "ft900_d2xx_default_config.inc"
.endif
.endif

D2XX_partition_end = .

	.section ._flash_dlog_partition
	.align (10)
.global __dlog_partition
__dlog_partition:
dlog_partition_start = .
.if IS_IMG_DLOG_PRESENT
	.long	0xF7D1D106
	.rept  (0x1000-4)
	.byte	0xFF
	.endr
.endif
dlog_partition_end = .

	.section ._pm
.global	__sdbl_partition_sizeof
.global __D2XX_partition_sizeof
.global __dlog_partition_sizeof
	.if IS_IMG_SDBL_PRESENT
__sdbl_partition_sizeof = 0x2000
	.else
__sdbl_partition_sizeof = 0
	.endif

__D2XX_partition_sizeof = D2XX_partition_end - D2XX_partition_start
__dlog_partition_sizeof = dlog_partition_end - dlog_partition_start
