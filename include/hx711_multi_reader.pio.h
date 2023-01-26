// THIS FILE IS AUTOGENERATED BY GITHUB ACTIONS
// YOU SHOULD #include THIS FILE
// THE src/hx711_multi_reader.pio file IS NOT USABLE UNLESS
// YOU CONVERT IT USING pioasm
// SEE: /.github/workflows/generate_pio.yml

// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------------------ //
// hx711_multi_reader //
// ------------------ //

#define hx711_multi_reader_wrap_target 3
#define hx711_multi_reader_wrap 16

#define hx711_multi_reader_offset_bitloop_in_pins_bit_count 7u

static const uint16_t hx711_multi_reader_program_instructions[] = {
    0xe020, //  0: set    x, 0                       
    0x8080, //  1: pull   noblock                    
    0x6020, //  2: out    x, 32                      
            //     .wrap_target
    0xe057, //  3: set    y, 23                      
    0xc020, //  4: irq    wait 0                     
    0x2041, //  5: wait   0 irq, 1                   
    0xe001, //  6: set    pins, 1                    
    0x4001, //  7: in     pins, 1                    
    0xe000, //  8: set    pins, 0                    
    0x8020, //  9: push   block                      
    0x0086, // 10: jmp    y--, 6                     
    0x9880, // 11: pull   noblock         side 1     
    0x6020, // 12: out    x, 32                      
    0x1023, // 13: jmp    !x, 3           side 0     
    0xa041, // 14: mov    y, x                       
    0xe101, // 15: set    pins, 1                [1] 
    0x118f, // 16: jmp    y--, 15         side 0 [1] 
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program hx711_multi_reader_program = {
    .instructions = hx711_multi_reader_program_instructions,
    .length = 17,
    .origin = -1,
};

static inline pio_sm_config hx711_multi_reader_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + hx711_multi_reader_wrap_target, offset + hx711_multi_reader_wrap);
    sm_config_set_sideset(&c, 2, true, false);
    return c;
}

// MIT License
// 
// Copyright (c) 2023 Daniel Robertson
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
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hx711_multi.h"
void hx711_multi_pio_init(hx711_multi_t* const hxm) {
    assert(hxm != NULL);
    assert(hxm->_pio != NULL);
    assert(hxm->_chips_len > 0);
    pio_gpio_init(
        hxm->_pio,
        hxm->_clock_pin);
    {
        uint i = hxm->_data_pin_base;
        const uint l = hxm->_data_pin_base + hxm->_chips_len - 1;
        for(; i <= l; ++i) {
            pio_gpio_init(hxm->_pio, i);
        }
    }
    pio_interrupt_clear(hxm->_pio, HX711_MULTI_APP_WAIT_IRQ_NUM);
    pio_interrupt_clear(hxm->_pio, HX711_MULTI_DATA_READY_IRQ_NUM);
}
void hx711_multi_reader_program_init(hx711_multi_t* const hxm) {
    //set state machine to 10MHz clock speed
    static const uint SM_HZ = 10000000;
    assert(hxm != NULL);
    assert(hxm->_pio != NULL);
    hxm->_pio->instr_mem[hxm->_reader_offset + hx711_multi_reader_offset_bitloop_in_pins_bit_count] = 
        pio_encode_in(pio_pins, hxm->_chips_len);
    pio_sm_config cfg = hx711_multi_reader_program_get_default_config(
        hxm->_reader_offset);
    const float div = (float)(clock_get_hz(clk_sys)) / SM_HZ;
    sm_config_set_clkdiv(
        &cfg,
        div);
    //clock pin setup
    pio_sm_set_out_pins(
        hxm->_pio,
        hxm->_reader_sm,
        hxm->_clock_pin,
        1);
    pio_sm_set_set_pins(
        hxm->_pio,
        hxm->_reader_sm,
        hxm->_clock_pin,
        1);
    pio_sm_set_consecutive_pindirs(
        hxm->_pio,
        hxm->_reader_sm,
        hxm->_clock_pin,
        1,
        true);
    sm_config_set_set_pins(
        &cfg,
        hxm->_clock_pin,
        1);
    sm_config_set_out_pins(
        &cfg,
        hxm->_clock_pin,
        1);
    sm_config_set_sideset_pins(
        &cfg,
        hxm->_clock_pin);
    //data pins
    pio_sm_set_in_pins(
        hxm->_pio,
        hxm->_reader_sm,
        hxm->_data_pin_base);
    pio_sm_set_consecutive_pindirs(
        hxm->_pio,
        hxm->_reader_sm,
        hxm->_data_pin_base,
        hxm->_chips_len,
        false);         //false = input
    sm_config_set_in_pins(
        &cfg,
        hxm->_data_pin_base);
    sm_config_set_in_shift(
        &cfg,
        false,                    //false = shift in left
        false,                    //false = autopush disabled
        0);
    pio_sm_clear_fifos(
        hxm->_pio,
        hxm->_reader_sm);
    hxm->_reader_default_config = cfg;
}

#endif
