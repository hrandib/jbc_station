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

#include "lv_display.h"
#include "s1d157xx.h"

using namespace Drivers;

using Databus = Pinlist<Pc6, Pc7, Pc8, Pc9, Pb12, Pb13, Pb14, Pb15>;
using Display = Drivers::S1d157xx<Drivers::S1D15710, Databus, Pa11, Pa12, Pa10>;

void s1d15710_init()
{
    Display::Init();
}

void s1d15710_flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p);

void s1d15710_rounder_cb(lv_disp_drv_t* disp_drv, lv_area_t* area)
{
    // Round to 8 bit page size
    area->y1 &= ~0x07;
    area->y2 = (area->y2 & ~0x07) + 8;
}

void s1d15710_set_px_cb(lv_disp_drv_t* disp_drv,
                        uint8_t* buf,
                        lv_coord_t buf_w,
                        lv_coord_t x,
                        lv_coord_t y,
                        lv_color_t color,
                        lv_opa_t opa)
{
    buf += buf_w * (y >> 3) + x;
    if(color.full) {
        (*buf) |= (1 << (y % 8));
    }
    else {
        (*buf) &= ~(1 << (y % 8));
    }
}
