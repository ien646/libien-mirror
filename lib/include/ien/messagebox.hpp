#pragma once

#include <string>

namespace ien
{
    bool show_error_messagebox(const std::string& text);
    bool show_warning_messagebox(const std::string& text);
    bool show_info_messagebox(const std::string& text);
}