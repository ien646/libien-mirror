#pragma once

#include <array>

namespace ien
{
    template <typename T, std::size_t Length>
        requires(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>)
    class circular_array
    {
    public:
        using value_type = T;

        circular_array() { _array.fill(T{}); }

        void push(const T& val)
        {
            _array[_index++] = val;
            if (_index >= _array.size())
            {
                _index = 0;
            }
        }

        void push(T&& val)
        {
            _array[_index++] = std::move(val);
            if (_index >= _array.size())
            {
                _index = 0;
            }
        }

        constexpr std::size_t size() const { return Length; }

        T* data() { return _array.data(); }

        const T* data() const { return _array.data(); }

        const std::array<T, Length>& underlying_array() const { return _array; }

        void clear(T value = {}) { _array.fill(value); }

    private:
        std::array<T, Length> _array;
        std::size_t _index = 0;
    };
} // namespace ien