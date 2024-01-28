#pragma once

#include <ien/platform.hpp>

#include <cassert>
#include <variant>
#include <vector>

namespace ien
{
    template <typename T, uint8_t MaxStackBytes = 16>
    class svo_vector
    {
    private:
        static constexpr size_t max_stack_elems = MaxStackBytes / sizeof(T);

        uint8_t _stack_size = 0;
        std::array<T, max_stack_elems> _stack_data;
        std::vector<T> _heap_data;

    public:
        void push_back(const T& elem)
        {
            if (!is_heap_allocated())
            {
                if (_stack_size == max_stack_elems)
                {
                    _heap_data = std::vector<T>(max_stack_elems + 1);
                    std::copy(_stack_data.begin(), _stack_data.end(), _heap_data.begin());
                    _heap_data.back() = elem;
                }
                else
                {
                    _stack_data[_stack_size] = elem;
                    ++_stack_size;
                }
            }
            else
            {
                _heap_data.push_back(elem);
            }
        }

        void push_back(T&& elem)
        {
            if (!is_heap_allocated())
            {
                if (_stack_size == max_stack_elems)
                {
                    _heap_data = std::vector<T>(max_stack_elems + 1);
                    std::copy(_stack_data.begin(), _stack_data.end(), _heap_data.begin());
                    _heap_data.back() = std::move(elem);
                }
                else
                {
                    _stack_data[_stack_size] = std::move(elem);
                    ++_stack_size;
                }
            }
            else
            {
                _heap_data.push_back(elem);
            }
        }

        T& operator[](size_t i)
        {
            if (is_heap_allocated())
            {
                assert(i < _heap_data.size());
                return _heap_data[i];
            }
            else
            {
                assert(i < _stack_data.size());
                return _stack_data[i];
            }
        }

        const T& operator[](size_t i) const
        {
            if (is_heap_allocated())
            {
                assert(i < _heap_data.size());
                return _heap_data[i];
            }
            else
            {
                assert(i < _stack_data.size());
                return _stack_data[i];
            }
        }

        size_t size() const
        {
            if (is_heap_allocated())
            {
                return _heap_data.size();
            }
            else
            {
                return _stack_size;
            }
        }

        std::vector<T> orphan_vector()
        {
            if (!is_heap_allocated())
            {
                return { _stack_data.begin(), _stack_data.end() };
            }
            else
            {
                const auto result = std::move(_heap_data);
                return result;
            }
        }

        bool is_heap_allocated() const { return !_heap_data.empty(); }

        T* data()
        {
            if(is_heap_allocated())
            {
                return _heap_data.data();
            }
            else
            {
                return _stack_data.data();
            }
        }

        const T* data() const
        {
            if(is_heap_allocated())
            {
                return _heap_data.data();
            }
            else
            {
                return _stack_data.data();
            }
        }
    };
} // namespace ien