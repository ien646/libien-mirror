#include <ien/sftp/sftp.hpp>

#include <ien/net_utils.hpp>
#include <ien/io_utils.hpp>
#include <ien/platform.hpp>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <array>
#include <climits>
#include <iterator>
#include <limits>
#include <stdexcept>

#ifdef IEN_COMPILER_MSVC
    // Disable size_t cast warnings from libssh2
    #pragma warning (push)
    #pragma warning (disable: 4267)
#endif

#ifdef IEN_OS_WIN
    #define NOMINMAX
    #include <WinSock2.h>
    #include <Ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif

namespace ien::sftp
{
    class unique_sftp_handle
    {
    private:
        LIBSSH2_SFTP_HANDLE* _handle = nullptr;

    public:
        unique_sftp_handle(LIBSSH2_SFTP* session, const std::string& remote_path, unsigned long flags, long mode = NULL)
        {
            _handle = libssh2_sftp_open(session, remote_path.c_str(), flags, mode);
        }

        ~unique_sftp_handle()
        {
            libssh2_sftp_close_handle(_handle);
        }

        operator bool() const { return _handle != nullptr; }
        operator LIBSSH2_SFTP_HANDLE*() { return _handle; }
    };

    client::client(const client_params& params)
        : _params(params)
    #ifdef IEN_OS_WIN
        , _wsadata(new WSADATA)
    #endif
    {
    #ifdef IEN_OS_WIN
        if (int err = WSAStartup(MAKEWORD(2, 0), _wsadata))
        {
            throw std::logic_error("WSAStartup failed with error: " + std::to_string(err));
        }
    #endif
        init_address();
        init_socket();
        init_ssh_session();
        init_sftp_session();
    }

    client::~client()
    {
    #ifdef IEN_OS_WIN
        delete _wsadata;
    #endif
    }

    bool client::file_exists(const std::string& remote_path) const
    {
        LIBSSH2_SFTP_ATTRIBUTES attributes = {};
        return !libssh2_sftp_stat(_sftp_session, remote_path.c_str(), &attributes);
    }

    bool client::directory_exists(const std::string& remote_path) const
    {
        LIBSSH2_SFTP_ATTRIBUTES attributes = {};
        return !libssh2_sftp_stat(_sftp_session, remote_path.c_str(), &attributes);
    }

    directory_listing client::list_directory(const std::string& remote_path) const
    {
        directory_listing result = {};
        LIBSSH2_SFTP_HANDLE* handle = libssh2_sftp_opendir(_sftp_session, remote_path.c_str());
        if (!handle)
        {
            throw std::logic_error("Unable to open remote directory: " + remote_path);
        }

        std::string mem(1024, 0);
        LIBSSH2_SFTP_ATTRIBUTES attrs = {};
        while (true)
        {
            int path_len = libssh2_sftp_readdir(handle, mem.data(), mem.size(), &attrs);
            if (path_len == 0)
            {
                break;
            }

            std::string path(mem.begin(), mem.begin() + path_len);
            if (LIBSSH2_SFTP_S_ISDIR(attrs.permissions))
            {
                result.directories.push_back(path);
            }
            else if (LIBSSH2_SFTP_S_ISREG(attrs.permissions))
            {
                file_entry entry = {};
                entry.filename = path;
                entry.info.size = attrs.filesize;
                entry.info.mtime = attrs.mtime;
                entry.info.atime = attrs.atime;
                entry.info.gid = attrs.gid;
                entry.info.uid = attrs.uid;
                result.files.push_back(entry);
            }
        }

        libssh2_sftp_closedir(handle);

        return result;
    }

    void client::get_file(const std::string& remote_path, const std::string& local_path) const
    {
        ien::unique_file_descriptor fd(local_path, "wb");
        if (!fd.is_valid())
        {
            throw std::logic_error("Unable to open local file for writing: " + local_path);
        }

        unique_sftp_handle handle(_sftp_session, remote_path.c_str(), LIBSSH2_FXF_READ);
        if (!handle)
        {
            throw std::logic_error("Unable to get handle for remote file: " + remote_path);
        }

        ssize_t bytes_read = 0;
        std::vector<char> buff(300000, 0);
        do
        {
            bytes_read = libssh2_sftp_read(handle, buff.data(), buff.size());
            if (bytes_read > 0)
            {
                fd.write(buff.data(), bytes_read);
            }
        } while (bytes_read > 0);

        if (bytes_read < 0)
        {
            throw std::logic_error("Failure reading remote file: " + remote_path);
        }
    }

    void client::put_file(const std::string& local_path, const std::string& remote_path) const
    {
        ien::unique_file_descriptor fd(local_path, "rb");
        if (!fd.is_valid())
        {
            throw std::logic_error("Unable to open local file for reading: " + local_path);
        }

        if (fd.file_size() == 0)
        {
            return;
        }

        const std::string temp_path = remote_path + ".__iensftp_temp__";

        {
            auto mode = 0x777;
            auto flags = LIBSSH2_FXF_READ | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_WRITE;
            unique_sftp_handle handle(_sftp_session, temp_path.c_str(), flags, mode);
            if (!handle)
            {
                throw std::logic_error("Unable to get handle for temporary remote file: " + temp_path);
            }

            std::vector<char> buff(3000000, 0);
            while (true)
            {
                ssize_t bytes_read = fd.read(buff.data(), buff.size());
                if (bytes_read == 0)
                    break;

                ssize_t total_written = 0;
                while (total_written < bytes_read)
                {
                    ssize_t bytes_written = libssh2_sftp_write(handle, buff.data() + total_written, bytes_read - total_written);
                    if (bytes_written < 0)
                    {
                        throw std::logic_error("Failure writing to file: " + remote_path);
                    }
                    total_written += bytes_written;
                }
            }
        }

        if (file_exists(remote_path))
        {
            libssh2_sftp_unlink(_sftp_session, remote_path.c_str());
        }

        if (int err = libssh2_sftp_rename(_sftp_session, temp_path.c_str(), remote_path.c_str()))
        {
            throw std::logic_error("Unable to rename temporary file to: " + remote_path);
        }
    }

    bool client::create_directory(const std::string& path) const
    {
        if (int err = libssh2_sftp_mkdir(_sftp_session, path.c_str(), 755))
        {
            return false;
        }
        return true;
    }

    file_info client::get_file_info(const std::string& remote_path) const
    {
        LIBSSH2_SFTP_ATTRIBUTES attrs = {};
        if (int err = libssh2_sftp_stat(_sftp_session, remote_path.c_str(), &attrs))
        {
            throw std::logic_error("Failure obtaining file stat: " + remote_path);
        }

        file_info result = {};
        result.atime = attrs.atime;
        result.mtime = attrs.mtime;
        result.size = attrs.filesize;
        result.gid = attrs.gid;
        result.uid = attrs.uid;

        return result;
    }

    void client::set_atime(const std::string& remote_path, unsigned long atime) const
    {
        file_info finfo = get_file_info(remote_path);

        LIBSSH2_SFTP_ATTRIBUTES attrs = {};
        attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
        attrs.atime = atime;
        attrs.mtime = finfo.mtime;
        libssh2_sftp_setstat(_sftp_session, remote_path.c_str(), &attrs);
    }

    void client::set_mtime(const std::string& remote_path, unsigned long mtime) const
    {
        if (mtime > INT_MAX)
        {
            throw std::invalid_argument("mtime does not represent a valid epoch value");
        }
        file_info finfo = get_file_info(remote_path);

        LIBSSH2_SFTP_ATTRIBUTES attrs = {};
        attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
        attrs.atime = finfo.atime;
        attrs.mtime = mtime;
        libssh2_sftp_setstat(_sftp_session, remote_path.c_str(), &attrs);
    }

    // -- private --

    void client::init_address()
    {
        const auto addr = resolve_hostname_ipv4(_params.hostname);
        if(addr)
        {
            _address = *addr;
        }
        else
        {
            throw std::logic_error("Invalid hostname");
        }
    }

    void client::init_socket()
    {
        _socket = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in sin = {};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(_params.port);
        inet_pton(AF_INET, _address.c_str(), &(sin.sin_addr));

        if (int err = connect(_socket, reinterpret_cast<sockaddr*>(&sin), sizeof(sockaddr_in)))
        {
            std::string desc = strerror(err);
            throw std::logic_error("Unable to initialize socket connection: " + desc);
        }
    }

    void client::init_ssh_session()
    {
        _ssh_session = libssh2_session_init();
        if (!_ssh_session)
        {
            throw std::logic_error("Unable to initialize ssh session");
        }
        libssh2_session_set_blocking(_ssh_session, 1);

        if (int err = libssh2_session_handshake(_ssh_session, _socket))
        {
            throw std::logic_error("Failure establishing ssh session handshake");
        }

        if (int err = libssh2_userauth_password(_ssh_session, _params.username.c_str(), _params.password.c_str()))
        {
            throw std::logic_error("Failure authenticating user " + _params.username);
        }
    }

    void client::init_sftp_session()
    {
        _sftp_session = libssh2_sftp_init(_ssh_session);
        if (!_sftp_session)
        {
            throw std::logic_error("Unable to initialize sftp session");
        }
    }
}

#ifdef IEN_COMPILER_MSVC
    #pragma warning (pop)
#endif
