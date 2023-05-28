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

#include "backlight.h"
#include "hal.h"

namespace Drivers {
namespace Bl {

constexpr auto PWM_RESOLUTION = 16;

enum Ch {
    CH_RED,
    CH_GREEN
};

static const PWMConfig pwmcfg = {
  .frequency = 5000 * PWM_RESOLUTION, /* 5kHz PWM clock frequency. */
  .period = PWM_RESOLUTION,
  .callback = nullptr, /* Period callback. */
  .channels =
    {
      {PWM_OUTPUT_ACTIVE_HIGH, NULL}, /* CH1 mode and callback. */
      {PWM_OUTPUT_ACTIVE_LOW, NULL},  /* CH2 mode and callback. */
      {PWM_OUTPUT_DISABLED, NULL},    /* CH3 mode and callback. */
      {PWM_OUTPUT_DISABLED, NULL}     /* CH4 mode and callback. */
    },
  .cr2 = 0, /* Control Register 2.            */
  .bdtr = 0,
  .dier = 0, /* DMA/Interrupt Enable Register. */
};

static int8_t hueVal;

static void Update()
{
    uint32_t redVal{}, greenVal{};
    if(!hueVal) {
        redVal = PWM_RESOLUTION;
        greenVal = 0;
    }
    else if(hueVal > 0) {
        redVal = PWM_RESOLUTION - hueVal;
        greenVal = 0;
    }
    else if(hueVal < 0) {
        redVal = PWM_RESOLUTION;
        greenVal = -hueVal;
    }
    pwmEnableChannel(&PWMD3, CH_RED, redVal);
    pwmEnableChannel(&PWMD3, CH_GREEN, greenVal);
    // TODO: Store hue to EEPROM after timeout
}

void Init()
{
    // TODO: Get config values from EEPROM
    pwmStart(&PWMD3, &pwmcfg);
    hueVal = 4;
    Update();
}

int8_t IncrementHue()
{
    if(hueVal < 16) {
        ++hueVal;
        Update();
    }
    return hueVal;
}

int8_t DecrementHue()
{
    if(hueVal > -16) {
        --hueVal;
        Update();
    }
    return hueVal;
}

void Off()
{
    pwmEnableChannel(&PWMD3, CH_RED, 0);
    pwmEnableChannel(&PWMD3, CH_GREEN, PWM_RESOLUTION);
}

} // Bl
}
