#pragma once

#include <ien/lang_utils.hpp>

#include <cinttypes>
#include <type_traits>

namespace ien
{
	/// @brief Obtain the sum of all provided arguments.
	/// Arguments are cast to T before addition to prevent over/under-flows.
	template<concepts::Arithmetic T, concepts::Arithmetic ... TArgs>
	constexpr T safe_add(TArgs... args)
	{
		return (static_cast<T>(args) + ...);
	}

	/// @brief Obtain the product of all provided arguments.
	/// Arguments are cast to T before multiplication to prevent over/under-flows.
	template<concepts::Arithmetic T, concepts::Arithmetic ... TArgs>
	constexpr T safe_mul(TArgs... args)
	{
		return (static_cast<T>(args) * ...);
	}

	/// @brief Construct an arithmetic type from two half sized arguments.
	/// @param a1 Most significant half byte(s)
	/// @param a0 Least significant half byte(s)
	template<concepts::Arithmetic T, concepts::Arithmetic TArg1, concepts::Arithmetic TArg0>
	constexpr T construct2(TArg1 a1, TArg0 a0)
	{
		static_assert(sizeof(TArg0) + sizeof(TArg1) == sizeof(T), "Size sum does not match target type's size");
		return (static_cast<T>(a1) << ((sizeof(T) - sizeof(TArg0)) * 8)) | static_cast<T>(a0);
	}

	/// @brief Construct an arithmetic type from 4 quarter sized arguments.
	/// @param a3 Most significant first quarter byte(s)
	/// @param a2 Most significant second quarter byte(s)
	/// @param a1 Least significant first quarter byte(s)
	/// @param a0 Least significant second quarter byte(s)
	template<concepts::Arithmetic T, concepts::Arithmetic TArg0, concepts::Arithmetic TArg1, concepts::Arithmetic TArg2, concepts::Arithmetic TArg3>
	constexpr T construct4(TArg3 a3, TArg2 a2, TArg1 a1, TArg0 a0)
	{
		static_assert(
			sizeof(TArg0) + sizeof(TArg1) + sizeof(TArg2) + sizeof(TArg3) == sizeof(T),
			"Size sum does not match target type's size"
		);

		return (static_cast<T>(a3) << ((sizeof(T) - sizeof(TArg3)) * 8))
			| (static_cast<T>(a2) << ((sizeof(T) - sizeof(TArg3) - sizeof(TArg2)) * 8))
			| (static_cast<T>(a1) << ((sizeof(T) - sizeof(TArg3) - sizeof(TArg2) - sizeof(TArg1)) * 8))
			| static_cast<T>(a0);
	}

	template<concepts::Integral T>
	constexpr bool is_power_of_2(const T& val)
	{
		return (val > 1) && !(val & (val - 1));
	}
}