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

constexpr size_t RAW_BUF_SIZE = (Display::Props::X_DIM)*24;

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
    static lv_point_t line_points1[] = {{0, 0}, {0, 59}, {218, 59}, {218, 0}, {0, 0}};

    /*Create style*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_color_black());
    lv_style_set_line_color(&style, lv_color_white());
    lv_style_set_line_width(&style, 1);
    //    lv_style_set_line_rounded(&style_line, false);

    /*Create a line and apply the new style*/
    lv_obj_t* line1;
    line1 = lv_line_create(lv_scr_act());
    lv_line_set_points(line1, line_points1, 5); /*Set the points*/
    lv_obj_add_style(line1, &style, 0);
}

void lv_example_label(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    //    lv_style_set_bg_color(&style, lv_color_black());
    //    lv_style_set_text_color(&style, lv_color_white());
    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "T245");
    lv_obj_align(label, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_obj_t* label1 = lv_label_create(lv_scr_act());
    lv_label_set_text(label1, "T210");
    lv_obj_align(label1, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t* label2 = lv_label_create(lv_scr_act());
    lv_label_set_text(label2, "T115");
    lv_obj_align(label2, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t* label3 = lv_label_create(lv_scr_act());
    lv_label_set_text(label3, "T470");
    lv_obj_align(label3, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* label4 = lv_label_create(lv_scr_act());
    lv_label_set_text(label4, "T12");
    lv_obj_align(label4, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t* label5 = lv_label_create(lv_scr_act());
    lv_label_set_text(label5, "LONG.LONG_TEXT");
    lv_obj_align(label5, LV_ALIGN_TOP_LEFT, 1, 1);

    // lv_obj_set_pos(label, 0, 1);
}

static void event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        const char* txt = lv_checkbox_get_text(obj);
        const char* state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";
        LV_LOG_USER("%s: %s", txt, state);
    }
}

void lv_example_checkbox_1(void)
{
    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* cb;
    cb = lv_checkbox_create(lv_scr_act());
    lv_checkbox_set_text(cb, "Apple");
    lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

    cb = lv_checkbox_create(lv_scr_act());
    lv_checkbox_set_text(cb, "Banana");
    lv_obj_add_state(cb, LV_STATE_CHECKED);
    lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

    lv_obj_update_layout(cb);
}

static THD_WORKING_AREA(HANDLER_WA_SIZE, 2048);
static THD_FUNCTION(displayHandler, )
{
    while(true) {
        chThdSleepMilliseconds(5);
        lv_timer_handler();
    }
}

void init()
{
    lv_init();
    Display::Init();
    lv_disp_draw_buf_init(&disp_buf, raw_buf, nullptr, RAW_BUF_SIZE);
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.hor_res = Display::Props::X_DIM;
    disp_drv.ver_res = Display::Props::Y_DIM;
    disp_drv.flush_cb = flush_cb;
    disp_drv.rounder_cb = rounder_cb;
    disp_drv.set_px_cb = set_px_cb;
    lv_disp_drv_register(&disp_drv);

    auto* thd = chThdCreateStatic(HANDLER_WA_SIZE, sizeof(HANDLER_WA_SIZE), NORMALPRIO, displayHandler, nullptr);
    chRegSetThreadNameX(thd, "display_handler");

    lv_theme_t* th = lv_theme_mono_init(0, true, &lv_font_unscii_16);
    lv_disp_set_theme(nullptr, th);
    lv_example_label();
    lv_example_line();
}

void flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
{
    static_assert(sizeof(lv_color_t) == 1, "Cast to uint8_t is impossible");

    /*Return if the area is out the screen*/
    if(area->x2 < 0 || area->y2 < 0 || area->x1 > disp_drv->hor_res - 1 || area->y1 > disp_drv->ver_res - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }
    uint8_t* buf8 = (uint8_t*)color_p;
    lv_coord_t buf_w = area->x2 - area->x1 + 1;

    size_t y1 = area->y1 >> 3;
    size_t y2 = area->y2 >> 3;
    for(size_t y = y1; y < y2; ++y) {
        Display::PutPage(area->x1, area->x2, y, buf8);
        buf8 += buf_w;
    }
    lv_disp_flush_ready(disp_drv);
}

void rounder_cb(lv_disp_drv_t*, lv_area_t* area)
{
    // Round to 8 bit page size
    area->y1 &= ~0x07;
    area->y2 = (area->y2 | 0x07) + 1;
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
