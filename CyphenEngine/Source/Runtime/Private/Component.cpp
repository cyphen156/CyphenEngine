#include "pch.h"

#include "Runtime/Public/Component.h"

Component::Component(ObjectHandle inHandle)
	: Object(inHandle)
{
}

Component::~Component() = default;
