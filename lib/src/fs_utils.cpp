#include <ien/fs_utils.hpp>

#include <ien/platform.hpp>
#include <ien/str_utils.hpp>

#include <sys/stat.h>

#ifdef IEN_POSIX
    #include <unistd.h>
#endif

#if __has_include(<utime.h>)
    #include <utime.h>
#elif __has_include(<sys/utime.h>)
    #include <sys/utime.h>
#endif

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <format>
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
    #include <pwd.h>
    #include <sys/types.h>
#endif

namespace ien
{
    template <concepts::AnyStr TPath>
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

    template <concepts::AnyStr TPath>
    size_t _get_file_size(const TPath& path)
    {
        return _get_stat(path).st_size;
    }

    template <concepts::AnyStr TPath>
    time_t _get_atime(const TPath& path)
    {
        return _get_stat(path).st_atime;
    }

    template <concepts::AnyStr TPath>
    time_t _get_mtime(const TPath& path)
    {
        return _get_stat(path).st_mtime;
    }

    template <concepts::AnyStr TPath>
    bool _file_exists(const TPath& path)
    {
        return _get_stat(path).st_mode & S_IFREG;
    }

    template <concepts::AnyStr TPath>
    bool _dir_exists(const TPath& path)
    {
        return (_get_stat(path).st_mode & S_IFDIR);
    }

    template <concepts::AnyStr TPath>
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

    template <concepts::AnyStr TPath>
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

    std::string remove_trailing_dir_separator(const std::string& path)
    {
#ifdef IEN_OS_WIN
        if (path.ends_with('/') || path.ends_with('\\'))
        {
            return path.substr(0, path.size() - 1);
        }
#else
        if (path.ends_with('/'))
        {
            return path.substr(0, path.size() - 1);
        }
#endif
        return path;
    }

    std::string remove_trailing_dir_separator(const std::u8string& path)
    {
#ifdef IEN_OS_WIN
        if (path.ends_with('/') || path.ends_with('\\'))
        {
            return u8str_to_str(path.substr(0, path.size() - 1));
        }
#else
        if (path.ends_with('/'))
        {
            return u8str_to_str(path.substr(0, path.size() - 1));
        }
#endif
        return u8str_to_str(path);
    }
} // namespace ien

std::string remove_trailing_dir_separator(const std::filesystem::path& path)
{
#ifdef IEN_OS_WIN
    const auto wpath = path.wstring();
    if (wpath.ends_with(L'/') || wpath.ends_with(L'\\'))
    {
        return wstr_to_str(wpath.substr(0, wpath.size() - 1));
    }
    return wstr_to_str(wpath);
#else
    const auto spath = path.string();
    if (spath.ends_with('/'))
    {
        return spath.substr(0, spath.size() - 1);
    }
    return spath;
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
std::string remove_trailing_dir_separator(const std::wstring& path)
{
    if (path.ends_with('/') || path.ends_with('\\'))
    {
        return wstr_to_str(path.substr(0, path.size() - 1));
    }
    return wstr_to_str(path);
}

#endif

std::string get_current_user_homedir()
{
#ifdef IEN_OS_WIN
    const wchar_t* userprofile = _wgetenv(L"USERPROFILE");
    if (userprofile)
    {
        return userprofile;
    }

    const wchar_t* homedrive = _wgetenv(L"HOMEDRIVE");
    const wchar_t* homepath = _wgetenv(L"HOMEPATH");
    if (homedrive && homepath)
    {
        return wstr_to_str((std::filesystem::path(homedrive) / homepath).wstring());
    }
#elifdef IEN_POSIX
    passwd* pwd = getpwuid(getuid());
    if (pwd)
    {
        return pwd->pw_dir;
    }
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
    std::chrono::time_point tp = std::chrono::system_clock::time_point(std::chrono::seconds(ts));
    return std::format(std::locale::classic(), "{:%F %T}", tp);
}

bool exists_in_envpath(const std::string& path)
{
    if (path.empty())
    {
        return false;
    }

    const auto envpath = std::getenv("PATH");
    if (!envpath)
    {
        return false;
    }

    const auto path_lines = ien::str_splitv(envpath, ':');
    for (const auto& line : path_lines)
    {
        const auto test_path = std::string(line) + (line.ends_with('/') || line.ends_with('\\') ? "" : "/") + path;
        if (std::filesystem::exists(test_path))
        {
            return true;
        }
    }
    return false;
}
