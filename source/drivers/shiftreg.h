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

#ifndef SHIFTREG_H
#define SHIFTREG_H

#include "gpio.h"
#include <concepts>

namespace Drivers {

using namespace Mcucpp::Gpio;
using std::size_t;

template<std::unsigned_integral V, PinType Clk, PinType Dat, PinType Cs = Nullpin, PinType KeyPoll = Nullpin>
struct ShiftReg
{
    using base_t = V;

    static void Init()
    {
        constexpr static auto conf = OutputConf::OutputFastest;
        constexpr static auto mode = OutputMode::PushPull;
        Clk::template SetConfig<conf, mode>();
        Dat::template SetConfig<conf, mode>();
        Cs::template SetConfig<conf, mode>();
        KeyPoll::template SetConfig<InputConf::Input, InputMode::PullUp>();
    }

    static void Write(V val)
    {
        Cs::Clear();
        for(auto bitnum = sizeof(V) * 8; bitnum > 0; --bitnum) {
            Clk::Clear();
            Dat::SetOrClear(val & (1 << bitnum));
            Clk::Set();
        }
        Cs::Set();
    }

    static V Read()
    {
        Cs::Clear();
        Clk::Clear();
        Dat::Set();
        Clk::Set();
        Cs::Set();
        Dat::Clear();
        V result = KeyPoll::IsSet();
        for(size_t i{1}; i < 8 * sizeof(V); ++i) {
            Cs::Clear();
            Clk::Clear();
            Clk::Set();
            Cs::Set();
            result = KeyPoll::IsSet() << i;
        }
        return result;
    }
};

} // Drivers

#endif // SHIFTREG_H
