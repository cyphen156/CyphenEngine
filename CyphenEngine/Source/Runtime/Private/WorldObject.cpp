#include "pch.h"

#include "Runtime/Public/WorldObject.h"
#include "Runtime/Public/GameRuntime.h"

WorldObject::~WorldObject()
{
	ClearComponents();

}

//Transform WorldObject::GetTransform() const
//{
//	Transform outTransform;
//	if (transformStorage == nullptr)
//	{
//		return;
//	}
//
//	if (transformStorage->TryGet(transformSlot, outTransform) == false)
//	{
//		return;
//	}
//	return outTransform;
//}
