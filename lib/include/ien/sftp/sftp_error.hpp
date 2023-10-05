#pragma once

namespace ien::sftp
{
    constexpr const char* _err2str[] = {
        "OK",
        "EOF",
        "File not found",
        "Permission denied",
        "Failure",
        "Bad message",
        "No connection",
        "Connection lost",
        "Operation unsupported",
        "Invalid handle",
        "No such path",
        "File already exists",
        "Write protected",
        "No media",
        "Out of space",
        "Quota exceeded",
        "Unknown principal",
        "Lock conflict",
        "Directory not empty",
        "Not a directory",
        "Invalid filename",
        "Link loop",
        "Cannot delete",
        "Invalid parameter",
        "File is a directory",
        "Range lock conflict",
        "Delete pending",
        "File corrupt",
        "Owner invalid",
        "Group invalid",
        "No matching byte range lock"
    };
    constexpr auto _last_err_valid_id = 30;

    constexpr const char* error_to_string(unsigned long code)
    {
        if(code > _last_err_valid_id)
        {
            return "Unknown";
        }
        return _err2str[code];
    }
}