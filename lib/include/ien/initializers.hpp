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
} // namespace ien