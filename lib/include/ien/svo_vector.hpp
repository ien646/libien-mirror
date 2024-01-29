#pragma once

#include <ien/alloc.hpp>
#include <ien/platform.hpp>

#include <cassert>
#include <cstring>
#include <variant>
#include <vector>

namespace ien
{
    template <typename T, uint8_t MaxStackBytes = 16>
    class svo_vector
    {
    private:
        static constexpr size_t max_stack_elems = MaxStackBytes / sizeof(T);

        using stack_t = std::array<T, max_stack_elems>;
        using heap_t = std::vector<T>;

        uint8_t _stack_size = 0;
        std::variant<stack_t, heap_t> _data = stack_t{};

    public:
        using iterator = T*;
        using const_iterator = const T*;

        void push_back(const T& elem)
        {
            if (!is_heap_allocated())
            {
                if (_stack_size == max_stack_elems)
                {
                    const size_t bytes = max_stack_elems * sizeof(T);
                    void* tempbuff = IEN_STACK_ALLOC(bytes);
                    std::memcpy(tempbuff, get_stack_data().data(), bytes);
                    _data = std::vector<T>(max_stack_elems + 1);

                    auto& heap_data = get_heap_data();
                    std:memcpy(heap_data.data(), tempbuff, bytes);
                    heap_data.back() = elem;
                }
                else
                {
                    get_stack_data()[_stack_size] = elem;
                    ++_stack_size;
                }
            }
            else
            {
                get_heap_data().push_back(elem);
            }
        }

        void push_back(T&& elem)
        {
            if (!is_heap_allocated())
            {
                if (_stack_size == max_stack_elems)
                {
                    const size_t bytes = max_stack_elems * sizeof(T);
                    void* tempbuff = IEN_STACK_ALLOC(bytes);
                    std::memcpy(tempbuff, get_stack_data().data(), bytes);
                    _data = std::vector<T>(max_stack_elems + 1);

                    auto& heap_data = get_heap_data();
                    std:memcpy(heap_data.data(), tempbuff, bytes);
                    heap_data.back() = std::move(elem);
                }
                else
                {
                    get_stack_data()[_stack_size] = std::move(elem);
                    ++_stack_size;
                }
            }
            else
            {
                get_heap_data().push_back(std::move(elem));
            }
        }

        T& operator[](size_t i)
        {
            if (is_heap_allocated())
            {
                assert(i < get_heap_data().size());
                return get_heap_data()[i];
            }
            else
            {
                assert(i < get_stack_data().size());
                return get_stack_data()[i];
            }
        }

        const T& operator[](size_t i) const
        {
            if (is_heap_allocated())
            {
                assert(i < get_heap_data().size());
                return get_heap_data()[i];
            }
            else
            {
                assert(i < get_stack_data().size());
                return get_stack_data()[i];
            }
        }

        size_t size() const
        {
            if (is_heap_allocated())
            {
                return get_heap_data().size();
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
                return { get_stack_data().begin(), get_stack_data().end() };
            }
            else
            {
                const auto result = std::move(get_heap_data());
                return result;
            }
        }

        inline bool is_heap_allocated() const { return std::holds_alternative<heap_t>(_data); }

        T* data()
        {
            if (is_heap_allocated())
            {
                return get_heap_data().data();
            }
            else
            {
                return get_stack_data().data();
            }
        }

        const T* data() const
        {
            if (is_heap_allocated())
            {
                return get_heap_data().data();
            }
            else
            {
                return get_stack_data().data();
            }
        }

        iterator begin() { return data(); }

        const_iterator begin() const { return data(); }

        iterator end()
        {
            if (is_heap_allocated())
            {
                return get_heap_data().data() + get_heap_data().size();
            }
            else
            {
                return get_stack_data().data() + _stack_size;
            }
        }

        const iterator end() const
        {
            if (is_heap_allocated())
            {
                return get_heap_data().data() + get_heap_data().size();
            }
            else
            {
                return get_stack_data().data() + _stack_size;
            }
        }

    private:
        inline stack_t& get_stack_data() { return std::get<stack_t>(_data); }
        inline heap_t& get_heap_data() { return std::get<heap_t>(_data); }
    };
} // namespace ien