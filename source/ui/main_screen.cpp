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
#include "styles.h"
#include "ui.h"

static void add_iron_section(lv_obj_t* parent, lv_align_t align, int32_t val)
{
    auto iron_unit = lv_obj_create(parent);
    lv_obj_set_size(iron_unit, lv_pct(100), 21);
    lv_obj_align(iron_unit, align, 0, 0);
    Styles::add(iron_unit, Styles::box_iron_section);

    auto bar = lv_bar_create(iron_unit);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_RIGHT, -1, -3);
    lv_obj_set_size(bar, 30, 5);
    lv_bar_set_value(bar, val, LV_ANIM_OFF);

    auto temp = lv_label_create(iron_unit);
    lv_obj_align(temp, LV_ALIGN_RIGHT_MID, -3, -4);
    Styles::add(temp, Styles::font_normal);
    lv_label_set_text(temp, "285");

    auto state = lv_label_create(iron_unit);
    lv_obj_align(state, LV_ALIGN_CENTER, -2, 0);
    Styles::add(state, Styles::font_normal);
    lv_label_set_text(state, "BST");

    auto iron_type = lv_label_create(iron_unit);
    lv_obj_align(iron_type, LV_ALIGN_TOP_LEFT, 1, 1);
    lv_label_set_text(iron_type, "T245");

    auto tip_type = lv_label_create(iron_unit);
    lv_obj_align(tip_type, LV_ALIGN_BOTTOM_LEFT, 7, -1);
    lv_label_set_text(tip_type, "BC3");
}

static void add_profile_section(lv_obj_t* parent, lv_align_t align, int32_t val, bool add_mark = false)
{
    auto profile_unit = lv_obj_create(parent);
    lv_obj_set_size(profile_unit, lv_pct(100), 18);
    lv_obj_align(profile_unit, align, 0, 0);
    Styles::add(profile_unit, Styles::box_profile_section);

    auto temp = lv_label_create(profile_unit);
    lv_obj_align(temp, LV_ALIGN_CENTER, 0, 0);
    Styles::add(temp, !add_mark ? Styles::font_small : Styles::font_normal);
    lv_label_set_text_fmt(temp, "%i", val);
}

lv_obj_t* ui_init()
{
    auto irons_section = lv_obj_create(lv_scr_act());
    lv_obj_set_size(irons_section, 85, lv_pct(100));
    lv_obj_align(irons_section, LV_ALIGN_LEFT_MID, 0, 0);
    Styles::add(irons_section, Styles::box_zero_border);

    add_iron_section(irons_section, LV_ALIGN_TOP_LEFT, 25);
    add_iron_section(irons_section, LV_ALIGN_LEFT_MID, 50);
    add_iron_section(irons_section, LV_ALIGN_BOTTOM_LEFT, 75);

    auto profile_section = lv_obj_create(lv_scr_act());
    lv_obj_set_size(profile_section, 29, lv_pct(100));
    lv_obj_align(profile_section, LV_ALIGN_RIGHT_MID, 0, 0);
    Styles::add(profile_section, Styles::box_zero_border);

    add_profile_section(profile_section, LV_ALIGN_TOP_LEFT, 310, true);
    add_profile_section(profile_section, LV_ALIGN_LEFT_MID, 280);
    add_profile_section(profile_section, LV_ALIGN_BOTTOM_LEFT, 150);

    lv_obj_t* temp_actual = lv_label_create(lv_scr_act());
    lv_label_set_text_static(temp_actual, "255");
    lv_obj_align(temp_actual, LV_ALIGN_TOP_RIGHT, -41, 3);
    Styles::add(temp_actual, Styles::font_big);

    auto degree = lv_obj_create(lv_scr_act());
    lv_obj_set_size(degree, 8, 8);
    lv_obj_align(degree, LV_ALIGN_TOP_RIGHT, -34, 2);
    Styles::add(degree, Styles::style_degree);
    lv_obj_set_scrollbar_mode(degree, LV_SCROLLBAR_MODE_OFF);

    auto marker = lv_obj_create(lv_scr_act());
    lv_obj_set_size(marker, 2, 8);
    lv_obj_align(marker, LV_ALIGN_RIGHT_MID, -29, -21);
    lv_obj_set_scrollbar_mode(marker, LV_SCROLLBAR_MODE_OFF);
    return temp_actual;
}
