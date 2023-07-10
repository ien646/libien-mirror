#include <cstdlib>
#include <ien/fs_utils.hpp>

#include <ien/platform.hpp>
#include <ien/str_utils.hpp>

#include <sys/stat.h>

#if IEN_POSIX
    #include <unistd.h>
#endif

#if __has_include(<utime.h>)
    #include <utime.h>
#elif __has_include(<sys/utime.h>)
    #include <sys/utime.h>
#endif

#include <array>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
namespace fs = std::filesystem;

#ifdef IEN_OS_WIN
    #include <ien/win32/windows.h>
using stat_t = struct _stat;
using utimbuf_t = struct _utimbuf;
#else
using stat_t = struct stat;
using utimbuf_t = struct utimbuf;
#endif

#ifdef IEN_OS_UNIX
    #include <sys/types.h>
    #include <pwd.h>
#endif

namespace ien
{
    template <typename TPath>
    stat_t _get_stat(const TPath& path)
    {
        stat_t s = {};
#ifdef IEN_OS_WIN
        _wstat(xstr_to_wstr(path).c_str(), &s);
#else
        stat(xstr_to_str(path).c_str(), &s);
#endif
        return s;
    }

    template <typename TPath>
    size_t _get_file_size(const TPath& path)
    {
        return _get_stat(path).st_size;
    }

    template <typename TPath>
    time_t _get_atime(const TPath& path)
    {
        return _get_stat(path).st_atime;
    }

    template <typename TPath>
    time_t _get_mtime(const TPath& path)
    {
        return _get_stat(path).st_mtime;
    }

    template <typename TPath>
    bool _file_exists(const TPath& path)
    {
        return _get_stat(path).st_mode & S_IFREG;
    }

    template <typename TPath>
    bool _dir_exists(const TPath& path)
    {
        return (_get_stat(path).st_mode & S_IFDIR);
    }

    template <typename TPath>
    bool _set_mtime(const TPath& path, time_t mtime)
    {
        if (mtime > std::numeric_limits<int>::max())
        {
            throw std::invalid_argument("mtime too large");
        }

        utimbuf_t ubuff = {};
        ubuff.actime = _get_atime(path);
        ubuff.modtime = mtime;

#ifdef IEN_OS_WIN
        return _wutime(xstr_to_wstr(path).c_str(), &ubuff) == 0;
#else
        return utime(xstr_to_str(path).c_str(), &ubuff) == 0;
#endif
    }

    template <typename TPath>
    bool _set_atime(const TPath& path, time_t atime)
    {
        if (atime > std::numeric_limits<int>::max())
        {
            throw std::invalid_argument("atime too large");
        }

        utimbuf_t ubuff = {};
        ubuff.actime = atime;
        ubuff.modtime = _get_mtime(path);

#ifdef IEN_OS_WIN
        return _wutime(xstr_to_wstr(path).c_str(), &ubuff) == 0;
#else
        return utime(xstr_to_str(path).c_str(), &ubuff) == 0;
#endif
    }

    size_t get_file_size(const std::string& path)
    {
        return _get_file_size(path);
    }
    size_t get_file_size(const std::u8string& path)
    {
        return _get_file_size(path);
    }
    size_t get_file_size(const fs::path& path)
    {
        return _get_file_size(path.u8string());
    }

    time_t get_file_atime(const std::string& path)
    {
        return _get_atime(path);
    }
    time_t get_file_atime(const std::u8string& path)
    {
        return _get_atime(path);
    }
    time_t get_file_atime(const fs::path& path)
    {
        return _get_atime(path.u8string());
    }

    time_t get_file_mtime(const std::string& path)
    {
        return _get_mtime(path);
    }
    time_t get_file_mtime(const std::u8string& path)
    {
        return _get_mtime(path);
    }
    time_t get_file_mtime(const fs::path& path)
    {
        return _get_mtime(path.u8string());
    }

    bool set_file_atime(const std::string& path, time_t atime)
    {
        return _set_atime(path, atime);
    }
    bool set_file_atime(const std::u8string& path, time_t atime)
    {
        return _set_atime(path, atime);
    }
    bool set_file_atime(const fs::path& path, time_t atime)
    {
        return _set_atime(path.u8string(), atime);
    }

    bool set_file_mtime(const std::string& path, time_t mtime)
    {
        return _set_mtime(path, mtime);
    }
    bool set_file_mtime(const std::u8string& path, time_t mtime)
    {
        return _set_mtime(path, mtime);
    }
    bool set_file_mtime(const fs::path& path, time_t mtime)
    {
        return _set_mtime(path.u8string(), mtime);
    }

    bool file_exists(const std::string& path)
    {
        return _file_exists(path);
    }
    bool file_exists(const std::u8string& path)
    {
        return _file_exists(path);
    }
    bool file_exists(const fs::path& path)
    {
        return _file_exists(path.u8string());
    }

    bool directory_exists(const std::string& path)
    {
        return _dir_exists(path);
    }
    bool directory_exists(const std::u8string& path)
    {
        return _dir_exists(path);
    }
    bool directory_exists(const fs::path& path)
    {
        return _dir_exists(path.u8string());
    }

    std::string get_file_name(const std::string& path)
    {
#ifdef IEN_OS_WIN
        return wstr_to_str(fs::path(path).filename().wstring());
#else
        return fs::path(path).filename().string();
#endif
    }

    std::string get_file_name(const std::u8string& path)
    {
#ifdef IEN_OS_WIN
        return wstr_to_str(fs::path(path).filename().wstring());
#else
        return fs::path(path).filename().string();
#endif
    }

    std::string get_file_name(const fs::path& path)
    {
#ifdef IEN_OS_WIN
        return wstr_to_str(path.filename().wstring());
#else
        return path.filename().string();
#endif
    }

    std::string get_file_extension(const std::string& path)
    {
#ifdef IEN_OS_WIN
        return wstr_to_str(fs::path(path).extension().wstring());
#else
        return fs::path(path).extension().string();
#endif
    }

    std::string get_file_extension(const std::u8string& path)
    {
#ifdef IEN_OS_WIN
        return wstr_to_str(fs::path(path).extension().wstring());
#else
        return fs::path(path).extension().string();
#endif
    }

    std::string get_file_extension(const fs::path& path)
    {
#ifdef IEN_OS_WIN
        return wstr_to_str(path.extension().wstring());
#else
        return path.extension().string();
#endif
    }

    std::string get_file_directory(const std::string& path)
    {
#ifdef IEN_OS_WIN
        return wstr_to_str(fs::path(path).parent_path().wstring());
#else
        return fs::path(path).parent_path().string();
#endif
    }

    std::string get_file_directory(const std::u8string& path)
    {
#ifdef IEN_OS_WIN
        return wstr_to_str(fs::path(path).parent_path().wstring());
#else
        return fs::path(path).parent_path().string();
#endif
    }

    std::string get_file_directory(const std::filesystem::path& path)
    {
#ifdef IEN_OS_WIN
        return wstr_to_str(path.parent_path().wstring());
#else
        return path.parent_path().string();
#endif
    }

#ifdef IEN_OS_WIN
    size_t get_file_size(const std::wstring& path)
    {
        return _get_file_size(path);
    }
    time_t get_file_atime(const std::wstring& path)
    {
        return _get_atime(path);
    }
    time_t get_file_mtime(const std::wstring& path)
    {
        return _get_mtime(path);
    }
    bool set_file_atime(const std::wstring& path, time_t atime)
    {
        return _set_atime(path, atime);
    }
    bool set_file_mtime(const std::wstring& path, time_t mtime)
    {
        return _set_mtime(path, mtime);
    }
    bool file_exists(const std::wstring& path)
    {
        return _file_exists(path);
    }
    bool directory_exists(const std::wstring& path)
    {
        return _dir_exists(path);
    }
    std::string get_file_name(const std::wstring& path)
    {
        return wstr_to_str(fs::path(path).filename().wstring());
    }
    std::string get_file_extension(const std::wstring& path)
    {
        return wstr_to_str(fs::path(path).extension().wstring());
    }
    std::string get_file_directory(const std::wstring& path)
    {
        return wstr_to_str(fs::path(path).parent_path().wstring());
    }
#endif

    std::string get_current_user_homedir()
    {
#ifdef IEN_OS_WIN
        const char* userprofile = std::getenv("USERPROFILE");
        if (userprofile)
        {
            return userprofile;
        }

        const char* homedrive = std::getenv("HOMEDRIVE");
        const char* homepath = std::getenv("HOMEPATH");
        if (homedrive && homepath)
        {
            return wstr_to_str((std::filesystem::path(homedrive) / homepath).wstring());
        }
#else
    #ifdef IEN_POSIX
        passwd* pwd = getpwuid(getuid());
        if(pwd)
        {
            return pwd->pw_dir;
        }
    #endif
        const char* home = std::getenv("HOME");
        if (home)
        {
            return home;
        }        
#endif
        throw std::logic_error("Unable to find home directory for current user");
    }

    std::string format_timestamp_iso8601(time_t ts)
    {
        const std::string formatstr = "%F %T %z";
        std::tm* ltime = std::localtime(&ts);
        std::array<char, 64> buff;
        size_t bytes = std::strftime(buff.data(), 64, formatstr.c_str(), ltime);
        return std::string(buff.data(), bytes);
    }
} // namespace ien
