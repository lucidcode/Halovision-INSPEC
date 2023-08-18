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
const ARG_IDX_MODE 2

const P_DIV_KEY "pDiv"
const N_DIV_KEY "nDiv"
const K_DIV_KEY "kDiv"

const SUCCESS 0
const ERROR_ARG_COUNT 1
const ERROR_ID 2
const ERROR_ARG_VALUE 3

set channelName stdout

if {[chan names ModusToolbox] eq "ModusToolbox"} {
    set channelName ModusToolbox
}

proc solve_pll {} {
    if {$::argc != $::ARG_IDX_MODE + 1} {
        error "PLL Solver requires 3 arguments:
\tdouble srcFreqMHz - Source clock frequency for the PLL in MHz.
\tdouble targetFreqMHz - Output frequency of the PLL in MHz.
\tint mode - 0 = USB PLL mode (K2 fixed),  1 = SYS PLL mode (K2 divider)."
        return $::ERROR_ARG_COUNT
    }
    set srcFreqMHz [lindex $::argv $::ARG_IDX_SRC_FREQ]
    set targetFreqMHz [lindex $::argv $::ARG_IDX_TARGET_FREQ]
    set mode [lindex $::argv $::ARG_IDX_MODE]
    if {![string is double $srcFreqMHz] || ![string is double $targetFreqMHz]} {
        error "Unable to parse argument values: either $srcFreqMHz or $targetFreqMHz is not a floating-point number."
        return $::ERROR_ARG_VALUE
    }
    if {![string is int $mode]} {
        error "Unable to parse argument values: $mode is not a integer number."
        return $::ERROR_ARG_VALUE
    }
    set srcFreqMHz [expr {double($srcFreqMHz)}]
    set targetFreqMHz [expr {double($targetFreqMHz)}]
    set mode [expr {int($mode)}]
    return [solve_pll_internal $srcFreqMHz $targetFreqMHz $mode]
}

proc solve_pll_internal {srcFreqMHz targetFreqMHz mode} {

    const MAX_FIN 16
    const MIN_FIN 4
    const MAX_FVCO 520
    const MIN_FVCO 260
    const MAX_NDIV 128
    const MIN_KDIV [expr {($mode != 0) ? 1 : 2}]
    const MAX_KDIV [expr {($mode != 0) ? 128 : 2}]
    const MAX_PDIV 16

    const MAX_DOUBLE 1.0e308

    set p 1
    set n 1
    set k 1

    set freqRatio [expr {$srcFreqMHz / $targetFreqMHz}]

    set leastErrorFound $MAX_DOUBLE
    set fvco_max 0
    for {set p_div 1} {$p_div <= $MAX_PDIV} {incr p_div} {
        set fin [expr ($srcFreqMHz / $p_div)]
        if {($fin >= $MIN_FIN) && ($fin <= $MAX_FIN)} {
            for {set n_div 1} {$n_div <= $MAX_NDIV} {incr n_div} {
                set fvco [expr ($fin * $n_div)]
                if {($fvco >= $MIN_FVCO) && ($fvco <= $MAX_FVCO)} {
                    for {set k_div $MIN_KDIV} {$k_div <= $MAX_KDIV} {incr k_div} {
                        set fpll [expr ($fvco / $k_div)]
                        set error [expr {abs($fpll -  $targetFreqMHz)}]
                        
                        if {($error < $leastErrorFound) ||
                            ([double_equals $error $leastErrorFound] && ([is_odd $k] || [is_odd $p]))} {
                            #puts $::channelName "fPLL = $fpll, fVCO = $fvco, PDIV = $p_div, NDIV = $n_div, KDIV = $k_div"
                            set leastErrorFound $error
                            set p $p_div
                            set n $n_div
                            set k $k_div
                        }
                    }
                }
            }
        }
    }
    
    puts $::channelName "param:$::P_DIV_KEY=$p"
    puts $::channelName "param:$::N_DIV_KEY=$n"
    puts $::channelName "param:$::K_DIV_KEY=$k"
    return $::SUCCESS
}

proc double_equals {a b} {
    const DOUBLE_COMPARE_EPSILON 0.00001

    return [expr {abs($a - $b) < $DOUBLE_COMPARE_EPSILON}]
}

proc is_odd {x} {
    return [expr {(($x % 2) != 0) && ($x > 1)}]
}

solve_pll
