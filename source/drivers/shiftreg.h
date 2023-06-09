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

#include "ch.h"
#include "gpio.h"
#include <concepts>

namespace Drivers {

using namespace Mcucpp::Gpio;
using std::size_t;

// Helper class for 74HC164 and 74HC595 shift registers using as a port expander
template<std::unsigned_integral V, PinType Clk, PinType Dat, PinType Cs = Nullpin, PinType KeyPoll = Nullpin>
struct ShiftReg
{
    using base_t = V;
    static constexpr auto DELAY_NOP_NUM_INPUT = 8;
    static constexpr auto DELAY_NOP_NUM_OUTPUT = 1;

    static void Init()
    {
        constexpr static auto conf = OutputConf::OutputFastest;
        constexpr static auto mode = OutputMode::PushPull;
        Clk::template SetConfig<conf, mode>();
        Dat::template SetConfig<conf, mode>();
        Cs::template SetConfig<conf, mode>();
        KeyPoll::template SetConfig<InputConf::Input, InputMode::PullDown>();
    }

    static void Write(V val)
    {
        Cs::Clear();
        for(auto bitnum = (sizeof(V) * 8); bitnum--;) {
            Clk::Clear();
            Mcucpp::NopDelay<DELAY_NOP_NUM_OUTPUT>();
            Dat::SetOrClear(val & (V{1} << bitnum));
            Mcucpp::NopDelay<DELAY_NOP_NUM_OUTPUT>();
            Clk::Set();
            Mcucpp::NopDelay<DELAY_NOP_NUM_OUTPUT>();
        }
        Cs::Set();
    }

    static V Read()
    {
        Write(0);
        Cs::Clear();
        Clk::Clear();
        Dat::Set();
        Mcucpp::NopDelay<DELAY_NOP_NUM_OUTPUT>();
        Clk::Set();
        Cs::Set();
        Mcucpp::NopDelay<DELAY_NOP_NUM_OUTPUT>();
        Dat::Clear();
        Mcucpp::NopDelay<DELAY_NOP_NUM_INPUT>();
        V result = KeyPoll::IsSet();
        for(size_t bitnum{1}; bitnum < 8 * sizeof(V); ++bitnum) {
            Cs::Clear();
            Clk::Clear();
            Mcucpp::NopDelay<DELAY_NOP_NUM_OUTPUT>();
            Clk::Set();
            Cs::Set();
            Mcucpp::NopDelay<DELAY_NOP_NUM_INPUT>();
            result |= KeyPoll::IsSet() << bitnum;
        }
        return result;
    }
};

} // Drivers

#endif // SHIFTREG_H
