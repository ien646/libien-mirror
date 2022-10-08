#include <ien/str_utils.hpp>

#ifdef IEN_OS_WIN
    #include <ien/win32/windows.h>
#endif

namespace ien
{
#ifdef IEN_OS_WIN
    std::string wstr_to_str(std::wstring_view wstr)
    {
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
        std::wstring wstr((str.size() + 1) * sizeof(wchar_t), L'\0');
        int ok = MultiByteToWideChar(
            CP_UTF8,
            NULL,
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
        std::wstring wstr((u8str.size() + 1) * sizeof(wchar_t), L'\0');
        int ok = MultiByteToWideChar(
            CP_UTF8,
            NULL, reinterpret_cast<const char*>(u8str.data()),
            static_cast<int>(u8str.size()),
            wstr.data(),
            static_cast<int>(wstr.size()));
        if (!ok)
        {
            throw std::logic_error("Failure converting string to wide-string");
        }
        return std::wstring(wstr.c_str());
    }
#endif
}