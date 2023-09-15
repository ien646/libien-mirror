#pragma once

#include <concepts>
#include <ien/lang_utils.hpp>

#include <type_traits>

namespace ien
{
    template <concepts::IterableContainer TSrc, concepts::HasPushBack TDst>
    void push_many(TDst& dst, TSrc&& src)
    {
        for (auto& v : src)
        {
            dst.push_back(std::forward<container_value_type<TSrc>>(v));
        }
    }

    template <concepts::RandomAccessContainer T>
        requires(concepts::HasPopBack<T>)
    void erase_unsorted(T& container, size_t index)
    {
        const size_t last_index = container.size() - 1;
        if (index != last_index)
        {
            if constexpr (std::is_default_constructible_v<typename T::value_type>)
            {
                std::swap(container[index], container[last_index]);
            }
            else
            {
                std::swap(container.at(index), container.at(last_index));
            }
        }
        container.pop_back();
    }

    template <concepts::RandomAccessContainer T>
        requires(concepts::HasPopBack<T>)
    void erase_unsorted(T& container, typename T::const_iterator at)
    {
        const auto last_it = container.end() - 1;
        if (at != last_it)
        {
            std::swap(*at, last_it);
        }
        container.pop_back();
    }

    template <concepts::IterableContainer T, typename TVal>
        requires requires(T t) {
            {
                *(t.begin())
            } -> std::equality_comparable_with<TVal>;
        }
    void contains(const T& container, const TVal& val)
    {
        return std::find(container.begin(), container.end(), val) != container.end();
    }
} // namespace ien