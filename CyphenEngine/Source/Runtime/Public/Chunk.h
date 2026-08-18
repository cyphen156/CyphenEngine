#pragma once

#include "Core/Math/Public/Coordinate.h"
#include "Core/Math/Public/Vector.h"

struct Chunk
{
	Coordinate coordinate = Coordinate::Origin();
	Vector min = Vector::Zero();
	Vector max = Vector::Zero();
};
