// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------------ //
// hx711_reader //
// ------------ //

#define hx711_reader_wrap_target 3
#define hx711_reader_wrap 13

static const uint16_t hx711_reader_program_instructions[] = {
    0xe020, //  0: set    x, 0                       
    0x8080, //  1: pull   noblock                    
    0x6020, //  2: out    x, 32                      
            //     .wrap_target
    0xe057, //  3: set    y, 23                      
    0x2020, //  4: wait   0 pin, 0                   
    0xe001, //  5: set    pins, 1                    
    0x4001, //  6: in     pins, 1                    
    0x1185, //  7: jmp    y--, 5          side 0 [1] 
    0x9880, //  8: pull   noblock         side 1     
    0x6020, //  9: out    x, 32                      
    0x1023, // 10: jmp    !x, 3           side 0     
    0xa041, // 11: mov    y, x                       
    0xe101, // 12: set    pins, 1                [1] 
    0x118c, // 13: jmp    y--, 12         side 0 [1] 
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program hx711_reader_program = {
    .instructions = hx711_reader_program_instructions,
    .length = 14,
    .origin = -1,
};

static inline pio_sm_config hx711_reader_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + hx711_reader_wrap_target, offset + hx711_reader_wrap);
    sm_config_set_sideset(&c, 2, true, false);
    return c;
}

// MIT License
// 
// Copyright (c) 2022 Daniel Robertson
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <assert.h>
#include <stddef.h>
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hx711.h"
#include "util.h"
void hx711_reader_pio_init(hx711_t* const hx) {
    UTIL_ASSERT_NOT_NULL(hx)
    UTIL_ASSERT_NOT_NULL(hx->_pio)
    pio_gpio_init(
        hx->_pio,
        hx->_clock_pin);
    //clock pin setup
    pio_sm_set_out_pins(
        hx->_pio,
        hx->_reader_sm,
        hx->_clock_pin,
        1);
    pio_sm_set_set_pins(
        hx->_pio,
        hx->_reader_sm,
        hx->_clock_pin,
        1);
    pio_sm_set_consecutive_pindirs(
        hx->_pio,
        hx->_reader_sm,
        hx->_clock_pin,
        1,
        true);
    //data pin setup
    pio_gpio_init(
        hx->_pio,
        hx->_data_pin);
    pio_sm_set_in_pins(
        hx->_pio,
        hx->_reader_sm,
        hx->_data_pin);
    pio_sm_set_consecutive_pindirs(
        hx->_pio,
        hx->_reader_sm,
        hx->_data_pin,
        1,
        false);
}
void hx711_reader_program_init(hx711_t* const hx) {
    //set state machine to 10MHz clock speed
    static const uint SM_HZ = 10000000;
    UTIL_ASSERT_NOT_NULL(hx)
    UTIL_ASSERT_NOT_NULL(hx->_pio)
    pio_sm_config cfg = hx711_reader_program_get_default_config(hx->_reader_offset);
    const float div = (float)(clock_get_hz(clk_sys)) / SM_HZ;
    sm_config_set_clkdiv(
        &cfg,
        div);
    sm_config_set_set_pins(
        &cfg,
        hx->_clock_pin,
        1);
    sm_config_set_out_pins(
        &cfg,
        hx->_clock_pin,
        1);
    sm_config_set_sideset_pins(
        &cfg,
        hx->_clock_pin);
    sm_config_set_in_pins(
        &cfg,
        hx->_data_pin);
    /**
     * Why enable autopush?
     *
     * "The state machine keeps an eye on the total amount of data shifted into the ISR, and on the in which reaches or
     *  breaches a total shift count of 32 (or whatever number you have configured), the ISR contents, along with the new data
     *  from the in. goes straight to the RX FIFO. The ISR is cleared to zero in the same operation."
     *  - Raspberry Pi Pico C/C++ SDK pg. 45
     *
     * When manually pushing using noblock, the FIFO contents are NOT changed.
     * 
     * "The PIO assembler sets the Block bit by default. If the Block bit is not set, the PUSH does not stall on a full RX FIFO, instead
     * continuing immediately to the next instruction. The FIFO state and contents are unchanged when this happens. The ISR
     * is still cleared to all-zeroes, and the FDEBUG_RXSTALL flag is set (the same as a blocking PUSH or autopush to a full RX FIFO)
     * to indicate data was lost."
     * - Raspberry Pi Pico C/C++ SDK pg. 64
     * 
     * Manually pushing is not ideal. Application code should be able to look at the FIFO and assume
     * that the value inside is the most up-to-date data available. Autopushing does this.
     */
    sm_config_set_in_shift(
        &cfg,
        false,            //false = shift in left
        true,             //true = autopush enabled
        HX711_READ_BITS); //autopush on 24 bits
    //store a copy of the configuration for resetting the sm
    hx->_reader_prog_default_config = cfg;
}

#endif

