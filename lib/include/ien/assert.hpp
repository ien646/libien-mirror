#pragma once

#include <stdexcept>
#include <string>

namespace ien
{
    class assertion_failed_error
        : public std::logic_error
    {
    private:
        const std::string _file;
        const std::string _func;
        const size_t _line;
        const std::string _msg;

    public:
        assertion_failed_error(const std::string& file, const std::string& func, size_t line, const std::string& msg)
            : _file(file)
            , _func(func)
            , _line(line)
            , _msg(msg)
            , std::logic_error(gen_err_msg())
        { }

    private:
        std::string get_filename(const std::string& f) const;
        [[nodiscard]] std::string gen_err_msg() const;
    };
}

#ifndef NDEBUG
    #define IEN_ASSERT(cond) \
        if(!(cond)) { throw ien::assertion_failed_error(__FILE__, __func__, __LINE__, #cond); }
#else
    #define IEN_ASSERT(cond) (void(0))
#endif
