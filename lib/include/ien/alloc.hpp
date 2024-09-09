#pragma once

#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <cstdint>

#include <cstdlib>
#include <ien/arithmetic.hpp>
#include <ien/platform.hpp>

namespace ien::detail
{
    [[nodiscard]] inline void* aligned_alloc(size_t bytes, size_t alignment)
    {
        assert(is_power_of_2(alignment));
        bytes = bytes + (alignment - (bytes % alignment));
#ifdef IEN_OS_WIN
        return _aligned_malloc(bytes, alignment);
#else
        return ::aligned_alloc(alignment, bytes);
#endif
    }

    inline void aligned_free(void* ptr)
    {
        assert(ptr != nullptr);
        IEN_OS_WIN_SELECT(_aligned_free, ::free)(ptr);
    }
} // namespace ien::detail

#ifdef IEN_COMPILER_MSVC
    #define IEN_ASSUME_ALIGNED(ptr, alig) (__assume((((char*)ptr) - ((char*)ptr)) % (alig) == 0), ptr)
#elif defined(IEN_COMPILER_GNU) || defined(IEN_COMPILER_CLANG) || defined(IEN_COMPILER_INTEL)
    #define IEN_ASSUME_ALIGNED(ptr, alig) (__builtin_assume_aligned(ptr, alig))
#endif
#define IEN_ASSUME_ALIGNED_T(ptr, alig, ptr_type) reinterpret_cast<ptr_type*>(IEN_ASSUME_ALIGNED(ptr, alig))

namespace ien
{
    template <typename T = uint8_t>
    [[nodiscard]] inline T* aligned_alloc(size_t len, size_t alignment)
    {
        assert(is_power_of_2(alignment));
        return reinterpret_cast<T*>(detail::aligned_alloc(len * sizeof(T), alignment));
    }

    template <typename T>
    inline void aligned_free(T* ptr)
    {
        detail::aligned_free(reinterpret_cast<void*>(ptr));
    }

    template <typename T>
    [[nodiscard]] inline T* aligned_realloc(T* ptr, size_t len, size_t alignment)
    {
        assert(is_power_of_2(alignment));
#ifdef IEN_COMPILER_MSVC
        return reinterpret_cast<T*>(_aligned_realloc(ptr, len, alignment));
#else
        aligned_free(ptr);
        return reinterpret_cast<T*>(aligned_alloc(len, alignment));
#endif
    }

#ifdef IEN_COMPILER_MSVC
    #define IEN_STACK_ALLOC(bytes) _malloca(bytes)
    #define IEN_STACK_FREE(ptr) _freea(ptr)
#else
    #define IEN_STACK_ALLOC(bytes) alloca(bytes)
    #define IEN_STACK_FREE(ptr) (void(0))
#endif

    template <typename T, size_t Alignment>
    class aligned_allocator
    {
    public:
        using value_type = T;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;

        constexpr aligned_allocator() noexcept = default;
        aligned_allocator(const aligned_allocator&) noexcept = default;
        aligned_allocator(aligned_allocator&&) noexcept = default;

        constexpr T* allocate(size_t n) const { return aligned_alloc<T>(n, Alignment); }

        void deallocate(T* ptr, [[maybe_unused]] size_t n) const { return aligned_free(ptr); }

        // Removed in c++20 but some compilers require it
        template<typename U>
        struct rebind
        {
            using other = aligned_allocator<U, Alignment>;
        };
    };
} // namespace ien