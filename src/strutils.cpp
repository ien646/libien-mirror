#include <ien/strutils.hpp>

#include <ien/platform.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <type_traits>

namespace ien::strutils
{
    template<typename TRetVecVal, typename TArg>
    std::vector<TRetVecVal> split(TArg str, char delim)
    {
        static_assert(
            std::is_same_v<std::decay_t<TRetVecVal>, std::string>
            || std::is_same_v<std::decay_t<TRetVecVal>, std::string_view>
        );
        static_assert(
            std::is_same_v<std::decay_t<TArg>, std::string>
            || std::is_same_v<std::decay_t<TArg>, std::string_view>
        );

        std::vector<TRetVecVal> result;
        auto it = str.cbegin();
        auto found_it = std::find(it, str.end(), delim);

        while (found_it != str.end())
        {
            if (it != found_it)
            {
                #ifdef LIEN_HAS_CPP20
                    result.push_back(TRetVecVal(it, found_it));
                #else
                    if constexpr (std::is_same_v<std::decay_t<TRetVecVal>, std::string>)
                    {
                        result.push_back(std::string(it, found_it));
                    }
                    else
                    {
                        result.push_back(std::string_view(&(*it), std::distance(it, found_it)));
                    }
                #endif
            }
            it = ++found_it;
            found_it = std::find(it, str.end(), delim);
        }
        if (it != str.end())
        {
            #ifdef LIEN_HAS_CPP20
                result.push_back(TRetVecVal(it, str.end()));
            #else
                if constexpr (std::is_same_v<std::decay_t<TRetVecVal>, std::string>)
                {
                    result.push_back(std::string(it, str.end()));
                }
                else
                {
                    result.push_back(std::string_view(&(*it), std::distance(it, str.end())));
                }
            #endif
        }

        return result;
    }

    std::vector<std::string> split(const std::string& str, char delim)
    {
        return split<std::string, const std::string&>(str, delim);
    }

    std::vector<std::string> split(const std::string_view str, char delim)
    {
        return split<std::string, const std::string_view>(str, delim);
    }

    std::vector<std::string_view> splitv(const std::string& str, char delim)
    {
        return split<std::string_view, const std::string&>(str, delim);
    }

    std::vector<std::string_view> splitv(const std::string_view str, char delim)
    {
        return split<std::string_view, const std::string_view>(str, delim);
    }

    bool contains(const std::string& str, char ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string& str, const std::string& ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string& str, const std::string_view ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string_view str, char ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string_view str, const std::string& ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string_view str, const std::string_view ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    std::string replace(const std::string& str, char ocurrence, char replacement)
    {
        std::string copy = str;
        std::replace(copy.begin(), copy.end(), ocurrence, replacement);
        return copy;
    }

    std::string replace(const std::string_view str, char ocurrence, char replacement)
    {
        std::string copy(str);
        std::replace(copy.begin(), copy.end(), ocurrence, replacement);
        return copy;
    }

    std::string replace(const std::string& str, const std::string& ocurrence, const std::string& replacement)
    {
        std::vector<size_t> found_indices;
        size_t current_offset = 0;
        size_t idx = str.find(ocurrence, current_offset);
        while (idx != std::string::npos)
        {
            found_indices.push_back(idx);
            idx = str.find(ocurrence, current_offset);
        }
        if (current_offset == 0)
        {
            return str;
        }

        int64_t replacement_size_change = static_cast<int64_t>(replacement.size()) - ocurrence.size();
        int64_t result_size = str.size() - (static_cast<int64_t>(found_indices.size() * replacement_size_change));

        std::string result;
        result.resize(result_size);

        std::string::const_iterator str_iterator = str.begin();
        std::string::iterator result_iterator = result.begin();
        for (size_t i = 0; i < found_indices.size(); ++i)
        {
            const size_t idx = found_indices[i];
            std::copy(str_iterator, str_iterator + idx, result_iterator);
            str_iterator += idx + ocurrence.size();
            result_iterator += idx;
            std::copy(replacement.begin(), replacement.end(), result_iterator);
            result_iterator += replacement.size();
        }

        if (str_iterator != str.end())
        {
            std::copy(str_iterator, str.end(), result_iterator);
        }
        return result;
    }

    void replace_in_place(std::string& str, char ocurrence, char replacement)
    {
        std::replace(str.begin(), str.end(), ocurrence, replacement);
    }

    std::string to_upper(std::string_view str)
    {
        std::string result;
        result.reserve(str.size());
        std::transform(str.cbegin(), str.cend(), std::back_inserter(result), toupper);
        return result;
    }

    std::string to_lower(std::string_view str)
    {
        std::string result;
        result.reserve(str.size());
        std::transform(str.cbegin(), str.cend(), std::back_inserter(result), tolower);
        return result;
    }

    void to_upper_in_place(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }

    void to_lower_in_place(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
    }

    std::string_view trim_start(std::string_view str)
    {
        size_t idx = 0;
        while(str[idx] == ' ') { ++idx; }
        return str.substr(idx);
    }

    std::string_view trim_end(std::string_view str)
    {
        size_t idx = str.size() - 1;
        while(str[idx] == ' ') { --idx; }
        return str.substr(0, idx + 1);
    }

    std::string_view trim(std::string_view str)
    {
        return trim_end(trim_start(str));
    }
}