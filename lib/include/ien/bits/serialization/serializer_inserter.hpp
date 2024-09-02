#pragma once

#include <cstdint>
#include <vector>

namespace ien
{
    class serializer_inserter
    {
    private:
        std::back_insert_iterator<std::vector<std::byte>> _inserter;

        explicit serializer_inserter(std::vector<std::byte>& data)
            : _inserter(std::back_inserter(data))
        { }
    
    public:
        void operator+=(std::byte d) { _inserter = d; }

        friend class serializer;
    };
}