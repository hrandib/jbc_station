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

#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "hal.h"

namespace Input {

enum Event {
    EV_MODE_MENU = 1U << 0, // Switch between Main<->Aux mode on short tap and enter Settings on long tap
    EV_CONTEXT_1 = 1U << 1, // Up
    EV_CONTEXT_2 = 1U << 2, // Down
    EV_CONTEXT_3 = 1U << 3, // Back
    EV_IRON_1 = 1U << 4,
    EV_IRON_2 = 1U << 5,
    EV_IRON_3 = 1U << 6,

    EV_PUSH_LONG = 1U << 7,
};

class EventHandler
{
public:
    using raw_event_t = uint8_t;
    using RawReaderCb = raw_event_t (*)();
    EventHandler(RawReaderCb cb) : cb_{cb}
    {
        chEvtObjectInit(&source_);
    }
    void ProcessEvent();
private:
    RawReaderCb cb_;
    raw_event_t prevState_{};
    size_t cycleCounter_{};
    event_source_t source_;

    void Emit(raw_event_t rawEvent, bool longTap);
};

} // Input

#endif // INPUT_HANDLER_H
