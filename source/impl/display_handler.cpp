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
#include "chlog.h"
#include "lvgl.h"
#include "monofonts.h"
#include "s1d157xx.h"
#include "shiftreg.h"
#include "ui.h"

namespace Ui {

using namespace Drivers;

namespace Pins {

using A0_Dat = Pc15; // Common pin for the shift register DAT and display controller D/C
using Res = Pc14;
using Cs = Pc13;
using Clk = Pb12;
using KeysIn = Pb2;
static inline void Init()
{
    PinsInit<OutputConf::OutputSlow, OutputMode::PushPull, A0_Dat, Res, Cs, Clk>();
    KeysIn::SetConfig(InputConf::Input, InputMode::PullDown);
}
} // Pins

using ShiftRegBus = ShiftReg<uint8_t, Pins::Clk, Pins::A0_Dat, Nullpin, Pins::KeysIn>;
using Display = S1d157xx<S1D15710, ShiftRegBus, Pins::Cs, Pins::A0_Dat, Pins::Res>;

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

void ui_handler(lv_obj_t* l)
{
    int16_t hue{};
    size_t input_poll_counter{};
    uint8_t buttons_val{};
    if(input_poll_counter++ == 15) {
        input_poll_counter = 0;
        buttons_val = ShiftRegBus::Read();
        if(buttons_val == 0x80) {
            hue = Bl::IncrementHue();
        }
        else if(buttons_val == 0x40) {
            hue = Bl::DecrementHue();
        }
        else if(buttons_val == 0x10) {
            Bl::Off();
        }
        lv_label_set_text_fmt(l, "%d", hue);
    }
}

static THD_WORKING_AREA(HANDLER_WA_SIZE, 2048);
static THD_FUNCTION(displayHandler, )
{
    auto l = ui_init();
    while(true) {
        ui_handler(l);
        lv_timer_handler();
        chThdSleepMilliseconds(10);
    }
}

void Init()
{
    lv_init();
    Pins::Init();
    Bl::Init();
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

    lv_theme_t* th = lv_theme_mono_init(0, true, &lv_font_font5x7);
    lv_disp_set_theme(nullptr, th);
}

void flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
{
    static_assert(sizeof(lv_color_t) == 1, "lv_color_t set for color displays");

    /*Return if the area is out the screen*/
    if(area->x2 < 0 || area->y2 < 0 || area->x1 > disp_drv->hor_res - 1 || area->y1 > disp_drv->ver_res - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }
    uint8_t* buf8 = (uint8_t*)color_p;
    lv_coord_t x_len = area->x2 - area->x1 + 1;

    size_t y1 = area->y1 >> 3;
    size_t y2 = area->y2 >> 3;
    for(size_t y = y1; y <= y2; ++y) {
        Display::PutPage(area->x1, x_len, y, buf8);
        buf8 += x_len;
    }
    lv_disp_flush_ready(disp_drv);
}

void rounder_cb(lv_disp_drv_t*, lv_area_t* area)
{
    // Round to 8 bit page size
    area->y1 &= ~0x07;
    area->y2 = (area->y2 | 0x07);
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
