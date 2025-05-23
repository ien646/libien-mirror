#include <cinttypes>
#include <cstddef>
#include <string>
#include <vector>

namespace ien::base64
{
    std::string encode(const void* src, size_t len);
    std::vector<uint8_t> decode(const void* data, size_t len);
}