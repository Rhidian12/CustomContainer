#pragma once

#include "Utils.h"

#include <stdint.h>

struct Size_P final
{
	constexpr explicit Size_P(const uint64_t n)
		: _Size{ n }
	{}

	uint64_t _Size;
};
struct Capacity_P final
{
	constexpr explicit Capacity_P(const uint64_t n)
		: _Capacity{ n }
	{}

	uint64_t _Capacity;
};

__NODISCARD __INLINE constexpr Size_P operator""_size(const uint64_t i)
{
	return Size_P{ i };
}
__NODISCARD __INLINE constexpr Capacity_P operator""_capacity(const uint64_t i)
{
	return Capacity_P{ i };
}