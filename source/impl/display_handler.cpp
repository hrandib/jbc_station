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

#include "hal.h"
#include "lvgl.h"
#include "s1d157xx.h"

namespace Ui {

using namespace Drivers;

using Databus = Pinlist<Pc6, Pc7, Pc8, Pc9, Pb12, Pb13, Pb14, Pb15>;
using Display = S1d157xx<S1D15710, Databus, Pa11, Pa12, Pa10>;

constexpr size_t RAW_BUF_SIZE = (Display::Props::X_DIM)*20;

static lv_disp_drv_t disp_drv;
static lv_disp_draw_buf_t disp_buf;
static lv_color_t raw_buf[RAW_BUF_SIZE];

static void flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p);
static void rounder_cb(lv_disp_drv_t* disp_drv, lv_area_t* area);
static void set_px_cb(lv_disp_drv_t* disp_drv,
                      uint8_t* buf,
                      lv_coord_t buf_w,
                      lv_coord_t x,
                      lv_coord_t y,
                      lv_color_t color,
                      lv_opa_t opa);

void lv_example_line(void)
{
    /*Create an array for the points of the line*/
    static lv_point_t line_points[] = {{110, 5}, {110, 55}};

    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 8);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_rounded(&style_line, true);

    /*Create a line and apply the new style*/
    lv_obj_t* line1;
    line1 = lv_line_create(lv_scr_act());
    lv_line_set_points(line1, line_points, 5); /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);
    lv_obj_center(line1);
}

static THD_WORKING_AREA(HANDLER_WA_SIZE, 2048);
static THD_FUNCTION(displayHandler, )
{
    while(true) {
        chThdSleepMilliseconds(5);
        //        lv_timer_handler();
        Display::Check();
    }
}

void init()
{
    //    lv_init();
    Display::Init();
    //    lv_disp_draw_buf_init(&disp_buf, raw_buf, nullptr, RAW_BUF_SIZE);
    //    lv_disp_drv_init(&disp_drv);
    //    disp_drv.draw_buf = &disp_buf;
    //    disp_drv.hor_res = Display::Props::X_DIM;
    //    disp_drv.ver_res = Display::Props::Y_DIM;
    //    disp_drv.flush_cb = flush_cb;
    //    disp_drv.rounder_cb = rounder_cb;
    //    disp_drv.set_px_cb = set_px_cb;
    //    lv_disp_drv_register(&disp_drv);

    auto* thd = chThdCreateStatic(HANDLER_WA_SIZE, sizeof(HANDLER_WA_SIZE), NORMALPRIO, displayHandler, nullptr);
    chRegSetThreadNameX(thd, "display_handler");

    //    lv_example_line();
}

void flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
{
    static_assert(sizeof(lv_color_t) == 1, "Cast to uint8_t is impossible");

    for(int32_t y = area->y1; y <= area->y2; ++y) {
        for(int32_t x = area->x1; x <= area->x2; ++x) {
            Display::PutByte(x, y / 8, *reinterpret_cast<uint8_t*>(color_p));
            color_p++;
        }
    }
    lv_disp_flush_ready(disp_drv);
}

void rounder_cb(lv_disp_drv_t*, lv_area_t* area)
{
    // Round to 8 bit page size
    area->y1 &= ~0x07;
    area->y2 = (area->y2 & ~0x07) + 8;
}

void set_px_cb(lv_disp_drv_t*, uint8_t* buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t)
{
    buf += buf_w * (y >> 3) + x;
    if(color.full) {
        (*buf) |= (1 << (y % 8));
    }
    else {
        (*buf) &= ~(1 << (y % 8));
    }
}

} // Ui
