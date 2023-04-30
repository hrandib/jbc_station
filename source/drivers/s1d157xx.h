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

#ifndef S1D157XX_H
#define S1D157XX_H

#include "pinlist.h"
#include "port_driver.h"
#include <array>
#include <utility>

namespace Drivers {

using namespace Mcucpp::Gpio;

template<PinlistType DataBus, PinType CsPin, PinType A0Pin, PinType ResetPin>
class S1d157xx
{
    enum InstructionSet : uint8_t {
        C_COLUMN_LOW = 0x00,  // 4 bit LSB - Low order bits of column address
        C_COLUMN_HIGH = 0x10, // 4 bit LSB - High order bits of column address
        C_V5_ADJUST = 0x20,   // 3 bit LSB - Rb to Ra ratio
        C_POWER_CTRL = 0x28,  // 2nd bit - Boost circuit; 1st bit - V adjust; 0 bit - V/F circuit
        C_POWER_CTRL_ON = 0x28 | 0b0111,
        C_STARTLINE = 0x40,                        // 6 bit LSB - Start address
        C_ELECTRONIC_CONTROL_1 = 0x81,             // First byte of the 2-byte command
        C_ELECTRONIC_CONTROL_2 = 0x00,             // 6 bit LSB - adjusts V5, 0x20 to switch off
        C_SEGMENT_DIR_CLOCKWISE = 0xA0,            // Clockwise segment rotation
        C_SEGMENT_DIR_COUNTERCLOCKWISE = 0xA0 | 1, // Counterclockwise segment rotation
        C_BIAS_1_9 = 0xA2,                         // 1/9 bias
        C_BIAS_1_7 = 0xA2 | 1,                     // 1/7 bias
        C_DISP_FORCE_NORMAL = 0xA4,                // 1 bit LSB - 0(Normal), 1(Force all pixels on)
        C_DISP_FORCE_ON = 0xA4 | 1,                //
        C_DISP_NONINVERT = 0xA6,                   // 1 bit LSB - 0(Normal), 1(Invert)
        C_DISP_INVERT = 0xA6 | 1,
        C_STANDBY = 0xA8,             // Enter sleep mode
        C_SLEEP = 0xA8 | 1,           // Enter stand-by mode
        C_OSC_ENABLE = 0xAB,          // Internal oscillator enable
        C_OFF = 0xAE,                 // Display Off
        C_ON = C_OFF | 1,             // Display On
        C_PAGEADDRESS = 0xB0,         // 4 bit LSB - Page address
        C_COM_SCANDIR = 0xC0,         // Normal COM scan direction
        C_COM_SCANDIR_REVERSE = 0xC8, // Reversal COM scan direction
        C_RMW_START = 0xE0,           // Starts Read-modify-write sequence
        C_POWERSAVE_RESET = 0xE1,     // Resets previously set power save mode
        C_RESET = 0xE2,               // Resets all internal registers to initial values except data RAM
        C_NOP = 0xE3,                 // No operation
        C_RMW_END = 0xEE,             // Ends Read-modify-write sequence
    };

    constexpr friend InstructionSet operator|(InstructionSet a, auto b)
    {
        return static_cast<InstructionSet>(static_cast<uint8_t>(a) | b);
    }

    enum class Mode {
        COMMAND,
        DATA
    };

    constexpr static uint8_t V5_ADJUST = 0x06;
    constexpr static uint8_t V5_VAL = 0x13;
    constexpr static std::array init_seq{
      C_RESET,
      C_OSC_ENABLE,
      C_BIAS_1_7,
      C_SEGMENT_DIR_CLOCKWISE,
      C_COM_SCANDIR_REVERSE,
      C_V5_ADJUST | V5_ADJUST,
      C_ELECTRONIC_CONTROL_1,
      C_ELECTRONIC_CONTROL_2 | V5_VAL,
      C_POWER_CTRL_ON,
      C_DISP_NONINVERT,
      C_STARTLINE | 0,
      C_ON,
    };

    static void SetMode(Mode mode)
    {
        A0Pin::SetOrClear(std::to_underlying(mode));
    }

    static void Reset()
    {
        ResetPin::Clear();
        delay_ms(2);
        ResetPin::Set();
    }

    static void Send(uint8_t cmd)
    {
        DataBus::Write(cmd);
        CsPin::Clear();
        delay_ms(1);
        CsPin::Set();
        delay_ms(1);
    }
public:
    static void Init()
    {
        Reset();
        CsPin::Set();
        SetMode(Mode::COMMAND);
        for(auto cmd : init_seq) {
            delay_ms(10);
            Send(cmd);
        }
    }

    static void Check()
    {
        Send(C_DISP_FORCE_ON);
        delay_ms(500);
        Send(C_DISP_FORCE_NORMAL);
        delay_ms(500);
        Send(C_DISP_INVERT);
        delay_ms(500);
        Send(C_DISP_NONINVERT);
        delay_ms(500);
    }
};

} // Drivers

#endif // S1D157XX_H
