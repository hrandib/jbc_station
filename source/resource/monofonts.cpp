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

#include "monofonts.h"
#include "Adafruit5x7.h"
#include "Stang5x7.h"
#include "System5x7.h"
#include "fixednums15x31.h"
#include "font5x7.h"
#include "lcd5x7.h"
#include "lcdnums12x16.h"
#include "lcdnums14x24.h"

#include "driver_utils.h"
#include <array>

// Font Indices
/** No longer used Big Endian length field. Now indicates font type.
 *
 * 00 00 (fixed width font with 1 padding pixel on right and below)
 *
 * 00 01 (fixed width font with no padding pixels)
 */
#define FONT_LENGTH 0
/** Maximum character width. */
#define FONT_WIDTH 2
/** Font hight in pixels */
#define FONT_HEIGHT 3
/** Ascii value of first character */
#define FONT_FIRST_CHAR 4
/** count of characters in font. */
#define FONT_CHAR_COUNT 5
/** Offset to width table. */
#define FONT_WIDTH_TABLE 6

struct GlcdFont
{
    const uint16_t size;
    const uint8_t width;
    const uint8_t height;
    const uint8_t firstChar;
    const uint8_t charCount;
    const uint8_t data[];
    constexpr uint8_t end() const
    {
        return firstChar + charCount;
    }
    constexpr bool withinRange(uint8_t letter) const
    {
        return letter >= firstChar || letter < end();
    }
    constexpr uint8_t padSize() const
    {
        return (width >> 3) + 1;
    }
    constexpr const uint8_t* getGlyphData(uint32_t letter) const
    {
        if(withinRange(letter)) {
            auto index = (letter - firstChar) * width * heightBytes();
            return &data[index];
        }
        else {
            return nullptr;
        }
    }
    constexpr uint8_t heightBytes() const
    {
        return (height >> 3) + 1;
    }
};

/* Get info about glyph of `unicode_letter` in `font` font.
 * Store the result in `dsc_out`.
 * The next letter (`unicode_letter_next`) might be used to calculate the width required by this glyph (kerning)
 */
bool get_glyph_dsc_cb(const lv_font_t* font,
                      lv_font_glyph_dsc_t* dsc_out,
                      uint32_t unicode_letter,
                      uint32_t /*unicode_letter_next*/)
{
    auto gfont = (const GlcdFont*)font->dsc;
    if(!gfont->withinRange(unicode_letter)) {
        return false;
    }

    dsc_out->adv_w = gfont->width + gfont->padSize(); /*Horizontal space required by the glyph in [px]*/
    dsc_out->box_h = gfont->height + 1;               /*Height of the bitmap in [px]*/
    dsc_out->box_w = gfont->width;                    /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = 1;                               /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = 0;                               /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 1;                                 /*Bits per pixel: 1/2/4/8*/

    return true; /*true: glyph found; false: glyph was not found*/
}

/* Get info about glyph of `unicode_letter` in `font` font.
 * Store the result in `dsc_out`.
 * The next letter (`unicode_letter_next`) might be used to calculate the width required by this glyph (kerning)
 */
bool get_glyph_dsc_2x_cb(const lv_font_t* font,
                         lv_font_glyph_dsc_t* dsc_out,
                         uint32_t unicode_letter,
                         uint32_t /*unicode_letter_next*/)
{
    auto gfont = (const GlcdFont*)font->dsc;
    if(!gfont->withinRange(unicode_letter)) {
        return false;
    }

    dsc_out->adv_w = (gfont->width * 2) + gfont->padSize(); /*Horizontal space required by the glyph in [px]*/
    dsc_out->box_h = (gfont->height * 2) + 1;               /*Height of the bitmap in [px]*/
    dsc_out->box_w = (gfont->width * 2);                    /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = 1;                                     /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = 0;                                     /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 1;                                       /*Bits per pixel: 1/2/4/8*/

    return true; /*true: glyph found; false: glyph was not found*/
}

/* Get the bitmap of `unicode_letter` from `font`. */
const uint8_t* get_glyph_bitmap_cb(const lv_font_t* font, uint32_t unicode_letter)
{
    static uint8_t buf[20];
    auto gfont = (const GlcdFont*)font->dsc;

    if(auto bytes = gfont->getGlyphData(unicode_letter); bytes) {
        for(size_t h{}; h < gfont->heightBytes(); ++h) {
            for(size_t i{}; i < gfont->width; ++i) {
                for(size_t b{}; b < 8; ++b) {
                    auto bitIndex = b * gfont->width + i;
                    if(*bytes & (1 << b)) {
                        buf[(bitIndex >> 3) + (h * gfont->width)] |= 1 << (7 - (bitIndex % 8));
                    }
                    else {
                        buf[(bitIndex >> 3) + (h * gfont->width)] &= ~(1 << (7 - (bitIndex % 8)));
                    }
                }
                bytes++;
            }
        }
        return &buf[0];
    }
    else {
        return nullptr;
    }
}

#define DEFINE_LVFONT(origin)                                                                                     \
    static_assert(origin[0] == 0 && origin[1] == 0); /*Only simple monospace fonts are supported by the wrapper*/ \
    const lv_font_t lv_font_##origin{                                                                             \
      .get_glyph_dsc = get_glyph_dsc_cb,                  /*Set a callback to get info about glyphs*/             \
      .get_glyph_bitmap = get_glyph_bitmap_cb,            /*Set a callback to get bitmap of a glyph*/             \
      .line_height = lv_coord_t(origin[FONT_HEIGHT] + 2), /*The real line height where any text fits*/            \
      .base_line = 0,                                     /*Base line measured from the top of line_height*/      \
      .subpx = LV_FONT_SUBPX_NONE,                        /*Not used for monochrome*/                             \
      .underline_position = 0,                            /*Not used*/                                            \
      .underline_thickness = 0,                           /*Not used*/                                            \
      .dsc = (const void*)origin,                         /*Store any implementation specific data here*/         \
      .fallback = &lv_font_unscii_8,                      /*Use if glyph is absent*/                              \
      .user_data = nullptr,                               /*Optionally some extra user data*/                     \
    };

// TODO: leave only required
DEFINE_LVFONT(Adafruit5x7)
DEFINE_LVFONT(lcd5x7)
DEFINE_LVFONT(Stang5x7)
DEFINE_LVFONT(System5x7)
DEFINE_LVFONT(font5x7)
DEFINE_LVFONT(fixednums15x31)
DEFINE_LVFONT(lcdnums12x16)
DEFINE_LVFONT(lcdnums14x24)
