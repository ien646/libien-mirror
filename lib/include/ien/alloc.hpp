#pragma once

#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <cstdint>

#include <ien/arithmetic.hpp>
#include <ien/assert.hpp>
#include <ien/platform.hpp>

namespace ien::detail
{
	[[nodiscard]] inline void* aligned_alloc(size_t bytes, size_t alignment)
	{
		IEN_ASSERT(is_power_of_2(alignment));
		return IEN_OS_WIN_SELECT(_aligned_malloc, ::aligned_alloc)(bytes, alignment);
	}

	inline void aligned_free(void* ptr)
	{
		IEN_ASSERT(ptr != nullptr);
		IEN_OS_WIN_SELECT(_aligned_free, ::free)(ptr);
	}
}

#ifdef IEN_COMPILER_MSVC
	#define IEN_ASSUME_ALIGNED(ptr, alig) (__assume((((char*)ptr) - ((char*)ptr)) % (alig) == 0), ptr)
#elif defined(IEN_COMPILER_GNU) || defined(IEN_COMPILER_CLANG) || defined(IEN_COMPILER_INTEL)
	#define IEN_ASSUME_ALIGNED(ptr, alig) (__builtin_assume_aligned(ptr, alig))
#endif
#define IEN_ASSUME_ALIGNED_T(ptr, alig, ptr_type) reinterpret_cast<ptr_type*>(IEN_ASSUME_ALIGNED(ptr, alig))

namespace ien
{
	template<typename T = uint8_t>
	[[nodiscard]] inline T* aligned_alloc(size_t len, size_t alignment)
	{
		IEN_ASSERT(is_power_of_2(alignment));
		return reinterpret_cast<T*>(detail::aligned_alloc(len * sizeof(T), alignment));
	}

	template<typename T>
	inline void aligned_free(T* ptr)
	{
		detail::aligned_free(reinterpret_cast<void*>(ptr));
	}

	template<typename T>
	[[nodiscard]] inline T* aligned_realloc(T* ptr, size_t len, size_t alignment)
	{
		IEN_ASSERT(is_power_of_2(alignment));
		#ifdef IEN_COMPILER_MSVC
			return reinterpret_cast<T*>(_aligned_realloc(ptr, len, alignment));
		#else
			aligned_free(ptr);
			return reinterpret_cast<T*>(aligned_alloc(len, alignment));
		#endif
	}
}