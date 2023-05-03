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

#ifndef DRIVER_UTILS_H
#define DRIVER_UTILS_H

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace Mcucpp {

using std::size_t;

constexpr unsigned Unpack4bit(unsigned mask, unsigned value, unsigned configuration)
{
    mask = (mask & 0xf0) << 12 | (mask & 0x0f);
    mask = (mask & 0x000C000C) << 6 | (mask & 0x00030003);
    mask = (mask & 0x02020202) << 3 | (mask & 0x01010101);
    return (value & ~(mask * 0x15)) | mask * configuration;
}

constexpr unsigned Unpack2bit(unsigned mask, unsigned value, unsigned configuration)
{
    mask = (mask & 0xff00) << 8 | (mask & 0x00ff);
    mask = (mask & 0x00f000f0) << 4 | (mask & 0x000f000f);
    mask = (mask & 0x0C0C0C0C) << 2 | (mask & 0x03030303);
    mask = (mask & 0x22222222) << 1 | (mask & 0x11111111);
    return (value & ~(mask * 0x03)) | mask * configuration;
}

consteval size_t PopulateMask4bit(size_t mask)
{
    unsigned mask1 = (mask & 0xf0) << 12 | (mask & 0x0f);
    unsigned mask2 = (mask1 & 0x000C000C) << 6 | (mask1 & 0x00030003);
    unsigned mask3 = (mask2 & 0x02020202) << 3 | (mask2 & 0x01010101);
    return mask3;
};

consteval size_t PopulateMask2bit(size_t mask)
{
    unsigned mask1 = (mask & 0xff00) << 8 | (mask & 0x00ff);
    unsigned mask2 = (mask1 & 0x00f000f0) << 4 | (mask1 & 0x000f000f);
    unsigned mask3 = (mask2 & 0x0C0C0C0C) << 2 | (mask2 & 0x03030303);
    unsigned mask4 = (mask3 & 0x22222222) << 1 | (mask3 & 0x11111111);
    return mask4;
};

constexpr auto PopulateBits(uint16_t bitsNumber)
{
    size_t result{};
    for(int i{}; i < bitsNumber; ++i) {
        result |= 1U << i;
    }
    return result;
}

// Concepts

template<typename, template<typename...> typename>
inline constexpr bool is_specialization_of = false;

template<template<typename...> typename T, typename... Args>
inline constexpr bool is_specialization_of<T<Args...>, T> = true;

template<typename, template<auto...> typename>
inline constexpr bool is_nontype_specialization_of = false;

template<template<auto...> typename T, auto... Args>
inline constexpr bool is_nontype_specialization_of<T<Args...>, T> = true;

template<typename T, typename... U>
concept is_any_of = (std::same_as<T, U> || ...);

template<typename T, typename... U>
concept is_derived_from_any = (std::derived_from<T, U> || ...);

}

#endif // DRIVER_UTILS_H
