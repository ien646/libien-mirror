#pragma once

#include <concepts>
#include <ien/platform.hpp>

#include <type_traits>
#include <vector>

namespace ien
{
    namespace detail
    {
        template <typename T>
        concept MovableContainer = requires(T t) {
            typename std::remove_cvref_t<T>::iterator;
            {
                t.begin()
            } -> std::same_as<typename std::remove_cvref_t<T>::iterator>;
            {
                t.end()
            } -> std::same_as<typename std::remove_cvref_t<T>::iterator>;
        };

        template <typename T>
        concept ReadonlyContainer = requires(T t) {
            typename std::remove_cvref_t<T>::const_iterator;
            {
                t.begin()
            } -> std::same_as<typename std::remove_cvref_t<T>::const_iterator>;
            {
                t.end()
            } -> std::same_as<typename std::remove_cvref_t<T>::const_iterator>;
        };

        template <typename T>
        concept HasPushBack = requires(T t, typename std::remove_cvref_t<T>::value_type v) { t.push_back(v); };
    } // namespace detail

    template <typename TSrc, detail::HasPushBack TDst>
        requires detail::MovableContainer<TSrc> || detail::ReadonlyContainer<TSrc>
    void push_many(TDst& dst, TSrc&& src)
    {
        for (auto& v : src)
        {
            dst.push_back(std::forward<typename std::remove_cvref_t<TSrc>::value_type>(v));
        }
    }
} // namespace ien