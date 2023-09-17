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

#include "input_handler.h"
#include "ui_config.h"

namespace Input {

enum RawState : EventHandler::raw_event_t {
    RAW_IDLE = 0x00,
    RAW_IRON_1 = 0x04,
    RAW_IRON_2 = 0x02,
    RAW_IRON_3 = 0x01,
    RAW_CONTEXT_1 = 0x08,
    RAW_CONTEXT_2 = 0x10,
    RAW_CONTEXT_3 = 0x20,
    RAW_MENU = 0x40,
};

constexpr auto LONG_TAP_CYCLES = LONG_TAP_MS / LV_TIMER_POLL_MS;

void EventHandler::ProcessEvent()
{
    auto rawState = static_cast<RawState>(cb_());

    if(rawState != RAW_IDLE) {
        if(++cycleCounter_ == LONG_TAP_CYCLES) {
            Emit(rawState, true);
        }
    }
    else if(prevState_ != RAW_IDLE) {
        Emit(rawState, false);
    }
    else {
        cycleCounter_ = 0;
    }
    prevState_ = rawState;
}

void EventHandler::Emit(raw_event_t rawEvent, bool longTap)
{
    eventmask_t events{};
    switch(rawEvent) {
    case RAW_IRON_1:
        events |= EV_IRON_1;
        break;
    case RAW_IRON_2:
        events |= EV_IRON_2;
        break;
    case RAW_IRON_3:
        events |= EV_IRON_2;
        break;
    case RAW_CONTEXT_1:
        events |= EV_CONTEXT_1;
        break;
    case RAW_CONTEXT_2:
        events |= EV_CONTEXT_2;
        break;
    case RAW_CONTEXT_3:
        events |= EV_CONTEXT_3;
        break;
    case RAW_MENU:
        events |= EV_MODE_MENU;
        break;
    case RAW_IDLE:
        break;
    }
    events |= longTap ? EV_PUSH_LONG : 0;
    chEvtBroadcastFlags(&source_, events);
}

} // Input
