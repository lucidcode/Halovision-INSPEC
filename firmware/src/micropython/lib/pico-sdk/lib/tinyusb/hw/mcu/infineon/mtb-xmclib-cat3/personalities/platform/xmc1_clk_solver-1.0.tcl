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

const I_DIV_KEY "iDiv"
const F_DIV_KEY "fDiv"

const SUCCESS 0
const ERROR_ARG_COUNT 1
const ERROR_ID 2
const ERROR_ARG_VALUE 3

set channelName stdout

if {[chan names ModusToolbox] eq "ModusToolbox"} {
    set channelName ModusToolbox
}

proc solve_clk {} {
    if {$::argc != $::ARG_IDX_TARGET_FREQ + 1} {
        error "Clock Solver requires 2 arguments:
\tdouble srcFreqMHz - Source clock frequency for the PLL in MHz.
\tdouble targetFreqMHz - Output frequency of the PLL in MHz."
        return $::ERROR_ARG_COUNT
    }
    set srcFreqMHz [lindex $::argv $::ARG_IDX_SRC_FREQ]
    set targetFreqMHz [lindex $::argv $::ARG_IDX_TARGET_FREQ]
    if {![string is double $srcFreqMHz] || ![string is double $targetFreqMHz]} {
        error "Unable to parse argument values: either $srcFreqMHz or $targetFreqMHz is not a floating-point number."
        return $::ERROR_ARG_VALUE
    }
    set srcFreqMHz [expr {double($srcFreqMHz)}]
    set targetFreqMHz [expr {double($targetFreqMHz)}]
    return [solve_clk_internal $srcFreqMHz $targetFreqMHz]
}

proc solve_clk_internal {srcFreqMHz targetFreqMHz} {
    set freqRatio [expr {$srcFreqMHz / $targetFreqMHz}]

    set idiv [expr {int($freqRatio)}]
    set fdiv [expr {int(fmod($freqRatio,1) * 1024)}]

    puts $::channelName "param:$::I_DIV_KEY=$idiv"
    puts $::channelName "param:$::F_DIV_KEY=$fdiv"
    return $::SUCCESS
}

solve_clk
