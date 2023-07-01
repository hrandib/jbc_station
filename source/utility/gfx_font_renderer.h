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

#ifndef GFX_FONT_RENDERER_H
#define GFX_FONT_RENDERER_H

#include "lvgl.h"

#define GLCDFONTDECL(_n) constexpr uint8_t _n[]
#define ADAFRUIT_ASCII96 1

namespace Fonts {

// Font Indices
/** No longer used Big Endian length field. Now indicates font type.
 *
 * 00 00 (fixed width font with 1 padding pixel on right and below)
 *
 * 00 01 (fixed width font with no padding pixels)
 */
constexpr struct
{
    size_t LENGTH = 0;
    /** Maximum character width. */
    size_t WIDTH = 2;
    /** Font hight in pixels */
    size_t HEIGHT = 3;
    /** Ascii value of first character */
    size_t FIRST_CHAR = 4;
    /** count of characters in font. */
    size_t CHAR_COUNT = 5;
    /** Offset to width table. */
    size_t WIDTH_TABLE = 6;
} GfxIndex;

extern bool get_glyph_dsc_cb(const lv_font_t* font,
                             lv_font_glyph_dsc_t* dsc_out,
                             uint32_t unicode_letter,
                             uint32_t /*unicode_letter_next*/);
extern const uint8_t* get_glyph_bitmap_cb(const lv_font_t* font, uint32_t unicode_letter);

} // Fonts

#define DECLARE_LVFONT(origin) extern const lv_font_t lv_font_##origin;
#define DEFINE_LVFONT(origin)                                                                                     \
    static_assert(origin[0] == 0 && origin[1] == 0); /*Only simple monospace fonts are supported by the wrapper*/ \
    const lv_font_t lv_font_##origin{                                                                             \
      .get_glyph_dsc = Fonts::get_glyph_dsc_cb,                      /*Set a callback to get info about glyphs*/  \
      .get_glyph_bitmap = Fonts::get_glyph_bitmap_cb,                /*Set a callback to get bitmap of a glyph*/  \
      .line_height = lv_coord_t(origin[Fonts::GfxIndex.HEIGHT] + 2), /*The real line height where any text fits*/ \
      .base_line = 0,                /*Base line measured from the top of line_height*/                           \
      .subpx = LV_FONT_SUBPX_NONE,   /*Not used for monochrome*/                                                  \
      .underline_position = 0,       /*Not used*/                                                                 \
      .underline_thickness = 0,      /*Not used*/                                                                 \
      .dsc = (const void*)origin,    /*Store any implementation specific data here*/                              \
      .fallback = &lv_font_unscii_8, /*Use if glyph is absent*/                                                   \
      .user_data = nullptr,          /*Optionally some extra user data*/                                          \
    };

#endif // GFX_FONT_RENDERER_H
