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

#include <stdlib.h>
#include <stdio.h>
#include "pico/stdio.h"
#include "../include/hx711.h"
#include "../include/hx711_noblock.pio.h"
#include "../include/hx711_multi.h"
#include "../include/hx711_multi_awaiter.pio.h"
#include "../include/hx711_multi_reader.pio.h"

#include <string.h>

int main(void) {

    stdio_init_all();

/*
    sleep_ms(3000);

    // SET THESE TO THE GPIO PINS CONNECTED TO THE
    // HX711's CLOCK AND DATA PINS
    // PINOUT REFERENCE: https://learn.adafruit.com/assets/99339
    const uint clkPin = 14; // GP14, PAD19
    const uint datPin = 15; // GP15, PAD20

    hx711_t hx;

    // 1. Initialise
    hx711_init(
        &hx,
        clkPin, // Pico GPIO pin connected to HX711's clock pin
        datPin, // Pico GPIO pin connected to HX711's data pin
        pio0, // the RP2040 PIO to use (either pio0 or pio1)
        &hx711_noblock_program, // the PIO program which reads data from the HX711
        &hx711_noblock_program_init); // the PIO program's init function

    //2. Power up the hx711 and use a gain of 128
    hx711_power_up(&hx, hx711_gain_128);

    //3. This step is optional. Only do this if you want to
    //change the gain AND save it to the HX711 chip
    //
    //hx711_set_gain(&hx, hx711_gain_64);
    //hx711_power_down(&hx);
    //hx711_wait_power_down();
    //hx711_power_up(&hx, hx711_gain_64);

    // 4. Wait for readings to settle
    hx711_wait_settle(hx711_rate_10); // or hx711_rate_80 depending on your chip's config

    // 5. Read values
    int32_t val;

    // wait (block) until a value is read
    val = hx711_get_value(&hx);
    printf("blocking value: %li\n", val);

    // or use a timeout
    if(hx711_get_value_timeout(&hx, 250000, &val)) {
        // value was obtained within the timeout period
        // in this case, within 250 milliseconds
        printf("timeout value: %li\n", val);
    }
    else {
        printf("value was not obtained within the timeout period\n");
    }

    // or see if there's a value, but don't block if not
    if(hx711_get_value_noblock(&hx, &val)) {
        printf("noblock value: %li\n", val);
    }
    else {
        printf("value was not present\n");
    }

    //6. Stop communication with HX711
    hx711_close(&hx);

    printf("cleaned up\n");

*/

    hx711_multi_t hxm;
    hx711_multi_config_t cfg;
    const size_t chips = 1;

    // 1. initialise
    cfg.clock_pin = 14;
    cfg.data_pin_base = 15;
    cfg.chips_len = chips;
    cfg.pio = pio0;
    cfg.pio_init = hx711_multi_pio_init;
    cfg.awaiter_prog = &hx711_multi_awaiter_program;
    cfg.awaiter_prog_init = hx711_multi_awaiter_program_init;
    cfg.reader_prog = &hx711_multi_reader_program;
    cfg.reader_prog_init = hx711_multi_reader_program_init;

    hx711_multi_init(&hxm, &cfg);

    // 2. Power up the HX711 chips and use a gain of 128
    hx711_multi_power_up(&hxm, hx711_gain_128);

    //3. This step is optional. Only do this if you want to
    //change the gain AND save it to each HX711 chip
    //
    //hx711_multi_set_gain(&hxm, hx711_gain_64);
    //hx711_multi_power_down(&hxm);
    //hx711_wait_power_down();
    //hx711_multi_power_up(&hxm, hx711_gain_64);

    // 4. Wait for readings to settle
    hx711_wait_settle(hx711_rate_80); // or hx711_rate_80 depending on each chip's config

    // 5. Read values
    int32_t arr[chips];

    sleep_ms(3000);

    while(true) {

        // wait (block) until a value is read from each chip
        hx711_multi_get_values(&hxm, arr);

        for(uint i = 0; i < chips; ++i) {
            printf("hx711_multi_t chip %i: %li\n", i, arr[i]);
        }

    }

    // 6. Stop communication with all HX711 chips
    hx711_multi_close(&hxm);

    printf("cleaned up");

    while(1);

    return EXIT_SUCCESS;

}
