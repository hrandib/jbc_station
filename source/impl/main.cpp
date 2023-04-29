/*
 * Copyright (c) 2023 Dmytro Shestakov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ch.h"
#include "hal.h"
#include "s1d157xx.h"
#include "sensor_handler.h"

#include <cmath>
using namespace Mcucpp;
using Gpio::Pinlist;
using Gpio::SequenceOf;

// using Databus = Pinlist<Gpio::Inverted<Pa1>>;
using Display = S1d157xx<Portc, Pa1, Pb2, Pb1>;

int main()
{
    halInit();
    chSysInit();
    Sensors::init();
    Display::Init();

    float x = 0.1f;
    //    float y{};
    while(true) {
        x += 0.1f;
        chThdSleepMilliseconds(100);
    }
}
