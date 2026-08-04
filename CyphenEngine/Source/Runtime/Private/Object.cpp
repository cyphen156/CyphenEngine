#include "pch.h"

#if _DEBUG
#include <cassert>
#endif

#include "Runtime/Public/Object.h"
#include "Runtime/Public/ObjectManager.h"

bool Object::AttachSubObject(Object& subObject)
{
	if (&subObject == this)
	{
		return false;
	}

	if (subObject.outer == this)
	{
		return true;
	}

	if (subObject.outer != nullptr)
	{
		return false;
	}

	// 직접 부모와 자식의 구조적 관계를 검사합니다.
	if (subObject.CanAttachTo(*this) == false)
	{
		return false;
	}

	// 부착으로 발생할 계층 전파를
	// 서브트리 전체가 받아들일 수 있는지 검사합니다.
	if (subObject.CanAttachSubtreeTo(*this) == false)
	{
		return false;
	}

	const Object* ancestor = GetOuter();

	while (ancestor != nullptr)
	{
		if (ancestor == &subObject)
		{
			return false;
		}

		ancestor = ancestor->outer;
	}

	subObjects.push_back(&subObject);
	subObject.outer = this;

	subObject.OnAttached();

	return true;
}

bool Object::DetachSubObject(Object& subObject)
{
	if (subObject.outer != this)
	{
		return false;
	}

	std::vector<Object*>::iterator iterator;
	for (iterator = subObjects.begin(); iterator != subObjects.end(); ++iterator)
	{
		if (*iterator != &subObject)
		{
			continue;
		}

		subObject.OnDetaching();

		subObjects.erase(iterator);
		subObject.outer = nullptr;

		return true;
	}

	return false;
}

ObjectHandle Object::GetHandle() const
{
	return handle;
}

Object* Object::GetOuter()
{
	return outer;
}

const Object* Object::GetOuter() const
{
	return outer;
}

Object* Object::GetSubObject(uint32 index)
{
	if (index >= subObjects.size())
	{
		return nullptr;
	}

	return subObjects[index];
}

const Object* Object::GetSubObject(uint32 index) const
{
	if (index >= subObjects.size())
	{
		return nullptr;
	}

	return subObjects[index];
}

uint32 Object::GetSubObjectCount() const
{
	return static_cast<uint32>(subObjects.size());
}

bool Object::Destroy()
{
	return ObjectManager::DestroyObject(this);
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

bool Object::CanAttachTo(const Object& outer) const
{
	static_cast<void>(outer);

	return true;
}

bool Object::CanAttachSubtreeTo(const Object& outer) const
{
	for (const Object* subObject : subObjects)
	{
		if (subObject == nullptr || subObject->CanAttachSubtreeTo(outer) == false)
		{
			return false;
		}
	}

	return true;
}

bool Object::OnDestroy()
{
	return true;
}

void Object::OnAttached()
{
	for (Object* subObject : subObjects)
	{
		if (subObject != nullptr)
		{
			subObject->OnAttached();
		}
	}
}

void Object::OnDetaching()
{
	for (Object* subObject : subObjects)
	{
		if (subObject != nullptr)
		{
			subObject->OnDetaching();
		}
	}
}
