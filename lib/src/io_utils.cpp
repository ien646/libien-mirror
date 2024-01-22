#include <ien/io_utils.hpp>

#include <ien/str_utils.hpp>

#include <cstdio>

#ifdef IEN_OS_WIN
    #include <ien/win32/windows.h>
#endif

namespace ien
{
    template<typename TPath, typename TMode>
    FILE* get_file_descriptor(const TPath& path, const TMode& mode)
    {
    #ifdef IEN_OS_WIN
        return _wfopen(ien::xstr_to_wstr(path).c_str(), ien::xstr_to_wstr(mode).c_str());
    #else
        return std::fopen(ien::xstr_to_str(path).c_str(), ien::xstr_to_str(mode).c_str());
    #endif
    }

    unique_file_descriptor::unique_file_descriptor(std::string_view path, std::string_view mode)
    {
        value = get_file_descriptor(path, mode);
    }

#ifdef IEN_OS_WIN
    unique_file_descriptor::unique_file_descriptor(std::wstring_view path, std::wstring_view mode)
    {
        value = _wfopen(std::wstring(path).c_str(), std::wstring(mode).c_str());
    }
#endif

    unique_file_descriptor::unique_file_descriptor(const std::u8string& path, std::string_view mode)
    {
        get_file_descriptor(path, mode);
    }

    unique_file_descriptor::unique_file_descriptor(FILE* fd, int(*f_close)(FILE*))
    {
        value = fd;
        func_close = f_close;
    }

    unique_file_descriptor::~unique_file_descriptor()
    {
        if (value)
        {
            func_close(value);
        }
    }

    long unique_file_descriptor::file_size()
    {
        const long previous_pos = current_position();
        move_to_end();
        const long result = current_position();
        set_position(previous_pos);
        return result;
    }
}
