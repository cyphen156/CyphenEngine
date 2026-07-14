#include "pch.h"

#include "Runtime/Public/Object.h"

Object::Object(ObjectHandle objectHandle)
	: handle(objectHandle)
{
}

Object::~Object() = default;

ObjectHandle Object::GetHandle() const
{
	return handle;
}
