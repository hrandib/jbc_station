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

#ifndef MONOFONTS_H
#define MONOFONTS_H

#include "lvgl.h"

#define GLCDFONTDECL(_n) constexpr uint8_t _n[]
#define ADAFRUIT_ASCII96 1

#define DECLARE_LVFONT(origin) extern const lv_font_t lv_font_##origin;

DECLARE_LVFONT(Adafruit5x7)
DECLARE_LVFONT(lcd5x7)
DECLARE_LVFONT(Stang5x7)
DECLARE_LVFONT(System5x7)
DECLARE_LVFONT(font5x7)
DECLARE_LVFONT(fixednums15x31)
DECLARE_LVFONT(lcdnums12x16)
DECLARE_LVFONT(lcdnums14x24)

#endif // MONOFONTS_H
