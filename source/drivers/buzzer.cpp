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

#include "buzzer.h"
#include "hal.h"

namespace Drivers {
namespace Buzzer {

constexpr auto PWM_RESOLUTION = 2;

static const PWMConfig pwmcfg = {
  .frequency = 1500 * PWM_RESOLUTION, /* 2.5kHz PWM clock frequency. */
  .period = PWM_RESOLUTION,
  .callback = nullptr, /* Period callback. */
  .channels =
    {
      {PWM_OUTPUT_ACTIVE_HIGH, NULL}, /* CH1 mode and callback. */
      {PWM_OUTPUT_DISABLED, NULL},    /* CH2 mode and callback. */
      {PWM_OUTPUT_DISABLED, NULL},    /* CH3 mode and callback. */
      {PWM_OUTPUT_DISABLED, NULL}     /* CH4 mode and callback. */
    },
  .cr2 = 0, /* Control Register 2.            */
  .bdtr = 0,
  .dier = 0, /* DMA/Interrupt Enable Register. */
};

void Init()
{
    pwmStart(&PWMD2, &pwmcfg);
}

void Beep(BuzType type)
{
    pwmEnableChannel(&PWMD2, 0, 1);
    chThdSleepMilliseconds(type == BuzType::SHORT ? 35 : 500);
    pwmDisableChannel(&PWMD2, 0);
}

} // Buzzer

} // Drivers
