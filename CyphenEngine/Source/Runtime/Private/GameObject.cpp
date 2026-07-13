#include "pch.h"

#include "Runtime/Public/GameObject.h"

GameObject::GameObject(ObjectHandle inHandle)
	: Object(inHandle)
{
}

GameObject::~GameObject() = default;
