#pragma once

#include <concepts>
#include <ien/lang_utils.hpp>

#include <algorithm>
#include <type_traits>

namespace ien
{
    template <concepts::IterableContainer TSrc, concepts::HasPushBack TDst>
    void push_many(TDst& dst, TSrc&& src)
    {
        for (auto&& v : std::forward<TSrc>(src))
        {
            dst.push_back(std::forward<container_value_type<TSrc>>(v));
        }
    }

    template <concepts::RandomAccessContainer T>
        requires(concepts::HasPopBack<T>)
    void erase_unsorted(T& container, size_t index)
    {
        if (const size_t last_index = container.size() - 1; index != last_index)
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
    void erase_unsorted(T& container, typename T::iterator at)
    {
        if (auto last_it = container.end() - 1; at != last_it)
        {
            std::swap(*at, *last_it);
        }
        container.pop_back();
    }

    template <concepts::IterableContainer TContainer, typename TVal>
        requires(std::is_convertible_v<TVal, container_value_type<TContainer>>)
    void contains(const TContainer& container, const TVal& val)
    {
        return std::find(std::begin(container), std::end(container), val) != std::end(container);
    }
} // namespace ien