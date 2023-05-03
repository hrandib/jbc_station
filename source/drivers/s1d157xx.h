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

#include "ch_port.h"
#include "pinlist.h"
#include <array>
#include <utility>

namespace Drivers {

using namespace Mcucpp;
using namespace Mcucpp::Gpio;

struct S1D15710
{
    constexpr static bool ROTATE_180 = true;
    constexpr static uint8_t V5_ADJUST = 0x07;
    constexpr static uint8_t V5_VAL = 0x09;
    enum {
        X_DIM = 219,
        X_EXT = 221,
        Y_DIM = 60,
        PAGES = 8,
        PAGES_EXT = 9,

        GRAM_COLS = 255,
        Y_OFFSET = ROTATE_180 ? 0 : Y_DIM,
        X_OFFSET = ROTATE_180 ? GRAM_COLS - X_DIM : 1,
        X_OFFSET_EXT = ROTATE_180 ? GRAM_COLS - X_DIM : 0
    };
};

struct S1D15705
{
    constexpr static bool ROTATE_180 = false;
    constexpr static uint8_t V5_ADJUST = 0x06;
    constexpr static uint8_t V5_VAL = 0x11;
    enum {
        X_DIM = 162,
        X_EXT = 165,
        Y_DIM = 64,
        PAGES = 8,
        PAGES_EXT = 9,

        GRAM_COLS = 255,
        Y_OFFSET = 0,
        X_OFFSET = ROTATE_180 ? GRAM_COLS - X_DIM : 1,
        X_OFFSET_EXT = ROTATE_180 ? GRAM_COLS - X_DIM : 0
    };
};

template<typename T>
concept S1D157xxType = is_derived_from_any<T, S1D15705, S1D15710>;

template<S1D157xxType Disp, PinlistType DataBus, PinType CsPin, PinType A0Pin, PinType ResetPin>
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

    constexpr static std::array init_seq{
      C_OSC_ENABLE,
      C_BIAS_1_7,
      Disp::ROTATE_180 ? C_SEGMENT_DIR_COUNTERCLOCKWISE : C_SEGMENT_DIR_CLOCKWISE,
      Disp::ROTATE_180 ? C_COM_SCANDIR : C_COM_SCANDIR_REVERSE,
      C_V5_ADJUST | Disp::V5_ADJUST,
      C_ELECTRONIC_CONTROL_1,
      C_ELECTRONIC_CONTROL_2 | Disp::V5_VAL,
      C_POWER_CTRL_ON,
      C_DISP_INVERT,
      C_STARTLINE | Disp::Y_OFFSET,
      C_ON,
    };

    static void SetMode(Mode mode)
    {
        A0Pin::SetOrClear(std::to_underlying(mode));
    }

    static void Reset()
    {
        ResetPin::Clear();
        delay_ms(1);
        ResetPin::Set();
    }

    static void Send(uint8_t byte)
    {
        CsPin::Clear();
        DataBus::Write(byte);
        CsPin::Set();
    }

    static void SendCommand(uint8_t cmd)
    {
        SetMode(Mode::COMMAND);
        Send(cmd);
    }

    static void SendData(uint8_t cmd)
    {
        SetMode(Mode::DATA);
        Send(cmd);
    }

    static void Clear()
    {
        for(size_t page{}; page < Disp::PAGES_EXT; ++page) {
            SendCommand(C_PAGEADDRESS | page);
            SendCommand(C_COLUMN_HIGH | (Disp::X_OFFSET_EXT >> 4));
            SendCommand(C_COLUMN_LOW | (Disp::X_OFFSET_EXT & 0x0F));
            for(size_t i = 0; i < Disp::X_EXT; ++i) {
                SendData(0);
            }
        }
    }

    static void Fill()
    {
        for(size_t page{}; page < Disp::PAGES; ++page) {
            SendCommand(C_PAGEADDRESS | page);
            SendCommand(C_COLUMN_HIGH | (Disp::X_OFFSET >> 4));
            SendCommand(C_COLUMN_LOW | (Disp::X_OFFSET & 0x0F));
            for(size_t i = 0; i < Disp::X_DIM; ++i) {
                delay_ms(5);
                SendData(0xFF);
            }
        }
    }
public:
    using Props = Disp;

    static void Init()
    {
        delay_ms(30);
        Reset();
        CsPin::Set();
        for(auto cmd : init_seq) {
            SendCommand(cmd);
        }
        Clear();
    }

    static void PutByte(size_t x, size_t y_page, uint8_t byte)
    {
        x += Disp::X_OFFSET;
        SendCommand(C_PAGEADDRESS | y_page);
        SendCommand(C_COLUMN_HIGH | (x >> 4));
        SendCommand(C_COLUMN_LOW | (x & 0x0F));
        SendData(byte);
    }

    static void PutPage(size_t x1, size_t x2, size_t y_page, uint8_t* buf)
    {
        if(x1 >= x2) {
            return;
        }
        if(x2 >= Disp::X_DIM) {
            x2 = Disp::X_DIM;
        }
        if(y_page >= Disp::PAGES) {
            y_page = Disp::PAGES;
        }
        size_t x = x1 + Disp::X_OFFSET;
        SendCommand(C_PAGEADDRESS | y_page);
        SendCommand(C_COLUMN_HIGH | (x >> 4));
        SendCommand(C_COLUMN_LOW | (x & 0x0F));
        size_t len = x2 - x1;
        for(size_t i{}; i < len; ++i) {
            SendData(*buf++);
        }
    }

    static void Check()
    {
        SendCommand(C_DISP_NONINVERT);
        Fill();
        delay_ms(2000);
        Clear();
        delay_ms(2000);

        SendCommand(C_DISP_INVERT);
        delay_ms(2000);
        Fill();
        delay_ms(2000);
        Clear();
        delay_ms(2000);
    }
};

} // Drivers

#endif // S1D157XX_H
