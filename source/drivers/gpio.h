/*
 * Copyright (c) 2015,2023 Dmytro Shestakov
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

#pragma once

#include "stm32f4xx.h"
#include "utils.h"
#include <concepts>

namespace Mcucpp::Gpio {

enum InputConf {
    Input
};
enum InputMode {
    Analog = 0x18,
    Floating = 0x00,
    PullUp = 0x01,
    PullDown = 0x02,
};
enum OutputConf {
    // 2 MHz
    OutputSlow,
    // 10 MHz
    OutputFast,
    // 50 MHz
    OutputFastest = 0x03
};
enum OutputMode {
    PushPull = 0x08,
    OpenDrain = 0x0C,
    OpenDrainPullUp = 0x0D,
    AltPushPull = 0x10,
    AltOpenDrain = 0x14,
    AltOpenDrainPullUp = 0x15,
};
enum class AF {
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7
};

namespace Private {
using DataT = uint16_t;

template<uint32_t baseaddr, uint32_t ID>
class PortImplementation
{
private:
    constexpr static inline GPIO_TypeDef* Regs()
    {
        return reinterpret_cast<GPIO_TypeDef*>(baseaddr);
    }
public:
    enum {
        id = ID
    };
    inline static void Set(DataT value)
    {
        Regs()->BSRR = value;
    }
    inline static void Clear(DataT value)
    {
        Regs()->BSRR = value << 8;
    }
    inline static void ClearAndSet(DataT clearMask, DataT value)
    {
        Regs()->BSRR = (value | (uint32_t)clearMask << 16);
    }
    inline static void Toggle(DataT value)
    {
        Regs()->ODR ^= value;
    }
    inline static void Write(DataT value)
    {
        Regs()->ODR = value;
    }
    inline static DataT Read()
    {
        return Regs()->IDR;
    }
    inline static DataT ReadODR()
    {
        return Regs()->ODR;
    }
    inline static GPIO_TypeDef* GetRegs()
    {
        return Regs();
    }

    // constant interface

    template<DataT value>
    inline static void Set()
    {
        Regs()->BSRR = value;
    }
    template<DataT value>
    inline static void Clear()
    {
        Regs()->BSRR = value << 8;
    }
    template<DataT clearMask, DataT value>
    inline static void ClearAndSet()
    {
        Regs()->BSRR = value | (uint32_t)clearMask << 16;
    }
    template<DataT value>
    inline static void Toggle()
    {
        Regs()->ODR ^= value;
    }
    template<DataT value>
    inline static void Write()
    {
        Regs()->ODR = value;
    }

    // end of constant interface

    // all pins except mask will be inputs with pull down
    template<DataT mask, OutputConf speed, OutputMode mode>
    inline static void WriteConfig()
    {
        enum {
            mask2bit = PopulateMask2bit(mask)
        };
        Regs()->MODER = (mode >> 3) * mask2bit;
        Regs()->OSPEEDR = speed * mask2bit;
        Regs()->OTYPER = ((mode >> 2) & 0x01) * mask;
        Regs()->PUPDR = (mode & 0x03) * mask2bit | (~mask2bit) * PullDown;
    }
    template<DataT mask, OutputConf speed, OutputMode mode>
    inline static void SetConfig()
    {
        enum {
            mask2bit = PopulateMask2bit(mask)
        };
        Regs()->MODER = (Regs()->MODER & ~(mask2bit * 0x03)) | (mode >> 3) * mask2bit;
        Regs()->OTYPER = (Regs()->OTYPER & ~mask) | ((mode >> 2) & 0x01) * mask;
        Regs()->OSPEEDR = (Regs()->OSPEEDR & ~(mask2bit * 0x03)) | speed * mask2bit;
        Regs()->PUPDR = (Regs()->PUPDR & ~(mask2bit * 0x03)) | (mode & 0x03) * mask2bit;
    }
    // all pins except mask will be inputs with pull down
    template<DataT mask, InputConf, InputMode mode>
    inline static void WriteConfig()
    {
        enum {
            mask2bit = PopulateMask2bit(mask)
        };
        Regs()->MODER = (mode >> 3) * mask2bit;
        Regs()->PUPDR = (mode & 0x03) * mask2bit | (~mask2bit) * PullDown;
    }
    template<DataT mask, InputConf, InputMode mode>
    inline static void SetConfig()
    {
        enum {
            mask2bit = PopulateMask2bit(mask)
        };
        Regs()->MODER = (Regs()->MODER & ~(mask2bit * 0x03)) | (mode >> 3) * mask2bit;
        Regs()->PUPDR = (Regs()->PUPDR & ~(mask2bit * 0x03)) | (mode & 0x03) * mask2bit;
    }
    template<typename Conf, typename Mode>
    inline static void SetConfig(DataT mask, Conf conf, Mode mode)
    {
        static_assert((std::is_same<InputConf, Conf>::value && std::is_same<InputMode, Mode>::value) ||
                        (std::is_same<OutputConf, Conf>::value && std::is_same<OutputMode, Mode>::value),
                      "SetConfig args error");
        if(std::is_same<OutputConf, Conf>::value) {
            Regs()->OSPEEDR = UnpackConfig2bit(mask, Regs()->OSPEEDR, conf);
            Regs()->OTYPER = (Regs()->OTYPER & ~mask) | ((mode >> 2) & 0x01) * mask;
        }
        Regs()->MODER = UnpackConfig2bit(mask, Regs()->MODER, mode >> 3);
        Regs()->PUPDR = UnpackConfig2bit(mask, Regs()->PUPDR, mode & 0x03);
    }
    inline static void SetSpeed(DataT mask, OutputConf speed)
    {
        Regs()->OSPEEDR = Unpack2bit(mask, Regs()->OSPEEDR, speed);
    }
    inline static void SetPUPD(DataT mask, InputMode pull)
    {
        Regs()->PUPDR = Unpack2bit(mask, Regs()->PUPDR, pull & 0x03);
    }
    inline static void SetDriverType(DataT mask, OutputMode mode)
    {
        Regs()->OTYPER = (Regs()->OTYPER & ~mask) | ((mode >> 2) & 0x01) * mask;
    }
    template<DataT mask, AF af_>
    inline static void AltFuncNumber()
    {
        constexpr uint32_t af = static_cast<uint32_t>(af_);
        static_assert((baseaddr == GPIOA_BASE && af < 8) || (baseaddr == GPIOB_BASE && af < 4),
                      "Wrong alt function number (or port)");
        enum {
            mask4bitLow = PopulateMask4bit(mask & 0xFF),
            mask4bitHigh = PopulateMask4bit((mask >> 8) & 0xFF)
        };
        if(mask & 0xFF)
            Regs()->AFR[0] = (Regs()->AFR[0] & ~mask4bitLow) | af * mask4bitLow;
        if((mask >> 8) & 0xFF)
            Regs()->AFR[1] = (Regs()->AFR[1] & ~mask4bitHigh) | af * mask4bitHigh;
    }
    inline static void AltFuncNumber(DataT mask, uint8_t number)
    {
        if(mask & 0xFF)
            Regs()->AFR[0] = Unpack4bit(mask & 0xFF, Regs()->AFR[0], number);
        if((mask >> 8) & 0xFF)
            Regs()->AFR[1] = Unpack4bit((mask >> 8) & 0xff, Regs()->AFR[1], number);
    }
    inline static void Enable()
    {
        STATIC_ASSERT(id < 6);
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN << id;
    }
    inline static void Disable()
    {
        STATIC_ASSERT(id < 6);
        RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOAEN << id);
    }
};

struct NullPort
{
    enum {
        id = 0xFF
    };
    inline static void Set(DataT)
    { }
    inline static void Clear(DataT)
    { }
    inline static void ClearAndSet(DataT, DataT)
    { }
    inline static void Toggle(DataT)
    { }
    inline static void Write(DataT)
    { }
    // constant interface
    template<DataT>
    inline static void Set()
    { }
    template<DataT>
    inline static void Clear()
    { }
    template<DataT, DataT>
    inline static void ClearAndSet()
    { }
    template<DataT>
    inline static void Toggle()
    { }
    template<DataT>
    inline static void Write()
    { }
    // end of constant interface

    template<DataT, OutputConf, OutputMode> // all pins except mask will be inputs with pull down
    static void WriteConfig()
    { }
    template<DataT, InputConf, InputMode> // all pins except mask will be inputs with pull down
    static void WriteConfig()
    { }
    template<DataT, OutputConf, OutputMode>
    inline static void SetConfig()
    { }
    template<DataT, InputConf, InputMode>
    inline static void SetConfig()
    { }
    inline static void SetSpeed(DataT, OutputConf)
    { }
    inline static void SetPullUp(DataT, InputMode)
    { }
    inline static void SetDriverType(DataT, OutputMode)
    { }
    inline static void Enable()
    { }
    inline static void Disable()
    { }

    template<DataT, AF>
    inline static void AltFuncNumber()
    { }
    inline static void AltFuncNumber(DataT, uint8_t)
    { }
};

template<typename T>
concept PortType = is_nontype_specialization_of<T, Gpio::Private::PortImplementation> || std::is_same_v<T, NullPort>;

enum class Trigger {
    No,
    RisingEdge,
    FallingEdge,
    BothEdges
};

// FIXME: Implementation is broken for F4
template<typename Pin>
class ExtiImplementation
{
private:
    static constexpr IRQn_Type NVIC_IRQ = Pin::position < 2 ? EXTI1_IRQn : Pin::position < 4 ? EXTI2_IRQn : EXTI3_IRQn;
    static void SetTriggerEdge(Trigger tr)
    {
        const auto tr_ = static_cast<uint32_t>(tr);
        EXTI->RTSR |= (tr_ & 0x01 ? Pin::mask : 0);
        EXTI->FTSR |= (tr_ & 0x02 ? Pin::mask : 0);
    }
public:
    static void EnableIRQ(Trigger tr = Trigger::RisingEdge)
    {
        EXTI->IMR |= Pin::mask;
        SetTriggerEdge(tr);
        NVIC_EnableIRQ(NVIC_IRQ);
        SYSCFG->EXTICR[Pin::position / 4] |= Pin::port_id << ((Pin::position % 4) * 4);
    }
    static void DisableIRQ()
    {
        EXTI->IMR &= ~Pin::mask;
        EXTI->RTSR &= ~Pin::mask;
        EXTI->FTSR &= ~Pin::mask;
        NVIC_DisableIRQ(NVIC_IRQ);
        SYSCFG->EXTICR[Pin::position / 4] &= ~(Pin::port_id << ((Pin::position % 4) * 4));
    }
    static void EnableEvent(Trigger tr = Trigger::RisingEdge)
    {
        EXTI->EMR |= Pin::mask;
        SetTriggerEdge(tr);
    }
    static void DisableEvent()
    {
        EXTI->EMR &= ~Pin::mask;
        EXTI->RTSR &= ~Pin::mask;
        EXTI->FTSR &= ~Pin::mask;
    }
    static void ClearPending()
    {
        EXTI->PR = Pin::mask;
    }
    static void SetPriority(uint8_t prio)
    {
        NVIC_SetPriority(NVIC_IRQ, prio);
    }
};

template<PortType PORT, uint16_t pos>
class TPin
{
private:
    using Self = TPin<PORT, pos>;
public:
    using Port = PORT;
    enum {
        position = pos,
        mask = 1 << pos,
        port_id = Port::id
    };
    using Exti = ExtiImplementation<Self>;

    template<OutputConf conf, OutputMode mode>
    inline static void SetConfig()
    {
        Port::template SetConfig<mask, conf, mode>();
    }

    template<InputConf conf, InputMode mode>
    inline static void SetConfig()
    {
        Port::template SetConfig<mask, conf, mode>();
    }
    template<typename Conf, typename Mode>
    inline static void SetConfig(Conf conf, Mode mode)
    {
        Port::SetConfig(mask, conf, mode);
    }

    template<AF altfunc>
    inline static void AltFuncNumber()
    {
        Port::template AltFuncNumber<mask, altfunc>();
    }
    inline static void AltFuncNumber(uint8_t number)
    {
        Port::AltFuncNumber(mask, number);
    }

    inline static void Set()
    {
        Port::template Set<mask>();
    }
    inline static void Clear()
    {
        Port::template Clear<mask>();
    }
    inline static void SetOrClear(bool cond)
    {
        if(cond) {
            Set();
        }
        else {
            Clear();
        }
    }
    inline static void Toggle()
    {
        Port::template Toggle<mask>();
    }
    inline static bool IsSet()
    {
        return Port::Read() & mask;
    }
    inline static bool IsSetODR()
    {
        return Port::ReadODR() & mask;
    }
};

template<typename T>
concept PinType = requires(T t)
{
    []<PortType p, uint16_t pos>(TPin<p, pos>&) {}(t);
};

template<PortType... ports>
struct PortsEnableMask;
template<PortType First, PortType... Rest>
struct PortsEnableMask<First, Rest...>
{
    static_assert(First::id < 6 || First::id == 8, "This port is not present");
    enum {
        value = RCC_AHB1ENR_GPIOAEN << First::id | PortsEnableMask<Rest...>::value
    };
};
template<>
struct PortsEnableMask<>
{
    enum {
        value = 0
    };
};

} // Private

template<typename First, typename... Rest>
inline void EnablePorts()
{
    using namespace Private;
    RCC->AHB1ENR |= PortsEnableMask<First, Rest...>::value;
}

template<Gpio::Private::PinType Pin>
struct Inverted : public Pin
{
    static void Set()
    {
        Pin::Clear();
    }
    static void Clear()
    {
        Pin::Set();
    }
};

using Private::Trigger;
template<typename Pin>
using Exti = Private::ExtiImplementation<Pin>;

#define PORTDEF(x, y, z) using Port##y = Gpio::Private::PortImplementation<GPIO##x##_BASE, z>

PORTDEF(A, a, 0);
PORTDEF(B, b, 1);
PORTDEF(C, c, 2);
PORTDEF(D, d, 3);
PORTDEF(E, e, 4);
PORTDEF(H, h, 7);

using NullPort = Private::NullPort; // Dummy port
using Private::PortType;

#define PINSDEF(x)                                     \
    using P##x##0 = Gpio::Private::TPin<Port##x, 0>;   \
    using P##x##1 = Gpio::Private::TPin<Port##x, 1>;   \
    using P##x##2 = Gpio::Private::TPin<Port##x, 2>;   \
    using P##x##3 = Gpio::Private::TPin<Port##x, 3>;   \
    using P##x##4 = Gpio::Private::TPin<Port##x, 4>;   \
    using P##x##5 = Gpio::Private::TPin<Port##x, 5>;   \
    using P##x##6 = Gpio::Private::TPin<Port##x, 6>;   \
    using P##x##7 = Gpio::Private::TPin<Port##x, 7>;   \
    using P##x##8 = Gpio::Private::TPin<Port##x, 8>;   \
    using P##x##9 = Gpio::Private::TPin<Port##x, 9>;   \
    using P##x##10 = Gpio::Private::TPin<Port##x, 10>; \
    using P##x##11 = Gpio::Private::TPin<Port##x, 11>; \
    using P##x##12 = Gpio::Private::TPin<Port##x, 12>; \
    using P##x##13 = Gpio::Private::TPin<Port##x, 13>; \
    using P##x##14 = Gpio::Private::TPin<Port##x, 14>; \
    using P##x##15 = Gpio::Private::TPin<Port##x, 15>;

PINSDEF(a)
PINSDEF(b)
PINSDEF(c)
PINSDEF(d)
PINSDEF(e)
PINSDEF(h)

using Nullpin = Private::TPin<NullPort, 0x00>; // Dummy pin
using Private::PinType;

} // Mcucpp::Gpio
