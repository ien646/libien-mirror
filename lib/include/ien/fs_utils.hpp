#pragma once

#include <ien/platform.hpp>

#include <ctime>
#include <filesystem>
#include <string>

namespace ien
{
    size_t get_file_size(const std::string& path);
	size_t get_file_size(const std::u8string& path);
	size_t get_file_size(const std::filesystem::path& path);

	time_t get_file_atime(const std::string& path);
	time_t get_file_atime(const std::u8string& path);
	time_t get_file_atime(const std::filesystem::path& path);

	time_t get_file_mtime(const std::string& path);
	time_t get_file_mtime(const std::u8string& path);
	time_t get_file_mtime(const std::filesystem::path& path);

	bool set_file_atime(const std::string& path, time_t atime);
	bool set_file_atime(const std::u8string& path, time_t atime);
	bool set_file_atime(const std::filesystem::path& path, time_t atime);

	bool set_file_mtime(const std::string& path, time_t mtime);
	bool set_file_mtime(const std::u8string& path, time_t mtime);
	bool set_file_mtime(const std::filesystem::path& path, time_t mtime);

	bool file_exists(const std::string& path);
	bool file_exists(const std::u8string& path);
	bool file_exists(const std::filesystem::path& path);

	bool directory_exists(const std::string& path);
	bool directory_exists(const std::u8string& path);
	bool directory_exists(const std::filesystem::path& path);

	std::string get_file_name(const std::string& path);
	std::string get_file_name(const std::u8string& path);
	std::string get_file_name(const std::filesystem::path& path);

	std::string get_file_extension(const std::string& path);
	std::string get_file_extension(const std::u8string& path);
	std::string get_file_extension(const std::filesystem::path& path);

	std::string get_file_directory(const std::string& path);
	std::string get_file_directory(const std::u8string& path);
	std::string get_file_directory(const std::filesystem::path& path);

#ifdef IEN_OS_WIN
	size_t get_file_size(const std::wstring& path);
	time_t get_file_atime(const std::wstring& path);
	time_t get_file_mtime(const std::wstring& path);

	bool set_file_atime(const std::wstring& path, time_t atime);
	bool set_file_mtime(const std::wstring& path, time_t mtime);

	bool file_exists(const std::wstring& path);
	bool directory_exists(const std::wstring& path);

	std::string get_file_name(const std::wstring& path);
	std::string get_file_extension(const std::wstring& path);
	std::string get_file_directory(const std::wstring& path);
#endif

	std::string get_current_user_homedir();

 	std::string format_timestamp_iso8601(time_t ts);
}