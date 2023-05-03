/*
 * Copyright (c) 2022 Dmytro Shestakov
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

#ifndef CPPSTREAMS_H
#define CPPSTREAMS_H

#include "hal.h"
#include "hal_streams.h"

namespace streams {

struct BaseSequentialStream
{
    // Padding to match with ::BaseSequentialStream VMT
    virtual void dummy()
    { }
    /* Stream write buffer method.*/
    virtual size_t write(const uint8_t* bp, size_t n) = 0;
    /* Stream read buffer method.*/
    virtual size_t read(uint8_t* bp, size_t n) = 0;
    /* Channel put method, blocking.*/
    virtual msg_t put(uint8_t b) = 0;
    /* Channel get method, blocking.*/
    virtual msg_t get() = 0;

    ::BaseSequentialStream* getBase()
    {
        return reinterpret_cast<::BaseSequentialStream*>(this);
    }
};

}
#endif // CPPSTREAMS_H
