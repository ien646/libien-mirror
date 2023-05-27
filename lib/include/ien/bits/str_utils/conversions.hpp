#pragma once

#include <ien/lang_utils.hpp>
#include <ien/platform.hpp>
#include <stdexcept>

namespace ien
{
#ifdef IEN_OS_WIN
    extern std::string wstr_to_str(std::wstring_view wstr);
    extern std::wstring str_to_wstr(std::string_view wstr);
    extern std::wstring u8str_to_wstr(std::u8string_view wstr);
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
    template <concepts::AnyStr T>
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

    template <concepts::AnyStr TString>
    std::string xstr_to_str(const TString& str)
    {
        if constexpr (concepts::StdAnyStrStrV<TString>)
        {
            using char_type = typename TString::value_type;
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
        }
        else if constexpr (concepts::RawAnyStr<TString>)
        {
            using char_type = raw_str_char_t<TString>;
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
        }
        throw std::logic_error("Unhandled xstr_to_str conversion");
    }
} // namespace ien