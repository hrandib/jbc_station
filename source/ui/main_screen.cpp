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

#include "lvgl.h"
#include "monofonts.h"
#include "ui.h"

static void add_iron_section(lv_obj_t* parent, lv_style_t* /*style*/, lv_align_t align, int32_t val)
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

static void add_profile_section(lv_obj_t* parent,
                                lv_align_t align,
                                int32_t val,
                                lv_style_t* style,
                                bool add_mark = false)
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

lv_obj_t* ui_init()
{
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

    static lv_style_t font1;
    lv_style_init(&font1);
    //    lv_style_set_text_font(&font1, &lv_font_MonoDigits10x16);
    lv_style_set_text_font(&font1, &lv_font_unscii_8);

    lv_obj_t* label1 = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label1, "0");
    lv_obj_align(label1, LV_ALIGN_TOP_RIGHT, -50, 2);
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
    return label1;
}
