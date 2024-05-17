#include <ien/messagebox.hpp>

#include <ien/platform.hpp>

#if defined(IEN_OS_WIN)
    #include <ien/win32/windows.h>
    #include <ien/str_utils.hpp>
#elif defined(IEN_OS_LINUX)
    #include <cstdlib>
#else
    #include <iostream>
#endif

#include <format>

namespace ien
{
#ifdef IEN_OS_WIN
    bool show_error_messagebox(const std::string& text)
    {
        std::wstring wtext = ien::str_to_wstr(text);
        MessageBoxW(NULL, wtext.c_str(), L"Error", MB_OK | MB_ICONERROR);
        return true;
    }

    bool show_warning_messagebox(const std::string& text)
    {
        std::wstring wtext = ien::str_to_wstr(text);
        MessageBoxW(NULL, wtext.c_str(), L"Error", MB_OK | MB_ICONWARNING);
        return true;
    }

    bool show_info_messagebox(const std::string& text)
    {
        std::wstring wtext = ien::str_to_wstr(text);
        MessageBoxW(NULL, wtext.c_str(), L"Error", MB_OK | MB_ICONINFORMATION);
        return true;
    }
#elif defined(IEN_OS_LINUX)
    bool show_error_messagebox(const std::string& text)
    {
        return std::system(std::format("zenity --error --text=\"{}\" --title=Error", text).c_str()) == 0;
    }

    bool show_warning_messagebox(const std::string& text)
    {
        return std::system(std::format("zenity --warning --text=\"{}\" --title=Warning", text).c_str()) == 0;
    }

    bool show_info_messagebox(const std::string& text)
    {
        return std::system(std::format("zenity --info --text=\"{}\" --title=Info", text).c_str()) == 0;
    }
#else
    bool show_error_messagebox(const std::string& text)
    {
        std::cerr << "Error: " << text << std::endl;
    }

    bool show_warning_messagebox(const std::string& text)
    {
        std::cerr << "Warning: " << text << std::endl;
    }

    bool show_info_messagebox(const std::string& text)
    {
        std::cerr << "Info: " << text << std::endl;
    }
#endif
}