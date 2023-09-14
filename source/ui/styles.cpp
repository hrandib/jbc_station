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

#include "styles.h"
#include "monofonts.h"

extern const lv_font_t Hooge;

namespace Styles {

static const lv_style_const_prop_t props_font_big[] = {LV_STYLE_CONST_TEXT_FONT(&Hooge)};
static const lv_style_const_prop_t props_font_normal[] = {LV_STYLE_CONST_TEXT_FONT(&lv_font_unscii_8)};
static const lv_style_const_prop_t props_font_small[] = {LV_STYLE_CONST_TEXT_FONT(&lv_font_font5x7)};

LV_STYLE_CONST_INIT(font_big, props_font_big);
LV_STYLE_CONST_INIT(font_normal, props_font_normal);
LV_STYLE_CONST_INIT(font_small, props_font_small);

} // Styles
