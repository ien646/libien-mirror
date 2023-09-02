#pragma once

#include <ien/bits/serialization/deserializer_iterator.hpp>
#include <ien/bits/serialization/value_deserializers.hpp>

#include <cstddef>
#include <cstdint>
#include <span>

namespace ien
{
    class deserializer
    {
    private:
        deserializer_iterator _iterator;

    public:
        deserializer(const void* data, size_t len);

        template <typename T>
        explicit deserializer(const std::span<T>& data);

        template <typename T>
        T deserialize();

        inline void advance(size_t bytes) { _iterator.advance(bytes); }

        template <typename T, bool Advance = true>
        void deserialize_into_buffer(T* dst, size_t len);

        inline const uint8_t* data() const { return _iterator.data(); }
        inline size_t length() const { return _iterator.length(); }
        inline size_t position() const { return _iterator.position(); }

    private:
        void deserialize_into_buffer_uint8(uint8_t* dst, size_t len);
    };

    template <>
    void ien::deserializer::deserialize_into_buffer<uint8_t, true>(uint8_t* dst, size_t len);

    template <>
    void ien::deserializer::deserialize_into_buffer<uint8_t, false>(uint8_t* dst, size_t len);
} // namespace ien