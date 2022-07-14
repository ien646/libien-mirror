#pragma once

#include <ien/lang_utils.hpp>

#include <cinttypes>
#include <type_traits>

namespace ien
{
	template<concepts::Arithmetic T, concepts::Arithmetic ... TArgs>
	constexpr T safe_add(TArgs... args)
	{
		return (static_cast<T>(args) + ...);
	}

	template<concepts::Arithmetic T, concepts::Arithmetic ... TArgs>
	constexpr T safe_mul(TArgs... args)
	{
		return (static_cast<T>(args) * ...);
	}

	template<concepts::Arithmetic T, concepts::Arithmetic TArg0, concepts::Arithmetic TArg1>
	constexpr T construct2(TArg0 a0, TArg1 a1)
	{
		static_assert(sizeof(TArg0) + sizeof(TArg1) == sizeof(T), "Size sum does not match target type's size");
		return (static_cast<T>(a0) << ((sizeof(T) - sizeof(TArg0)) * 8)) | static_cast<T>(a1);
	}

	template<concepts::Arithmetic T, concepts::Arithmetic TArg0, concepts::Arithmetic TArg1, concepts::Arithmetic TArg2, concepts::Arithmetic TArg3>
	constexpr T construct4(TArg0 a0, TArg1 a1, TArg2 a2, TArg3 a3)
	{
		static_assert(
			sizeof(TArg0) + sizeof(TArg1) + sizeof(TArg2) + sizeof(TArg3) == sizeof(T),
			"Size sum does not match target type's size"
		);

		return (static_cast<T>(a0) << ((sizeof(T) - sizeof(TArg0)) * 8))
			| (static_cast<T>(a1) << ((sizeof(T) - sizeof(TArg0) - sizeof(TArg1)) * 8))
			| (static_cast<T>(a2) << ((sizeof(T) - sizeof(TArg0) - sizeof(TArg1) - sizeof(TArg2)) * 8))
			| static_cast<T>(a3);
	}

	template<concepts::Integral T>
	constexpr bool is_power_of_2(const T& val)
	{
		return (val > 1) && !(val & (val - 1));
	}
}