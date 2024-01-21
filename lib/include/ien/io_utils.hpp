#pragma once

#include <ien/lang_utils.hpp>
#include <ien/platform.hpp>

#include <optional>
#include <string>

namespace ien
{
    struct unique_file_descriptor
    {
        FILE* value = nullptr;
        int (*func_close)(FILE*) = &fclose;

        constexpr unique_file_descriptor() noexcept {}
        unique_file_descriptor(const unique_file_descriptor&) = delete;
        constexpr unique_file_descriptor(unique_file_descriptor&& mv_src) noexcept
        {
            value = mv_src.value;
            mv_src.value = nullptr;
        }
        unique_file_descriptor(std::string_view path, std::string_view mode);

#ifdef IEN_OS_WIN
        unique_file_descriptor(std::wstring_view path, std::wstring_view mode);
#endif

        unique_file_descriptor(const std::u8string& path, std::string_view mode);
        unique_file_descriptor(FILE* fd, int (*f_close)(FILE*));
        ~unique_file_descriptor();

        constexpr bool is_valid() { return value != nullptr; }

        long file_size();

        inline long current_position() { return std::ftell(value); }

        inline void set_position(long pos) { std::fseek(value, pos, SEEK_SET); }

        inline void move_position(long offset) { std::fseek(value, offset, SEEK_CUR); }

        inline void move_to_start() { std::fseek(value, 0, SEEK_SET); }

        inline void move_to_end() { std::fseek(value, 0, SEEK_END); }

        inline size_t read(void* target_buffer, size_t count, size_t elem_size = 1)
        {
            return std::fread(target_buffer, elem_size, count, value);
        }

        inline void write(const void* source_buffer, size_t count, size_t elem_size = 1)
        {
            std::fwrite(source_buffer, elem_size, count, value);
        }

        inline operator FILE*() { return value; }
        inline operator bool() { return is_valid(); }
    };

    namespace detail
    {
        template <bool Binary, typename TPath>
            requires(concepts::AnyStr<TPath>)
        unique_file_descriptor get_read_fd(const TPath& path)
        {
            using path_char_type = underlying_char_t<TPath>;

            constexpr const path_char_type* mode = Binary ? select_template<const path_char_type*>(L"rb", "rb")
                                                          : select_template<const path_char_type*>(L"r", "r");
            return unique_file_descriptor(path, mode);
        }

        template <bool Binary, typename TPath>
            requires(concepts::AnyStr<TPath>)
        unique_file_descriptor get_write_fd(const TPath& path)
        {
            using path_char_type = underlying_char_t<TPath>;

            constexpr const path_char_type* mode = Binary ? select_template<const path_char_type*>(L"wb", "wb")
                                                          : select_template<const path_char_type*>(L"w", "w");

            return unique_file_descriptor(path, mode);
        }

        template <bool Binary, typename TPath>
            requires(concepts::AnyStr<TPath>)
        unique_file_descriptor get_append_fd(const TPath& path)
        {
            using path_char_type = underlying_char_t<TPath>;

            constexpr const path_char_type* mode = Binary ? select_template<const path_char_type*>(L"ab", "ab")
                                                          : select_template<const path_char_type*>(L"a", "a");

            return unique_file_descriptor(path, mode);
        }
    } // namespace detail

    template <bool Binary, typename TVal, typename TPath>
        requires(concepts::AnyStr<TPath>)
    std::optional<std::conditional_t<Binary, std::vector<TVal>, std::basic_string<TVal>>> read_file(const TPath& path)
    {
        unique_file_descriptor fd = detail::get_read_fd<Binary>(path);
        if (!fd.is_valid())
        {
            return std::nullopt;
        }

        long file_size = fd.file_size();
        std::conditional_t<Binary, std::vector<TVal>, std::basic_string<TVal>> result;
        result.resize(
            static_cast<size_t>(file_size / sizeof(TVal)) +
            // Add odd element in case of non perfectly divisible file length
            static_cast<size_t>((file_size % sizeof(TVal)) ? 1 : 0)
        );
        size_t read_bytes = fd.read(result.data(), result.size(), sizeof(TVal));
        result.resize(read_bytes); // discard null bytes if present (\r is discarded in text mode, for example)
        return result;
    }

    template <typename TChar = char, typename TPath>
        requires(concepts::AnyStr<TPath>)
    std::optional<std::basic_string<TChar>> read_file_text(const TPath& path)
    {
        return read_file<false, TChar>(path);
    }

    template <typename TVal = uint8_t, typename TPath>
        requires(concepts::AnyStr<TPath>)
    std::optional<std::vector<TVal>> read_file_binary(const TPath& path)
    {
        return read_file<true, TVal>(path);
    }

    template <typename TPath, typename TChar>
        requires(concepts::AnyStr<TPath>)
    bool write_file_text(const TPath& path, const std::basic_string<TChar>& text)
    {
        unique_file_descriptor fd = detail::get_write_fd<false>(path);
        if (!fd.is_valid())
        {
            return false;
        }
        fd.write(text.data(), sizeof(TChar), text.size());
        return true;
    }

    template <typename TChar = uint8_t, typename TPath, typename TContainer>
        requires(concepts::AnyStr<TPath>)
    bool write_file_binary(const TPath& path, const TContainer& data)
    {
        unique_file_descriptor fd = detail::get_write_fd<false>(path);
        if (!fd.is_valid())
        {
            return false;
        }
        fd.write(data.data(), sizeof(typename TContainer::value_type), data.size());
        return true;
    }
} // namespace ien
