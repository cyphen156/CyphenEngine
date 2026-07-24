#include "pch.h"

#include <cstdio>

#include "Test/Runtime/RuntimeTest.h"

#include "Runtime/Public/GameRuntime.h"
#include "Runtime/Public/Object.h"
#include "Runtime/Public/ObjectManager.h"
#include "Runtime/Public/World.h"
#include "Runtime/Public/WorldObject.h"
#include "Test/Runtime/Sprite.h"
#include "Test/Runtime/Square.h"

namespace
{
	struct TestContext
	{
		int32 passCount = 0;
		int32 failCount = 0;
	};

	void WriteTestLine(const char* message)
	{
#ifdef _DEBUG
		PRINT_DEBUG_OUTPUT(message);
		PRINT_DEBUG_OUTPUT("\n");
#endif
	}

	void Expect(
		TestContext& context,
		bool condition,
		const char* name)
	{
		if (condition)
		{
			++context.passCount;
#ifdef _DEBUG
			PRINT_DEBUG_OUTPUT("[PASS] ");
#endif
		}
		else
		{
			++context.failCount;
#ifdef _DEBUG
			PRINT_DEBUG_OUTPUT("[FAIL] ");
#endif
		}

		WriteTestLine(name);
	}
}

void RunRuntimeTests(
	GameRuntime& runtime,
	World& world,
	const std::vector<ResourceId>& resourceIds)
{
	TestContext context;

	WriteTestLine("[RuntimeTests] Begin");

	Expect(
		context,
		runtime.IsInitialized(),
		"GameRuntime is initialized");

	Expect(
		context,
		&world.GetGameRuntime() == &runtime,
		"World belongs to the target GameRuntime");

	Expect(
		context,
		resourceIds.size() >= 2,
		"Runtime fixture has two ResourceIds");

	if (resourceIds.size() >= 2)
	{
		constexpr uint32 RuntimeTestObjectCount = 2;

		const Transform initialTransforms[RuntimeTestObjectCount] =
		{
			Transform::FromPlanar(
				Vector2(-0.5f, 0.0f),
				0.0f,
				Vector2::One()),
			Transform::FromPlanar(
				Vector2(0.5f, 0.0f),
				0.0f,
				Vector2::One())
		};

		Square* squares[RuntimeTestObjectCount] = {};
		Sprite* sprites[RuntimeTestObjectCount] = {};

		for (uint32 index = 0;
			index < RuntimeTestObjectCount;
			++index)
		{
			squares[index] =
				world.Spawn<Square>(
					initialTransforms[index]);

			Expect(
				context,
				squares[index] != nullptr,
				"World.Spawn creates a Square");

			Square* square = squares[index];

			if (square == nullptr)
			{
				continue;
			}

			Expect(
				context,
				ObjectManager::FindObject(
					square->GetHandle()) == square,
				"Spawned Square is registered in ObjectManager");

			Expect(
				context,
				square->GetGameRuntime() == &runtime,
				"Spawned Square belongs to the target GameRuntime");

			Expect(
				context,
				square->GetWorld() == &world,
				"Spawned Square belongs to the target World");

			Expect(
				context,
				runtime.Admit(*square) == false,
				"GameRuntime rejects duplicate Admit");

			Expect(
				context,
				world.Join(
					*square,
					initialTransforms[index]) == false,
				"World rejects duplicate Join");

			Expect(
				context,
				square->HasUpdateParticipation(
					UpdateParticipation::Update) &&
				square->HasUpdateParticipation(
					UpdateParticipation::GlobalFinalUpdate),
				"Square declares Update and GlobalFinalUpdate participation");

			GameObject* gameObject = square;

			const uint32 squareUpdateCount =
				square->GetUpdateCount();

			const uint32 squareGlobalFinalUpdateCount =
				square->GetGlobalFinalUpdateCount();

			gameObject->Update(0.0);
			gameObject->GlobalFinalUpdate(0.0);

			Expect(
				context,
				square->GetUpdateCount() ==
				squareUpdateCount + 1,
				"GameObject base invokes Square Update");

			Expect(
				context,
				square->GetGlobalFinalUpdateCount() ==
				squareGlobalFinalUpdateCount + 1,
				"GameObject base invokes Square GlobalFinalUpdate");

			Transform worldTransform;

			Expect(
				context,
				world.TryGetTransform(
					square->GetHandle(),
					worldTransform) &&
				worldTransform.position ==
				initialTransforms[index].position,
				"World resolves the Square Transform");

			sprites[index] =
				square->AddSubObject<Sprite>(
					resourceIds[index]);

			Expect(
				context,
				sprites[index] != nullptr,
				"Square creates a Sprite SubObject");

			Sprite* sprite = sprites[index];

			if (sprite == nullptr)
			{
				continue;
			}

			Expect(
				context,
				ObjectManager::FindObject(
					sprite->GetHandle()) == sprite,
				"Sprite is registered in ObjectManager");

			Expect(
				context,
				sprite->GetOuter() == square &&
				sprite->GetOwner() == square,
				"Sprite Outer and Owner reference the Square");

			Expect(
				context,
				sprite->GetTextureId() ==
				resourceIds[index],
				"Sprite stores its ResourceId");

			Expect(
				context,
				square->GetSubObjectCount() == 1 &&
				square->GetComponentCount() == 1,
				"Square owns one Sprite Component");

			Expect(
				context,
				sprite->HasUpdateParticipation(
					UpdateParticipation::GlobalUpdate) &&
				sprite->HasUpdateParticipation(
					UpdateParticipation::Update),
				"Sprite declares GlobalUpdate and Update participation");

			Component* component = sprite;

			const uint32 spriteGlobalUpdateCount =
				sprite->GetGlobalUpdateCount();

			const uint32 spriteUpdateCount =
				sprite->GetUpdateCount();

			component->GlobalUpdate(0.0);
			component->Update(0.0);

			Expect(
				context,
				sprite->GetGlobalUpdateCount() ==
				spriteGlobalUpdateCount + 1,
				"Component base invokes Sprite GlobalUpdate");

			Expect(
				context,
				sprite->GetUpdateCount() ==
				spriteUpdateCount + 1,
				"Component base invokes Sprite Update");
		}

		if (squares[0] != nullptr &&
			squares[1] != nullptr)
		{
			Expect(
				context,
				squares[0]->GetHandle() !=
				squares[1]->GetHandle(),
				"Squares have independent ObjectHandles");
		}

		if (sprites[0] != nullptr &&
			sprites[1] != nullptr)
		{
			Expect(
				context,
				sprites[0]->GetHandle() !=
				sprites[1]->GetHandle(),
				"Sprites have independent ObjectHandles");
		}

		Object* invalidOuter =
			Object::NewObject<Object>();

		Sprite* detachedSprite =
			Object::NewObject<Sprite>(
				resourceIds[0]);

		Expect(
			context,
			invalidOuter != nullptr &&
			detachedSprite != nullptr,
			"Invalid Component Outer fixture is created");

		if (invalidOuter != nullptr &&
			detachedSprite != nullptr)
		{
			Expect(
				context,
				invalidOuter->AttachSubObject(
					*detachedSprite) == false,
				"Sprite rejects a non-GameObject Outer");
		}

		if (detachedSprite != nullptr)
		{
			detachedSprite->Destroy();
		}

		if (invalidOuter != nullptr)
		{
			invalidOuter->Destroy();
		}

		for (uint32 index = 0;
			index < RuntimeTestObjectCount;
			++index)
		{
			Square* square = squares[index];

			if (square == nullptr)
			{
				continue;
			}

			const ObjectHandle squareHandle =
				square->GetHandle();

			const ObjectHandle spriteHandle =
				sprites[index] != nullptr
				? sprites[index]->GetHandle()
				: ObjectHandle();

			Expect(
				context,
				square->Destroy(),
				"Square destruction succeeds");

			Expect(
				context,
				ObjectManager::FindObject(
					squareHandle) == nullptr,
				"Destroyed Square leaves ObjectManager");

			if (spriteHandle.IsSet())
			{
				Expect(
					context,
					ObjectManager::FindObject(
						spriteHandle) == nullptr,
					"Square destruction propagates to Sprite");
			}

			Transform destroyedTransform;

			Expect(
				context,
				world.TryGetTransform(
					squareHandle,
					destroyedTransform) == false,
				"Destroyed Square leaves World Transform storage");
		}

		for (uint32 index = 0;
			index < RuntimeTestObjectCount;
			++index)
		{
			squares[index] =
				world.Spawn<Square>(
					initialTransforms[index]);

			Expect(
				context,
				squares[index] != nullptr,
				"World.Spawn recreates a retained Square");

			Square* square = squares[index];

			if (square == nullptr)
			{
				continue;
			}

			Transform retainedTransform;

			Expect(
				context,
				world.TryGetTransform(
					square->GetHandle(),
					retainedTransform) &&
				retainedTransform.position ==
				initialTransforms[index].position,
				"Retained Square remains in World Transform storage");

			sprites[index] =
				square->AddSubObject<Sprite>(
					resourceIds[index]);

			Expect(
				context,
				sprites[index] != nullptr,
				"Retained Square recreates a Sprite SubObject");

			Sprite* sprite = sprites[index];

			if (sprite == nullptr)
			{
				continue;
			}

			Expect(
				context,
				sprite->GetTextureId() ==
				resourceIds[index],
				"Retained Sprite stores its ResourceId");
		}
	}

	char summary[128] = {};

	std::snprintf(
		summary,
		sizeof(summary),
		"[RuntimeTests] Summary PASS=%d FAIL=%d",
		context.passCount,
		context.failCount);

	WriteTestLine(summary);
	WriteTestLine("[RuntimeTests] End");
}
