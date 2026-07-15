#include "pch.h"

#include "Runtime/Public/Object.h"
#include "Runtime/Public/ObjectManager.h"

ObjectHandle Object::GetHandle() const
{
	return handle;
}

void Object::Destroy()
{
	std::vector<Object*>::iterator iterator;

	for (iterator = ObjectManager::objects.begin();
		iterator != ObjectManager::objects.end();
		++iterator)
	{
		if (*iterator != this)
		{
			continue;
		}

		ObjectManager::objects.erase(iterator);
		ObjectManager::destroyQueue.push(this);

		return;
	}
}

Object::Object(ObjectHandle objectHandle)
	: handle(objectHandle)
{
#ifdef _DEBUG
	if (objectHandle.IsSet() == false)
	{
		PRINT_DEBUG_OUTPUT("[Object] 유효한 핸들을 반드시 부여해야 합니다.\n");
		_ASSERT(objectHandle.IsSet());
	}
#endif
}

Object::~Object()
{
	handle.Release();
}
