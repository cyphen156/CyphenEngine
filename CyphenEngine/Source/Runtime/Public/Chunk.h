#pragma once

#include "Core/Public/Math/Coordinate.h"
#include "Core/Public/Math/Vector.h"

struct Chunk
{
	Coordinate coordinate = Coordinate::Origin();
	Vector min = Vector::Zero();
	Vector max = Vector::Zero();
};
