#include "pch.h"

#include "Test/Runtime/RuntimeTest.h"

#include "Runtime/Public/GameRuntime.h"
#include "Runtime/Public/Object.h"
#include "Runtime/Public/ObjectManager.h"
#include "Runtime/Public/WorldObject.h"
#include "Test/Runtime/Sprite.h"
#include "Test/Runtime/Square.h"

bool RunRuntimeTest(GameRuntime& runtime, ResourceId textureId)
{
	if (runtime.IsInitialized() == false ||
		textureId == InvalidResourceId)
	{
		return false;
	}

	Square* square = Object::NewObject<Square>();

	if (square == nullptr)
	{
		return false;
	}

	if (ObjectManager::FindObject(square->GetHandle()) != square)
	{
		square->Destroy();

		return false;
	}

	Sprite* sprite = square->AddSubObject<Sprite>(textureId);

	if (sprite == nullptr)
	{
		square->Destroy();

		return false;
	}

	if (ObjectManager::FindObject(sprite->GetHandle()) != sprite ||
		sprite->GetOuter() != square ||
		sprite->GetOwner() != square ||
		sprite->GetTextureId() != textureId ||
		square->GetSubObjectCount() != 1 ||
		square->GetComponentCount() != 1)
	{
		square->Destroy();

		return false;
	}

	const WorldObjectInstantiateParams parameters = WorldObjectInstantiateParams::DefaultWorld(runtime);

	if (square->Instantiate(parameters) == false)
	{
		square->Destroy();

		return false;
	}

	if (square->GetWorld() != &runtime.GetWorld())
	{
		square->Destroy();

		return false;
	}

	square->GetTransform();
	PRINT_DEBUG_OUTPUT("[RuntimeTest] Square Runtime 구성이 완료되었습니다.\n");

	/// 월드 편입
	return true;
}
