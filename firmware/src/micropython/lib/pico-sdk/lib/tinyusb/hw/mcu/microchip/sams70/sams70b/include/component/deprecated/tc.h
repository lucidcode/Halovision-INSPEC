/**
 * \file
 * \brief This file is for deprecated macro constants
 *
 *  Used to mark macro constants as deprecate.
 *  Uses a macro 'DEPRECTAED' to give warnings during compilation (GCC >= 4.8)
 *
 *  \remark To use, add defines and put a DEPRECATED statement between the macro name and the value
 *
 *  \note This file is manually maintained
 */

#ifndef _SAMS70_TC_COMPONENT_DEPRECATED_H_
#define _SAMS70_TC_COMPONENT_DEPRECATED_H_

#ifndef DEPRECATED
#define _DEP_STRING(X) #X

/** \hideinitializer
 * \brief Macro deprecation mark
 *
 * Putting this in a macro definition will emit deprecation warning when given
 * macro is used (GCC 4.8)
 *
 *  \code{.c}
 *  #define OLD_MACRO DEPRECATED(OLD_MACRO, "deprecated <or any other text>") <value>
 *  \endcode
 *
 *  \warning Using these macros in #if statements will not work
 */
#if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 408
#define DEPRECATED(macro, message) _Pragma (_DEP_STRING(GCC warning message))
#else
#define DEPRECATED(macro, message)
#endif
#endif

/* deprecated defines added below here */
#define TC_CMR_ABETRG_Msk DEPRECATED(TC_CMR_ABETRG_Msk, "Using deprecated macro TC_CMR_ABETRG_Msk") (_U_(0x1) << TC_CMR_ABETRG_Pos) /**< (TC_CMR) TIOAx or TIOBx External Trigger Selection Mask */
#define TC_CMR_ABETRG_Pos DEPRECATED(TC_CMR_ABETRG_Pos, "Using deprecated macro TC_CMR_ABETRG_Pos") 10 /**< (TC_CMR) TIOAx or TIOBx External Trigger Selection Position */
#define TC_CMR_ABETRG DEPRECATED(TC_CMR_ABETRG, "Using deprecated macro TC_CMR_ABETRG") TC_CMR_ABETRG_Msk /**< \deprecated Old style mask definition for 1 bit bitfield. Use TC_CMR_ABETRG_Msk instead */
#define TC_CMR_CPCTRG_Msk DEPRECATED(TC_CMR_CPCTRG_Msk, "Using deprecated macro TC_CMR_CPCTRG_Msk") (_U_(0x1) << TC_CMR_CPCTRG_Pos) /**< (TC_CMR) RC Compare Trigger Enable Mask */
#define TC_CMR_CPCTRG_Pos DEPRECATED(TC_CMR_CPCTRG_Pos, "Using deprecated macro TC_CMR_CPCTRG_Pos") 14 /**< (TC_CMR) RC Compare Trigger Enable Position */
#define TC_CMR_CPCTRG DEPRECATED(TC_CMR_CPCTRG, "Using deprecated macro TC_CMR_CPCTRG") TC_CMR_CPCTRG_Msk /**< \deprecated Old style mask definition for 1 bit bitfield. Use TC_CMR_CPCTRG_Msk instead */
#define TC_CMR_ETRGEDG_EDGE DEPRECATED(TC_CMR_ETRGEDG_EDGE, "Using deprecated macro TC_CMR_ETRGEDG_EDGE") (TC_CMR_ETRGEDG_EDGE_Val << TC_CMR_ETRGEDG_Pos) /**< (TC_CMR) Each edge Position */
#define TC_CMR_ETRGEDG_EDGE_Val DEPRECATED(TC_CMR_ETRGEDG_EDGE_Val, "Using deprecated macro TC_CMR_ETRGEDG_EDGE_Val") _U_(0x3) /**< (TC_CMR) Each edge */
#define TC_CMR_ETRGEDG_FALLING DEPRECATED(TC_CMR_ETRGEDG_FALLING, "Using deprecated macro TC_CMR_ETRGEDG_FALLING") (TC_CMR_ETRGEDG_FALLING_Val << TC_CMR_ETRGEDG_Pos) /**< (TC_CMR) Falling edge Position */
#define TC_CMR_ETRGEDG_FALLING_Val DEPRECATED(TC_CMR_ETRGEDG_FALLING_Val, "Using deprecated macro TC_CMR_ETRGEDG_FALLING_Val") _U_(0x2) /**< (TC_CMR) Falling edge */
#define TC_CMR_ETRGEDG_Msk DEPRECATED(TC_CMR_ETRGEDG_Msk, "Using deprecated macro TC_CMR_ETRGEDG_Msk") (_U_(0x3) << TC_CMR_ETRGEDG_Pos) /**< (TC_CMR) External Trigger Edge Selection Mask */
#define TC_CMR_ETRGEDG_NONE DEPRECATED(TC_CMR_ETRGEDG_NONE, "Using deprecated macro TC_CMR_ETRGEDG_NONE") (TC_CMR_ETRGEDG_NONE_Val << TC_CMR_ETRGEDG_Pos) /**< (TC_CMR) The clock is not gated by an external signal. Position */
#define TC_CMR_ETRGEDG_NONE_Val DEPRECATED(TC_CMR_ETRGEDG_NONE_Val, "Using deprecated macro TC_CMR_ETRGEDG_NONE_Val") _U_(0x0) /**< (TC_CMR) The clock is not gated by an external signal. */
#define TC_CMR_ETRGEDG_Pos DEPRECATED(TC_CMR_ETRGEDG_Pos, "Using deprecated macro TC_CMR_ETRGEDG_Pos") 8 /**< (TC_CMR) External Trigger Edge Selection Position */
#define TC_CMR_ETRGEDG_RISING DEPRECATED(TC_CMR_ETRGEDG_RISING, "Using deprecated macro TC_CMR_ETRGEDG_RISING") (TC_CMR_ETRGEDG_RISING_Val << TC_CMR_ETRGEDG_Pos) /**< (TC_CMR) Rising edge Position */
#define TC_CMR_ETRGEDG_RISING_Val DEPRECATED(TC_CMR_ETRGEDG_RISING_Val, "Using deprecated macro TC_CMR_ETRGEDG_RISING_Val") _U_(0x1) /**< (TC_CMR) Rising edge */
#define TC_CMR_ETRGEDG(value) DEPRECATED(TC_CMR_ETRGEDG(value), "Using deprecated macro TC_CMR_ETRGEDG(value)") (TC_CMR_ETRGEDG_Msk & ((value) << TC_CMR_ETRGEDG_Pos))
#define TC_CMR_LDBDIS_Msk DEPRECATED(TC_CMR_LDBDIS_Msk, "Using deprecated macro TC_CMR_LDBDIS_Msk") (_U_(0x1) << TC_CMR_LDBDIS_Pos) /**< (TC_CMR) Counter Clock Disable with RB Loading Mask */
#define TC_CMR_LDBDIS_Pos DEPRECATED(TC_CMR_LDBDIS_Pos, "Using deprecated macro TC_CMR_LDBDIS_Pos") 7 /**< (TC_CMR) Counter Clock Disable with RB Loading Position */
#define TC_CMR_LDBDIS DEPRECATED(TC_CMR_LDBDIS, "Using deprecated macro TC_CMR_LDBDIS") TC_CMR_LDBDIS_Msk /**< \deprecated Old style mask definition for 1 bit bitfield. Use TC_CMR_LDBDIS_Msk instead */
#define TC_CMR_LDBSTOP_Msk DEPRECATED(TC_CMR_LDBSTOP_Msk, "Using deprecated macro TC_CMR_LDBSTOP_Msk") (_U_(0x1) << TC_CMR_LDBSTOP_Pos) /**< (TC_CMR) Counter Clock Stopped with RB Loading Mask */
#define TC_CMR_LDBSTOP_Pos DEPRECATED(TC_CMR_LDBSTOP_Pos, "Using deprecated macro TC_CMR_LDBSTOP_Pos") 6 /**< (TC_CMR) Counter Clock Stopped with RB Loading Position */
#define TC_CMR_LDBSTOP DEPRECATED(TC_CMR_LDBSTOP, "Using deprecated macro TC_CMR_LDBSTOP") TC_CMR_LDBSTOP_Msk /**< \deprecated Old style mask definition for 1 bit bitfield. Use TC_CMR_LDBSTOP_Msk instead */
#define TC_CMR_LDRA_EDGE DEPRECATED(TC_CMR_LDRA_EDGE, "Using deprecated macro TC_CMR_LDRA_EDGE") (TC_CMR_LDRA_EDGE_Val << TC_CMR_LDRA_Pos) /**< (TC_CMR) Each edge of TIOAx Position */
#define TC_CMR_LDRA_EDGE_Val DEPRECATED(TC_CMR_LDRA_EDGE_Val, "Using deprecated macro TC_CMR_LDRA_EDGE_Val") _U_(0x3) /**< (TC_CMR) Each edge of TIOAx */
#define TC_CMR_LDRA_FALLING DEPRECATED(TC_CMR_LDRA_FALLING, "Using deprecated macro TC_CMR_LDRA_FALLING") (TC_CMR_LDRA_FALLING_Val << TC_CMR_LDRA_Pos) /**< (TC_CMR) Falling edge of TIOAx Position */
#define TC_CMR_LDRA_FALLING_Val DEPRECATED(TC_CMR_LDRA_FALLING_Val, "Using deprecated macro TC_CMR_LDRA_FALLING_Val") _U_(0x2) /**< (TC_CMR) Falling edge of TIOAx */
#define TC_CMR_LDRA_Msk DEPRECATED(TC_CMR_LDRA_Msk, "Using deprecated macro TC_CMR_LDRA_Msk") (_U_(0x3) << TC_CMR_LDRA_Pos) /**< (TC_CMR) RA Loading Edge Selection Mask */
#define TC_CMR_LDRA_NONE DEPRECATED(TC_CMR_LDRA_NONE, "Using deprecated macro TC_CMR_LDRA_NONE") (TC_CMR_LDRA_NONE_Val << TC_CMR_LDRA_Pos) /**< (TC_CMR) None Position */
#define TC_CMR_LDRA_NONE_Val DEPRECATED(TC_CMR_LDRA_NONE_Val, "Using deprecated macro TC_CMR_LDRA_NONE_Val") _U_(0x0) /**< (TC_CMR) None */
#define TC_CMR_LDRA_Pos DEPRECATED(TC_CMR_LDRA_Pos, "Using deprecated macro TC_CMR_LDRA_Pos") 16 /**< (TC_CMR) RA Loading Edge Selection Position */
#define TC_CMR_LDRA_RISING DEPRECATED(TC_CMR_LDRA_RISING, "Using deprecated macro TC_CMR_LDRA_RISING") (TC_CMR_LDRA_RISING_Val << TC_CMR_LDRA_Pos) /**< (TC_CMR) Rising edge of TIOAx Position */
#define TC_CMR_LDRA_RISING_Val DEPRECATED(TC_CMR_LDRA_RISING_Val, "Using deprecated macro TC_CMR_LDRA_RISING_Val") _U_(0x1) /**< (TC_CMR) Rising edge of TIOAx */
#define TC_CMR_LDRA(value) DEPRECATED(TC_CMR_LDRA, "Using deprecated macro TC_CMR_LDRA") (TC_CMR_LDRA_Msk & ((value) << TC_CMR_LDRA_Pos))
#define TC_CMR_LDRB_EDGE DEPRECATED(TC_CMR_LDRB_EDGE, "Using deprecated macro TC_CMR_LDRB_EDGE") (TC_CMR_LDRB_EDGE_Val << TC_CMR_LDRB_Pos) /**< (TC_CMR) Each edge of TIOAx Position */
#define TC_CMR_LDRB_EDGE_Val DEPRECATED(TC_CMR_LDRB_EDGE_Val, "Using deprecated macro TC_CMR_LDRB_EDGE_Val") _U_(0x3) /**< (TC_CMR) Each edge of TIOAx */
#define TC_CMR_LDRB_FALLING DEPRECATED(TC_CMR_LDRB_FALLING, "Using deprecated macro TC_CMR_LDRB_FALLING") (TC_CMR_LDRB_FALLING_Val << TC_CMR_LDRB_Pos) /**< (TC_CMR) Falling edge of TIOAx Position */
#define TC_CMR_LDRB_FALLING_Val DEPRECATED(TC_CMR_LDRB_FALLING_Val, "Using deprecated macro TC_CMR_LDRB_FALLING_Val") _U_(0x2) /**< (TC_CMR) Falling edge of TIOAx */
#define TC_CMR_LDRB_Msk DEPRECATED(TC_CMR_LDRB_Msk, "Using deprecated macro TC_CMR_LDRB_Msk") (_U_(0x3) << TC_CMR_LDRB_Pos) /**< (TC_CMR) RB Loading Edge Selection Mask */
#define TC_CMR_LDRB_NONE DEPRECATED(TC_CMR_LDRB_NONE, "Using deprecated macro TC_CMR_LDRB_NONE") (TC_CMR_LDRB_NONE_Val << TC_CMR_LDRB_Pos) /**< (TC_CMR) None Position */
#define TC_CMR_LDRB_NONE_Val DEPRECATED(TC_CMR_LDRB_NONE_Val, "Using deprecated macro TC_CMR_LDRB_NONE_Val") _U_(0x0) /**< (TC_CMR) None */
#define TC_CMR_LDRB_Pos DEPRECATED(TC_CMR_LDRB_Pos, "Using deprecated macro TC_CMR_LDRB_Pos") 18 /**< (TC_CMR) RB Loading Edge Selection Position */
#define TC_CMR_LDRB_RISING DEPRECATED(TC_CMR_LDRB_RISING, "Using deprecated macro TC_CMR_LDRB_RISING") (TC_CMR_LDRB_RISING_Val << TC_CMR_LDRB_Pos) /**< (TC_CMR) Rising edge of TIOAx Position */
#define TC_CMR_LDRB_RISING_Val DEPRECATED(TC_CMR_LDRB_RISING_Val, "Using deprecated macro TC_CMR_LDRB_RISING_Val") _U_(0x1) /**< (TC_CMR) Rising edge of TIOAx */
#define TC_CMR_LDRB(value) DEPRECATED(TC_CMR_LDRB, "Using deprecated macro TC_CMR_LDRB") (TC_CMR_LDRB_Msk & ((value) << TC_CMR_LDRB_Pos))
#define TC_CMR_SBSMPLR_EIGHTH DEPRECATED(TC_CMR_SBSMPLR_EIGHTH, "Using deprecated macro TC_CMR_SBSMPLR_EIGHTH") (TC_CMR_SBSMPLR_EIGHTH_Val << TC_CMR_SBSMPLR_Pos) /**< (TC_CMR) Load a Capture Register every 8 selected edges Position */
#define TC_CMR_SBSMPLR_EIGHTH_Val DEPRECATED(TC_CMR_SBSMPLR_EIGHTH_Val, "Using deprecated macro TC_CMR_SBSMPLR_EIGHTH_Val") _U_(0x3) /**< (TC_CMR) Load a Capture Register every 8 selected edges */
#define TC_CMR_SBSMPLR_FOURTH DEPRECATED(TC_CMR_SBSMPLR_FOURTH, "Using deprecated macro TC_CMR_SBSMPLR_FOURTH") (TC_CMR_SBSMPLR_FOURTH_Val << TC_CMR_SBSMPLR_Pos) /**< (TC_CMR) Load a Capture Register every 4 selected edges Position */
#define TC_CMR_SBSMPLR_FOURTH_Val DEPRECATED(TC_CMR_SBSMPLR_FOURTH_Val, "Using deprecated macro TC_CMR_SBSMPLR_FOURTH_Val") _U_(0x2) /**< (TC_CMR) Load a Capture Register every 4 selected edges */
#define TC_CMR_SBSMPLR_HALF DEPRECATED(TC_CMR_SBSMPLR_HALF, "Using deprecated macro TC_CMR_SBSMPLR_HALF") (TC_CMR_SBSMPLR_HALF_Val << TC_CMR_SBSMPLR_Pos) /**< (TC_CMR) Load a Capture Register every 2 selected edges Position */
#define TC_CMR_SBSMPLR_HALF_Val DEPRECATED(TC_CMR_SBSMPLR_HALF_Val, "Using deprecated macro TC_CMR_SBSMPLR_HALF_Val") _U_(0x1) /**< (TC_CMR) Load a Capture Register every 2 selected edges */
#define TC_CMR_SBSMPLR_Msk DEPRECATED(TC_CMR_SBSMPLR_Msk, "Using deprecated macro TC_CMR_SBSMPLR_Msk") (_U_(0x7) << TC_CMR_SBSMPLR_Pos) /**< (TC_CMR) Loading Edge Subsampling Ratio Mask */
#define TC_CMR_SBSMPLR_ONE DEPRECATED(TC_CMR_SBSMPLR_ONE, "Using deprecated macro TC_CMR_SBSMPLR_ONE") (TC_CMR_SBSMPLR_ONE_Val << TC_CMR_SBSMPLR_Pos) /**< (TC_CMR) Load a Capture Register each selected edge Position */
#define TC_CMR_SBSMPLR_ONE_Val DEPRECATED(TC_CMR_SBSMPLR_ONE_Val, "Using deprecated macro TC_CMR_SBSMPLR_ONE_Val") _U_(0x0) /**< (TC_CMR) Load a Capture Register each selected edge */
#define TC_CMR_SBSMPLR_Pos DEPRECATED(TC_CMR_SBSMPLR_Pos, "Using deprecated macro TC_CMR_SBSMPLR_Pos") 20 /**< (TC_CMR) Loading Edge Subsampling Ratio Position */
#define TC_CMR_SBSMPLR_SIXTEENTH DEPRECATED(TC_CMR_SBSMPLR_SIXTEENTH, "Using deprecated macro TC_CMR_SBSMPLR_SIXTEENTH") (TC_CMR_SBSMPLR_SIXTEENTH_Val << TC_CMR_SBSMPLR_Pos) /**< (TC_CMR) Load a Capture Register every 16 selected edges Position */
#define TC_CMR_SBSMPLR_SIXTEENTH_Val DEPRECATED(TC_CMR_SBSMPLR_SIXTEENTH_Val, "Using deprecated macro TC_CMR_SBSMPLR_SIXTEENTH_Val") _U_(0x4) /**< (TC_CMR) Load a Capture Register every 16 selected edges */
#define TC_CMR_SBSMPLR(value) DEPRECATED(TC_CMR_SBSMPLR, "Using deprecated macro TC_CMR_SBSMPLR") (TC_CMR_SBSMPLR_Msk & ((value) << TC_CMR_SBSMPLR_Pos))
#endif
