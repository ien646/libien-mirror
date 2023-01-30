#pragma once

#include <ien/net_utils.hpp>
#include <ien/platform.hpp>

#include <cinttypes>
#include <string>
#include <vector>

#ifndef LIBIEN_BUILD_SFTP
#error "libien must be compiled with CMake option 'LIBIEN_BUILD_SFTP' to use sftp functionality"
#endif

// need to forward declare session types just to avoid including libssh2 headers
// which for some reason include windows headers (yuck)
struct _LIBSSH2_SESSION;
struct _LIBSSH2_SFTP;
using LIBSSH2_SESSION = _LIBSSH2_SESSION;
using LIBSSH2_SFTP = _LIBSSH2_SFTP;

struct WSAData;
using WSADATA = WSAData;

namespace ien::sftp
{
    struct file_info
    {
        size_t size;
        unsigned long mtime;
        unsigned long atime;
        unsigned long uid;
        unsigned long gid;
    };

    struct file_entry
    {
        std::string path;
        file_info info;
    };

    struct directory_listing
    {
        std::vector<file_entry> files;
        std::vector<std::string> directories;
    };
} // namespace ien::sftp