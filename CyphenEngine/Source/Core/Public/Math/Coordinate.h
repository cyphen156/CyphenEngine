#pragma once

#include "Core/Public/CPrimitiveTypes.h"

struct Coordinate
{
	int64 x = 0;
	int64 y = 0;
	int64 z = 0;

	static constexpr Coordinate Origin()
	{
		return {};
	}
};

constexpr bool operator==(const Coordinate& left, const Coordinate& right)
{
	return left.x == right.x
		&& left.y == right.y
		&& left.z == right.z;
}

constexpr bool operator!=(const Coordinate& left, const Coordinate& right)
{
	return !(left == right);
}

static_assert(sizeof(Coordinate) == sizeof(int64) * 3,
	"Coordinate must remain tight three-axis storage.");
