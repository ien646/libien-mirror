#include <ien/messagebox.hpp>

#include <ien/platform.hpp>

#include <fmt/format.h>

#if defined(IEN_OS_WIN)
    #include <Windows.h>
#elif defined(IEN_OS_LINUX)
    #include <cstdlib>
#else
    #include <iostream>
#endif

namespace ien
{
#ifdef IEN_OS_WIN
    bool show_error_messagebox(const std::string& text)
    {
        MessageBox(NULL, text.c_str(), "Error", MB_OK | MB_ICONERROR);
        return true;
    }

    bool show_warning_messagebox(const std::string& text)
    {
        MessageBox(NULL, text.c_str(), "Error", MB_OK | MB_ICONWARNING);
        return true;
    }

    bool show_info_messagebox(const std::string& text)
    {
        MessageBox(NULL, text.c_str(), "Error", MB_OK | MB_ICONINFORMATION);
        return true;
    }
#elif defined(IEN_OS_LINUX)
    bool show_error_messagebox(const std::string& text)
    {
        return std::system(fmt::format("zenity --error --text=\"{}\" --title=Error", text).c_str()) == 0;
    }

    bool show_warning_messagebox(const std::string& text)
    {
        return std::system(fmt::format("zenity --warning --text=\"{}\" --title=Warning", text).c_str()) == 0;
    }

    bool show_info_messagebox(const std::string& text)
    {
        return std::system(fmt::format("zenity --info --text=\"{}\" --title=Info", text).c_str()) == 0;
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