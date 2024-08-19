#pragma once

#include <ien/bits/io_utils/unique_file_descriptor.hpp>
#include <ien/lang_utils.hpp>

namespace ien::detail
{
    template <bool Binary, typename TPath>
        requires(concepts::AnyStr<TPath>)
    unique_file_descriptor get_read_fd(const TPath& path)
    {
        constexpr const unique_file_descriptor_mode mode = Binary ? unique_file_descriptor_mode::READ_BINARY
                                                                  : unique_file_descriptor_mode::READ;
        return unique_file_descriptor(path, mode);
    }

    template <bool Binary, typename TPath>
        requires(concepts::AnyStr<TPath>)
    unique_file_descriptor get_write_fd(const TPath& path)
    {
        constexpr const unique_file_descriptor_mode mode = Binary ? unique_file_descriptor_mode::WRITE_BINARY
                                                                  : unique_file_descriptor_mode::WRITE;
        return unique_file_descriptor(path, mode);
    }

    template <bool Binary, typename TPath>
        requires(concepts::AnyStr<TPath>)
    unique_file_descriptor get_append_fd(const TPath& path)
    {
        constexpr const unique_file_descriptor_mode mode = Binary ? unique_file_descriptor_mode::APPEND_BINARY
                                                                  : unique_file_descriptor_mode::APPEND;
        return unique_file_descriptor(path, mode);
    }
} // namespace ien::detail