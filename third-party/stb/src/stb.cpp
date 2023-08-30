#include <type_traits>
#ifdef _MSC_VER
    #define IEN_WARNINGS_DISABLE_BEGIN _Pragma("warning(push, 0)")
    #define IEN_WARNINGS_DISABLE_END _Pragma("warning(pop)")
#else
    #define IEN_WARNINGS_DISABLE_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wall\"")
    #define IEN_WARNINGS_DISABLE_END _Pragma("GCC diagnostic pop")
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace ien::stb
{
    template <typename T>
        requires(std::is_arithmetic_v<T>)
    constexpr bool is_power_of_2(const T& val)
    {
        return (val > 1) && !(val & (val - 1));
    }

    [[nodiscard]] inline void* aligned_alloc(size_t bytes, size_t alignment)
    {
        assert(is_power_of_2(alignment));
        bytes = bytes + (alignment - (bytes % alignment));
#ifdef _MSC_VER
        auto ptr = _aligned_malloc(bytes, alignment);
#else
        auto ptr = ::aligned_alloc(alignment, bytes);
#endif
        return ptr;
    }

    inline void aligned_free(void* ptr)
    {
        assert(ptr != nullptr);
#ifdef _MSC_VER
        return _aligned_free
#else
        return ::free(ptr);
#endif
    }

    [[nodiscard]] inline void* aligned_realloc(void* ptr, size_t bytes, size_t alignment)
    {
        assert(is_power_of_2(alignment));
#ifdef _MSC_VER
        return _aligned_realloc(ptr, bytes, alignment);
#else
        aligned_free(ptr);
        return aligned_alloc(bytes, alignment);
#endif
    }
} // namespace ien::stb

#define STBI_WINDOWS_UTF8
#define STBIW_WINDOWS_UTF8

#define IEN_STBI_ALLOC_ALIGNMENT 32

#define STBI_MALLOC(sz) ien::stb::aligned_alloc(sz, IEN_STBI_ALLOC_ALIGNMENT)
#define STBI_FREE(ptr) ien::stb::aligned_free(ptr)
#define STBI_REALLOC(ptr, newsz) ien::stb::aligned_realloc(ptr, newsz, IEN_STBI_ALLOC_ALIGNMENT)

#define STBIR_MALLOC(sz, c) ((void)c, ien::stb::aligned_alloc(sz, IEN_STBI_ALLOC_ALIGNMENT))
#define STBIR_FREE(ptr, c) ((void)c, ien::stb::aligned_free(ptr))

#define STBIW_MALLOC(sz) ien::stb::aligned_alloc(sz, IEN_STBI_ALLOC_ALIGNMENT)
#define STBIW_FREE(ptr) ien::stb::aligned_free(ptr)
#define STBIW_REALLOC(ptr, newsz) ien::stb::aligned_realloc(ptr, newsz, IEN_STBI_ALLOC_ALIGNMENT)

IEN_WARNINGS_DISABLE_BEGIN
#include "../include/stb_image.h"
#include "../include/stb_image_resize.h"
#include "../include/stb_image_write.h"
IEN_WARNINGS_DISABLE_END