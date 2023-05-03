/*
 * Copyright (c) 2017 Dmytro Shestakov
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
#ifndef CH_EXTENDED_H
#define CH_EXTENDED_H

#include "ch.hpp"

namespace Rtos {

using chibios_rt::BaseStaticThread;
using chibios_rt::BaseThread;
using chibios_rt::BinarySemaphore;
using chibios_rt::Mailbox;
using chibios_rt::System;
using chibios_rt::ThreadReference;

enum class Status { Success, Failure };

struct SysLockGuard
{
    SysLockGuard()
    {
        System::lock();
    }
    ~SysLockGuard()
    {
        System::unlock();
    }
};
struct SysLockGuardFromISR
{
    SysLockGuardFromISR()
    {
        System::lockFromIsr();
    }
    ~SysLockGuardFromISR()
    {
        System::unlockFromIsr();
    }
};

struct SemLockGuard
{
    BinarySemaphore& sem;
    SemLockGuard(BinarySemaphore& sem_) : sem{sem_}
    {
        sem.wait();
    }
    ~SemLockGuard()
    {
        sem.signal();
    }
};

// chibios_rt::ThreadStayPoint implementation is broken
class ThreadStayPoint
{
private:
    ::thread_reference_t ref_;
public:
    ThreadStayPoint() : ref_{}
    { }
    msg_t Suspend(systime_t timeout = 0)
    {
        SysLockGuard lock;
        return timeout ? chThdSuspendTimeoutS(&ref_, timeout) : chThdSuspendS(&ref_);
    }
    void Resume(msg_t msg = MSG_OK)
    {
        SysLockGuard lock;
        chThdResumeS(&ref_, msg);
    }
    void ResumeFromISR(msg_t msg = MSG_OK)
    {
        SysLockGuardFromISR lock;
        chThdResumeI(&ref_, msg);
    }
};

static inline void Sleep(systime_t interval)
{
    chThdSleep(interval);
}

static inline ::thread_t* GetThdSelf()
{
    return chThdGetSelfX();
}
} // Rtos

#endif // CH_EXTENDED_H
