/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "pin.h"
#include "fsl_clock.h"
#include "fsl_iomuxc.h"
#include "hal/pwm_backport.h"

#define PWM_MIDDLE       (0)
#define PWM_BEGIN        (1)
#define PWM_END          (2)

#define PWM_CHANNEL1     (1)
#define PWM_CHANNEL2     (2)

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    PWM_Type *instance;
    bool is_flexpwm;
    uint8_t complementary;
    uint8_t module;
    uint8_t submodule;
    uint8_t channel1;
    uint8_t channel2;
    uint8_t invert;
    bool sync;
    uint32_t freq;
    int16_t prescale;
    uint16_t duty_u16;
    uint32_t duty_ns;
    uint16_t center;
    uint32_t deadtime;
    bool output_enable_1;
    bool output_enable_2;
    uint8_t xor;
    bool is_init;
} machine_pwm_obj_t;

static char channel_char[] = {'B', 'A', 'X' };
static char *ERRMSG_FREQ = "PWM frequency too low";
static char *ERRMSG_INIT = "PWM set-up failed";
static char *ERRMSG_VALUE = "value larger than period";

STATIC void machine_pwm_start(machine_pwm_obj_t *self);

STATIC void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->is_flexpwm) {
        mp_printf(print, "<FLEXPWM module=%u submodule=%u ", self->module, self->submodule);
        if (self->complementary) {
            mp_printf(print, "channel=%c/%c", channel_char[self->channel1], channel_char[self->channel2]);
        } else {
            mp_printf(print, "channel=%c", channel_char[self->channel1]);
        }
        if (self->duty_ns != 0) {
            mp_printf(print, " duty_ns=%u", self->duty_ns);
        } else {
            mp_printf(print, " duty_u16=%u", self->duty_u16);
        }
        mp_printf(print, " freq=%u center=%u, deadtime=%u, sync=%u>",
            self->freq, self->center, self->deadtime, self->sync);
    #ifdef FSL_FEATURE_SOC_TMR_COUNT
    } else {
        mp_printf(print, "<QTMR_PWM module=%u channel=%u freq1=%u ",
            self->module, self->channel1, self->freq);
        if (self->duty_ns != 0) {
            mp_printf(print, "duty_ns=%u>", self->duty_ns);
        } else {
            mp_printf(print, "duty_u16=%u>", self->duty_u16);
        }
    #endif
    }
}

// Utility functions for decoding and convertings
//
STATIC uint32_t duty_ns_to_duty_u16(uint32_t freq, uint32_t duty_ns) {
    uint64_t duty = (uint64_t)duty_ns * freq * PWM_FULL_SCALE / 1000000000ULL;
    if (duty >= PWM_FULL_SCALE) {
        mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_VALUE));
    }
    return (uint32_t)duty;
}

STATIC uint8_t module_decode(char channel) {
    switch (channel) {
        case '0':
            return kPWM_Module_0;
        case '1':
            return kPWM_Module_1;
        case '2':
            return kPWM_Module_2;
        case '3':
            return kPWM_Module_3;
        default:
            return kPWM_Module_1;
    }
}

STATIC uint8_t channel_decode(char channel) {
    switch (channel) {
        case 'A':
            return kPWM_PwmA;
        case 'B':
            return kPWM_PwmB;
        case 'X':
            return kPWM_PwmX;
        default:
            return kPWM_PwmA;
    }
}

// decode the AF objects module and Port numer. Returns NULL if it is not a FLEXPWM object
STATIC const machine_pin_af_obj_t *af_name_decode_flexpwm(const machine_pin_af_obj_t *af_obj,
    uint8_t *module, uint8_t *submodule, uint8_t *channel) {
    const char *str;
    size_t len;
    str = (char *)qstr_data(af_obj->name, &len);
    // test for the name starting with FLEXPWM
    if (len < 15 || strncmp(str, "FLEXPWM", 7) != 0) {
        return NULL;
    }
    // Get module, submodule and channel from the name, e.g. FLEXPWM1_PWM0_A
    *module = str[7] - '0';
    *submodule = module_decode(str[12]);
    *channel = channel_decode(str[14]);

    return af_obj;
}

#ifdef FSL_FEATURE_SOC_TMR_COUNT
STATIC uint8_t qtmr_decode(char channel) {
    switch (channel) {
        case '0':
            return kQTMR_Channel_0;
        case '1':
            return kQTMR_Channel_1;
        case '2':
            return kQTMR_Channel_2;
        case '3':
            return kQTMR_Channel_3;
        default:
            return kPWM_Module_1;
    }
}

// decode the AF objects module and Port numer. Returns NULL if it is not a QTMR object
STATIC const machine_pin_af_obj_t *af_name_decode_qtmr(const machine_pin_af_obj_t *af_obj, uint8_t *module, uint8_t *channel) {
    const char *str;
    size_t len;
    str = (char *)qstr_data(af_obj->name, &len);
    // test for the name starting with TMR
    if (len < 11 || strncmp(str, "TMR", 3) != 0) {
        return NULL;
    }
    // Get module, submodule and channel from the name, e.g. FLEXPWM1_PWM0_A
    *module = str[3] - '0';
    *channel = qtmr_decode(str[10]);

    return af_obj;
}
#endif

STATIC bool is_board_pin(const machine_pin_obj_t *pin) {
    for (int i = 0; i < num_board_pins; i++) {
        if (pin == machine_pin_board_pins[i]) {
            return true;
        }
    }
    return false;
}

// Functions for configuring the PWM Device
//
STATIC int calc_prescaler(uint32_t clock, uint32_t freq) {
    float temp = (float)clock / (float)PWM_FULL_SCALE / (float)freq;
    for (int prescale = 0; prescale < 8; prescale++, temp /= 2) {
        if (temp <= 1) {
            return prescale;
        }
    }
    // Frequency too low, cannot scale down.
    return -1;
}

STATIC void configure_flexpwm(machine_pwm_obj_t *self) {
    pwm_signal_param_u16_t pwmSignal;

    // Initialize PWM module.
    #if defined(MIMXRT117x_SERIES)
    uint32_t pwmSourceClockInHz = CLOCK_GetRootClockFreq(kCLOCK_Root_Bus);
    #else
    uint32_t pwmSourceClockInHz = CLOCK_GetFreq(kCLOCK_IpgClk);
    #endif

    int prescale = calc_prescaler(pwmSourceClockInHz, self->freq);
    if (prescale < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_FREQ));
    }
    if (self->prescale != prescale || self->is_init == false) {
        pwm_config_t pwmConfig;
        PWM_GetDefaultConfig(&pwmConfig);
        self->prescale = prescale;
        pwmConfig.prescale = prescale;
        pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle;
        if (self->complementary) {
            pwmConfig.pairOperation = self->channel1 == kPWM_PwmA ? kPWM_ComplementaryPwmA : kPWM_ComplementaryPwmB;
        } else {
            pwmConfig.pairOperation = kPWM_Independent;
        }
        pwmConfig.clockSource = kPWM_BusClock;
        pwmConfig.enableWait = false;
        pwmConfig.initializationControl = self->sync ? kPWM_Initialize_MasterSync : kPWM_Initialize_LocalSync;

        if (PWM_Init(self->instance, self->submodule, &pwmConfig) == kStatus_Fail) {
            mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_INIT));
        }
    }

    // Disable the fault detect function to avoid using the xbara
    PWM_SetupFaultDisableMap(self->instance, self->submodule, self->channel1, kPWM_faultchannel_0, 0);
    PWM_SetupFaultDisableMap(self->instance, self->submodule, self->channel1, kPWM_faultchannel_1, 0);
    if (self->complementary) {
        PWM_SetupFaultDisableMap(self->instance, self->submodule, self->channel2, kPWM_faultchannel_0, 0);
        PWM_SetupFaultDisableMap(self->instance, self->submodule, self->channel2, kPWM_faultchannel_1, 0);
    }

    if (self->channel1 != kPWM_PwmX) {  // Only for A/B channels
        // Initialize the channel parameters
        pwmSignal.pwmChannel = self->channel1;
        pwmSignal.level = (self->invert & PWM_CHANNEL1) ? kPWM_LowTrue : kPWM_HighTrue;
        pwmSignal.dutyCycle_u16 = self->duty_u16;
        pwmSignal.Center_u16 = self->center;
        pwmSignal.deadtimeValue = ((uint64_t)pwmSourceClockInHz * self->deadtime) / 1000000000ULL;
        PWM_SetupPwm_u16(self->instance, self->submodule, &pwmSignal, self->freq,
            pwmSourceClockInHz, self->output_enable_1);

        if (self->complementary) {
            // Initialize the second channel of the pair.
            pwmSignal.pwmChannel = self->channel2;
            pwmSignal.level = (self->invert & PWM_CHANNEL2) ? kPWM_LowTrue : kPWM_HighTrue;
            PWM_SetupPwm_u16(self->instance, self->submodule, &pwmSignal, self->freq,
                pwmSourceClockInHz, self->output_enable_2);
        }
        if (self->xor == 1) {
            // Set the DBLEN bit for A, B = A ^ B
            self->instance->SM[self->submodule].CTRL &= ~PWM_CTRL_SPLIT_MASK;
            self->instance->SM[self->submodule].CTRL |= PWM_CTRL_DBLEN_MASK;
        } else if (self->xor == 2) {
            // Set the DBLEN and SPLIT bits for A, B = A ^ B
            self->instance->SM[self->submodule].CTRL |= PWM_CTRL_DBLEN_MASK | PWM_CTRL_SPLIT_MASK;
        } else {
            self->instance->SM[self->submodule].CTRL &= ~(PWM_CTRL_DBLEN_MASK | PWM_CTRL_SPLIT_MASK);
        }
    } else {
        PWM_SetupPwmx_u16(self->instance, self->submodule, self->freq, self->duty_u16,
            self->invert, pwmSourceClockInHz);
        if (self->xor) {
            // Set the DBLX bit for X = A ^ B
            self->instance->SM[self->submodule].CTRL |= PWM_CTRL_DBLX_MASK;
        } else {
            self->instance->SM[self->submodule].CTRL &= ~PWM_CTRL_DBLX_MASK;
        }
    }
    // Set the load okay bit for the submodules
    PWM_SetPwmLdok(self->instance, 1 << self->submodule, true);

    // Start the PWM generation from the Submodules
    PWM_StartTimer(self->instance, 1 << self->submodule);
}

#ifdef FSL_FEATURE_SOC_TMR_COUNT
STATIC void configure_qtmr(machine_pwm_obj_t *self) {
    qtmr_config_t qtmrConfig;
    int prescale;
    #if defined(MIMXRT117x_SERIES)
    uint32_t pwmSourceClockInHz = CLOCK_GetRootClockFreq(kCLOCK_Root_Bus);
    #else
    uint32_t pwmSourceClockInHz = CLOCK_GetFreq(kCLOCK_IpgClk);
    #endif

    TMR_Type *instance = (TMR_Type *)self->instance;

    prescale = calc_prescaler(pwmSourceClockInHz, self->freq);
    if (prescale < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_FREQ));
    }
    if (prescale != self->prescale) {
        QTMR_GetDefaultConfig(&qtmrConfig);
        qtmrConfig.primarySource = prescale + kQTMR_ClockDivide_1;
        QTMR_Init(instance, self->channel1, &qtmrConfig);
        self->prescale = prescale;
    }
    // Set up the PWM channel
    if (QTMR_SetupPwm_u16(instance, self->channel1, self->freq, self->duty_u16,
        self->invert, pwmSourceClockInHz / (1 << prescale), self->is_init) == kStatus_Fail) {
        mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_INIT));
    }
    // Start the output
    QTMR_StartTimer(instance, self->channel1, kQTMR_PriSrcRiseEdge);
}
#endif // FSL_FEATURE_SOC_TMR_COUNT

STATIC void configure_pwm(machine_pwm_obj_t *self) {
    // Set the clock frequencies
    // Freq range is 15Hz to ~ 3 MHz.
    static bool set_frequency = true;
    // set the frequency only once
    if (set_frequency) {
        #if !defined(MIMXRT117x_SERIES)
        CLOCK_SetDiv(kCLOCK_IpgDiv, 0x3); // Set IPG PODF to 3, divide by 4
        #endif
        set_frequency = false;
    }

    if (self->duty_ns != 0) {
        self->duty_u16 = duty_ns_to_duty_u16(self->freq, self->duty_ns);
    }
    if (self->is_flexpwm) {
        configure_flexpwm(self);
    #ifdef FSL_FEATURE_SOC_TMR_COUNT
    } else {
        configure_qtmr(self);
    #endif
    }
}

// Micropython API functions
//
STATIC void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_center, ARG_align,
           ARG_invert, ARG_sync, ARG_xor, ARG_deadtime };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_duty_u16, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_duty_ns, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_center, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_align, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_sync, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_xor, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_deadtime, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if ((n_args + kw_args->used) > 0 || self->is_init == false) {
        // Maybe change PWM timer
        if (args[ARG_freq].u_int > 0) {
            self->freq = args[ARG_freq].u_int;
        }

        // Set duty_u16 cycle?
        uint32_t duty = args[ARG_duty_u16].u_int;
        if (duty != 0) {
            if (duty >= PWM_FULL_SCALE) {
                mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_VALUE));
            }
            self->duty_u16 = duty;
            self->duty_ns = 0;
        }
        // Set duty_ns value?
        duty = args[ARG_duty_ns].u_int;
        if (duty != 0) {
            self->duty_ns = duty;
            self->duty_u16 = duty_ns_to_duty_u16(self->freq, self->duty_ns);
        }
        // Set center value?
        int32_t center = args[ARG_center].u_int;
        if (center >= 0) {
            if (center >= PWM_FULL_SCALE) {
                mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_VALUE));
            }
            self->center = center;
        } else {  // Use alignment setting shortcut
            if (args[ARG_align].u_int >= 0) {
                uint8_t align = args[ARG_align].u_int & 3; // limit to 0..3
                if (align == PWM_BEGIN) {
                    self->center = self->duty_u16 / 2;
                } else if (align == PWM_END) {
                    self->center = PWM_FULL_SCALE - self->duty_u16 / 2;
                } else {
                    self->center = 32768; // Default value: mid.
                }
            }
        }

        if (args[ARG_invert].u_int >= 0) {
            self->invert = args[ARG_invert].u_int & (PWM_CHANNEL1 | PWM_CHANNEL2);
        }

        if (args[ARG_sync].u_int >= 0) {
            self->sync = args[ARG_sync].u_int != false && self->submodule != 0;
        }

        if (args[ARG_xor].u_int >= 0) {
            self->xor = args[ARG_xor].u_int & 0x03;
        }

        if (args[ARG_deadtime].u_int >= 0) {
            self->deadtime = args[ARG_deadtime].u_int;
        }
        configure_pwm(self);
        self->is_init = true;
    } else {
        machine_pwm_start(self);
    }

}

// PWM(pin | pin-tuple, freq, [args])
STATIC mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_obj_t *pins;
    const machine_pin_obj_t *pin1;
    const machine_pin_obj_t *pin2;

    // Get referred Pin object(s)
    if (mp_obj_is_type(args[0], &mp_type_tuple)) {
        mp_obj_get_array_fixed_n(args[0], 2, &pins);
        pin1 = pin_find(pins[0]);
        pin2 = pin_find(pins[1]);
    } else {
        pin1 = pin_find(args[0]);
        pin2 = NULL;
    }

    // Check whether it supports PWM and decode submodule & channel
    const machine_pin_af_obj_t *af_obj1 = NULL;
    uint8_t submodule1;
    uint8_t channel1;
    const machine_pin_af_obj_t *af_obj2 = NULL;
    uint8_t submodule2;
    uint8_t channel2;
    uint8_t module;
    bool is_flexpwm = false;

    for (int i = 0; i < pin1->af_list_len; ++i) {
        af_obj1 = af_name_decode_flexpwm(&(pin1->af_list[i]), &module, &submodule1, &channel1);
        if (af_obj1 != NULL) {
            break;
        }
    }
    if (pin2 != NULL) {
        for (int i = 0; i < pin1->af_list_len; ++i) {
            af_obj2 = af_name_decode_flexpwm(&(pin2->af_list[i]), &module, &submodule2, &channel2);
            if (af_obj2 != NULL) {
                break;
            }
        }
    }
    if (af_obj1 == NULL) {
        submodule1 = 0;
        #ifdef FSL_FEATURE_SOC_TMR_COUNT
        // Check for QTimer support
        if (is_board_pin(pin1)) {
            for (int i = 0; i < pin1->af_list_len; ++i) {
                af_obj1 = af_name_decode_qtmr(&(pin1->af_list[i]), &module, &channel1);
                if (af_obj1 != NULL) {
                    break;
                }
            }
        }
        #endif
        if (af_obj1 == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("the requested Pin(s) does not support PWM"));
        }
    } else {
        // is flexpwm, check for instance match
        is_flexpwm = true;
        if (pin2 != NULL && af_obj1->instance != af_obj2->instance && submodule1 != submodule2) {
            mp_raise_ValueError(MP_ERROR_TEXT("the pins must be a A/B pair of a submodule"));
        }
    }

    // Create and populate the PWM object.
    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->is_flexpwm = is_flexpwm;
    self->instance = af_obj1->instance;
    self->module = module;
    self->submodule = submodule1;
    self->channel1 = channel1;
    self->invert = 0;
    self->freq = 1000;
    self->prescale = -1;
    self->duty_u16 = 32768;
    self->duty_ns = 0;
    self->center = 32768;
    self->output_enable_1 = is_board_pin(pin1);
    self->sync = false;
    self->deadtime = 0;
    self->xor = 0;
    self->is_init = false;

    // Initialize the Pin(s).
    CLOCK_EnableClock(kCLOCK_Iomuxc); // just in case it was not set yet
    IOMUXC_SetPinMux(pin1->muxRegister, af_obj1->af_mode, af_obj1->input_register, af_obj1->input_daisy,
        pin1->configRegister, 0U);
    IOMUXC_SetPinConfig(pin1->muxRegister, af_obj1->af_mode, af_obj1->input_register, af_obj1->input_daisy,
        pin1->configRegister, pin_generate_config(PIN_PULL_DISABLED, PIN_MODE_OUT, PIN_DRIVE_5, pin1->configRegister));

    // Settings for the second pin, if given.
    if (pin2 != NULL && pin2 != pin1) {
        self->complementary = 1;
        self->channel2 = channel2;
        self->output_enable_2 = is_board_pin(pin2);
        // Initialize the Pin(s)
        IOMUXC_SetPinMux(pin2->muxRegister, af_obj2->af_mode, af_obj2->input_register, af_obj2->input_daisy,
            pin2->configRegister, 0U);
        IOMUXC_SetPinConfig(pin2->muxRegister, af_obj2->af_mode, af_obj2->input_register, af_obj2->input_daisy,
            pin2->configRegister, pin_generate_config(PIN_PULL_DISABLED, PIN_MODE_OUT, PIN_DRIVE_5, pin2->configRegister));
    } else {
        self->complementary = 0;
    }

    // Process the remaining parameters.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

// Disable all PWM devices. Called on soft reset
void machine_pwm_deinit_all(void) {
    static PWM_Type *const pwm_bases[] = PWM_BASE_PTRS;

    for (int i = 1; i < ARRAY_SIZE(pwm_bases); i++) {
        PWM_StopTimer(pwm_bases[i], 0x0f); // Stop all submodules
        pwm_bases[i]->OUTEN = 0; // Disable ouput on all submodules, all channels
    }

    #ifdef FSL_FEATURE_SOC_TMR_COUNT
    static TMR_Type *const tmr_bases[] = TMR_BASE_PTRS;
    for (int i = 1; i < ARRAY_SIZE(tmr_bases); i++) {
        for (int j = 0; j < 4; j++) {
            QTMR_StopTimer(tmr_bases[i], j); // Stop all timers
        }
    }
    #endif
}

STATIC void machine_pwm_start(machine_pwm_obj_t *self) {
    if (self->is_flexpwm) {
        PWM_StartTimer(self->instance, 1 << self->submodule);
    #ifdef FSL_FEATURE_SOC_TMR_COUNT
    } else {
        QTMR_StartTimer((TMR_Type *)self->instance, self->channel1, kQTMR_PriSrcRiseEdge);
    #endif
    }
}

STATIC void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    if (self->is_flexpwm) {
        PWM_StopTimer(self->instance, 1 << self->submodule);
    #ifdef FSL_FEATURE_SOC_TMR_COUNT
    } else {
        QTMR_StopTimer((TMR_Type *)self->instance, self->channel1);
    #endif
    }
}

mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->freq);
}

void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    self->freq = freq;
    configure_pwm(self);
}

mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->duty_u16);
}

void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty) {
    if (duty >= 0) {
        if (duty >= PWM_FULL_SCALE) {
            mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_VALUE));
        }
        self->duty_u16 = duty;
        self->duty_ns = 0;
        configure_pwm(self);
    }
}

mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(1000000000ULL / self->freq * self->duty_u16 / PWM_FULL_SCALE);
}

void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty) {
    if (duty >= 0) {
        self->duty_ns = duty;
        self->duty_u16 = duty_ns_to_duty_u16(self->freq, self->duty_ns);
        configure_pwm(self);
    }
}
