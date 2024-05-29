#pragma once

#include <cstdio>
#include <string>

namespace ien
{
    enum class unique_file_descriptor_mode
    {
        READ,
        READ_BINARY,
        WRITE,
        WRITE_BINARY,
        APPEND,
        APPEND_BINARY
    };

    struct unique_file_descriptor
    {
        FILE* value = nullptr;
        int (*func_close)(FILE*) = &fclose;

        constexpr unique_file_descriptor() noexcept {}
        unique_file_descriptor(const unique_file_descriptor&) = delete;
        constexpr unique_file_descriptor(unique_file_descriptor&& mv_src) noexcept;
        unique_file_descriptor(std::string_view path, unique_file_descriptor_mode mode);
        unique_file_descriptor(const std::u8string& path, unique_file_descriptor_mode mode);
        unique_file_descriptor(FILE* fd, int (*f_close)(FILE*));
        ~unique_file_descriptor();

        constexpr bool is_valid() { return value != nullptr; }

        long file_size();
        long current_position();
        void set_position(long pos);
        void move_position(long offset);
        void move_to_start();
        void move_to_end();
        size_t read(void* target_buffer, size_t count, size_t elem_size = 1);
        void write(const void* source_buffer, size_t count, size_t elem_size = 1);

        operator FILE*();
        operator bool();
    };
} // namespace ien