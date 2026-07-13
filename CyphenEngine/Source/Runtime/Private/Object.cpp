#include "pch.h"

#include "Runtime/Public/Object.h"

Object::Object(ObjectHandle inHandle)
	: handle(inHandle)
{
}

Object::~Object() = default;

ObjectHandle Object::GetHandle() const
{
	return handle;
}
