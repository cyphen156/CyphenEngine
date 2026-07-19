#include "pch.h"

#if _DEBUG
#include <cassert>
#endif

#include "Runtime/Public/Object.h"
#include "Runtime/Public/ObjectManager.h"

ObjectHandle Object::GetHandle() const
{
	return handle;
}

void Object::Destroy()
{
	ObjectManager::DestroyObject(this);
}

Object::Object(ObjectHandle objectHandle)
	: handle(objectHandle)
{
#ifdef _DEBUG
	if (objectHandle.IsSet() == false)
	{
		PRINT_DEBUG_OUTPUT("[Object] 유효한 핸들을 반드시 부여해야 합니다.\n");
		assert(objectHandle.IsSet());
	}
#endif
}

Object::~Object() = default;
