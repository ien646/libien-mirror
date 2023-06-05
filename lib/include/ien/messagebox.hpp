#pragma once

#include <string>

namespace ien
{
    extern bool show_error_messagebox(const std::string& text);
    extern bool show_warning_messagebox(const std::string& text);
    extern bool show_info_messagebox(const std::string& text);
}