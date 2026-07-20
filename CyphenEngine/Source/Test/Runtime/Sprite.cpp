#include "pch.h"

#include "Test/Runtime/Sprite.h"

ResourceId Sprite::GetTextureId() const
{
	return textureId;
}

Sprite::Sprite(ObjectHandle objectHandle, ResourceId textureIdValue)
	: Component(objectHandle),
	textureId(textureIdValue)
{
}

Sprite::~Sprite() = default;
