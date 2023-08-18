# Copyright 2020-2021 Cypress Semiconductor Corporation
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# From https://wiki.tcl-lang.org/page/constants
proc const {name value} {
    uplevel 1 [list set $name $value]
    uplevel 1 [list trace var $name w {error constant;} ]
}

const ARG_IDX_SRC_FREQ 0
const ARG_IDX_TARGET_FREQ 1
const ARG_IDX_OVERSAMPLING 2
const ARG_IDX_DIV_MODE 3

const BR_KEY "baudrate"
const STEP_KEY "step"
const P_DIV_KEY "pDiv"

const SUCCESS 0
const ERROR_ARG_COUNT 1
const ERROR_ID 2
const ERROR_ARG_VALUE 3

set channelName stdout

if {[chan names ModusToolbox] eq "ModusToolbox"} {
    set channelName ModusToolbox
}

proc solve_brg {} {
    if {$::argc != $::ARG_IDX_DIV_MODE + 1} {
        error "Clock Solver requires 4 arguments:
\tint srcFreq - Source clock frequency in Hz.
\tint targetFreq - Desrired baudrate in Bps.
\tint oversampling - Oversampling.
\tint clockDividerMode - Clock divider mode."
        return $::ERROR_ARG_COUNT
    }
    set srcFreq [lindex $::argv $::ARG_IDX_SRC_FREQ]
    set targetFreq [lindex $::argv $::ARG_IDX_TARGET_FREQ]
    set oversampling [lindex $::argv $::ARG_IDX_OVERSAMPLING]
    set clockDividerMode [lindex $::argv $::ARG_IDX_DIV_MODE]
    if {![string is int $srcFreq] || ![string is int $targetFreq] || ![string is int $oversampling]} {
        error "Unable to parse argument values: either $srcFreq or $targetFreq or $oversampling is not a integer number."
        return $::ERROR_ARG_VALUE
    }
    set srcFreq [expr {int($srcFreq)}]
    set targetFreq [expr {int($targetFreq)}]
    set oversampling [expr {int($oversampling)}]
    set clockDividerMode [expr {$clockDividerMode eq true}]
    return [solve_brg_internal $srcFreq $targetFreq $oversampling $clockDividerMode]
}

proc solve_brg_internal {peripheral_clock rate oversampling clockDividerMode} {
   
    set actual_rate 0
    set pdiv 1;
    set divider_step 1

    if {$clockDividerMode == 0} {
        # Fractional divider mode
        set peripheral_clock [expr {int($peripheral_clock / 100)}];
        set rate [expr {int($rate / 100)}];
        if {$rate == 0} { incr rate 1; }
        
        set divider_step_min 1024;
        
        set pdiv_int 0;
        set pdiv_int_min 1;

        set pdiv_frac 0;
        set pdiv_frac_min 0x3ff;
        
        for {set divider_step 1024} {$divider_step > 0} {incr divider_step -1} {
            set pdiv [expr {int((($peripheral_clock * $divider_step) / ($rate * $oversampling)))}];
            set pdiv_int [expr {$pdiv >> 10}];
            set pdiv_frac [expr {$pdiv & 0x3ff}];

            if {[expr {($pdiv_int > 0) && ($pdiv_int <= 1024) && ($pdiv_frac < $pdiv_frac_min)}]} {
                set pdiv_frac_min [expr {$pdiv_frac}];
                set pdiv_int_min [expr {$pdiv_int}];
                set divider_step_min [expr {$divider_step}];
            }
        }

        set actual_rate [expr {int(($peripheral_clock * $divider_step_min * 100) / (1024 * $pdiv_int_min * $oversampling))}]

        puts $::channelName "param:$::STEP_KEY=[expr {$divider_step_min - 1}]"
        puts $::channelName "param:$::P_DIV_KEY=$pdiv_int_min"
    } else {
        # Normal divider mode
        set brg_clock [expr {int($rate * $oversampling)}];

        set divider_step [expr {int($peripheral_clock / $brg_clock)}];
        while {$divider_step >= 1023} {
            incr pdiv;
            set brg_clock [expr {$rate * $oversampling * $pdiv}];
            set divider_step [expr {int($peripheral_clock / $brg_clock)}];
        }
        set actual_rate_upper [expr {int($peripheral_clock / ($divider_step * $oversampling * $pdiv))}];
        set actual_rate_lower [expr {int($peripheral_clock / (($divider_step + 1) * $oversampling * $pdiv))}];

        set diff_lower [expr {abs($rate - $actual_rate_lower)}]
        set diff_upper [expr {abs($rate - $actual_rate_upper)}]
        
        # choose better approximation if the peripheral frequency is not a multiple of the baudrate
        if {$diff_lower < $diff_upper} {
            incr divider_step;
        }

        set actual_rate [expr {int($peripheral_clock / ($divider_step * $pdiv * $oversampling))}]

        puts $::channelName "param:$::STEP_KEY=[expr {1024 - $divider_step}]"
        puts $::channelName "param:$::P_DIV_KEY=$pdiv"
    }


    puts $::channelName "param:$::BR_KEY=$actual_rate"
    return $::SUCCESS
}

solve_brg
