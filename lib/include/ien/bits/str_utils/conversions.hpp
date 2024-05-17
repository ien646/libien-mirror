#pragma once

#include <ien/lang_utils.hpp>
#include <ien/platform.hpp>
#include <stdexcept>

namespace ien
{
#ifdef IEN_OS_WIN
    std::string wstr_to_str(std::wstring_view wstr);
    std::wstring str_to_wstr(std::string_view wstr);
    std::wstring u8str_to_wstr(std::u8string_view wstr);

    std::string wstr_to_str(wchar_t wstr);
    std::wstring str_to_wstr(char str);
    std::wstring u8str_to_wstr(char8_t u8str);
#endif

    inline std::string u8str_to_str(std::u8string_view u8str)
    {
        return std::string(u8str.begin(), u8str.end());
    }

    inline std::u8string str_to_u8str(std::string_view str)
    {
        return std::u8string(str.begin(), str.end());
    }

#ifdef IEN_OS_WIN
    template <concepts::AnyStrOrChar T>
    std::wstring xstr_to_wstr(const T& str)
    {
        if constexpr (std::is_same_v<underlying_char_t<T>, char>)
        {
            return str_to_wstr(str);
        }
        else if constexpr (std::is_same_v<underlying_char_t<T>, char8_t>)
        {
            return u8str_to_wstr(str);
        }
        else if constexpr (std::is_same_v<underlying_char_t<T>, wchar_t>)
        {
            return std::wstring(str);
        }
        else
        {
            throw std::logic_error("Unhandled xstr_to_wstr conversion");
        }
    }
#endif

    template <concepts::AnyStrOrChar TString>
    std::string xstr_to_str(const TString& str)
    {
        using char_type = underlying_char_t<TString>;
        if constexpr (std::is_same_v<char_type, char>)
        {
            return std::string(str);
        }
        else if constexpr (std::is_same_v<char_type, char8_t>)
        {
            return u8str_to_str(str);
        }
#ifdef IEN_OS_WIN
        else if constexpr (std::is_same_v<char_type, wchar_t>)
        {
            return wstr_to_str(str);
        }
#endif
        throw std::logic_error("Unhandled xstr_to_str conversion");
    }
} // namespace ien