#include <ien/bits/serialization/serializer.hpp>

namespace ien
{
    template <typename T>
    void serializer::serialize(const T& e)
    {
        ien::value_serializer<T>().serialize(e, _inserter);
    }
    
    template <typename T>
    void serializer::serialize_buffer(const T* ptr, size_t len)
    {
        for (size_t i = 0; i < len; ++i)
        {
            ien::value_serializer<T>{}.serialize(ptr[i], _inserter);
        }
    }
}