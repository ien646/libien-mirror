#include <ien/base64.hpp>

#include <array>
#include <string>

const std::string b64charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string ien::base64::encode(const void* src, size_t len)
{
    uint8_t* out;
    uint8_t* pos;
    const uint8_t* end;
    const uint8_t* in;

    size_t olen;

    olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

    if (olen < len)
    {
        return {};
    }

    std::string result;
    result.resize(olen);
    out = reinterpret_cast<unsigned char*>(&result[0]);

    end = reinterpret_cast<const uint8_t*>(src) + len;
    in = reinterpret_cast<const uint8_t*>(src);
    pos = out;
    while (end - in >= 3)
    {
        *pos++ = b64charset[in[0] >> 2];
        *pos++ = b64charset[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = b64charset[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = b64charset[in[2] & 0x3f];
        in += 3;
    }

    if (end - in)
    {
        *pos++ = b64charset[in[0] >> 2];
        if (end - in == 1)
        {
            *pos++ = b64charset[(in[0] & 0x03) << 4];
            *pos++ = '=';
        }
        else
        {
            *pos++ = b64charset[((in[0] & 0x03) << 4) | (in[1] >> 4)];
            *pos++ = b64charset[(in[1] & 0x0f) << 2];
        }
        *pos++ = '=';
    }
    return result;
}

const std::array<int32_t, 256> b64i = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                        0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63, 52, 53, 54, 55, 56, 57,
                                        58, 59, 60, 61, 0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
                                        7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
                                        25, 0,  0,  0,  0,  63, 0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
                                        37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

std::vector<uint8_t> ien::base64::decode(const void* data, size_t len)
{
    const auto* p = reinterpret_cast<const uint8_t*>(data);
    const int pad = len > 0 && (len % 4 || p[len - 1] == '=');
    const size_t L = ((len + 3) / 4 - pad) * 4;
    std::vector<uint8_t> result;
    result.resize(L / 4 * 3 + pad, '\0');

    for (size_t i = 0, j = 0; i < L; i += 4)
    {
        const int n = b64i[p[i]] << 18 | b64i[p[i + 1]] << 12 | b64i[p[i + 2]] << 6 | b64i[p[i + 3]];
        result[j++] = static_cast<uint8_t>(n >> 16);
        result[j++] = static_cast<uint8_t>(n >> 8 & 0xFF);
        result[j++] = static_cast<uint8_t>(n & 0xFF);
    }
    if (pad)
    {
        int n = b64i[p[L]] << 18 | b64i[p[L + 1]] << 12;
        result[result.size() - 1] = static_cast<uint8_t>(n >> 16);

        if (len > L + 2 && p[L + 2] != '=')
        {
            n |= b64i[p[L + 2]] << 6;
            result.push_back(n >> 8 & 0xFF);
        }
    }
    return result;
}