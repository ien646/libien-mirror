#include <ien/bits/io_utils/unique_file_descriptor.hpp>

#include <ien/str_utils.hpp>

#include <cstdio>

#ifdef IEN_OS_WIN
    #include <ien/win32/windows.h>
#endif

namespace ien
{
    constexpr const char* mode_to_str(unique_file_descriptor_mode mode)
    {
        using enum unique_file_descriptor_mode;
        switch (mode)
        {
        case APPEND:
            return "a";
        case APPEND_BINARY:
            return "ab";
        case READ:
            return "r";
        case READ_BINARY:
            return "rb";
        case WRITE:
            return "w";
        case WRITE_BINARY:
            return "wb";
        }
        return "";
    }

    template <typename TPath, typename TMode>
    FILE* get_file_descriptor(const TPath& path, const TMode& mode)
    {
#ifdef IEN_OS_WIN
        return _wfopen(ien::xstr_to_wstr(path).c_str(), ien::xstr_to_wstr(mode).c_str());
#else
        return std::fopen(ien::xstr_to_str(path).c_str(), ien::xstr_to_str(mode).c_str());
#endif
    }

    unique_file_descriptor::unique_file_descriptor(std::string_view path, unique_file_descriptor_mode mode)
    {
        value = get_file_descriptor(path, mode_to_str(mode));
    }

    unique_file_descriptor::unique_file_descriptor(const std::u8string& path, unique_file_descriptor_mode mode)
    {
        get_file_descriptor(path, mode_to_str(mode));
    }

    unique_file_descriptor::unique_file_descriptor(FILE* fd, int (*f_close)(FILE*))
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

    long unique_file_descriptor::current_position()
    {
        return std::ftell(value);
    }

    void unique_file_descriptor::write(const void* source_buffer, size_t count, size_t elem_size)
    {
        std::fwrite(source_buffer, elem_size, count, value);
    }

    size_t unique_file_descriptor::read(void* target_buffer, size_t count, size_t elem_size)
    {
        return std::fread(target_buffer, elem_size, count, value);
    }

    void unique_file_descriptor::move_to_end()
    {
        std::fseek(value, 0, SEEK_END);
    }

    void unique_file_descriptor::move_to_start()
    {
        std::fseek(value, 0, SEEK_SET);
    }

    void unique_file_descriptor::move_position(long offset)
    {
        std::fseek(value, offset, SEEK_CUR);
    }

    void unique_file_descriptor::set_position(long pos)
    {
        std::fseek(value, pos, SEEK_SET);
    }

    unique_file_descriptor::operator FILE*()
    {
        return value;
    }

    unique_file_descriptor::operator bool()
    {
        return is_valid();
    }

    constexpr unique_file_descriptor::unique_file_descriptor(unique_file_descriptor&& mv_src) noexcept
    {
        value = mv_src.value;
        mv_src.value = nullptr;
    }
} // namespace ien
