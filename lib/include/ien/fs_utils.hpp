#pragma once

#include <ien/platform.hpp>

#include <ctime>
#include <filesystem>
#include <string>

namespace ien
{
    extern size_t get_file_size(const std::string& path);
	extern size_t get_file_size(const std::u8string& path);
	extern size_t get_file_size(const std::filesystem::path& path);

	extern time_t get_file_atime(const std::string& path);
	extern time_t get_file_atime(const std::u8string& path);
	extern time_t get_file_atime(const std::filesystem::path& path);

	extern time_t get_file_mtime(const std::string& path);
	extern time_t get_file_mtime(const std::u8string& path);
	extern time_t get_file_mtime(const std::filesystem::path& path);

	extern bool set_file_atime(const std::string& path, time_t atime);
	extern bool set_file_atime(const std::u8string& path, time_t atime);
	extern bool set_file_atime(const std::filesystem::path& path, time_t atime);

	extern bool set_file_mtime(const std::string& path, time_t mtime);
	extern bool set_file_mtime(const std::u8string& path, time_t mtime);
	extern bool set_file_mtime(const std::filesystem::path& path, time_t mtime);

	extern bool file_exists(const std::string& path);
	extern bool file_exists(const std::u8string& path);
	extern bool file_exists(const std::filesystem::path& path);

	extern bool directory_exists(const std::string& path);
	extern bool directory_exists(const std::u8string& path);
	extern bool directory_exists(const std::filesystem::path& path);

	extern std::string get_file_name(const std::string& path);
	extern std::string get_file_name(const std::u8string& path);
	extern std::string get_file_name(const std::filesystem::path& path);

	extern std::string get_file_extension(const std::string& path);
	extern std::string get_file_extension(const std::u8string& path);
	extern std::string get_file_extension(const std::filesystem::path& path);

#ifdef IEN_OS_WIN
	extern size_t get_file_size(const std::wstring& path);
	extern time_t get_file_atime(const std::wstring& path);
	extern time_t get_file_mtime(const std::wstring& path);

	extern bool set_file_atime(const std::wstring& path, time_t atime);
	extern bool set_file_mtime(const std::wstring& path, time_t mtime);

	extern bool file_exists(const std::wstring& path);
	extern bool directory_exists(const std::wstring& path);

	extern std::string get_file_name(const std::wstring& path);
	extern std::string get_file_extension(const std::wstring& path);
#endif

 	extern std::string format_timestamp_iso8601(time_t ts);
}