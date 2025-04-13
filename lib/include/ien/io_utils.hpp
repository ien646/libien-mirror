#pragma once

#include <ien/lang_utils.hpp>
#include <ien/platform.hpp>

#include <ien/bits/io_utils/detail.hpp>
#include <ien/bits/io_utils/unique_file_descriptor.hpp>

#include <optional>
#include <string>

namespace ien
{   
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

    template <typename TVal = std::byte, typename TPath>
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

    template <typename TPath, typename TContainer>
        requires(concepts::AnyStr<TPath>)
    bool write_file_binary(const TPath& path, const TContainer& data)
    {
        unique_file_descriptor fd = detail::get_write_fd<true>(path);
        if (!fd.is_valid())
        {
            return false;
        }
        fd.write(data.data(), sizeof(typename TContainer::value_type), data.size());
        return true;
    }
} // namespace ien
