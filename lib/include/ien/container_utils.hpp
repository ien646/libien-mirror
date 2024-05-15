#pragma once

#include <ien/lang_utils.hpp>

namespace ien
{
    template <concepts::SizedContainer TContainer, concepts::Integral TIndex>
    TIndex clamp_index_to_vector_bounds(const TContainer& container, const TIndex& index)
    {
        assert(container.size() < std::numeric_limits<TIndex>::max());

        TIndex result = std::min(index, static_cast<TIndex>(container.size() - 1));
        if constexpr (std::is_signed_v<TIndex>)
        {
            result = std::max(static_cast<TIndex>(0), result);
        }
        return result;
    }
} // namespace ien