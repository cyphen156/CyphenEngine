#include "pch.h"

#include "Test/Runtime/Sprite.h"

ResourceId Sprite::GetTextureId() const
{
	return textureId;
}

uint32 Sprite::GetGlobalUpdateCount() const
{
	return globalUpdateCount;
}

uint32 Sprite::GetUpdateCount() const
{
	return updateCount;
}

Sprite::Sprite(
	ObjectHandle objectHandle,
	ResourceId textureIdValue)
	: Component(
		objectHandle,
		UpdateParticipation::GlobalUpdate |
		UpdateParticipation::Update),
	textureId(textureIdValue)
{
}

Sprite::~Sprite() = default;

void Sprite::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++globalUpdateCount;
}

void Sprite::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);

	++updateCount;
}
