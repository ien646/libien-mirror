#include <ien/sftp/client.hpp>
#include <ien/sftp/sftp_error.hpp>

#include <ien/io_utils.hpp>
#include <ien/memory.hpp>
#include <ien/net_utils.hpp>
#include <ien/platform.hpp>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <climits>
#include <format>
#include <functional>
#include <stdexcept>

#ifdef IEN_COMPILER_MSVC
    // Disable size_t cast warnings from libssh2
    #pragma warning(push)
    #pragma warning(disable : 4267)
#endif

#ifdef IEN_OS_WIN
    #include <ien/win32/winsock2.h>
    #include <ien/win32/ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define GET_LAST_SOCKET_ERROR() std::string(strerror(WSAGetLastError()))
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #define GET_LAST_SOCKET_ERROR() std::string(strerror(errno))
#endif

void throw_sftp_error(const std::string& msg, int error_code, LIBSSH2_SFTP* session)
{
    if (error_code == LIBSSH2_ERROR_SFTP_PROTOCOL)
    {
        unsigned long sftp_err = libssh2_sftp_last_error(session);
        throw std::logic_error(std::format("{} | SFTP protocol error: {}", msg, ien::sftp::error_to_string(sftp_err)));
    }
    throw std::logic_error(msg);
}

void _wrap_sftp_call(std::function<int()> func, const std::string& errormsg, LIBSSH2_SFTP* session)
{
    if (int err = func())
    {
        throw_sftp_error(errormsg, err, session);
    }
}

#define wrap_sftp_call(func, errormsg) _wrap_sftp_call([&]() { return func; }, errormsg, _sftp_session)

namespace ien::sftp
{
    const std::string TEMP_SUFFIX = ".__iensftp_temp__";

    class unique_sftp_handle
    {
    private:
        LIBSSH2_SFTP_HANDLE* _handle = nullptr;

    public:
        unique_sftp_handle(LIBSSH2_SFTP* session, const std::string& remote_path, unsigned long flags, long mode = 0)
        {
            _handle = libssh2_sftp_open(session, remote_path.c_str(), flags, mode);
        }

        ~unique_sftp_handle() { libssh2_sftp_close_handle(_handle); }

        inline bool is_valid() const { return _handle != nullptr; }

        operator bool() const { return is_valid(); }
        operator LIBSSH2_SFTP_HANDLE*() { return _handle; }
    };

    client::client(client_params params)
        : _params(std::move(params))
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

    bool client::is_temp_file(const file_entry& entry) const
    {
        return entry.path.ends_with(TEMP_SUFFIX);
    }

    directory_listing client::list_directory(const std::string& remote_path) const
    {
        directory_listing result = {};
        result.basedir = remote_path;
        if(result.basedir.ends_with("/") || result.basedir.ends_with("\\"))
        {
            result.basedir = result.basedir.substr(0, result.basedir.size() - 1);
        }

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
                entry.path = path;
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

    void client::get_file(const std::string& remote_path, const std::string& local_path, std::function<void(int64_t)> progress_callback) const
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
        ssize_t total_read = 0;
        std::vector<char> buff(4096 * 1024, 0);
        do
        {
            bytes_read = libssh2_sftp_read(handle, buff.data(), buff.size());            
            if (bytes_read > 0)
            {
                fd.write(buff.data(), bytes_read);
            }
            total_read += bytes_read;
            progress_callback(total_read);
        } while (bytes_read > 0);

        if (bytes_read < 0)
        {
            throw std::logic_error("Failure reading remote file: " + remote_path);
        }
    }

    void client::put_file(const std::string& local_path, const std::string& remote_path, std::function<void(int64_t)> progress_callback) const
    {
        ien::unique_file_descriptor fd(local_path, "rb");
        if (!fd.is_valid())
        {
            throw std::logic_error("Unable to open local file for reading: " + local_path);
        }

        if(fd.file_size() == 0)
        {
            throw std::logic_error("File is 0 bytes");
        }

        const std::string temp_path = remote_path + TEMP_SUFFIX;

        {
            auto mode = LIBSSH2_SFTP_S_IRWXU | LIBSSH2_SFTP_S_IRWXG | LIBSSH2_SFTP_S_IRWXO;
            auto flags = LIBSSH2_FXF_READ | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_WRITE;
            unique_sftp_handle handle(_sftp_session, temp_path.c_str(), flags, mode);
            if (!handle)
            {
                throw std::logic_error("Unable to get handle for temporary remote file: " + temp_path);
            }

            std::vector<char> buff(4096 * 1024, 0);
            std::fill(buff.begin(), buff.end(), 0);
            ssize_t total_progress = 0;
            while (true)
            {
                ssize_t bytes_read = fd.read(buff.data(), buff.size());
                if (bytes_read == 0)
                    break;

                ssize_t total_written = 0;
                while (total_written < bytes_read)
                {
                    ssize_t bytes_written = libssh2_sftp_write(
                        handle,
                        buff.data() + total_written,
                        bytes_read - total_written
                    );
                    if (bytes_written < 0)
                    {
                        throw std::logic_error("Failure writing to file: " + remote_path);
                    }
                    total_written += bytes_written;
                    total_progress += total_written;
                    progress_callback(total_progress);
                }
            }
        }

        if (file_exists(remote_path))
        {
            wrap_sftp_call(
                libssh2_sftp_unlink(_sftp_session, remote_path.c_str()),
                std::format("Unable to remove previous original file: {}", remote_path)
            );
        }

        wrap_sftp_call(
            libssh2_sftp_rename(_sftp_session, temp_path.c_str(), remote_path.c_str()),
            std::format("Unable to rename temporary file from '{}' to '{}'", temp_path, remote_path)
        );
    }

    void client::create_directory(const std::string& path) const
    {
        wrap_sftp_call(
            libssh2_sftp_mkdir(
                _sftp_session,
                path.c_str(),
                LIBSSH2_SFTP_S_IRWXU | LIBSSH2_SFTP_S_IRWXG | LIBSSH2_SFTP_S_IRWXO
            ),
            "Unable to create directory: " + path
        );
    }

    void client::clean_temp_files(const directory_listing& listing) const
    {
        for (const auto& entry : listing.files)
        {
            if (is_temp_file(entry))
            {
                try
                {
                    const auto fullpath = listing.basedir + "/" + entry.path;
                    std::cout << "Removing stale temporary file: " << fullpath << std::endl;
                    remove_file(fullpath);
                }
                catch(const std::exception& ex)
                {
                    std::cout << ex.what() << std::endl;
                }
            }
        }
    }

    file_info client::get_file_info(const std::string& remote_path) const
    {
        LIBSSH2_SFTP_ATTRIBUTES attrs;
        zero_memory(&attrs);
        wrap_sftp_call(
            libssh2_sftp_stat(_sftp_session, remote_path.c_str(), &attrs),
            "Failure obtaining file stat: " + remote_path
        );

        file_info result = {};
        result.atime = attrs.atime;
        result.mtime = attrs.mtime;
        result.size = attrs.filesize;
        result.gid = attrs.gid;
        result.uid = attrs.uid;

        return result;
    }

    void client::remove_file(const std::string& remote_path) const
    {
        wrap_sftp_call(libssh2_sftp_unlink(_sftp_session, remote_path.c_str()), "Unable to remove file: " + remote_path);
    }

    void client::set_atime(const std::string& remote_path, unsigned long atime) const
    {
        file_info finfo = get_file_info(remote_path);

        LIBSSH2_SFTP_ATTRIBUTES attrs;
        zero_memory(&attrs);
        attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
        attrs.atime = atime;
        attrs.mtime = finfo.mtime;
        wrap_sftp_call(
            libssh2_sftp_setstat(_sftp_session, remote_path.c_str(), &attrs),
            "Unable to set stat for file: " + remote_path
        );
    }

    void client::set_mtime(const std::string& remote_path, unsigned long mtime) const
    {
        if (mtime > INT_MAX)
        {
            throw std::invalid_argument("mtime does not represent a valid epoch value");
        }
        file_info finfo = get_file_info(remote_path);

        LIBSSH2_SFTP_ATTRIBUTES attrs;
        zero_memory(&attrs);
        attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
        attrs.atime = finfo.atime;
        attrs.mtime = mtime;
        wrap_sftp_call(
            libssh2_sftp_setstat(_sftp_session, remote_path.c_str(), &attrs),
            "Unable to set stat for file: " + remote_path
        );
    }

    // -- private --

    void client::init_address()
    {
        const auto addr = resolve_hostname_ipv4(_params.hostname);
        if (addr)
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
            throw std::logic_error("Unable to initialize socket connection: " + GET_LAST_SOCKET_ERROR());
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
} // namespace ien::sftp

#ifdef IEN_COMPILER_MSVC
    #pragma warning(pop)
#endif