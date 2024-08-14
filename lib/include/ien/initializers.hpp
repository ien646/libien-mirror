#pragma once

#include <array>
#include <cstddef>

namespace ien
{
    template <typename T, size_t Len, typename TInvocable>
        requires(std::is_invocable_v<TInvocable> && std::is_convertible_v<std::invoke_result_t<TInvocable>, T>)
    inline constexpr std::array<T, Len> initialize_array_with_generator(const TInvocable& generator)
    {
        auto generate = [&]<size_t... Seq>(std::index_sequence<Seq...>) -> std::array<T, Len> {
            return { { (static_cast<void>(Seq), generator())... } };
        };

        return generate(std::make_index_sequence<Len>());
    }

    template <typename T, typename... Args>
    constexpr std::array<T, sizeof...(Args)> make_array(Args&&... args)
    {
        return { std::forward<Args>(args)... };
    }

    namespace detail
    {
        template <typename T, typename TResult>
        constexpr bool is_convertible_or_invoke_result = std::is_convertible_v<T, TResult> ||
                                                         (std::is_invocable_v<T> &&
                                                          std::is_convertible_v<std::invoke_result_t<T>, TResult>);
    }

    template <typename TResult, typename TA, typename TB>
        requires(detail::is_convertible_or_invoke_result<TA, TResult>() && detail::is_convertible_or_invoke_result<TB, TResult>())
    TResult conditional_init(bool cond, TA&& a, TB&& b)
    {
        if(cond)
        {
            if constexpr (std::is_invocable_v<TA>)
            {
                return a();
            }
            else
            {
                return std::forward<TA>(a);
            }
        }
        else
        {
            if constexpr (std::is_invocable_v<TB>)
            {
                return b();
            }
            else
            {
                return std::forward<TB>(b);
            }
        }
    }
} // namespace ien