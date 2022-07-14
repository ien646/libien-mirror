#pragma once

#include <ien/lang_utils.hpp>

#include <cinttypes>
#include <type_traits>

namespace ien
{
    template<concepts::Integral T, concepts::Integral ... SetBits>
    constexpr T bitmask(SetBits... bits)
    {
        return ((static_cast<T>(1) << bits) | ...);
    }

    template<concepts::Integral T>
    constexpr bool get_bit(T v, size_t i)
    {        
        return (v | bitmask<T>(i)) == v;
    }

    template<concepts::Integral T>
    constexpr void set_bit(T& v, size_t i)
    {
        v |= bitmask<T>(i);
    }

    template<concepts::Integral T>
    constexpr void unset_bit(T& v, size_t i)
    {
        v &= ~bitmask<T>(i);
    }

    template<concepts::Integral T>
    constexpr T lo_nibble(T v)
    {
        return static_cast<T>(v & 0x0F);
    }

    template<concepts::Integral T>
    constexpr std::conditional_t<std::is_signed_v<T>, int8_t, uint8_t> hi_nibble(T v)
    {        
        return (v & static_cast<T>(0xF0)) >> 4;
    }

    template<concepts::Integral T>
    constexpr std::conditional_t<std::is_signed_v<T>, int8_t, uint8_t> lo_byte(T v)
    {
        return static_cast<T>(v & 0x00FF);
    }

    template<concepts::Integral T>
    constexpr std::conditional_t<std::is_signed_v<T>, int8_t, uint8_t> hi_byte(T v)
    {
        return static_cast<T>((v & 0xFF00) >> 8);
    }

    template<concepts::Integral T>
    constexpr std::conditional_t<std::is_signed_v<T>, int16_t, uint16_t> lo_word(T v)
        requires(sizeof(T) >= 4)
    {
        return static_cast<T>(v & (0x0000FFFF));
    }

    template<concepts::Integral T>
    constexpr std::conditional_t<std::is_signed_v<T>, int16_t, uint16_t> hi_word(T v)
        requires(sizeof(T) >= 4)
    {
        return static_cast<T>((v & 0xFFFF0000) >> 16);
    }

    template<concepts::Integral T>
    constexpr std::conditional_t<std::is_signed_v<T>, int32_t, uint32_t> lo_dword(T v)
    {
        return static_cast<T>(v & 0x00000000FFFFFFFF);
    }

    template<concepts::Integral T>
    constexpr std::conditional_t<std::is_signed_v<T>, int32_t, uint32_t> hi_dword(T v)
    {
        return static_cast<T>((v & 0xFFFFFFFF00000000) >> 32);
    }
}