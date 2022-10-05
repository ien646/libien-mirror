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
    struct client_params
    {
        std::string hostname;
        std::string username;
        std::string password;
        uint16_t port = 22;
    };

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
        std::string filename;
        file_info info;
    };

    struct directory_listing
    {
        std::vector<file_entry> files;
        std::vector<std::string> directories;
    };

    class client
    {
    private:
        client_params _params = {};
        std::string _address = "";
        socket_t _socket = 0;
        LIBSSH2_SESSION* _ssh_session = nullptr;
        LIBSSH2_SFTP* _sftp_session = nullptr;
    #ifdef IEN_OS_WIN
        WSADATA* _wsadata = nullptr;
    #endif

    public:
        explicit client(const client_params& params);
        ~client();

        bool file_exists(const std::string& remote_path) const;
        bool directory_exists(const std::string& remote_path) const;

        directory_listing list_directory(const std::string& remote_path) const;

        void get_file(const std::string& remote_path, const std::string& local_path) const;
        void put_file(const std::string& local_path, const std::string& remote_path) const;

        bool create_directory(const std::string& path) const;

        file_info get_file_info(const std::string& remote_path) const;

        void set_atime(const std::string& remote_path, unsigned long mtime) const;
        void set_mtime(const std::string& remote_path, unsigned long mtime) const;

    private:
        void init_address();
        void init_socket();
        void init_ssh_session();
        void init_sftp_session();
    };
}
