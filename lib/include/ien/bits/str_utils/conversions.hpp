#pragma once

#include <ien/platform.hpp>

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

    template<typename TString>
    std::wstring xstr_to_wstr(const TString& str)
    {
        if constexpr (std::is_same_v<typename TString::value_type, char>)
        {
            return str_to_wstr(str);
        }
        else if constexpr (std::is_same_v<typename TString::value_type, char8_t>)
        {
            return u8str_to_wstr(str);
        }
        else if constexpr (std::is_same_v<typename TString::value_type, wchar_t>)
        {
            return std::wstring(str);
        }
        return { };
    }

    template<concepts::AnyStr TString>
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
            else if constexpr (std::is_same_v<char_type, wchar_t>)
            {
                return wstr_to_str(str);
            }
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
            else if constexpr (std::is_same_v<char_type, wchar_t>)
            {
                return wstr_to_str(str);
            }
        }
    }
}