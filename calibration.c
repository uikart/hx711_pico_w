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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tusb.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "../include/hx711.h"
#include "../include/scale.h"
#include "hx711_noblock.pio.h"

size_t getchars(char* arr, const size_t len) {

    memset(arr, 0, len);

    int c;
    size_t i = 0;

    for(; i < len;) {

        c = getchar();

        if(c == 0) {
            continue;
        }
        else if(c == '\r' || c == '\n') {
            putchar('\n');
            break;
        }

        putchar(c); //echo

        arr[i] = c;
        ++i;

    }

    return i + 1;

}

int main() {

    stdio_init_all();

    const uint clkPin = 14;
    const uint datPin = 15;

    char buff[32];
    char unit[10];
    double knownWeight;
    int32_t zeroValue;
    double raw;
    double refUnitFloat;
    int32_t refUnit;

    hx711_t hx;
    scale_t sc;
    scale_options_t opt = SCALE_DEFAULT_OPTIONS;

    hx711_init(
        &hx,
        clkPin,
        datPin,
        pio0,
        &hx711_noblock_program,
        &hx711_noblock_program_init);

    //set gain and reset
    //powering down and then powering back up after setting
    //the gain saves the gain to the HX711
    hx711_set_gain(&hx, hx711_gain_128);
    hx711_set_power(&hx, hx711_pwr_down);
    sleep_us(HX711_POWER_DOWN_TIMEOUT);
    hx711_set_power(&hx, hx711_pwr_up);
    sleep_ms(hx711_get_settling_time(hx711_rate_80));

    scale_init(&sc, &hx, 0, 1, mass_ug);

    while(!tud_cdc_connected()) {
        sleep_ms(10);
    }

    //clear the screen
    printf("\x1B[2J\x1B[H");

    printf(
"========================================\n\
HX711 Calibration\n\
========================================\n\
\n\
Find an object you know the weight of. If you can't find anything, \n\
try searching Google for your phone's specifications to find its weight. \n\
You can then use your phone to calibrate your scale.\n\
\n"
    );

    printf("1. Enter the unit you want to measure the object in (eg. g, kg, lb, oz): ");
    getchars(unit, sizeof(unit));

    printf("\n2. Enter the weight of the object in the unit you chose (eg. \
if you chose 'g', enter the weight of the object in grams): ");
    getchars(buff, sizeof(buff));
    knownWeight = atof(buff);

    printf("\n3. Enter the number of samples to take from the HX711 chip (eg. 1000): ");
    getchars(buff, sizeof(buff));
    opt.samples = (size_t)atol(buff);

    printf("\n4. Remove all objects from the scale and then press enter.");
    getchar();
    printf("\nWorking...");

    if(!scale_read(&sc, &raw, &opt)) {
        printf("ERROR: failed to read from scale");
        return EXIT_FAILURE;
    }

    zeroValue = (int32_t)round(raw);

    printf("\n\n5. Place object on the scale and then press enter.");
    getchar();
    printf("\nWorking...");

    if(!scale_read(&sc, &raw, &opt)) {
        printf("ERROR: failed to read from scale");
    }

    refUnitFloat = (raw - zeroValue) / knownWeight;
    refUnit = (int32_t)round(refUnitFloat);
    hx711_close(&hx);

    if(refUnit == 0) {
        refUnit = 1;
    }

    printf("\
\n\n\
Known weight (your object): %f %s\n\
Raw value over %u samples: %i\n\
\n\
-> REFERENCE UNIT: %i\n\
-> ZERO VALUE: %i\n\
\n\
You can provide these values to the scale_init() function. For example: \n\
\n\
scale_init(&sc, &hx, %i, %i, /* your chosen mass_unit_t */);\
\n",
        knownWeight, unit,
        opt.samples, (int32_t)raw,
        refUnit,
        zeroValue,
        zeroValue, refUnit,
        zeroValue,
        refUnit);

    getchar();

    return EXIT_SUCCESS;

}