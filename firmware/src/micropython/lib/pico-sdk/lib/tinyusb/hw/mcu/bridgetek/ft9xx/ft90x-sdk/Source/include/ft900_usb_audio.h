/**
    @file ft900_usb_audio.h

    @brief
    USB Audio class USB definitions.

	@details
    This contains USB definitions of constants and structures which are used in
    USB Audio implementations.

    Definitions for USB Audio Devices based on USB Audio Specification 1.0
    available from USBIF Developers.
**/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek") subject to the licence terms set out
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
 * ============================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef FT900_USB_AUDIO_H_
#define FT900_USB_AUDIO_H_

/* INCLUDES ************************************************************************/
#include "ft900_usb.h"

/* CONSTANTS ***********************************************************************/

/**
    @name USB Audio Class Request Codes
    @brief Common to all Audio Subclasses
        Based on Table C-30.
 */
//@{
#define USB_AUDIO_REQUEST_RC_UNDEFINED 0x00

#define USB_AUDIO_REQUEST_SET_CUR 0x01
#define USB_AUDIO_REQUEST_SET_MIN 0x02
#define USB_AUDIO_REQUEST_SET_MAX 0x03
#define USB_AUDIO_REQUEST_SET_RES 0x04
#define USB_AUDIO_REQUEST_SET_MEM 0x05

#define USB_AUDIO_REQUEST_GET_CUR 0x81
#define USB_AUDIO_REQUEST_GET_MIN 0x82
#define USB_AUDIO_REQUEST_GET_MAX 0x83
#define USB_AUDIO_REQUEST_GET_RES 0x84
#define USB_AUDIO_REQUEST_GET_MEM 0x85
//@}

// Table A-5: Audio Interface Subclass Codecs
//		Audio Interface Subclass Code 								Value
#define INTERFACE_SUBCLASS_UNDEFINED                               0x00 // INTERFACE_SUBCLASS_UNDEFINED
#define INTERFACE_SUBCLASS_AUDIOCONTROL                            0x01 // AUDIOCONTROL
#define INTERFACE_SUBCLASS_AUDIOSTREAMING                          0x02 // AUDIOSTREAMING
#define INTERFACE_SUBCLASS_MIDISTREAMING                           0x03 // MIDISTREAMING

// Table A.6 Audio Interface Protocol Codes
//      Audio Interface Protocol Code 								Value
#define INTERFACE_PROTOCOL_UNDEFINED                               0x00 // INTERFACE_PROTOCOL_UNDEFINED
#define INTERFACE_PROTOCOL_IP_VERSION_02_00 	                   0x20 // IP_VERSION_02_00

// Table A-7: Audio Function Category Codes
//      Audio Function Subclass Code 								Value
#define FUNCTION_SUBCLASS_UNDEFINED                                0x00
#define FUNCTION_SUBCLASS_DESKTOP_SPEAKER                          0x01
#define FUNCTION_SUBCLASS_HOME_THEATER                             0x02
#define FUNCTION_SUBCLASS_MICROPHONE                               0x03
#define FUNCTION_SUBCLASS_HEADSET                                  0x04
#define FUNCTION_SUBCLASS_TELEPHONE                                0x05
#define FUNCTION_SUBCLASS_CONVERTER                                0x06
#define FUNCTION_SUBCLASS_VOICE_SOUND_RECORDER                     0x07
#define FUNCTION_SUBCLASS_I_O_BOX                                  0x08
#define FUNCTION_SUBCLASS_MUSICAL_INSTRUMENT                       0x09
#define FUNCTION_SUBCLASS_PRO_AUDIO                                0x0A
#define FUNCTION_SUBCLASS_AUDIO_VIDEO                              0x0B
#define FUNCTION_SUBCLASS_CONTROL_PANEL                            0x0C
#define FUNCTION_SUBCLASS_OTHER                                    0xFF

// Table A-8: Audio Class-specific Descriptor Types
//		Descriptor Types											Value
#define USB_DESCRIPTOR_TYPE_AUDIO_UNDEFINED						   0x20 // CS_UNDEFINED
#define USB_DESCRIPTOR_TYPE_AUDIO_DEVICE						   0x21 // CS_DEVICE
#define USB_DESCRIPTOR_TYPE_AUDIO_CONFIGURATION					   0x22 // CS_CONFIGURATION
#define USB_DESCRIPTOR_TYPE_AUDIO_STRING						   0x23 // CS_STRING
#define USB_DESCRIPTOR_TYPE_AUDIO_INTERFACE						   0x24 // CS_INTERFACE
#define USB_DESCRIPTOR_TYPE_AUDIO_ENDPOINT						   0x25 // CS_ENDPOINT

// Table A-9: Audio Class-Specific AC (Audio Control) Interface Descriptor Subtypes
//		Interface Descriptor Subtypes								Value
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_UNDEFINED			   0x00 // AC_DESCRIPTOR_UNDEFINED
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_HEADER				   0x01 // HEADER
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_INPUT_TERMINAL		   0x02 // INPUT_TERMINAL
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_OUTPUT_TERMINAL	   0x03 // OUTPUT_TERMINAL
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_MIXER_UNIT			   0x04 // MIXER_UNIT
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_SELECTOR_UNIT		   0x05 // SELECTOR_UNIT
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_FEATURE_UNIT		   0x06 // FEATURE_UNIT
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_EFFECT_UNIT		   0x07 // EFFECT_UNIT
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_PROCESSING_UNIT	   0x08 // PROCESSING_UNIT
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_EXTENSION_UNIT		   0x09 // EXTENSION_UNIT
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_CLOCK_SOURCE		   0x0A // CLOCK_SOURCE
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_CLOCK_SELECTOR		   0x0B // CLOCK_SELECTOR
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_CLOCK_MULTIPLIER	   0x0C // CLOCK_MULTIPLIER
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_CONTROL_SAMPLE_RATE_CONVERTER 0x0D // SAMPLE_RATE_CONVERTER

// Table A-10: Audio Class-Specific AS (Audio Streaming) Interface Descriptor Subtypes
//		Interface Descriptor Subtypes								Value
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_UNDEFINED		   0x00 // AS_DESCRIPTOR_UNDEFINED
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_GENERAL			   0x01 // AS_GENERAL
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_FORMAT_TYPE		   0x02 // FORMAT_TYPE
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_ENCODER			   0x03 // ENCODER
#define USB_DESCRIPTOR_SUBTYPE_AUDIO_STREAMING_DECODER			   0x04 // DECODER

// Table A-11: Effect Unit Effect Types
//		Audio Effect Type											wEffectType - used with
// usb_audioEffectUnitDescriptor_t
#define USB_AUDIO_EFFECT_UNDEFINED		  0x00 // EFFECT_UNDEFINED
#define USB_AUDIO_EFFECT_PARAM_EQ_SECTION 0x01 // PARAM_EQ_SECTION_EFFECT
#define USB_AUDIO_EFFECT_REVERBERATION	  0x02 // REVERBERATION_EFFECT
#define USB_AUDIO_EFFECT_MOD_DELAY		  0x03 // MOD_DELAY_EFFECT
#define USB_AUDIO_EFFECT_DYN_RANGE_COMP	  0x04 // DYN_RANGE_COMP_EFFECT

// Table A-12: Processing Unit Process Types
//		Audio Process Type											wProcessType - used with
// usb_audioProcessUnitDescriptor_t
#define USB_AUDIO_PROCESS_UNDEFINED						0x00 // PROCESS_UNDEFINED
#define USB_AUDIO_PROCESS_UP_DOWN_MIX					0x01 // UP/DOWNMIX_PROCESS
#define USB_AUDIO_PROCESS_DOLBY_PROLOGIC				0x02 // DOLBY_PROLOGIC_PROCESS
#define USB_AUDIO_PROCESS_STEREO_EXTENDER				0x03 // STEREO_EXTENDER_PROCESS

// Table A-13: Audio Class-Specific Endpoint Descriptor Subtypes
//		Audio Endpoint Subtype										Value
#define USB_ENDPOINT_DESCRIPTOR_AUDIO_UNDEFINED			0x00 // DESCRIPTOR_UNDEFINED
#define USB_ENDPOINT_DESCRIPTOR_AUDIO_GENERAL			0x01 // EP_GENERAL

// Table 4-34: Class-Specific AS Isochronous Audio Data Endpoint Descriptor
#define USB_AUDIO_ENDPOINT_BMATTRIBUTE_MAX_PACKET 0x80
#define USB_AUDIO_ENDPOINT_BMCONTROL_PITCH_CONTROL_AVAILABLE 0x01
#define USB_AUDIO_ENDPOINT_BMCONTROL_PITCH_CONTROL_WRITABLE 0x02
#define USB_AUDIO_ENDPOINT_BMCONTROL_DATA_OVERRUNH_CONTROL_AVAILABLE 0x04
#define USB_AUDIO_ENDPOINT_BMCONTROL_DATA_OVERRUN_CONTROL_WRITABLE 0x08
#define USB_AUDIO_ENDPOINT_BMCONTROL_DATA_UNDERRUN_CONTROL_AVAILABLE 0x10
#define USB_AUDIO_ENDPOINT_BMCONTROL_DATA_UNDERRUN_CONTROL_WRITABLE 0x20
#define USB_AUDIO_ENDPOINT_BLOCKDELAY_UNITS_UNDEFINED 0
#define USB_AUDIO_ENDPOINT_BLOCKDELAY_UNITS_MILLISECONDS 1
#define USB_AUDIO_ENDPOINT_BLOCKDELAY_UNITS_DECODED_PCM_SAMPLES 2

/**
    @name USB AUDIO Request Codes
    @brief Table A-14: Audio Class-Specific Request Codes
 */
//@{
#define USB_REQUEST_CODE_AUDIO_CODE_UNDEFINED			0x00 // REQUEST_CODE_UNDEFINED
#define USB_REQUEST_CODE_AUDIO_CUR						0x01 // CUR
#define USB_REQUEST_CODE_AUDIO_RANGE					0x02 // RANGE
#define USB_REQUEST_CODE_AUDIO_MEM						0x03 // MEM
//@}

// Table A-15: Encoder Type Codes
//		Audio Encoder Type											Value
#define USB_AUDIO_ENCODER_TYPE_UNDEFINED				0x00 // ENCODER_UNDEFINED
#define USB_AUDIO_ENCODER_TYPE_OTHER					0x01 // OTHER_ENCODER
#define USB_AUDIO_ENCODER_TYPE_MPEG						0x02 // MPEG_ENCODER
#define USB_AUDIO_ENCODER_TYPE_AC_3						0x03 // AC-3_ENCODER
#define USB_AUDIO_ENCODER_TYPE_WMA						0x04 // WMA_ENCODER
#define USB_AUDIO_ENCODER_TYPE_DTS						0x05 // DTS_ENCODER

// Table A-16: Decoder Type Codes
//		Audio Decoder Type											Value
#define USB_AUDIO_DECODER_TYPE_UNDEFINED				0x00 // DECODER_UNDEFINED
#define USB_AUDIO_DECODER_TYPE_OTHER					0x01 // OTHER_DECODER
#define USB_AUDIO_DECODER_TYPE_MPEG						0x02 // MPEG_DECODER
#define USB_AUDIO_DECODER_TYPE_AC_3						0x03 // AC-3_DECODER
#define USB_AUDIO_DECODER_TYPE_WMA						0x04 // WMA_DECODER
#define USB_AUDIO_DECODER_TYPE_DTS						0x05 // DTS_DECODER

// Table A-17: Clock Source Control Selectors
//		Control Selector											Value
#define USB_AUDIO_CLOCK_SOURCE_UNDEFINED				0x00 // CS_CONTROL_UNDEFINED
#define USB_AUDIO_CLOCK_SOURCE_SAM_FREQ					0x01 // CS_SAM_FREQ_CONTROL
#define USB_AUDIO_CLOCK_SOURCE_CLOCK_VALID				0x02 // CS_CLOCK_VALID_CONTROL

// Table 4-6: Clock Source Descriptor
#define USB_AUDIO_CLOCK_SOURCE_BMATTRIBUTES_EXTERNAL_CLOCK 0x00 // External Clock
#define USB_AUDIO_CLOCK_SOURCE_BMATTRIBUTES_INTERNAL_FIX_CLOCK 0x01 // Internal fixed Clock
#define USB_AUDIO_CLOCK_SOURCE_BMATTRIBUTES_INTERNAL_VAR_CLOCK 0x02 // Internal variable Clock
#define USB_AUDIO_CLOCK_SOURCE_BMATTRIBUTES_INTERNAL_PRG_CLOCK 0x03 // Internal programmable Clock
#define USB_AUDIO_CLOCK_SOURCE_BMATTRIBUTES_SOF_CLOCK 0x04 // Clock synchronized to SOF

#define USB_AUDIO_CLOCK_SOURCE_BMCONTROLS_CLOCK_FREQ_AVAILABLE 0x01 // Clock Frequency Control
#define USB_AUDIO_CLOCK_SOURCE_BMCONTROLS_CLOCK_FREQ_WRITEABLE 0x02 // Clock Frequency Control
#define USB_AUDIO_CLOCK_SOURCE_BMCONTROLS_CLOCK_VALID_AVAILABLE 0x04 // Clock Validity Control
#define USB_AUDIO_CLOCK_SOURCE_BMCONTROLS_CLOCK_VALID_WRITEABLE 0x08 // Clock Validity Control

// Table A-18: Clock Selector Control Selectors
//		Control Selector											Value
#define USB_AUDIO_CLOCK_SELECTOR_UNDEFINFED				0x00 // CX_CONTROL_UNDEFINED
#define USB_AUDIO_CLOCK_SELECTOR_CONTROL				0x01 // CX_CLOCK_SELECTOR_CONTROL

// Table A-19: Clock Multiplier Control Selectors
//		Control Selector											Value
#define USB_AUDIO_CLOCK_MULTIPLIER_UNDEFINED			0x00 // CM_CONTROL_UNDEFINED
#define USB_AUDIO_CLOCK_MULTIPLIER_NUMERATOR			0x01 // CM_NUMERATOR_CONTROL
#define USB_AUDIO_CLOCK_MULTIPLIER_DENOMINATOR			0x02 // CM_DENOMINATOR_CONTROL

// Table A-20: Terminal Control Selectors
//		Control Selector											Value
#define USB_AUDIO_TERMINAL_UNDEFINED					0x00 // TE_CONTROL_UNDEFINED
#define USB_AUDIO_TERMINAL_COPY_PROTECT					0x01 // TE_COPY_PROTECT_CONTROL
#define USB_AUDIO_TERMINAL_CONNECTOR					0x02 // TE_CONNECTOR_CONTROL
#define USB_AUDIO_TERMINAL_OVERLOAD						0x03 // TE_OVERLOAD_CONTROL
#define USB_AUDIO_TERMINAL_CLUSTER						0x04 // TE_CLUSTER_CONTROL
#define USB_AUDIO_TERMINAL_UNDERFLOW					0x05 // TE_UNDERFLOW_CONTROL
#define USB_AUDIO_TERMINAL_OVERFLOW						0x06 // TE_OVERFLOW_CONTROL
#define USB_AUDIO_TERMINAL_LATENCY						0x07 // TE_LATENCY_CONTROL

// Table A-21: Mixer Control Selectors
//		Control Selector											Value
#define USB_AUDIO_MIXER_UNDEFINED						0x00 // MU_CONTROL_UNDEFINED
#define USB_AUDIO_MIXER_MIXER_CONTROL					0x01 // MU_MIXER_CONTROL
#define USB_AUDIO_MIXER_CLUSTER							0x02 // MU_CLUSTER_CONTROL
#define USB_AUDIO_MIXER_UNDERFLOW						0x03 // MU_UNDERFLOW_CONTROL
#define USB_AUDIO_MIXER_OVERFLOW						0x04 // MU_OVERFLOW_CONTROL
#define USB_AUDIO_MIXER_LATENCY							0x05 // MU_LATENCY_CONTROL

// Table A-22: Selector Control Selectors
//		Control Selector											Value
#define USB_AUDIO_SELECTOR_UNDEFINED					0x00 // SU_CONTROL_UNDEFINED
#define USB_AUDIO_SELECTOR_SELECTOR_CONTROL				0x01 // SU_SELECTOR_CONTROL
#define USB_AUDIO_SELECTOR_LATENCY						0x02 // SU_LATENCY_CONTROL

// Table A-23: Feature Unit Control Selectors
//		Control Selector											Value
#define USB_AUDIO_FEATURE_UNIT_UNDEFINED				0x00 // FU_CONTROL_UNDEFINED
#define USB_AUDIO_FEATURE_UNIT_MUTE						0x01 // FU_MUTE_CONTROL
#define USB_AUDIO_FEATURE_UNIT_VOLUME					0x02 // FU_VOLUME_CONTROL
#define USB_AUDIO_FEATURE_UNIT_BASS						0x03 // FU_BASS_CONTROL
#define USB_AUDIO_FEATURE_UNIT_MID						0x04 // FU_MID_CONTROL
#define USB_AUDIO_FEATURE_UNIT_TREBLE					0x05 // FU_TREBLE_CONTROL
#define USB_AUDIO_FEATURE_UNIT_GRAPHIC_EQUALIZER		0x06 // FU_GRAPHIC_EQUALIZER_CONTROL
#define USB_AUDIO_FEATURE_UNIT_AUTOMATIC_GAIN			0x07 // FU_AUTOMATIC_GAIN_CONTROL
#define USB_AUDIO_FEATURE_UNIT_DELAY					0x08 // FU_DELAY_CONTROL
#define USB_AUDIO_FEATURE_UNIT_BASS_BOOST				0x09 // FU_BASS_BOOST_CONTROL
#define USB_AUDIO_FEATURE_UNIT_LOUDNESS					0x0A // FU_LOUDNESS_CONTROL
#define USB_AUDIO_FEATURE_UNIT_INPUT_GAIN				0x0B // FU_INPUT_GAIN_CONTROL
#define USB_AUDIO_FEATURE_UNIT_INPUT_GAIN_PAD			0x0C // FU_INPUT_GAIN_PAD_CONTROL
#define USB_AUDIO_FEATURE_UNIT_PHASE_INVERTER			0x0D // FU_PHASE_INVERTER_CONTROL
#define USB_AUDIO_FEATURE_UNIT_UNDERFLOW				0x0E // FU_UNDERFLOW_CONTROL
#define USB_AUDIO_FEATURE_UNIT_OVERFLOW					0x0F // FU_OVERFLOW_CONTROL
#define USB_AUDIO_FEATURE_UNIT_LATENCY					0x10 // FU_LATENCY_CONTROL

// Feature Unit Control Selectors Request Lengths
#define USB_AUDIO_FEATURE_UNIT_MUTE_LENGTH				0x01 //
#define USB_AUDIO_FEATURE_UNIT_VOLUME_LENGTH			0x02 //
#define USB_AUDIO_FEATURE_UNIT_BASS_LENGTH				0x01 //
#define USB_AUDIO_FEATURE_UNIT_MID_LENGTH				0x01 //
#define USB_AUDIO_FEATURE_UNIT_TREBLE_LENGTH			0x01 //
#define USB_AUDIO_FEATURE_UNIT_GRAPHIC_EQUALIZER_LENGTH	0x04 //
#define USB_AUDIO_FEATURE_UNIT_AUTOMATIC_GAIN_LENGTH	0x01 //
#define USB_AUDIO_FEATURE_UNIT_DELAY_LENGTH				0x02 //
#define USB_AUDIO_FEATURE_UNIT_BASS_BOOST_LENGTH		0x01 //
#define USB_AUDIO_FEATURE_UNIT_LOUDNESS_LENGTH			0x01 //
#define USB_AUDIO_FEATURE_UNIT_INPUT_GAIN_LENGTH		0x01 //
#define USB_AUDIO_FEATURE_UNIT_INPUT_GAIN_PAD_LENGTH	0x01 //
#define USB_AUDIO_FEATURE_UNIT_PHASE_INVERTER_LENGTH	0x01 //
#define USB_AUDIO_FEATURE_UNIT_UNDERFLOW_LENGTH			0x01 //
#define USB_AUDIO_FEATURE_UNIT_OVERFLOW_LENGTH			0x01 //
#define USB_AUDIO_FEATURE_UNIT_LATENCY_LENGTH			0x01 //

// Table 4-7: Feature Unit Descriptor for V1.0
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_MUTE  (1 << 0) // Mute Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_VOLUME  (1 << 1) // Volume Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_BASS  (1 << 2) // Bass Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_MID  (1 << 3) // Mid Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_TREBLE  (1 << 4) // Treble Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_EQ  (1 << 5) // Graphic Equalizer Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_AGC  (1 << 6) // Automatic Gain Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_DELAY  (1 << 7) // Delay Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_BASS_BOOST  (1 << 8) // Bass Boost Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_LOUDNESS  (1 << 9) // Loudness Control

// Table 4-13: Feature Unit Descriptor for V2.0
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_MUTE_AVAILABLE  (1 << 0) // Mute Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_MUTE_WRITABLE (1 << 1) // Mute Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_VOLUME_AVAILABLE  (1 << 2) // Volume Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_VOLUME_WRITABLE (1 << 3) // Volume Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_BASS_AVAILABLE  (1 << 4) // Bass Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_BASS_WRITABLE (1 << 5) // Bass Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_MID_AVAILABLE  (1 << 6) // Mid Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_MID_WRITABLE (1 << 7) // Mid Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_TREBLE_AVAILABLE  (1 << 8) // Treble Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_TREBLE_WRITABLE (1 << 9) // Treble Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_EQ_AVAILABLE  (1 << 10) // Graphic Equalizer Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_EQ_WRITABLE (1 << 11) // Graphic Equalizer Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_AGC_AVAILABLE  (1 << 12) // Automatic Gain Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_AGC_WRITABLE (1 << 13) // Automatic Gain Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_DELAY_AVAILABLE  (1 << 14) // Delay Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_DELAY_WRITABLE (1 << 15) // Delay Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_BASS_BOOST_AVAILABLE  (1 << 16) // Bass Boost Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_BASS_BOOSTE_WRITABLE (1 << 17) // Bass Boost Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_LOUDNESS_AVAILABLE  (1 << 18) // Loudness Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_LOUDNESS_WRITABLE (1 << 19) // Loudness Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_INPUT_GAIN_AVAILABLE  (1 << 20) // Input Gain Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_INPUT_GAIN_WRITABLE (1 << 21) // Input Gain Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_INPUT_GAIN_PAD_AVAILABLE  (1 << 22) // Input Gain Pad Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_INPUT_GAIN_PAD_WRITABLE (1 << 23) // Input Gain Pad Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_PHASE_INV_AVAILABLE  (1 << 24) // Phase Inverter Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_PHASE_INV_WRITABLE (1 << 25) // Phase Inverter Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_UNDERFLOW_AVAILABLE  (1 << 26) // Underflow Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_UNDERFLOW_WRITABLE (1 << 27) // Underflow Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_OVERFLOW_AVAILABLE  (1 << 28) // Overfow Control
#define USB_AUDIO_FEATURE_UNIT_DESCRIPTOR_BMCONTROLS_20_OVERFLOW_WRITABLE (1 << 29) // Overfow Control

// Table 4-27: Audio Streaming bmControls for V2.0
#define USB_AUDIOSTREAMING_INTERFACE_DESCRIPTOR_BMCONTROLS_20_ALT_SET_AVAILABLE (1 << 0) // Active Alternate Setting Control
#define USB_AUDIOSTREAMING_INTERFACE_DESCRIPTOR_BMCONTROLS_20_ALT_VAL_AVAILABLE (1 << 2) // Valid Alternate Settings Control

// Table A-24: Parametric Equalizer Section Effect Unit Control Selectors
//		Control Selector											Value
#define USB_AUDIO_EFFECT_EQUALIZER_UNDEFINED			0x00 // PE_CONTROL_UNDEFINED
#define USB_AUDIO_EFFECT_EQUALIZER_ENABLE				0x01 // PE_ENABLE_CONTROL
#define USB_AUDIO_EFFECT_EQUALIZER_CENTERFREQ			0x02 // PE_CENTERFREQ_CONTROL
#define USB_AUDIO_EFFECT_EQUALIZER_QFACTOR				0x03 // PE_QFACTOR_CONTROL
#define USB_AUDIO_EFFECT_EQUALIZER_GAIN					0x04 // PE_GAIN_CONTROL
#define USB_AUDIO_EFFECT_EQUALIZER_UNDERFLOW			0x05 // PE_UNDERFLOW_CONTROL
#define USB_AUDIO_EFFECT_EQUALIZER_OVERFLOW				0x06 // PE_OVERFLOW_CONTROL
#define USB_AUDIO_EFFECT_EQUALIZER_LATENCY				0x07 // PE_LATENCY_CONTROL

// Table A-25: Reverberation Effect Unit Control Selectors
//		Control Selector											Value
#define USB_AUDIO_EFFECT_REVERB_UNDEFINED				0x00 // RV_CONTROL_UNDEFINED
#define USB_AUDIO_EFFECT_REVERB_ENABLE					0x01 // RV_ENABLE_CONTROL
#define USB_AUDIO_EFFECT_REVERB_TYPE					0x02 // RV_TYPE_CONTROL
#define USB_AUDIO_EFFECT_REVERB_LEVEL					0x03 // RV_LEVEL_CONTROL
#define USB_AUDIO_EFFECT_REVERB_TIME					0x04 // RV_TIME_CONTROL
#define USB_AUDIO_EFFECT_REVERB_FEEDBACK				0x05 // RV_FEEDBACK_CONTROL
#define USB_AUDIO_EFFECT_REVERB_PREDELAY				0x06 // RV_PREDELAY_CONTROL
#define USB_AUDIO_EFFECT_REVERB_DENSITY					0x07 // RV_DENSITY_CONTROL
#define USB_AUDIO_EFFECT_REVERB_HIFREQ_ROLLOFF			0x08 // RV_HIFREQ_ROLLOFF_CONTROL
#define USB_AUDIO_EFFECT_REVERB_UNDERFLOW				0x09 // RV_UNDERFLOW_CONTROL
#define USB_AUDIO_EFFECT_REVERB_OVERFLOW				0x0A // RV_OVERFLOW_CONTROL
#define USB_AUDIO_EFFECT_REVERB_LATENCY					0x0B // RV_LATENCY_CONTROL

// Table A-26: Modulation Delay Effect Unit Control Selectors
//		Control Selector											Value
#define USB_AUDIO_EFFECT_MODULATION_UNDEFINED			0x00 // MD_CONTROL_UNDEFINED
#define USB_AUDIO_EFFECT_MODULATION_ENABLE				0x01 // MD_ENABLE_CONTROL
#define USB_AUDIO_EFFECT_MODULATION_BALANCE				0x02 // MD_BALANCE_CONTROL
#define USB_AUDIO_EFFECT_MODULATION_RATE				0x03 // MD_RATE_CONTROL
#define USB_AUDIO_EFFECT_MODULATION_DEPTH				0x04 // MD_DEPTH_CONTROL
#define USB_AUDIO_EFFECT_MODULATION_TIME				0x05 // MD_TIME_CONTROL
#define USB_AUDIO_EFFECT_MODULATION_FEEDBACK			0x06 // MD_FEEDBACK_CONTROL
#define USB_AUDIO_EFFECT_MODULATION_UNDERFLOW			0x07 // MD_UNDERFLOW_CONTROL
#define USB_AUDIO_EFFECT_MODULATION_OVERFLOW			0x08 // MD_OVERFLOW_CONTROL
#define USB_AUDIO_EFFECT_MODULATION_LATENCY				0x09 // MD_LATENCY_CONTROL

// Table A-27: Dynamic Range Compressor Effect Unit Control Selectors
//		Control Selector											Value
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_UNDEFINED		0x00 // DR_CONTROL_UNDEFINED
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_ENABLE			0x01 // DR_ENABLE_CONTROL
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_COMPRESSION_RATE 0x02 // DR_COMPRESSION_RATE_CONTROL
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_MAXAMPL			0x03 // DR_MAXAMPL_CONTROL
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_THRESHOLD		0x04 // DR_THRESHOLD_CONTROL
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_ATTACK_TIME		0x05 // DR_ATTACK_TIME_CONTROL
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_RELEASE_TIME		0x06 // DR_RELEASE_TIME_CONTROL
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_UNDERFLOW		0x07 // DR_UNDERFLOW_CONTROL
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_OVERFLOW			0x08 // DR_OVERFLOW_CONTROL
#define USB_AUDIO_EFFECT_DYNAMIC_RANGE_LATENCY			0x09 // DR_LATENCY_CONTROL

// Table A-28: Up/Down-mix Processing Unit Control Selectors
//		Control Selector											Value
#define USB_AUDIO_PROCESS_UP_DOWN_MIX_UNDEFINED			0x00 // UD_CONTROL_UNDEFINED
#define USB_AUDIO_PROCESS_UP_DOWN_MIX_ENABLE			0x01 // UD_ENABLE_CONTROL
#define USB_AUDIO_PROCESS_UP_DOWN_MIX_MODE_SELECT		0x02 // UD_MODE_SELECT_CONTROL
#define USB_AUDIO_PROCESS_UP_DOWN_MIX_CLUSTER			0x03 // UD_CLUSTER_CONTROL
#define USB_AUDIO_PROCESS_UP_DOWN_MIX_UNDERFLOW			0x04 // UD_UNDERFLOW_CONTROL
#define USB_AUDIO_PROCESS_UP_DOWN_MIX_OVERFLOW			0x05 // UD_OVERFLOW_CONTROL
#define USB_AUDIO_PROCESS_UP_DOWN_MIX_LATENCY			0x06 // UD_LATENCY_CONTROL

// Table A-29: Dolby Prologic Processing Unit Control Selectors
//		Control Selector											Value
#define USB_AUDIO_PROCESS_DOLBY_PROLOGIC_UNDEFINED		0x00 // DP_CONTROL_UNDEFINED
#define USB_AUDIO_PROCESS_DOLBY_PROLOGIC_ENABLE			0x01 // DP_ENABLE_CONTROL
#define USB_AUDIO_PROCESS_DOLBY_PROLOGIC_MODE_SELECT	0x02 // DP_MODE_SELECT_CONTROL
#define USB_AUDIO_PROCESS_DOLBY_PROLOGIC_CLUSTER		0x03 // DP_CLUSTER_CONTROL
#define USB_AUDIO_PROCESS_DOLBY_PROLOGIC_UNDERFLOW		0x04 // DP_UNDERFLOW_CONTROL
#define USB_AUDIO_PROCESS_DOLBY_PROLOGIC_OVERFLOW		0x05 // DP_OVERFLOW_CONTROL
#define USB_AUDIO_PROCESS_DOLBY_PROLOGIC_LATENCY		0x06 // DP_LATENCY_CONTROL

// Table A-30: Stereo Extender Processing Unit Control Selectors
//		Control Selector											Value
#define USB_AUDIO_STEREO_EXTENDER_UNDEFINED				0x00 // ST_EXT_CONTROL_UNDEFINED
#define USB_AUDIO_STEREO_EXTENDER_ENABLE				0x01 // ST_EXT_ENABLE_CONTROL
#define USB_AUDIO_STEREO_EXTENDER_WIDTH					0x02 // ST_EXT_WIDTH_CONTROL
#define USB_AUDIO_STEREO_EXTENDER_UNDERFLOW				0x03 // ST_EXT_UNDERFLOW_CONTROL
#define USB_AUDIO_STEREO_EXTENDER_OVERFLOW				0x04 // ST_EXT_OVERFLOW_CONTROL
#define USB_AUDIO_STEREO_EXTENDER_LATENCY				0x05 // ST_EXT_LATENCY_CONTROL

// Table A-31: Extension Unit Control Selectors
//		Control Selector											Value
#define USB_AUDIO_EXTENSION_UNIT_UNDEFINED				0x00 // XU_CONTROL_UNDEFINED
#define USB_AUDIO_EXTENSION_UNIT_ENABLE					0x01 // XU_ENABLE_CONTROL
#define USB_AUDIO_EXTENSION_UNIT_CLUSTER				0x02 // XU_CLUSTER_CONTROL
#define USB_AUDIO_EXTENSION_UNIT_UNDERFLOW				0x03 // XU_UNDERFLOW_CONTROL
#define USB_AUDIO_EXTENSION_UNIT_OVERFLOW				0x04 // XU_OVERFLOW_CONTROL
#define USB_AUDIO_EXTENSION_UNIT_LATENCY				0x05 // XU_LATENCY_CONTROL

// Table A-32: AudioStreaming Interface Control Selectors
//		Control Selector											Value
#define USB_AUDIO_STREAMING_UNDEFINED					0x00 // AS_CONTROL_UNDEFINED
#define USB_AUDIO_STREAMING_ACT_ALT_SETTING				0x01 // AS_ACT_ALT_SETTING_CONTROL
#define USB_AUDIO_STREAMING_VAL_ALT_SETTINGS			0x02 // AS_VAL_ALT_SETTINGS_CONTROL
#define USB_AUDIO_STREAMING_AUDIO_DATA_FORMAT			0x03 // AS_AUDIO_DATA_FORMAT_CONTROL

// Table A-33: Encoder Control Selectors
//		Control Selector											Value
#define USB_AUDIO_ENCODER_UNDEFINED						0x00 // EN_CONTROL_UNDEFINED
#define USB_AUDIO_ENCODER_BIT_RATE						0x01 // EN_BIT_RATE_CONTROL
#define USB_AUDIO_ENCODER_QUALITY						0x02 // EN_QUALITY_CONTROL
#define USB_AUDIO_ENCODER_VBR							0x03 // EN_VBR_CONTROL
#define USB_AUDIO_ENCODER_TYPE							0x04 // EN_TYPE_CONTROL
#define USB_AUDIO_ENCODER_UNDERFLOW						0x05 // EN_UNDERFLOW_CONTROL
#define USB_AUDIO_ENCODER_OVERFLOW						0x06 // EN_OVERFLOW_CONTROL
#define USB_AUDIO_ENCODER_ENCODER_ERROR					0x07 // EN_ENCODER_ERROR_CONTROL
#define USB_AUDIO_ENCODER_PARAM1						0x08 // EN_PARAM1_CONTROL
#define USB_AUDIO_ENCODER_PARAM2						0x09 // EN_PARAM2_CONTROL
#define USB_AUDIO_ENCODER_PARAM3						0x0A // EN_PARAM3_CONTROL
#define USB_AUDIO_ENCODER_PARAM4						0x0B // EN_PARAM4_CONTROL
#define USB_AUDIO_ENCODER_PARAM5						0x0C // EN_PARAM5_CONTROL
#define USB_AUDIO_ENCODER_PARAM6						0x0D // EN_PARAM6_CONTROL
#define USB_AUDIO_ENCODER_PARAM7						0x0E // EN_PARAM7_CONTROL
#define USB_AUDIO_ENCODER_PARAM8						0x0F // EN_PARAM8_CONTROL

// Table A-34: MPEG Decoder Control Selectors
//		Control Selector											Value
#define USB_AUDIO_DECODER_MPEG_UNDEFINED				0x00 // MD_CONTROL_UNDEFINED
#define USB_AUDIO_DECODER_MPEG_DUAL_CHANNEL				0x01 // MD_DUAL_CHANNEL_CONTROL
#define USB_AUDIO_DECODER_MPEG_SECOND_STEREO			0x02 // MD_SECOND_STEREO_CONTROL
#define USB_AUDIO_DECODER_MPEG_MULTILINGUAL				0x03 // MD_MULTILINGUAL_CONTROL
#define USB_AUDIO_DECODER_MPEG_DYN_RANGE				0x04 // MD_DYN_RANGE_CONTROL
#define USB_AUDIO_DECODER_MPEG_SCALING					0x05 // MD_SCALING_CONTROL
#define USB_AUDIO_DECODER_MPEG_HILO_SCALING				0x06 // MD_HILO_SCALING_CONTROL
#define USB_AUDIO_DECODER_MPEG_UNDERFLOW				0x07 // MD_UNDERFLOW_CONTROL
#define USB_AUDIO_DECODER_MPEG_OVERFLOW					0x08 // MD_OVERFLOW_CONTROL
#define USB_AUDIO_DECODER_MPEG_DECODER_ERROR			0x09 // MD_DECODER_ERROR_CONTROL

// Table A-35: AC-3 Decoder Control Selectors
//		Control Selector											Value
#define USB_AUDIO_DECODER_AC_3_UNDEFINED				0x00 // AD_CONTROL_UNDEFINED
#define USB_AUDIO_DECODER_AC_3_MODE						0x01 // AD_MODE_CONTROL
#define USB_AUDIO_DECODER_AC_3_DYN_RANGE				0x02 // AD_DYN_RANGE_CONTROL
#define USB_AUDIO_DECODER_AC_3_SCALING					0x03 // AD_SCALING_CONTROL
#define USB_AUDIO_DECODER_AC_3_HILO_SCALING				0x04 // AD_HILO_SCALING_CONTROL
#define USB_AUDIO_DECODER_AC_3_UNDERFLOW				0x05 // AD_UNDERFLOW_CONTROL
#define USB_AUDIO_DECODER_AC_3_OVERFLOW					0x06 // AD_OVERFLOW_CONTROL
#define USB_AUDIO_DECODER_AC_3_DECODER_ERROR			0x07 // AD_DECODER_ERROR_CONTROL

// Table A-36: WMA Decoder Control Selectors
//		Control Selector											Value
#define USB_AUDIO_DECODER_WMA_UNDEFINED					0x00 // WD_CONTROL_UNDEFINED
#define USB_AUDIO_DECODER_WMA_UNDERFLOW					0x01 // WD_UNDERFLOW_CONTROL
#define USB_AUDIO_DECODER_WMA_OVERFLOW					0x02 // WD_OVERFLOW_CONTROL
#define USB_AUDIO_DECODER_WMA_DECODER_ERROR				0x03 // WD_DECODER_ERROR_CONTROL

// Table A-37: DTS Decoder Control Selectors
//		Control Selector											Value
#define USB_AUDIO_DECODER_DTS_UNDEFINED					0x00 // DD_CONTROL_UNDEFINED
#define USB_AUDIO_DECODER_DTS_UNDERFLOW					0x01 // DD_UNDERFLOW_CONTROL
#define USB_AUDIO_DECODER_DTS_OVERFLOW					0x02 // DD_OVERFLOW_CONTROL
#define USB_AUDIO_DECODER_DTS_DECODER_ERROR				0x03 // DD_DECODER_ERROR_CONTROL

// Table A-38: Endpoint Control Selectors
//		Control Selector											Value
#define USB_AUDIO_ENDPOINT_UNDEFINED					0x00 // EP_CONTROL_UNDEFINED
#define USB_AUDIO_ENDPOINT_PITCH						0x01 // EP_PITCH_CONTROL
#define USB_AUDIO_ENDPOINT_DATA_OVERRUN					0x02 // EP_DATA_OVERRUN_CONTROL
#define USB_AUDIO_ENDPOINT_DATA_UNDERRUN				0x03 // EP_DATA_UNDERRUN_CONTROL

#define USB_AUDIO_SPACIAL_FL (1 << 0) // D0: Front Left - FL
#define USB_AUDIO_SPACIAL_FR (1 << 1) // D1: Front Right - FR
#define USB_AUDIO_SPACIAL_FC (1 << 2) // D2: Front Center - FC
#define USB_AUDIO_SPACIAL_LFE (1 << 3) // D3: Low Frequency Effects - LFE
#define USB_AUDIO_SPACIAL_BL (1 << 4) // D4: Back Left - BL
#define USB_AUDIO_SPACIAL_BR (1 << 5)// D5: Back Right - BR
#define USB_AUDIO_SPACIAL_FLC (1 << 6) // D6: Front Left of Center - FLC
#define USB_AUDIO_SPACIAL_FRC (1 << 7) // D7: Front Right of Center - FRC
#define USB_AUDIO_SPACIAL_BC (1 << 8) // D8: Back Center - BC
#define USB_AUDIO_SPACIAL_SL (1 << 9) // D9: Side Left - SL
#define USB_AUDIO_SPACIAL_SR (1 << 10) // D10: Side Right - SR
#define USB_AUDIO_SPACIAL_TC (1 << 11) // D11: Top Center - TC
#define USB_AUDIO_SPACIAL_TFL (1 << 12) // D12: Top Front Left - TFL (V2.0 only)
#define USB_AUDIO_SPACIAL_TFC (1 << 13) // D13: Top Front Center - TFC (V2.0 only)
#define USB_AUDIO_SPACIAL_TFR (1 << 14) // D14: Top Front Right - TFR (V2.0 only)
#define USB_AUDIO_SPACIAL_TBL (1 << 15) // D15: Top Back Left - TBL (V2.0 only)
#define USB_AUDIO_SPACIAL_TBC (1 << 16) // D16: Top Back Center - TBC (V2.0 only)
#define USB_AUDIO_SPACIAL_TBR (1 << 17) // D17: Top Back Right – TBR (V2.0 only)
#define USB_AUDIO_SPACIAL_TFLC (1 << 18) // D18: Top Front Left of Center – TFLC (V2.0 only)
#define USB_AUDIO_SPACIAL_TFRC (1 << 19) // D19: Top Front Right of Center – TFRC (V2.0 only)
#define USB_AUDIO_SPACIAL_LLFE (1 << 20) // D20: Left Low Frequency Effects– LLFE (V2.0 only)
#define USB_AUDIO_SPACIAL_RLFE (1 << 21) // D21: Right Low Frequency Effects– RLFE (V2.0 only)
#define USB_AUDIO_SPACIAL_TSL (1 << 22) // D22: Top Side Left – TSL (V2.0 only)
#define USB_AUDIO_SPACIAL_TSR (1 << 23) // D23: Top Side Right – TSR (V2.0 only)
#define USB_AUDIO_SPACIAL_BTC (1 << 24) // D24: Bottom Center – BC (V2.0 only)
#define USB_AUDIO_SPACIAL_BLC (1 << 25) // D25: Back Left of Center – BLC (V2.0 only)
#define USB_AUDIO_SPACIAL_BRC (1 << 26) // D26: Back Right of Center – BRC (V2.0 only)
#define USB_AUDIO_SPACIAL_RD (1 << 31) // D31: Raw Data – RD; Mutually exclusive with all other spatial locations (V2.0 only)

// Standard Audio Control (AC) Interrupt Endpoint Descriptor
// Standard Audio Streaming (AS) Isochronous Audio Data Endpoint Descriptor
// Standard Audio Streaming (AS) Isochronous Feedback Endpoint Descriptor
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_512	9
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_256	8
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_128	7
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_64 	6
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_32 	5
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_16 	4
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_8  	3
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_4  	2
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_2 	1
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH_1 	0
#define USB_AUDIO_ENDPOINT_DESCRIPTOR_REFRESH(A) 	(1 << (A))

/* TYPES ***************************************************************************/

/* Structures with a postpended "_20" are specific to V2.0 of the USB Audio Devices
 * specification. If there is no postpended version then V1.0 is assumed.
 */

// V1.0 Table 4-1: Audio Channel Cluster Descriptor
typedef struct PACK _usb_audioChannelClusterDescriptor_t
{
    unsigned char bNrChannels;
    unsigned short wChannelConfig;
    unsigned char iChannelNames;
} usb_audioChannelClusterDescriptor_t;

// V2.0 Table 4-1: Audio Channel Cluster Descriptor
typedef struct PACK _usb_audioChannelClusterDescriptor_20_t
{
    unsigned char bNrChannels;
    unsigned long bmChannelConfig;
    unsigned char iChannelNames;
} usb_audioChannelClusterDescriptor_20_t;

// V2.0 Table 4-3: Standard Interface Association Descriptor
typedef struct PACK _usb_audioInterfaceAssociationDescriptor_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bFirstInterface;
    unsigned char bInterfaceCount;
    unsigned char bFunctionClass;
    unsigned char bFunctionSubClass;
    unsigned char bFunctionProtocol;
    unsigned char iFunction;
} usb_audioInterfaceAssociationDescriptor_t;

// V1.0 Table 4-1: Standard AC Interface Descriptor
typedef struct PACK _usb_audioControlInterfaceDescriptor_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bInterfaceNumber;
    unsigned char bAlternateSetting;
    unsigned char bNumEndpoints;
    unsigned char bInterfaceClass;
    unsigned char bInterfaceSubClass;
    unsigned char bInterfaceProtocol;
    unsigned char iInterface;
} usb_audioControlInterfaceDescriptor_t;

// V2.0 Table 4-4: Standard AC Interface Descriptor
typedef usb_audioControlInterfaceDescriptor_t usb_audioControlInterfaceDescriptor_20_t;

// V1.0 Table 4-2: Class-Specific AC Interface Header Descriptor
#define usb_audioControlInterfaceHeaderDescriptor_t(A) \
struct PACK \
{ \
    unsigned char  bLength; \
    unsigned char  bDescriptorType; \
    unsigned char  bDescriptorSubtype; \
    unsigned short bcdADC; \
    unsigned short wTotalLength; \
    unsigned char  bInCollection; \
    unsigned char  baInterfaceNr[A]; \
}

// V2.0 Table 4-5: Class-Specific AC Interface Header Descriptor
typedef struct PACK _usb_audioControlInterfaceHeaderDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned short bcdADC;
    unsigned char  bCategory;
    unsigned short wTotalLength;
    unsigned char  bmControls;
} usb_audioControlInterfaceHeaderDescriptor_20_t;

// V2.0 Table 4-6: Clock Source Descriptor
typedef struct PACK _usb_audioClockSourceDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bClockID;
    unsigned char bmAttributes;
    unsigned char bmControls;
    unsigned char bAssocTerminal;
    unsigned char iClockSource;
} usb_audioClockSourceDescriptor_20_t;

// V2.0 Table 4-7: Clock Selector Descriptor
#define usb_audioClockSelectorDescriptor_20_t(A) \
struct PACK \
{ \
    unsigned char bLength; \
    unsigned char bDescriptorType; \
    unsigned char bDescriptorSubtype; \
    unsigned char bClockID; \
    unsigned char bNrInPins; /* p */ \
    unsigned char baCSourceID[A]; \
    unsigned char bmControls; \
    unsigned char iClockSelector; \
}

// V2.0 Table 4-8: Clock Multiplier Descriptor
typedef struct PACK _usb_audioClockMultiplierDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bClockID;
    unsigned char bCSourceID;
    unsigned char bmControls;
    unsigned char iClockMultiplier;
} usb_audioClockMultiplierDescriptor_20_t;

// V1.0 Table 4-3: Input Terminal Descriptor
typedef struct PACK _usb_audioInputTerminalDescriptor_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bTerminalID;
    unsigned short wTerminalType;
    unsigned char  bAssocTerminal;
    unsigned char  bNrChannels;
    unsigned short wChannelConfig;
    unsigned char  iChannelNames;
    unsigned char  iTerminal;
} usb_audioInputTerminalDescriptor_t;

// V2.0 Table 4-9: Input Terminal Descriptor
typedef struct PACK _usb_audioInputTerminalDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bTerminalID;
    unsigned short wTerminalType;
    unsigned char  bAssocTerminal;
    unsigned char  bCSourceID;
    unsigned char  bNrChannels;
    unsigned long  bmChannelConfig;
    unsigned char  iChannelNames;
    unsigned short bmControls;
    unsigned char  iTerminal;
} usb_audioInputTerminalDescriptor_20_t;

// V1.0 Table 4-4: Output Terminal Descriptor
typedef struct PACK _usb_audioOutputTerminalDescriptor_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bTerminalID;
    unsigned short wTerminalType;
    unsigned char  bAssocTerminal;
    unsigned char  bSourceID;
    unsigned char  iTerminal;
} usb_audioOutputTerminalDescriptor_t;

// V2.0 Table 4-10: Output Terminal Descriptor
typedef struct PACK _usb_audioOutputTerminalDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bTerminalID;
    unsigned short wTerminalType;
    unsigned char  bAssocTerminal;
    unsigned char  bSourceID;
    unsigned char  bCSourceID;
    unsigned short bmControls;
    unsigned char  iTerminal;
} usb_audioOutputTerminalDescriptor_20_t;

// V1.0 Table 4-5: Mixer Unit Descriptor
#define usb_audioMixerUnitDescriptor_t(A) \
struct PACK \
{ \
    unsigned char bLength; \
    unsigned char bDescriptorType; \
    unsigned char bDescriptorSubtype; \
    unsigned char bUnitID; \
    unsigned char bNrInPins; /* p */ \
    unsigned char baSourceID[A]; \
    unsigned char bNrChannels; \
    unsigned long bmChannelConfig; \
    unsigned char iChannelNames; \
    unsigned char bmControls; \
    unsigned char iMixer; \
}

// V2.0 Table 4-11: Mixer Unit Descriptor
#define usb_audioMixerUnitDescriptor_20_t(A) \
struct PACK \
{ \
    unsigned char bLength; \
    unsigned char bDescriptorType; \
    unsigned char bDescriptorSubtype; \
    unsigned char bUnitID; \
    unsigned char bNrInPins; /* p */ \
    unsigned char baSourceID[A]; \
    unsigned char bNrChannels; \
    unsigned long bmChannelConfig; \
    unsigned char iChannelNames; \
    unsigned long bmMixerControls; /* NOTE: can be char, short or long */ \
    unsigned char bmControls; \
    unsigned char iMixer; \
}

// V1.0 Table 4-6: Selector Unit Descriptor
#define usb_audioSelectorUnitDescriptor_t(A) \
struct PACK \
{ \
    unsigned char bLength; \
    unsigned char bDescriptorType; \
    unsigned char bDescriptorSubtype; \
    unsigned char bUnitID; \
    unsigned char bNrInPins; \
    unsigned char baSourceID[A]; \
    unsigned char iSelector; \
}

// V2.0 Table 4-12: Selector Unit Descriptor
#define usb_audioSelectorUnitDescriptor_20_t(A) \
struct PACK \
{ \
    unsigned char bLength; \
    unsigned char bDescriptorType; \
    unsigned char bDescriptorSubtype; \
    unsigned char bUnitID; \
    unsigned char bNrInPins; \
    unsigned char baSourceID[A]; \
    unsigned char bmControls; \
    unsigned char iSelector; \
}

// V1.0 Table 4-7: Feature Unit Descriptor
#define usb_audioFeatureUnitDescriptor_t(A) \
struct PACK { \
    unsigned char bLength; \
    unsigned char bDescriptorType; \
    unsigned char bDescriptorSubtype; \
    unsigned char bUnitID; \
    unsigned char bSourceID; \
    unsigned char bControlSize; \
    unsigned char bmaControls0; \
    unsigned char bmaControls[A]; \
    unsigned char iFeature; \
}

// V2.0 Table 4-13: Feature Unit Descriptor
#define usb_audioFeatureUnitDescriptor_20_t(A) \
struct PACK { \
    unsigned char bLength; \
    unsigned char bDescriptorType; \
    unsigned char bDescriptorSubtype; \
    unsigned char bUnitID; \
    unsigned char bSourceID; \
    unsigned long bmaControls0; \
    unsigned long bmaControls[A]; \
    unsigned char iFeature; \
}

// V2.0 Table 4-14: Sampling Rate Converter Unit Descriptor
typedef struct PACK _usb_audioSamplingRateConverterUnitDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bUnitID;
    unsigned char bSourceID;
    unsigned char bCSourceInID;
    unsigned char bCSourceOutID;
    unsigned char iSRC;
} usb_audioSamplingRateConverterUnitDescriptor_20_t;

// V2.0 Table 4-15: Common Part of the Effect Unit Descriptor
// Also applies to...
// V2.0 Table 4-16: Parametric Equalizer Section Effect Unit Descriptor
// V2.0 Table 4-17: Reverberation Effect Unit Descriptor
// V2.0 Table 4-18: Modulation Delay Effect Unit Descriptor
// V2.0 Table 4-19: Dynamic Range Compressor Effect Unit Descriptor
#define usb_audioEffectUnitDescriptor_20_t(A) \
struct PACK \
{ \
    unsigned char  bLength; \
    unsigned char  bDescriptorType; \
    unsigned char  bDescriptorSubtype; \
    unsigned char  bUnitID; \
    unsigned short wEffectType; \
    unsigned char  bSourceID; \
    unsigned long  bmaControls[A]; \
    unsigned char  iEffects; \
}

// V1.0 Table 4-8: Processing Unit Descriptor
// V1.0 Table 4-9: Up/Down-mix Processing Unit Descriptor
// V1.0 Table 4-10: Dolby Prologic Processing Unit Descriptor
// V1.0 Table 4-11: 3D-Stereo Extender Processing Unit Descriptor
// V1.0 Table 4-12: Reverberation Processing Unit Descriptor
// V1.0 Table 4-13: Chorus Processing Unit Descriptor
// V1.0 Table 4-14: Dynamic Range Compressor Processing Unit Descriptor
#define usb_audioProcessUnitDescriptor_t(A) \
struct PACK \
{ \
    unsigned char  bLength; \
    unsigned char  bDescriptorType; \
    unsigned char  bDescriptorSubtype; \
    unsigned char  bUnitID; \
    unsigned short wProcessType; \
    unsigned char  bNrInPins; \
    unsigned char  baSourceID[A]; \
    unsigned char  bNrChannels; \
    unsigned short wChannelConfig; \
    unsigned char  iChannelNames; \
    unsigned char  bControlSize; \
    unsigned short bmControls; \
    unsigned char  iProcessing; \
    /* process specific descriptor appended here... */ \
}

// V2.0 Table 4-20: Common Part of the Processing Unit Descriptor
// Also applies to...
// V2.0 Table 4-21: Up/Down-mix Processing Unit Descriptor
// V2.0 Table 4-22: Dolby Prologic Processing Unit Descriptor
// V2.0 Table 4-23: Stereo Extender Processing Unit Descriptor
#define usb_audioProcessUnitDescriptor_20_t(A) \
struct PACK \
{ \
    unsigned char  bLength; \
    unsigned char  bDescriptorType; \
    unsigned char  bDescriptorSubtype; \
    unsigned char  bUnitID; \
    unsigned short wProcessType; \
    unsigned char  bNrInPins; \
    unsigned char  baSourceID[A]; \
    unsigned char  bNrChannels; \
    unsigned long  bmChannelConfig; \
    unsigned char  iChannelNames; \
    unsigned short bmControls; \
    unsigned char  iProcessing; \
    /* process specific descriptor appended here... */ \
}

// V1.0 Table 4-15: Extension Unit Descriptor
#define usb_audioExtensionUnitDescriptor_t(A) \
struct PACK { \
    unsigned char  bLength; \
    unsigned char  bDescriptorType; \
    unsigned char  bDescriptorSubtype; \
    unsigned char  bUnitID; \
    unsigned short wExtensionCode; \
    unsigned char  bNrInPins; \
    unsigned char  baSourceID[A]; \
    unsigned char  bNrChannels; \
    unsigned short wChannelConfig; \
    unsigned char  iChannelNames; \
    unsigned char  bmControls; \
    unsigned char  iExtension; \
}

// V2.0 Table 4-24: Extension Unit Descriptor
#define usb_audioExtensionUnitDescriptor_20_t(A) \
struct PACK { \
    unsigned char  bLength; \
    unsigned char  bDescriptorType; \
    unsigned char  bDescriptorSubtype; \
    unsigned char  bUnitID; \
    unsigned short wExtensionCode; \
    unsigned char  bNrInPins; \
    unsigned char  baSourceID[A]; \
    unsigned char  bNrChannels; \
    unsigned long  bmChannelConfig; \
    unsigned char  iChannelNames; \
    unsigned char  bmControls; \
    unsigned char  iExtension; \
}

// V1.0 Table 4-16: Associated Interfaces Descriptor
#define usb_audioAssociatedInterfacesDescriptor_t(A) \
struct PACK { \
{ \
    unsigned char  bLength; \
    unsigned char  bDescriptorType; \
    unsigned char  bDescriptorSubtype; \
    unsigned char  bInterfaceNr; \
    unsigned char  Association_specific[A]; \
}

// V1.0 Table 4-17: Standard Audio Control (AC) Interrupt Endpoint Descriptor
typedef struct PACK _usb_audioControlInterruptEndpointDescriptor_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bEndpointAddress;
    unsigned char  bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char  bInterval;
    unsigned char  bRefresh;
    unsigned char  bSynchAddress;
} usb_audioControlInterruptEndpointDescriptor_t;

// V2.0 Table 4-25: Standard Audio Control (AC) Interrupt Endpoint Descriptor
typedef struct PACK _usb_audioControlInterruptEndpointDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bEndpointAddress;
    unsigned char  bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char  bInterval;
} usb_audioControlInterruptEndpointDescriptor_20_t;

// V1.0 Table 4-18: Standard Audio Streaming (AS) Interface Descriptor
typedef struct PACK _usb_audioStreamingStandardInterfaceDescriptor_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bInterfaceNumber;
    unsigned char bAlternateSetting;
    unsigned char bNumEndpoints;
    unsigned char bInterfaceClass;
    unsigned char bInterfaceSubClass;
    unsigned char bInterfaceProtocol;
    unsigned char iInterface;
} usb_audioStreamingStandardInterfaceDescriptor_t;

// V2.0 Table 4-26: Standard Audio Streaming (AS) Interface Descriptor
typedef usb_audioStreamingStandardInterfaceDescriptor_t usb_audioStreamingStandardInterfaceDescriptor_20_t;

// V1.0 Table 4-19: Class-Specific Audio Streaming (AS) Interface Descriptor
typedef struct PACK _usb_audioStreamingClassInterfaceDescriptor_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bTerminalLink;
    unsigned char  bDelay;
    unsigned short wFormatTag;
} usb_audioStreamingClassInterfaceDescriptor_t;

// V2.0 Table 4-27: Class-Specific Audio Streaming (AS) Interface Descriptor
typedef struct PACK _usb_audioStreamingClassInterfaceDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bTerminalLink;
    unsigned char bmControls;
    unsigned char bFormatType;
    unsigned long bmFormats;
    unsigned char bNrChannels;
    unsigned long bmChannelConfig;
    unsigned char iChannelNames;
} usb_audioStreamingClassInterfaceDescriptor_20_t;

// V2.0 Table 4-28: Encoder Descriptor
typedef struct PACK _usb_audioEncoderDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bEncoderID;
    unsigned char bEncoder;
    unsigned long bmControls;
    unsigned char iParam1;
    unsigned char iParam2;
    unsigned char iParam3;
    unsigned char iParam4;
    unsigned char iParam5;
    unsigned char iParam6;
    unsigned char iParam7;
    unsigned char iParam8;
    unsigned char iEncoder;
} usb_audioEncoderDescriptor_20_t;

// V2.0 Table 4-29: MPEG Decoder Descriptor
typedef struct PACK _usb_audioMPEGDecoderDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bDecoderID;
    unsigned char  bDecoder;
    unsigned short bmMPEGCapabilities;
    unsigned char  bmMPEGFeatures;
    unsigned char  bmControls;
    unsigned char  iDecoder;
} usb_audioMPEGDecoderDescriptor_20_t;

// V2.0 Table 4-30: AC-3 Decoder Descriptor
typedef struct PACK _usb_audioAC3DecoderDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bDecoderID;
    unsigned char bDecoder;
    unsigned long bmBSID;
    unsigned char bmAC3Features;
    unsigned char bmControls;
    unsigned char iDecoder;
} usb_audioAC3DecoderDescriptor_20_t;

// V2.0 Table 4-31: WMA Decoder Descriptor
typedef struct PACK _usb_audioWMADecoderDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bDecoderID;
    unsigned char  bDecoder;
    unsigned short bmWMAProfile;
    unsigned char  bmControls;
    unsigned char  iDecoder;
} usb_audioWMADecoderDescriptor_20_t;

// V2.0 Table 4-32: DTS Decoder Descriptor
typedef struct PACK _usb_audioDTSDecoderDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bDecoderID;
    unsigned char bDecoder;
    unsigned char bmCapabilities;
    unsigned char bmControls;
    unsigned char iDecoder;
} usb_audioDTSDecoderDescriptor_20_t;

// V1.0 Table 4-20: Standard Audio Streaming (AS) Isochronous Audio Data Endpoint Descriptor
typedef struct PACK _usb_audioStreamingDataStandardEndpointDescriptor_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bEndpointAddress;
    unsigned char  bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char  bInterval;
    unsigned char  bRefresh;
    unsigned char  bSynchAddress;
} usb_audioStreamingDataStandardEndpointDescriptor_t;

// V2.0 Table 4-33: Standard Audio Streaming (AS) Isochronous Audio Data Endpoint Descriptor
typedef struct PACK _usb_audioStreamingDataStandardEndpointDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bEndpointAddress;
    unsigned char  bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char  bInterval;
} usb_audioStreamingDataStandardEndpointDescriptor_20_t;

// V1.0 Table 4-21: Class-Specific Audio Streaming (AS) Isochronous Audio Data Endpoint Descriptor
typedef struct PACK _usb_audioStreamingDataClassEndpointDescriptor_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bmAttributes;
    unsigned char  bmControls;
    unsigned char  bLockDelayUnits;
    unsigned short wLockDelay;
} usb_audioStreamingDataClassEndpointDescriptor_t;

// V2.0 Table 4-34: Class-Specific Audio Streaming (AS) Isochronous Audio Data Endpoint Descriptor
typedef usb_audioStreamingDataClassEndpointDescriptor_t usb_audioStreamingDataClassEndpointDescriptor_20_t;

// V1.0 Table 4-22: Standard Audio Streaming (AS) Isochronous Feedback Endpoint Descriptor
typedef struct PACK _usb_audioStreamingFeedbackStandardEndpointDescriptor_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bEndpointAddress;
    unsigned char  bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char  bInterval;
    unsigned char  bRefresh;
    unsigned char  bSynchAddress;
} usb_audioStreamingFeedbackStandardEndpointDescriptor_t;

// V2.0 Table 4-35: Standard Audio Streaming (AS) Isochronous Feedback Endpoint Descriptor
typedef struct PACK _usb_audioStreamingFeedbackStandardEndpointDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bEndpointAddress;
    unsigned char  bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char  bInterval;
} usb_audioStreamingFeedbackStandardEndpointDescriptor_20_t;

//  Table 5-3:  1-byte Control RANGE Parameter Block
#define usb_audioRangeLayout1_20_t(A) \
struct PACK { \
{ \
    unsigned short wNumSubRanges; \
    struct PACK {\
    	unsigned char   dMIN;\
    	unsigned char   dMAX;\
    	unsigned char   dRES;\
	} range_val [A];\
}

//  Table 5-5:  2-byte Control RANGE Parameter Block
#define usb_audioRangeLayout2_20_t(A) \
struct PACK { \
    unsigned short wNumSubRanges; \
    struct PACK {\
    	unsigned short   dMIN;\
    	unsigned short   dMAX;\
    	unsigned short   dRES;\
	} range_val [A];\
}

//  Table 5-7:  4-byte Control RANGE Parameter Block
#define usb_audioRangeLayout3_20_t(A) \
struct PACK { \
    unsigned short wNumSubRanges; \
    struct PACK {\
    	unsigned long   dMIN;\
    	unsigned long   dMAX;\
    	unsigned long   dRES;\
	} range_val [A];\
}

//  Table 5-14:  Valid Alternate Settings Control CUR Parameter Block
#define usb_validAlternateSettings_20_t(A) \
struct PACK { \
    unsigned char bControlSize; \
    unsigned char bmValidAltSettings[A];\
}

// *************************************************************************************************
//
// Definitions taken from USB Device Class Definition for Audio Data Formats V1.0
//
// *************************************************************************************************

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

// Table 2-2: Type I Format Type Descriptor
#define usb_audioFormatTypeIDescriptor_t(A) \
struct PACK \
{ \
    unsigned char bLength; \
    unsigned char bDescriptorType; \
    unsigned char bDescriptorSubtype; \
    unsigned char bFormatType; \
    unsigned char bNrChannels; \
    unsigned char bSubframeSize; \
    unsigned char bBitResolution; \
    unsigned char bSamFreqType; \
    unsigned char tSamFreq[A][3]; \
}

// Table 2-3: Type II Format Type Descriptor
#define _usb_audioFormatTypeIIDescriptor_t(A) \
struct PACK \
{ \
    unsigned char  bLength; \
    unsigned char  bDescriptorType; \
    unsigned char  bDescriptorSubtype; \
    unsigned char  bFormatType; \
    unsigned short wMaxBitRate; \
    unsigned short wSampolesPerFrame; \
    unsigned char  bSamFreqType; \
    unsigned char  tSamFreq[A][3]; \
}

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

// *************************************************************************************************
//
// Definitions taken from USB Device Class Definition for Audio Data Formats V2.0
//
// *************************************************************************************************

/* CONSTANTS ***********************************************************************/

// Table A-1: Format Type Codes
//		Format Type													Value
#define USB_AUDIO_FORMAT_TYPE_UNDEFINED 0x00 // FORMAT_TYPE_UNDEFINED
#define USB_AUDIO_FORMAT_TYPE_I			0x01 // FORMAT_TYPE_I
#define USB_AUDIO_FORMAT_TYPE_II		0x02 // FORMAT_TYPE_II
#define USB_AUDIO_FORMAT_TYPE_III		0x03 // FORMAT_TYPE_III
#define USB_AUDIO_FORMAT_TYPE_IV		0x04 // FORMAT_TYPE_IV
#define USB_AUDIO_FORMAT_TYPE_EXT_I		0x81 // EXT_FORMAT_TYPE_I
#define USB_AUDIO_FORMAT_TYPE_EXT_II	0x82 // EXT_FORMAT_TYPE_II
#define USB_AUDIO_FORMAT_TYPE_EXT_III	0x83 // EXT_FORMAT_TYPE_III

// Table A-2: Audio Data Format Type I Bit Allocations
// For use with the bmFormats element of usb_audioStreamingClassInterfaceDescriptor_t
//		Format														bmFormats
#define USB_AUDIO_BMFORMATS_TYPE_I_PCM		  (1 << 0)  // PCM
#define USB_AUDIO_BMFORMATS_TYPE_I_PCM8		  (1 << 1)  // PCM8
#define USB_AUDIO_BMFORMATS_TYPE_I_IEEE_FLOAT (1 << 2)  // IEEE_FLOAT
#define USB_AUDIO_BMFORMATS_TYPE_I_ALAW		  (1 << 3)  // ALAW
#define USB_AUDIO_BMFORMATS_TYPE_I_MULAW	  (1 << 4)  // MULAW
#define USB_AUDIO_BMFORMATS_TYPE_I_RAW_DATA	  (1 << 31) // TYPE_I_RAW_DATA

// Table A-3: Audio Data Format Type II Bit Allocations
// For use with the bmFormats element of usb_audioStreamingClassInterfaceDescriptor_t
//		Format														bmFormats
#define USB_AUDIO_BMFORMATS_TYPE_II_MPEG	 (1 << 0)  // MPEG
#define USB_AUDIO_BMFORMATS_TYPE_II_AC_3	 (1 << 1)  // AC-3
#define USB_AUDIO_BMFORMATS_TYPE_II_WMA		 (1 << 2)  // WMA
#define USB_AUDIO_BMFORMATS_TYPE_II_DTS		 (1 << 3)  // DTS
#define USB_AUDIO_BMFORMATS_TYPE_II_RAW_DATA (1 << 31) // TYPE_II_RAW_DATA

// Table A-4: Audio Data Format Type III Bit Allocations
// For use with the bmFormats element of usb_audioStreamingClassInterfaceDescriptor_t
//		Format														bmFormats
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_AC_3				 (1 << 0)  // IEC61937_AC-3
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_MPEG_1_LAYER1		 (1 << 1)  // IEC61937_MPEG-1_Layer1
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_MPEG_1_LAYER2_3	 (1 << 2)  // IEC61937_MPEG-1_Layer2/3 or
                                                                           // IEC61937_MPEG-2_NOEXT
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_MPEG_2_EXT		 (1 << 3)  // IEC61937_MPEG-2_EXT
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_MPEG_2_AAC_ADTS	 (1 << 4)  // IEC61937_MPEG-2_AAC_ADTS
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_MPEG_2_LAYER1_LS	 (1 << 5)  // IEC61937_MPEG-2_Layer1_LS
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_MPEG_2_LAYER2_3_LS (1 << 6)  // IEC61937_MPEG-2_Layer2/3_LS
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_DTS_I				 (1 << 7)  // IEC61937_DTS-I
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_DTS_II			 (1 << 8)  // IEC61937_DTS-II
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_DTS_III			 (1 << 9)  // IEC61937_DTS-III
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_ATRAC				 (1 << 10) // IEC61937_ATRAC
#define USB_AUDIO_BMFORMATS_TYPE_III_IEC61937_ATRAC_2_3			 (1 << 11) // IEC61937_ATRAC_2_3
#define USB_AUDIO_BMFORMATS_TYPE_III_WMA						 (1 << 12) // TYPE_III_WMA

// Table A-5: Audio Data Format Type IV Bit Allocations
// For use with the bmFormats element of usb_audioStreamingClassInterfaceDescriptor_t
//		Format														bmFormats
#define USB_AUDIO_BMFORMATS_TYPE_IV_PCM							(1 << 0)  // PCM
#define USB_AUDIO_BMFORMATS_TYPE_IV_PCM8						(1 << 1)  // PCM8
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEEE_FLOAT					(1 << 2)  // IEEE_FLOAT
#define USB_AUDIO_BMFORMATS_TYPE_IV_ALAW						(1 << 3)  // ALAW
#define USB_AUDIO_BMFORMATS_TYPE_IV_MULAW						(1 << 4)  // MULAW
#define USB_AUDIO_BMFORMATS_TYPE_IV_MPEG						(1 << 5)  // MPEG
#define USB_AUDIO_BMFORMATS_TYPE_IV_AC_3						(1 << 6)  // AC-3
#define USB_AUDIO_BMFORMATS_TYPE_IV_WMA							(1 << 7)  // WMA
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_AC_3				(1 << 8)  // IEC61937_AC-3
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_MPEG_1_LAYER1		(1 << 9)  // IEC61937_MPEG-1_Layer1
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_MPEG_1_LAYER2_3	(1 << 10) // IEC61937_MPEG-1_Layer2/3 or
                                                                          // IEC61937_MPEG-2_NOEXT
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_MPEG_2_EXT			(1 << 11) // IEC61937_MPEG-2_EXT
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_MPEG_2_AAC_ADTS	(1 << 12) // IEC61937_MPEG-2_AAC_ADTS
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_MPEG_2_LAYER1_LS	(1 << 13) // IEC61937_MPEG-2_Layer1_LS
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_MPEG_2_LAYER2_3_LS (1 << 14) // IEC61937_MPEG-2_Layer2/3_LS
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_DTS_I				(1 << 15) // IEC61937_DTS-I
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_DTS_II				(1 << 16) // IEC61937_DTS-II
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_DTS_III			(1 << 17) // IEC61937_DTS-III
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_ATRAC				(1 << 18) // IEC61937_ATRAC
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC61937_ATRAC_2_3			(1 << 19) // IEC61937_ATRAC_2_3
#define USB_AUDIO_BMFORMATS_TYPE_IV_TYPE_III_WMA				(1 << 20) // TYPE_III_WMA
#define USB_AUDIO_BMFORMATS_TYPE_IV_IEC60958_PCM				(1 << 21) // IEC60958_PCM

// Table A-6: Side Band Protocol Codes
//		Side Band Protocol											Value
#define USB_AUDIO_SIDE_BAND_PROTOCOL_UNDEFINED					0x00 // PROTOCOL_UNDEFINED
#define USB_AUDIO_SIDE_BAND_PROTOCOL_PRES_TIMESTAMP				0x01 // PRES_TIMESTAMP_PROTOCOL

/* TYPES ***************************************************************************/

// Table 2-2: Type I Format Type Descriptor
typedef struct PACK _usb_audioFormatTypeIDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bFormatType;
    unsigned char bSubslotSize;
    unsigned char bBitResolution;
} usb_audioFormatTypeIDescriptor_20_t;

// Table 2-3: Type II Format Type Descriptor
typedef struct PACK _usb_audioFormatTypeIIDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bFormatType;
    unsigned short wMaxBitRate;
    unsigned short wSlotsPerFrame;
} usb_audioFormatTypeIIDescriptor_20_t;

// Table 2-4: Type III Format Type Descriptor
typedef struct PACK _usb_audioFormatTypeIIIDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bFormatType;
    unsigned char bSubslotSize;
    unsigned char bBitResolution;
} usb_audioFormatTypeIIIDescriptor_20_t;

// Table 2-5: Type IV Format Type Descriptor
typedef struct PACK _usb_audioFormatTypeIVDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bFormatType;
} usb_audioFormatTypeIVDescriptor_20_t;

// Table 2-6: Extended Type I Format Type Descriptor
typedef struct PACK _usb_audioFormatTypeIExtendedDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bFormatType;
    unsigned char bSubslotSize;
    unsigned char bBitResolution;
    unsigned char bHeaderLength;
    unsigned char bControlSize;
    unsigned char bSideBandProtocol;
} usb_audioFormatTypeIExtendedDescriptor_20_t;

// Table 2-7: Extended Type II Format Type Descriptor
typedef struct PACK _usb_audioFormatTypeIIExtendedDescriptor_20_t
{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bDescriptorSubtype;
    unsigned char  bFormatType;
    unsigned short wMaxBitRate;
    unsigned short wSamplesPerFrame;
    unsigned char  bHeaderLength;
    unsigned char  bSideBandProtocol;
} usb_audioFormatTypeIIExtendedDescriptor_20_t;

// Table 2-8: Extended Type III Format Type Descriptor
typedef struct PACK _usb_audioFormatTypeIIIExtendedDescriptor_20_t
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bDescriptorSubtype;
    unsigned char bFormatType;
    unsigned char bSubslotSize;
    unsigned char bBitResolution;
    unsigned char bHeaderLength;
    unsigned char bSideBandProtocol;
} usb_audioFormatTypeIIIExtendedDescriptor_20_t;

// Table 2-9: Hi-Res Presentation TimeStamp Layout
typedef struct PACK _usb_audioHiResPresentationTimeStamp_20_t
{
    unsigned long bmFlags;
    unsigned long qNanoSecondsHi;
    unsigned long qNanoSecondsLo;
} usb_audioHiResPresentationTimeStamp_20_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

// *************************************************************************************************
//
// Definitions taken from USB Device Class Definition for Terminal Types V2.0
//
// *************************************************************************************************

/* CONSTANTS ***********************************************************************/

// Table 2-1: USB Terminal Types
#define USB_AUDIO_TERMINAL_TYPE_UNDEFINED 0x0100 // Undefined Type.
#define USB_AUDIO_TERMINAL_TYPE_STREAMING 0x0101 // AudioStreaming interface.
#define USB_AUDIO_TERMINAL_TYPE_VENDOR_SPECIFIC 0x01FF // Vendor-specific interface.

// Table 2-2: Input Terminal Types
#define USB_AUDIO_TERMINAL_TYPE_INPUT_UNDEFINED 0x0200 // Input Terminal, undefined Type.
#define USB_AUDIO_TERMINAL_TYPE_INPUT_MICROPHONE 0x0201 // A generic microphone.
#define USB_AUDIO_TERMINAL_TYPE_INPUT_DESKTOP_MICROPHONE 0x0202 // A microphone normally placed on the desktop or integrated into the monitor.
#define USB_AUDIO_TERMINAL_TYPE_INPUT_PERSONAL_MICROPHONE 0x0203 // A head-mounted or clip-on microphone.
#define USB_AUDIO_TERMINAL_TYPE_INPUT_OMNI_DIR_MICROPHONE 0x0204 // A microphone designed to pick up voice from more than one speaker.
#define USB_AUDIO_TERMINAL_TYPE_INPUT_MICROPHONE_ARRAY 0x0205 // An array of microphones.
#define USB_AUDIO_TERMINAL_TYPE_INPUT_PROCESSING_MICROPHONE_ARRAY 0x0206 // An array of microphones with an embedded signal processor.

// Table 2-3: Output Terminal Types
//Terminal Type Code
#define USB_AUDIO_TERMINAL_TYPE_OUTPUT_UNDEFINED 0x0300 // Output Terminal, undefined Type.
#define USB_AUDIO_TERMINAL_TYPE_OUTPUT_SPEAKER 0x0301 // A generic speaker or set of speakers.
#define USB_AUDIO_TERMINAL_TYPE_OUTPUT_HEADPHONES 0x0302 // A head-mounted audio output device.
#define USB_AUDIO_TERMINAL_TYPE_OUTPUT_HEAD_MOUNTED_AUDIO 0x0303 // The audio part of a VR head mounted display.
#define USB_AUDIO_TERMINAL_TYPE_OUTPUT_ROOM_SPEAKER 0x0305 // Larger speaker or set of speakers.
#define USB_AUDIO_TERMINAL_TYPE_OUTPUT_COMMS_SPEAKER 0x0306 // Speaker or set of speakers designed for voice communication.
#define USB_AUDIO_TERMINAL_TYPE_OUTPUT_LOW_FREQ_EFFECTS 0x0307 // Speaker designed for low frequencies (subwoofer).

/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USB_AUDIO_H_ */
