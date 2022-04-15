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

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <string.h>
#include "../include/hx711.h"
#include "hx711_noblock.pio.h"
#include "../include/scale.h"

int main() {

    stdio_init_all();

    const uint clkPin = 14;
    const uint datPin = 15;
    const int32_t refUnit = -440;
    const int32_t offset = -369500;
    const mass_unit_t unit = mass_g;

    hx711_t hx;
    scale_t sc;

    hx711_init(
        &hx,
        clkPin,
        datPin,
        pio0,
        &hx711_noblock_program,
        &hx711_noblock_program_init);

    hx711_set_power(&hx, hx711_pwr_down);
    sleep_ms(1);
    hx711_set_power(&hx, hx711_pwr_up);

    hx711_set_gain(&hx, hx711_gain_128);

    //sleep_ms(400); //settling time @ 10Hz
    sleep_ms(50); //settling time @ 80Hz

    scale_init(&sc, &hx, offset, refUnit, unit);

    mass_t mass;
    scale_options_t opt;
    char buff[MASS_TO_STRING_BUFF_SIZE];

    opt.strat_type = strategy_type_samples;
    opt.read_type = read_type_median;
    opt.samples = 500;

    scale_zero(&sc, &opt);
    printf("Zeroing done!\n");

    opt.samples = 30;

    while(true) {
        
        memset(&buff[0], 0, sizeof(buff));

        scale_weight(&sc, &mass, &opt);
        mass_to_string(&mass, buff);
        printf("%s\n", buff);

        /*
        const int32_t val = hx711_get_value(&hx);

        if(hx711_is_min_saturated(val)) {
            printf("ERROR: MIN\n");
        }
        else if(hx711_is_max_saturated(val)) {
            printf("ERROR: MAX\n");
        }
        else {
            printf("%i\n", val);
        }
        */

    }

    return EXIT_SUCCESS;

}
