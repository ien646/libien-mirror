#include <ien/str_utils.hpp>

#ifdef IEN_OS_WIN
    #include <ien/win32/windows.h>
#endif

namespace ien
{
#ifdef IEN_OS_WIN
    std::string wstr_to_str(std::wstring_view wstr)
    {
        if(wstr.empty())
        {
            return {};
        }

        std::string str((wstr.size() + 1) * 2, '\0');
        int ok = WideCharToMultiByte(
            CP_UTF8,
            WC_COMPOSITECHECK,
            wstr.data(),
            static_cast<int>(wstr.size()),
            str.data(),
            static_cast<int>(str.size()),
            NULL,
            NULL
        );
        if (!ok)
        {
            throw std::logic_error("Failure converting wide-string to (utf8)string");
        }
        return std::string(str.c_str());
    }

    std::wstring str_to_wstr(std::string_view str)
    {
        if(str.empty())
        {
            return {};
        }

        std::wstring wstr((str.size() + 1) * sizeof(wchar_t), L'\0');
        int ok = MultiByteToWideChar(
            CP_UTF8,
            static_cast<DWORD>(NULL),
            str.data(),
            static_cast<int>(str.size()),
            wstr.data(),
            static_cast<int>(wstr.size())
        );
        if (!ok)
        {
            throw std::logic_error("Failure converting string to wide-string");
        }
        return std::wstring(wstr.c_str());
    }

    std::wstring u8str_to_wstr(std::u8string_view u8str)
    {
        if(u8str.empty())
        {
            return {};
        }
        
        std::wstring wstr((u8str.size() + 1) * sizeof(wchar_t), L'\0');
        int ok = MultiByteToWideChar(
            CP_UTF8,
            static_cast<DWORD>(NULL),
            reinterpret_cast<const char*>(u8str.data()),
            static_cast<int>(u8str.size()),
            wstr.data(),
            static_cast<int>(wstr.size())
        );
        if (!ok)
        {
            throw std::logic_error("Failure converting string to wide-string");
        }
        return std::wstring(wstr.c_str());
    }

    std::string wstr_to_str(wchar_t wstr)
    {
        return wstr_to_str(std::wstring_view(&wstr, 1));
    }

    std::wstring str_to_wstr(char str)
    {
        return str_to_wstr(std::string_view(&str, 1));
    }

    std::wstring u8str_to_wstr(char8_t u8str)
    {
        return u8str_to_wstr(std::u8string_view(&u8str, 1));
    }
#endif
} // namespace ien
