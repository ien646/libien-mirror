#include <ien/assert.hpp>

#include <ien/str_utils.hpp>

#include <filesystem>
#include <fmt/format.h>

namespace ien
{
    std::string assertion_failed_error::get_filename(const std::string& fname) const
    {
        return u8str_to_str(std::filesystem::path(fname).filename().u8string());
    }

    std::string assertion_failed_error::gen_err_msg() const
    {
        return fmt::format("At {}:{} [{}] => {}", _file, _line, _func, _msg);
    }
}