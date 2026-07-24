#include "pch.h"

#include "Test/Runtime/Square.h"

uint32 Square::GetUpdateCount() const
{
	return updateCount;
}

uint32 Square::GetGlobalFinalUpdateCount() const
{
	return globalFinalUpdateCount;
}

Square::Square(ObjectHandle objectHandle)
	: WorldObject(
		objectHandle,
		UpdateParticipation::Update |
		UpdateParticipation::GlobalFinalUpdate)
{
}

Square::~Square() = default;

void Square::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCount;
}

void Square::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++globalFinalUpdateCount;
}
