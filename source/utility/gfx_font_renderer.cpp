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

#include "gfx_font_renderer.h"

#include <array>

namespace Fonts {

// Must fit the largest defined symbol
constexpr auto CONVERT_BUF_SIZE = 20;

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

/* Get the bitmap of `unicode_letter` from `font`. */
const uint8_t* get_glyph_bitmap_cb(const lv_font_t* font, uint32_t unicode_letter)
{
    static uint8_t buf[CONVERT_BUF_SIZE];
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

} // Fonts
