#pragma once

#include <ien/sftp/types.hpp>

#include <functional>

namespace ien::sftp
{
    struct client_params
    {
        std::string hostname;
        std::string username;
        std::string password;
        uint16_t port = 22;
    };

    class client
    {
    private:
        client_params _params = {};
        std::string _address;
        socket_t _socket = 0;
        LIBSSH2_SESSION* _ssh_session = nullptr;
        LIBSSH2_SFTP* _sftp_session = nullptr;
#ifdef IEN_OS_WIN
        WSADATA* _wsadata = nullptr;
#endif

    public:
        explicit client(client_params params);
        ~client();

        [[nodiscard]] bool file_exists(const std::string& remote_path) const;
        [[nodiscard]] bool directory_exists(const std::string& remote_path) const;

        [[nodiscard]] bool is_temp_file(const file_entry& entry) const;

        [[nodiscard]] directory_listing list_directory(const std::string& remote_path) const;

        void get_file(const std::string& remote_path, const std::string& local_path, std::function<void(int64_t)> progress_callback = {}) const;
        void put_file(const std::string& local_path, const std::string& remote_path, std::function<void(int64_t)> progress_callback = {}) const;

        void create_directory(const std::string& path) const;

        void clean_temp_files(const directory_listing&) const;

        [[nodiscard]] file_info get_file_info(const std::string& remote_path) const;

        void remove_file(const std::string& remote_path) const;

        void set_atime(const std::string& remote_path, unsigned long mtime) const;
        void set_mtime(const std::string& remote_path, unsigned long mtime) const;

    private:
        void init_address();
        void init_socket();
        void init_ssh_session();
        void init_sftp_session();
    };
} // namespace ien::sftp