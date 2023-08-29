#pragma once

#include <cstdint>
#include <vector>

namespace ien
{
    class serializer_inserter
    {
    private:
        std::back_insert_iterator<std::vector<uint8_t>> _inserter;

        serializer_inserter(std::vector<uint8_t>& data)
            : _inserter(std::back_inserter(data))
        { }
    
    public:
        void operator+=(uint8_t d) { _inserter = d; }

        friend class serializer;
    };
}