#pragma once

#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <cstdint>

#include <ien/arithmetic.hpp>
#include <ien/platform.hpp>

namespace ien::detail
{
	[[nodiscard]] inline void* aligned_alloc(size_t bytes, size_t alignment)
	{
		assert(is_power_of_2(alignment));
		#ifdef IEN_OS_WIN_ANY
			return _aligned_malloc(bytes, alignment);
		#else
			return aligned_alloc(bytes, alignment);
		#endif
	}

	inline void aligned_free(void* ptr)
	{
		assert(ptr != nullptr);
		#ifdef IEN_OS_WIN_ANY
			_aligned_free(ptr);
		#else
			free(ptr);
		#endif
	}
}

namespace ien
{
	template<typename T>
    [[nodiscard]] inline T* assume_aligned(T* ptr, size_t alignment)
    {
		#ifdef IEN_COMPILER_MSVC
			__assume((((char*)ptr) - ((char*)ptr)) % (alignment) == 0);
			return ptr;

		#elif defined(IEN_COMPILER_CLANG) || defined(IEN_COMPILER_GNU)
			return reinterpret_cast<T*>(__builtin_assume_aligned(ptr, alignment));

		#elif defined(IEN_COMPILER_INTEL)
			__assume_aligned(ptr, alignment);
			return ptr;
		#endif
    }

	template<typename T = uint8_t>
	[[nodiscard]] inline T* aligned_alloc(size_t len, size_t alignment)
	{
		assert(is_power_of_2(alignment));
		return assume_aligned<T>(reinterpret_cast<T*>(detail::aligned_alloc(len * sizeof(T), alignment)), alignment);
	}

	template<typename T>
	inline void aligned_free(T* ptr)
	{
		detail::aligned_free(reinterpret_cast<void*>(ptr));
	}

	template<typename T>
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
}