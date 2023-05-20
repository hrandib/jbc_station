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

#include "chlog.h"
#include "lvgl.h"
#include "monofonts.h"
#include "s1d157xx.h"
#include "shiftreg.h"

extern const lv_font_t Hooge;

namespace Ui {

using namespace Drivers;

namespace Pins {

using A0_Dat = Pc0; // Common pin for the shift register DAT and display controller D/C
using Res = Pc1;
using Cs = Pc2;
using Clk = Pc3;

static inline void Init()
{
    PinsInit<OutputConf::OutputFastest, OutputMode::PushPull, A0_Dat, Res, Cs, Clk>();
}
} // Pins

// using Databus = Pinlist<Pc6, Pc7, Pc8, Pc9, Pb12, Pb13, Pb14, Pb15>;
using ShiftRegBus = ShiftReg<uint8_t, Pins::Clk, Pins::A0_Dat>;
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
    lv_label_set_text(label5, "LONG+LONG^TEX");
    lv_obj_align(label5, LV_ALIGN_TOP_LEFT, 1, 1);

    // lv_obj_set_pos(label, 0, 1);
}

void add_iron_section(lv_obj_t* parent, lv_style_t* /*style*/, lv_align_t align, int32_t val)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_border_width(&style, 1);
    lv_style_set_pad_all(&style, 0);
    lv_style_set_prop(&style, LV_STYLE_RADIUS, lv_style_value_t{5});

    auto iron_section = lv_obj_create(parent);
    lv_obj_set_size(iron_section, lv_pct(100), 21);
    lv_obj_align(iron_section, align, 0, 0);
    lv_obj_add_style(iron_section, &style, 0);

    static lv_style_t big_font;
    lv_style_init(&big_font);
    lv_style_set_text_font(&big_font, &lv_font_unscii_8);

    auto bar = lv_bar_create(iron_section);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_RIGHT, -1, -3);
    lv_obj_set_size(bar, 30, 5);
    lv_bar_set_value(bar, val, LV_ANIM_OFF);

    auto temp = lv_label_create(iron_section);
    lv_obj_align(temp, LV_ALIGN_RIGHT_MID, -3, -4);
    lv_obj_add_style(temp, &big_font, LV_PART_MAIN);
    lv_label_set_text(temp, "285");

    auto state = lv_label_create(iron_section);
    lv_obj_align(state, LV_ALIGN_CENTER, -2, 0);
    lv_obj_add_style(state, &big_font, LV_PART_MAIN);
    lv_label_set_text(state, "BST");

    auto iron_type = lv_label_create(iron_section);
    lv_obj_align(iron_type, LV_ALIGN_TOP_LEFT, 1, 1);
    //    lv_obj_add_style(iron_type, &big_font, LV_PART_MAIN);
    lv_label_set_text(iron_type, "T245");

    auto tip_type = lv_label_create(iron_section);
    lv_obj_align(tip_type, LV_ALIGN_BOTTOM_LEFT, 7, -1);
    //    lv_obj_add_style(tip_type, &big_font, LV_PART_MAIN);
    lv_label_set_text(tip_type, "BC3");
}

void add_profile_section(lv_obj_t* parent, lv_align_t align, int32_t val, lv_style_t* style, bool add_mark = false)
{
    auto profile_section = lv_obj_create(parent);
    lv_obj_set_size(profile_section, lv_pct(100), 18);
    lv_obj_align(profile_section, align, 0, 0);
    lv_obj_add_style(profile_section, style, 0);

    static lv_style_t profile_font;
    lv_style_init(&profile_font);
    static lv_style_t select_font;
    lv_style_init(&profile_font);
    lv_style_set_text_font(&select_font, &lv_font_unscii_8);
    lv_style_set_text_font(&profile_font, &lv_font_font5x7);

    auto temp = lv_label_create(profile_section);
    lv_obj_align(temp, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(temp, !add_mark ? &profile_font : &select_font, LV_PART_MAIN);
    lv_label_set_text_fmt(temp, "%i", val);

    static lv_style_t s;
    lv_style_init(&s);
    // lv_style_set_bg_color(&s, lv_color_white());
    lv_style_set_border_width(&s, 0);
    lv_style_set_pad_all(&s, 0);
}

void ui_skeleton()
{
    //    static size_t counter;
    //    const char* const arr[] = {"530", "641", "752", "863", "974", "085", "196", "207"};
    //    lv_label_set_text(l, arr[counter++]);
    //    if(counter > 7) {
    //        counter = 0;
    //    }
    auto irons_section = lv_obj_create(lv_scr_act());
    lv_obj_set_size(irons_section, 85, lv_pct(100));
    lv_obj_align(irons_section, LV_ALIGN_LEFT_MID, 0, 0);
    static lv_style_t style_border;
    lv_style_init(&style_border);
    lv_style_set_border_width(&style_border, 0);
    lv_style_set_pad_all(&style_border, 0);
    lv_obj_add_style(irons_section, &style_border, 0);

    add_iron_section(irons_section, &style_border, LV_ALIGN_TOP_LEFT, 25);
    add_iron_section(irons_section, &style_border, LV_ALIGN_LEFT_MID, 50);
    add_iron_section(irons_section, &style_border, LV_ALIGN_BOTTOM_LEFT, 75);

    auto profile_section = lv_obj_create(lv_scr_act());
    lv_obj_set_size(profile_section, 29, lv_pct(100));
    lv_obj_align(profile_section, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_style(profile_section, &style_border, 0);

    static lv_style_t p_style1;
    lv_style_init(&p_style1);
    //   lv_style_set_border_width(&p_style1, 2);
    lv_style_set_pad_all(&p_style1, 0);
    lv_style_set_prop(&p_style1, LV_STYLE_RADIUS, lv_style_value_t{5});

    add_profile_section(profile_section, LV_ALIGN_TOP_LEFT, 310, &p_style1, true);
    add_profile_section(profile_section, LV_ALIGN_LEFT_MID, 280, &p_style1);
    add_profile_section(profile_section, LV_ALIGN_BOTTOM_LEFT, 150, &p_style1);

    //    static lv_style_t big_font;
    //    lv_style_init(&big_font);
    //    lv_style_set_text_font(&big_font, &lv_font_unscii_16);

    //    static lv_style_t font2;
    //    lv_style_init(&font2);
    //    lv_style_set_text_font(&font2, &lv_font_lcd5x7);

    //    static lv_style_t font3;
    //    lv_style_init(&font3);
    //    lv_style_set_text_font(&font3, &lv_font_Stang5x7);

    //    static lv_style_t font4;
    //    lv_style_init(&font4);
    //    lv_style_set_text_font(&font4, &lv_font_font5x7);

    //    lv_obj_t* label = lv_label_create(lv_scr_act());
    //    lv_label_set_text_static(label, "300");
    //    lv_obj_align(label, LV_ALIGN_TOP_MID, 25, 5);
    //    lv_obj_add_style(label, &big_font, 0);

    //    lv_obj_t* label2 = lv_label_create(lv_scr_act());
    //    lv_label_set_text_static(label2, "ABCD1230789zyz");
    //    lv_obj_align(label2, LV_ALIGN_TOP_MID, 20, 8);
    //    lv_obj_add_style(label2, &font2, 0);

    //    lv_obj_t* label3 = lv_label_create(lv_scr_act());
    //    lv_label_set_text_static(label3, "ABCD1230789zyz");
    //    lv_obj_align(label3, LV_ALIGN_TOP_MID, 20, 16);
    //    lv_obj_add_style(label3, &font3, 0);

    //    lv_obj_t* label4 = lv_label_create(lv_scr_act());
    //    lv_label_set_text_static(label4, "ABCD1230789xyz");
    //    lv_obj_align(label4, LV_ALIGN_TOP_MID, 20, 24);
    //    lv_obj_add_style(label4, &font4, 0);
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

void lv_example_arc_1(void)
{
    /*Create an Arc*/
    lv_obj_t* arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(arc, 50, 50);
    lv_arc_set_rotation(arc, 90);
    lv_arc_set_bg_angles(arc, 0, 350);
    lv_arc_set_value(arc, 90);
    lv_obj_center(arc);
}

static THD_WORKING_AREA(HANDLER_WA_SIZE, 2048);
static THD_FUNCTION(displayHandler, )
{
    ui_skeleton();
    static lv_style_t font1;
    lv_style_init(&font1);
    //    lv_style_set_text_font(&font1, &lv_font_MonoDigits10x16);
    lv_style_set_text_font(&font1, &Hooge);

    lv_obj_t* label1 = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label1, "0");
    lv_obj_align(label1, LV_ALIGN_TOP_RIGHT, -41, 2);
    lv_obj_add_style(label1, &font1, 0);

    static lv_style_t deg_style;
    lv_style_init(&deg_style);
    lv_style_set_border_width(&deg_style, 3);
    // lv_style_set_prop(&deg_style, LV_STYLE_RADIUS, lv_style_value_t{LV_RADIUS_CIRCLE});

    auto degree = lv_obj_create(lv_scr_act());
    lv_obj_set_size(degree, 8, 8);
    lv_obj_align(degree, LV_ALIGN_TOP_RIGHT, -34, 2);
    lv_obj_add_style(degree, &deg_style, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(degree, LV_SCROLLBAR_MODE_OFF);

    auto marker = lv_obj_create(lv_scr_act());
    lv_obj_set_size(marker, 2, 8);
    lv_obj_align(marker, LV_ALIGN_RIGHT_MID, -29, -21);
    lv_obj_set_scrollbar_mode(marker, LV_SCROLLBAR_MODE_OFF);

    size_t counter{};
    size_t value{};
    while(true) {
        lv_timer_handler();
        chThdSleepMilliseconds(5);
        if(++counter == 100) {
            counter = 0;
            if(value == 999) {
                value = 0;
            }
            lv_label_set_text_fmt(label1, "%i", value++);
        }
        //        Display::Check();
    }
}

void init()
{
    lv_init();
    Pins::Init();
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
    //    lv_example_label();
    //    lv_example_line();
    //    lv_example_arc_1();
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
