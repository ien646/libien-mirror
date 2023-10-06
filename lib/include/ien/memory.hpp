#pragma once

#include <cstring>

namespace ien
{
    template<typename T>
    void zero_memory(T* v)
    {
        std::memset(v, 0, sizeof(T));
    }
}