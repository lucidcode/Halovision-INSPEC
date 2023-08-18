/**
  @file
  @brief    FreeRTOS port for iHOME.
  @version  1.0
  @date     02-Sep-15
*/
/*
 * =============================================================================
 * Copyright (C) Bridgetek Pte Ltd
 * =============================================================================
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
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * =============================================================================
 * Modification History
 * v1.0 @ 02-Sep-15
 * 1. Creation.
 */
################################################################################
# portasm.S
#
# Assembly functions for FreeRTOS port of FT32 core.
################################################################################

################################################################################
# vPortYield
#
# vPortYield stores the processor context on the task's stack
# The stack pointer is then used as the task control block (TCB)
# This is stored in the variable pxCurrentTCB.
#
# The task to resume is then chosen using the function vTaskSwitchContext.
#
# This sets the variable pxCurrentTCB to the TCB of the task to resume.
# The registers are then loaded from the stack, and the function then
# returns to the chosen task.
#
# vPortYield should never be call within an ISR; use portYIELD_FROM_ISR().
#
# vPortYield is accessed via wrappers in the kernel:
#	portYIELD_WITHIN_API
#	taskYIELD_IF_USING_PREEMPTION
#	queueYIELD_IF_USING_PREEMPTION
#	taskYIELD
#
# vPortYield may be called within critical section in the kernel.
# As such, if we were to make vPortYield's context switching as atomic,
# we should use vPortDisableInterrupts and vPortEnableInterrupts.
################################################################################
.extern vPortDisableInterrupts
.extern vPortEnableInterrupts
.extern vPortSetInterrupts
.extern vPortGetInterrupts
.extern pxCurrentTCB


.global vPortStart
vPortStart:
    ############################################################################
    # Note1: Since this is called by application, by the calling convention,   #
    #        {$r0, .. $r12} are scratch registers (caller saved).              #
    #                                                                          #
    #        Thus it is ok to call vPortDisableInterrupts() before saving      #
    #        processor context.                                                #
    ############################################################################
    # We disable interrupt to avoid systick pre-emption while context switching
    # by application.

    # Save processor context
    push.l  $cc             # $cc needs to be saved first because during context restore, it may be corrupted.
    push.l  $r0
    push.l  $r1
    push.l  $r2
    push.l  $r3
    push.l  $r4
    push.l  $r5
    push.l  $r6
    push.l  $r7
    push.l  $r8
    push.l  $r9
    push.l  $r10
    push.l  $r11
    push.l  $r12
    push.l  $r13
    push.l  $r14
    push.l  $r15
    push.l  $r16
    push.l  $r17
    push.l  $r18
    push.l  $r19
    push.l  $r20
    push.l  $r21
    push.l  $r22
    push.l  $r23
    push.l  $r24
    push.l  $r25
    push.l  $r26
    push.l  $r27
    push.l  $r28
    push.l  $r29

    #save interrupt context.
    call    vPortGetInterrupts
    #lda.b   $r0,0x100e3
    push.l  $r0

    ######################################
    # Context already known, start it.
    ######################################
    jmp pop_context


.global vPortYield
vPortYield:
    ############################################################################
    # Note1: Since this is called by application, by the calling convention,   #
    #        {$r0, .. $r12} are scratch registers (caller saved).              #
    #                                                                          #
    #        Thus it is ok to call vPortDisableInterrupts() before saving      #
    #        processor context.                                                #
    ############################################################################
	# We disable interrupt to avoid systick pre-emption while context switching
	# by application.

    # Save processor context
    push.l  $cc				# $cc needs to be saved first because during context restore, it may be corrupted.
    push.l  $r0
    push.l  $r1
    push.l  $r2
    push.l  $r3
    push.l  $r4
    push.l  $r5
    push.l  $r6
    push.l  $r7
    push.l  $r8
    push.l  $r9
    push.l  $r10
    push.l  $r11
    push.l  $r12
    push.l  $r13
    push.l  $r14
    push.l  $r15
    push.l  $r16
    push.l  $r17
    push.l  $r18
    push.l  $r19
    push.l  $r20
    push.l  $r21
    push.l  $r22
    push.l  $r23
    push.l  $r24
    push.l  $r25
    push.l  $r26
    push.l  $r27
    push.l  $r28
    push.l  $r29

    #save interrupt context.
    call    vPortGetInterrupts
    #lda.b   $r0,0x100e3
    push.l  $r0

    ######################################
    # Start-of-Context-Switching Operation
    ######################################
    # save stack
    lda.l   $r0, pxCurrentTCB
    sti.l   $r0, 0, $sp

    call    vPortDisableInterrupts

    # Choose new TCB
    call    vTaskSwitchContext

pop_context:

    # Restore stack
    lda.l   $r0, pxCurrentTCB
    ldi.l   $sp, $r0, 0

    # Restore interrupt context
    pop.l   $r0
    call    vPortSetInterrupts

    ######################################
    # End-of-Context-Switching Operation
    ######################################
    pop.l   $r29
    pop.l   $r28
    pop.l   $r27
    pop.l   $r26
    pop.l   $r25
    pop.l   $r24
    pop.l   $r23
    pop.l   $r22
    pop.l   $r21
    pop.l   $r20
    pop.l   $r19
    pop.l   $r18
    pop.l   $r17
    pop.l   $r16
    pop.l   $r15
    pop.l   $r14
    pop.l   $r13
    pop.l   $r12
    pop.l   $r11
    pop.l   $r10
    pop.l   $r9
    pop.l   $r8
    pop.l   $r7
    pop.l   $r6
    pop.l   $r5
    pop.l   $r4
    pop.l   $r3
    pop.l   $r2
    pop.l   $r1
    pop.l   $r0
    pop.l   $cc


    # and return
    return