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
#include "Test/Runtime/TestComponent.h"
#include "Test/Runtime/TestRuntimeObject.h"
#include "Test/Runtime/TestWorldObject.h"

namespace
{
	struct TestContext
	{
		int32 passCount = 0;
		int32 failCount = 0;
	};

	struct UpdateExecutionFixture
	{
		const TestUpdateCallCounts* callCounts = nullptr;
		UpdateParticipation participation = UpdateParticipation::None;
	};

	TestContext runtimeTestContext;

	constexpr uint32 TestParticipationTypeCount = 16;

	constexpr UpdateParticipation TestParticipations[TestParticipationTypeCount] =
	{
		UpdateParticipation::None,
		UpdateParticipation::GlobalUpdate,
		UpdateParticipation::Update,
		UpdateParticipation::FinalUpdate,
		UpdateParticipation::GlobalFinalUpdate,
		UpdateParticipation::GlobalUpdate | UpdateParticipation::Update,
		UpdateParticipation::GlobalUpdate | UpdateParticipation::FinalUpdate,
		UpdateParticipation::GlobalUpdate | UpdateParticipation::GlobalFinalUpdate,
		UpdateParticipation::Update | UpdateParticipation::FinalUpdate,
		UpdateParticipation::Update | UpdateParticipation::GlobalFinalUpdate,
		UpdateParticipation::FinalUpdate | UpdateParticipation::GlobalFinalUpdate,
		UpdateParticipation::GlobalUpdate | UpdateParticipation::Update | UpdateParticipation::FinalUpdate,
		UpdateParticipation::GlobalUpdate | UpdateParticipation::Update | UpdateParticipation::GlobalFinalUpdate,
		UpdateParticipation::GlobalUpdate | UpdateParticipation::FinalUpdate | UpdateParticipation::GlobalFinalUpdate,
		UpdateParticipation::Update | UpdateParticipation::FinalUpdate | UpdateParticipation::GlobalFinalUpdate,
		UpdateParticipation::GlobalUpdate | UpdateParticipation::Update | UpdateParticipation::FinalUpdate | UpdateParticipation::GlobalFinalUpdate
	};

	UpdateExecutionFixture worldObjectExecutionFixtures[TestParticipationTypeCount];
	UpdateExecutionFixture runtimeObjectExecutionFixtures[TestParticipationTypeCount];
	UpdateExecutionFixture componentExecutionFixtures[TestParticipationTypeCount];

	bool runtimeExecutionVerificationPending = false;

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

	template<typename TargetType, typename BaseType>
	BaseType* CaptureUpdateExecutionFixture(
		TargetType* target,
		const TestUpdateCallCounts*& outUpdateCallCounts)
	{
		outUpdateCallCounts = target != nullptr ?
			&target->GetUpdateCallCounts() :
			nullptr;

		return target;
	}

	bool HasTestParticipation(
		UpdateParticipation participation,
		UpdateParticipation targetParticipation)
	{
		return
			(static_cast<uint8>(participation) &
				static_cast<uint8>(targetParticipation)) != 0;
	}

	bool MatchesExpectedUpdateCalls(
		const UpdateExecutionFixture& fixture)
	{
		if (fixture.callCounts == nullptr)
		{
			return false;
		}

		const TestUpdateCallCounts& callCounts = *fixture.callCounts;

		return
			callCounts.globalUpdate ==
				(HasTestParticipation(fixture.participation, UpdateParticipation::GlobalUpdate) ? 1u : 0u) &&
			callCounts.update ==
				(HasTestParticipation(fixture.participation, UpdateParticipation::Update) ? 1u : 0u) &&
			callCounts.finalUpdate ==
				(HasTestParticipation(fixture.participation, UpdateParticipation::FinalUpdate) ? 1u : 0u) &&
			callCounts.globalFinalUpdate ==
				(HasTestParticipation(fixture.participation, UpdateParticipation::GlobalFinalUpdate) ? 1u : 0u);
	}

	// UpdateParticipation 16조합 테스트 타입을 typeIndex로 순환 선택합니다.
	// 타입 순서는 TestWorldObject / TestRuntimeObject / TestComponent 헤더의
	// 선언 순서를 따릅니다.
	WorldObject* SpawnTestWorldObject(
		World& world,
		const Transform& initialTransform,
		uint32 typeIndex,
		const TestUpdateCallCounts*& outUpdateCallCounts)
	{
		switch (typeIndex % TestParticipationTypeCount)
		{
		case 0:
			return CaptureUpdateExecutionFixture<TestWorldObject_None, WorldObject>(world.Spawn<TestWorldObject_None>(initialTransform), outUpdateCallCounts);
		case 1:
			return CaptureUpdateExecutionFixture<TestWorldObject_GU, WorldObject>(world.Spawn<TestWorldObject_GU>(initialTransform), outUpdateCallCounts);
		case 2:
			return CaptureUpdateExecutionFixture<TestWorldObject_U, WorldObject>(world.Spawn<TestWorldObject_U>(initialTransform), outUpdateCallCounts);
		case 3:
			return CaptureUpdateExecutionFixture<TestWorldObject_FU, WorldObject>(world.Spawn<TestWorldObject_FU>(initialTransform), outUpdateCallCounts);
		case 4:
			return CaptureUpdateExecutionFixture<TestWorldObject_GFU, WorldObject>(world.Spawn<TestWorldObject_GFU>(initialTransform), outUpdateCallCounts);
		case 5:
			return CaptureUpdateExecutionFixture<TestWorldObject_GU_U, WorldObject>(world.Spawn<TestWorldObject_GU_U>(initialTransform), outUpdateCallCounts);
		case 6:
			return CaptureUpdateExecutionFixture<TestWorldObject_GU_FU, WorldObject>(world.Spawn<TestWorldObject_GU_FU>(initialTransform), outUpdateCallCounts);
		case 7:
			return CaptureUpdateExecutionFixture<TestWorldObject_GU_GFU, WorldObject>(world.Spawn<TestWorldObject_GU_GFU>(initialTransform), outUpdateCallCounts);
		case 8:
			return CaptureUpdateExecutionFixture<TestWorldObject_U_FU, WorldObject>(world.Spawn<TestWorldObject_U_FU>(initialTransform), outUpdateCallCounts);
		case 9:
			return CaptureUpdateExecutionFixture<TestWorldObject_U_GFU, WorldObject>(world.Spawn<TestWorldObject_U_GFU>(initialTransform), outUpdateCallCounts);
		case 10:
			return CaptureUpdateExecutionFixture<TestWorldObject_FU_GFU, WorldObject>(world.Spawn<TestWorldObject_FU_GFU>(initialTransform), outUpdateCallCounts);
		case 11:
			return CaptureUpdateExecutionFixture<TestWorldObject_GU_U_FU, WorldObject>(world.Spawn<TestWorldObject_GU_U_FU>(initialTransform), outUpdateCallCounts);
		case 12:
			return CaptureUpdateExecutionFixture<TestWorldObject_GU_U_GFU, WorldObject>(world.Spawn<TestWorldObject_GU_U_GFU>(initialTransform), outUpdateCallCounts);
		case 13:
			return CaptureUpdateExecutionFixture<TestWorldObject_GU_FU_GFU, WorldObject>(world.Spawn<TestWorldObject_GU_FU_GFU>(initialTransform), outUpdateCallCounts);
		case 14:
			return CaptureUpdateExecutionFixture<TestWorldObject_U_FU_GFU, WorldObject>(world.Spawn<TestWorldObject_U_FU_GFU>(initialTransform), outUpdateCallCounts);
		default:
			return CaptureUpdateExecutionFixture<TestWorldObject_GU_U_FU_GFU, WorldObject>(world.Spawn<TestWorldObject_GU_U_FU_GFU>(initialTransform), outUpdateCallCounts);
		}
	}

	WorldObject* SpawnTestWorldObject(
		World& world,
		const Transform& initialTransform,
		uint32 typeIndex)
	{
		const TestUpdateCallCounts* ignoredUpdateCallCounts = nullptr;

		return SpawnTestWorldObject(world, initialTransform, typeIndex, ignoredUpdateCallCounts);
	}

	GameObject* CreateTestRuntimeObject(
		uint32 typeIndex,
		const TestUpdateCallCounts*& outUpdateCallCounts)
	{
		switch (typeIndex % TestParticipationTypeCount)
		{
		case 0:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_None, GameObject>(Object::NewObject<TestRuntimeObject_None>(), outUpdateCallCounts);
		case 1:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_GU, GameObject>(Object::NewObject<TestRuntimeObject_GU>(), outUpdateCallCounts);
		case 2:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_U, GameObject>(Object::NewObject<TestRuntimeObject_U>(), outUpdateCallCounts);
		case 3:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_FU, GameObject>(Object::NewObject<TestRuntimeObject_FU>(), outUpdateCallCounts);
		case 4:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_GFU, GameObject>(Object::NewObject<TestRuntimeObject_GFU>(), outUpdateCallCounts);
		case 5:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_GU_U, GameObject>(Object::NewObject<TestRuntimeObject_GU_U>(), outUpdateCallCounts);
		case 6:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_GU_FU, GameObject>(Object::NewObject<TestRuntimeObject_GU_FU>(), outUpdateCallCounts);
		case 7:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_GU_GFU, GameObject>(Object::NewObject<TestRuntimeObject_GU_GFU>(), outUpdateCallCounts);
		case 8:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_U_FU, GameObject>(Object::NewObject<TestRuntimeObject_U_FU>(), outUpdateCallCounts);
		case 9:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_U_GFU, GameObject>(Object::NewObject<TestRuntimeObject_U_GFU>(), outUpdateCallCounts);
		case 10:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_FU_GFU, GameObject>(Object::NewObject<TestRuntimeObject_FU_GFU>(), outUpdateCallCounts);
		case 11:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_GU_U_FU, GameObject>(Object::NewObject<TestRuntimeObject_GU_U_FU>(), outUpdateCallCounts);
		case 12:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_GU_U_GFU, GameObject>(Object::NewObject<TestRuntimeObject_GU_U_GFU>(), outUpdateCallCounts);
		case 13:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_GU_FU_GFU, GameObject>(Object::NewObject<TestRuntimeObject_GU_FU_GFU>(), outUpdateCallCounts);
		case 14:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_U_FU_GFU, GameObject>(Object::NewObject<TestRuntimeObject_U_FU_GFU>(), outUpdateCallCounts);
		default:
			return CaptureUpdateExecutionFixture<TestRuntimeObject_GU_U_FU_GFU, GameObject>(Object::NewObject<TestRuntimeObject_GU_U_FU_GFU>(), outUpdateCallCounts);
		}
	}

	GameObject* CreateTestRuntimeObject(uint32 typeIndex)
	{
		const TestUpdateCallCounts* ignoredUpdateCallCounts = nullptr;

		return CreateTestRuntimeObject(typeIndex, ignoredUpdateCallCounts);
	}

	Component* CreateTestComponent(
		uint32 typeIndex,
		const TestUpdateCallCounts*& outUpdateCallCounts)
	{
		switch (typeIndex % TestParticipationTypeCount)
		{
		case 0:
			return CaptureUpdateExecutionFixture<TestComponent_None, Component>(Object::NewObject<TestComponent_None>(), outUpdateCallCounts);
		case 1:
			return CaptureUpdateExecutionFixture<TestComponent_GU, Component>(Object::NewObject<TestComponent_GU>(), outUpdateCallCounts);
		case 2:
			return CaptureUpdateExecutionFixture<TestComponent_U, Component>(Object::NewObject<TestComponent_U>(), outUpdateCallCounts);
		case 3:
			return CaptureUpdateExecutionFixture<TestComponent_FU, Component>(Object::NewObject<TestComponent_FU>(), outUpdateCallCounts);
		case 4:
			return CaptureUpdateExecutionFixture<TestComponent_GFU, Component>(Object::NewObject<TestComponent_GFU>(), outUpdateCallCounts);
		case 5:
			return CaptureUpdateExecutionFixture<TestComponent_GU_U, Component>(Object::NewObject<TestComponent_GU_U>(), outUpdateCallCounts);
		case 6:
			return CaptureUpdateExecutionFixture<TestComponent_GU_FU, Component>(Object::NewObject<TestComponent_GU_FU>(), outUpdateCallCounts);
		case 7:
			return CaptureUpdateExecutionFixture<TestComponent_GU_GFU, Component>(Object::NewObject<TestComponent_GU_GFU>(), outUpdateCallCounts);
		case 8:
			return CaptureUpdateExecutionFixture<TestComponent_U_FU, Component>(Object::NewObject<TestComponent_U_FU>(), outUpdateCallCounts);
		case 9:
			return CaptureUpdateExecutionFixture<TestComponent_U_GFU, Component>(Object::NewObject<TestComponent_U_GFU>(), outUpdateCallCounts);
		case 10:
			return CaptureUpdateExecutionFixture<TestComponent_FU_GFU, Component>(Object::NewObject<TestComponent_FU_GFU>(), outUpdateCallCounts);
		case 11:
			return CaptureUpdateExecutionFixture<TestComponent_GU_U_FU, Component>(Object::NewObject<TestComponent_GU_U_FU>(), outUpdateCallCounts);
		case 12:
			return CaptureUpdateExecutionFixture<TestComponent_GU_U_GFU, Component>(Object::NewObject<TestComponent_GU_U_GFU>(), outUpdateCallCounts);
		case 13:
			return CaptureUpdateExecutionFixture<TestComponent_GU_FU_GFU, Component>(Object::NewObject<TestComponent_GU_FU_GFU>(), outUpdateCallCounts);
		case 14:
			return CaptureUpdateExecutionFixture<TestComponent_U_FU_GFU, Component>(Object::NewObject<TestComponent_U_FU_GFU>(), outUpdateCallCounts);
		default:
			return CaptureUpdateExecutionFixture<TestComponent_GU_U_FU_GFU, Component>(Object::NewObject<TestComponent_GU_U_FU_GFU>(), outUpdateCallCounts);
		}
	}

	Component* CreateTestComponent(uint32 typeIndex)
	{
		const TestUpdateCallCounts* ignoredUpdateCallCounts = nullptr;

		return CreateTestComponent(typeIndex, ignoredUpdateCallCounts);
	}
}

void RunRuntimeTests(
	GameRuntime& runtime,
	World& world,
	const std::vector<ResourceId>& resourceIds)
{
	runtimeTestContext = {};
	TestContext& context = runtimeTestContext;
	runtimeExecutionVerificationPending = false;

	for (uint32 typeIndex = 0; typeIndex < TestParticipationTypeCount; ++typeIndex)
	{
		worldObjectExecutionFixtures[typeIndex] = {};
		runtimeObjectExecutionFixtures[typeIndex] = {};
		componentExecutionFixtures[typeIndex] = {};
	}

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
				runtime.Admit(*square),
				"GameRuntime accepts duplicate Admit without new registration");

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

			Expect(
				context,
				detachedSprite->GetOuter() == nullptr &&
				invalidOuter->GetSubObjectCount() == 0,
				"Rejected Sprite attachment preserves both Objects");
		}

		if (detachedSprite != nullptr)
		{
			detachedSprite->Destroy();
		}

		if (invalidOuter != nullptr)
		{
			invalidOuter->Destroy();
		}

		Object* relationRoot =
			Object::NewObject<Object>();

		Object* relationMiddle =
			Object::NewObject<Object>();

		Object* relationLeaf =
			Object::NewObject<Object>();

		Object* alternateOuter =
			Object::NewObject<Object>();

		Expect(
			context,
			relationRoot != nullptr &&
			relationMiddle != nullptr &&
			relationLeaf != nullptr &&
			alternateOuter != nullptr,
			"Object relation fixtures are created");

		if (relationRoot != nullptr &&
			relationMiddle != nullptr &&
			relationLeaf != nullptr &&
			alternateOuter != nullptr)
		{
			Expect(
				context,
				relationRoot->AttachSubObject(
					*relationRoot) == false,
				"Object rejects attachment to itself");

			Expect(
				context,
				relationRoot->AttachSubObject(
					*relationMiddle),
				"Object attaches a direct SubObject");

			Expect(
				context,
				relationMiddle->AttachSubObject(
					*relationLeaf),
				"Object attaches a nested SubObject");

			Expect(
				context,
				relationRoot->AttachSubObject(
					*relationMiddle),
				"Object accepts duplicate attachment to the same Outer");

			Expect(
				context,
				relationRoot->GetSubObjectCount() == 1 &&
				relationMiddle->GetOuter() ==
				relationRoot,
				"Duplicate attachment does not duplicate the relation");

			Expect(
				context,
				alternateOuter->AttachSubObject(
					*relationMiddle) == false,
				"Object rejects attachment to a second Outer");

			Expect(
				context,
				relationMiddle->GetOuter() ==
				relationRoot &&
				relationRoot->GetSubObjectCount() == 1 &&
				alternateOuter->GetSubObjectCount() == 0,
				"Rejected reparenting preserves the original relation");

			Expect(
				context,
				relationLeaf->AttachSubObject(
					*relationRoot) == false,
				"Object rejects a circular attachment");

			Expect(
				context,
				relationRoot->GetOuter() == nullptr &&
				relationLeaf->GetSubObjectCount() == 0,
				"Rejected circular attachment preserves the hierarchy");

			Expect(
				context,
				alternateOuter->DetachSubObject(
					*relationMiddle) == false,
				"Object rejects detachment by a non-Outer");

			Expect(
				context,
				relationRoot->DetachSubObject(
					*relationMiddle),
				"Object detaches its direct SubObject");

			Expect(
				context,
				relationMiddle->GetOuter() == nullptr &&
				relationRoot->GetSubObjectCount() == 0,
				"Detached Object becomes an independent root");

			Expect(
				context,
				alternateOuter->AttachSubObject(
					*relationMiddle),
				"Detached Object attaches to a new Outer");

			Expect(
				context,
				alternateOuter->DetachSubObject(
					*relationMiddle),
				"Reattached Object detaches from its new Outer");
		}

		if (relationMiddle != nullptr &&
			relationMiddle->GetOuter() != nullptr)
		{
			relationMiddle->GetOuter()->
				DetachSubObject(*relationMiddle);
		}

		if (relationLeaf != nullptr &&
			relationLeaf->GetOuter() != nullptr)
		{
			relationLeaf->GetOuter()->
				DetachSubObject(*relationLeaf);
		}

		if (relationRoot != nullptr)
		{
			relationRoot->Destroy();
		}

		if (relationMiddle != nullptr)
		{
			relationMiddle->Destroy();
		}

		if (relationLeaf != nullptr)
		{
			relationLeaf->Destroy();
		}

		if (alternateOuter != nullptr)
		{
			alternateOuter->Destroy();
		}

		GameObject* flatRuntimeRoot =
			Object::NewObject<GameObject>();

		GameObject* flatRuntimeMiddle =
			Object::NewObject<GameObject>();

		GameObject* flatRuntimeLeaf =
			Object::NewObject<GameObject>();

		Expect(
			context,
			flatRuntimeRoot != nullptr &&
			flatRuntimeMiddle != nullptr &&
			flatRuntimeLeaf != nullptr,
			"Flat Runtime hierarchy fixtures are created");

		if (flatRuntimeRoot != nullptr &&
			flatRuntimeMiddle != nullptr &&
			flatRuntimeLeaf != nullptr)
		{
			Expect(
				context,
				flatRuntimeMiddle->AttachSubObject(
					*flatRuntimeLeaf),
				"Flat Runtime hierarchy attaches its leaf");

			Expect(
				context,
				flatRuntimeRoot->AttachSubObject(
					*flatRuntimeMiddle),
				"Flat Runtime hierarchy attaches its middle");

			Expect(
				context,
				runtime.Admit(*flatRuntimeRoot),
				"GameRuntime admits a prebuilt GameObject hierarchy");

			Expect(
				context,
				flatRuntimeRoot->GetGameRuntime() ==
				&runtime &&
				flatRuntimeMiddle->GetGameRuntime() ==
				&runtime &&
				flatRuntimeLeaf->GetGameRuntime() ==
				&runtime,
				"Admit applies flat Runtime membership to every GameObject");

			const ObjectHandle flatRuntimeRootHandle =
				flatRuntimeRoot->GetHandle();

			const ObjectHandle flatRuntimeMiddleHandle =
				flatRuntimeMiddle->GetHandle();

			const ObjectHandle flatRuntimeLeafHandle =
				flatRuntimeLeaf->GetHandle();

			const bool flatRuntimeHierarchyDestroyed =
				flatRuntimeRoot->Destroy();

			Expect(
				context,
				flatRuntimeHierarchyDestroyed,
				"Destroy removes the admitted GameObject hierarchy");

			Expect(
				context,
				ObjectManager::FindObject(
					flatRuntimeRootHandle) == nullptr &&
				ObjectManager::FindObject(
					flatRuntimeMiddleHandle) == nullptr &&
				ObjectManager::FindObject(
					flatRuntimeLeafHandle) == nullptr,
				"Destroy recursively removes every flat Runtime member");

			if (flatRuntimeHierarchyDestroyed)
			{
				flatRuntimeRoot = nullptr;
				flatRuntimeMiddle = nullptr;
				flatRuntimeLeaf = nullptr;
			}
		}

		if (flatRuntimeRoot != nullptr)
		{
			flatRuntimeRoot->Destroy();
		}

		if (flatRuntimeMiddle != nullptr)
		{
			flatRuntimeMiddle->Destroy();
		}

		if (flatRuntimeLeaf != nullptr)
		{
			flatRuntimeLeaf->Destroy();
		}

		GameObject* attachedRuntimeOwner =
			Object::NewObject<GameObject>();

		GameObject* reattachmentRuntimeOwner =
			Object::NewObject<GameObject>();

		Object* attachedRuntimeBridge =
			Object::NewObject<Object>();

		GameObject* attachedRuntimeChild =
			Object::NewObject<GameObject>();

		GameObject* attachedRuntimeLeaf =
			Object::NewObject<GameObject>();

		Expect(
			context,
			attachedRuntimeOwner != nullptr &&
			reattachmentRuntimeOwner != nullptr &&
			attachedRuntimeBridge != nullptr &&
			attachedRuntimeChild != nullptr &&
			attachedRuntimeLeaf != nullptr,
			"Runtime attachment fixtures are created");

		if (attachedRuntimeOwner != nullptr &&
			reattachmentRuntimeOwner != nullptr &&
			attachedRuntimeBridge != nullptr &&
			attachedRuntimeChild != nullptr &&
			attachedRuntimeLeaf != nullptr)
		{
			Expect(
				context,
				attachedRuntimeChild->AttachSubObject(
					*attachedRuntimeLeaf),
				"Runtime attachment child owns its leaf");

			Expect(
				context,
				attachedRuntimeBridge->AttachSubObject(
					*attachedRuntimeChild),
				"Runtime attachment bridge owns its GameObject hierarchy");

			Expect(
				context,
				runtime.Admit(
					*attachedRuntimeOwner),
				"GameRuntime admits the attachment owner");

			Expect(
				context,
				attachedRuntimeOwner->AttachSubObject(
					*attachedRuntimeBridge),
				"Participating GameObject accepts a non-participating subtree");

			Expect(
				context,
				attachedRuntimeChild->GetGameRuntime() ==
				&runtime &&
				attachedRuntimeLeaf->GetGameRuntime() ==
				&runtime,
				"Attachment propagates Runtime membership through an Object bridge");

			Expect(
				context,
				attachedRuntimeOwner->DetachSubObject(
					*attachedRuntimeBridge),
				"Participating GameObject detaches its admitted subtree");

			Expect(
				context,
				attachedRuntimeBridge->GetOuter() ==
				nullptr &&
				attachedRuntimeChild->GetGameRuntime() ==
				&runtime &&
				attachedRuntimeLeaf->GetGameRuntime() ==
				&runtime,
				"Detached subtree preserves flat Runtime membership");

			Expect(
				context,
				runtime.Admit(
					*reattachmentRuntimeOwner),
				"GameRuntime admits the reattachment owner");

			Expect(
				context,
				reattachmentRuntimeOwner->
				AttachSubObject(
					*attachedRuntimeBridge),
				"Detached subtree reattaches inside the same Runtime");

			Expect(
				context,
				attachedRuntimeBridge->GetOuter() ==
				reattachmentRuntimeOwner &&
				attachedRuntimeChild->GetGameRuntime() ==
				&runtime &&
				attachedRuntimeLeaf->GetGameRuntime() ==
				&runtime,
				"Reattachment preserves the subtree Runtime");

			const ObjectHandle attachedRuntimeBridgeHandle =
				attachedRuntimeBridge->GetHandle();

			const ObjectHandle attachedRuntimeChildHandle =
				attachedRuntimeChild->GetHandle();

			const ObjectHandle attachedRuntimeLeafHandle =
				attachedRuntimeLeaf->GetHandle();

			const bool attachedRuntimeOwnerDestroyed =
				attachedRuntimeOwner->Destroy();

			Expect(
				context,
				attachedRuntimeOwnerDestroyed,
				"Detached Runtime owner destruction succeeds");

			if (attachedRuntimeOwnerDestroyed)
			{
				attachedRuntimeOwner = nullptr;
			}

			const bool reattachedRuntimeHierarchyDestroyed =
				reattachmentRuntimeOwner->Destroy();

			Expect(
				context,
				reattachedRuntimeHierarchyDestroyed,
				"Reattached Runtime hierarchy destruction succeeds");

			Expect(
				context,
				ObjectManager::FindObject(
					attachedRuntimeBridgeHandle) ==
				nullptr &&
				ObjectManager::FindObject(
					attachedRuntimeChildHandle) ==
				nullptr &&
				ObjectManager::FindObject(
					attachedRuntimeLeafHandle) ==
				nullptr,
				"Reattached Runtime hierarchy destruction propagates through the subtree");

			if (reattachedRuntimeHierarchyDestroyed)
			{
				reattachmentRuntimeOwner = nullptr;
				attachedRuntimeBridge = nullptr;
				attachedRuntimeChild = nullptr;
				attachedRuntimeLeaf = nullptr;
			}
		}

		if (attachedRuntimeOwner != nullptr &&
			ObjectManager::FindObject(
				attachedRuntimeOwner->GetHandle()) ==
			attachedRuntimeOwner)
		{
			attachedRuntimeOwner->Destroy();
		}

		if (reattachmentRuntimeOwner != nullptr &&
			ObjectManager::FindObject(
				reattachmentRuntimeOwner->GetHandle()) ==
			reattachmentRuntimeOwner)
		{
			reattachmentRuntimeOwner->Destroy();
		}

		if (attachedRuntimeBridge != nullptr &&
			ObjectManager::FindObject(
				attachedRuntimeBridge->GetHandle()) ==
			attachedRuntimeBridge)
		{
			attachedRuntimeBridge->Destroy();
		}

		if (attachedRuntimeChild != nullptr &&
			ObjectManager::FindObject(
				attachedRuntimeChild->GetHandle()) ==
			attachedRuntimeChild)
		{
			attachedRuntimeChild->Destroy();
		}

		if (attachedRuntimeLeaf != nullptr &&
			ObjectManager::FindObject(
				attachedRuntimeLeaf->GetHandle()) ==
			attachedRuntimeLeaf)
		{
			attachedRuntimeLeaf->Destroy();
		}

		GameObject* partialRuntimeRoot =
			Object::NewObject<GameObject>();

		GameObject* partialRuntimeMiddle =
			Object::NewObject<GameObject>();

		GameObject* partialRuntimeLeaf =
			Object::NewObject<GameObject>();

		Expect(
			context,
			partialRuntimeRoot != nullptr &&
			partialRuntimeMiddle != nullptr &&
			partialRuntimeLeaf != nullptr,
			"Partial Runtime hierarchy fixtures are created");

		if (partialRuntimeRoot != nullptr &&
			partialRuntimeMiddle != nullptr &&
			partialRuntimeLeaf != nullptr)
		{
			Expect(
				context,
				partialRuntimeMiddle->AttachSubObject(
					*partialRuntimeLeaf),
				"Partial Runtime hierarchy attaches its leaf");

			Expect(
				context,
				partialRuntimeRoot->AttachSubObject(
					*partialRuntimeMiddle),
				"Partial Runtime hierarchy attaches its middle");

			Expect(
				context,
				runtime.Admit(
					*partialRuntimeLeaf),
				"GameRuntime admits only the selected nested GameObject");

			Expect(
				context,
				partialRuntimeRoot->GetGameRuntime() ==
				nullptr &&
				partialRuntimeMiddle->GetGameRuntime() ==
				nullptr &&
				partialRuntimeLeaf->GetGameRuntime() ==
				&runtime,
				"Nested Admit does not propagate Runtime membership upward");

			Expect(
				context,
				runtime.Admit(
					*partialRuntimeRoot),
				"GameRuntime later admits the partial hierarchy root");

			Expect(
				context,
				partialRuntimeRoot->GetGameRuntime() ==
				&runtime &&
				partialRuntimeMiddle->GetGameRuntime() ==
				&runtime &&
				partialRuntimeLeaf->GetGameRuntime() ==
				&runtime,
				"Root Admit fills missing flat Runtime membership");

			const ObjectHandle partialRuntimeMiddleHandle =
				partialRuntimeMiddle->GetHandle();

			const ObjectHandle partialRuntimeLeafHandle =
				partialRuntimeLeaf->GetHandle();

			const bool partialRuntimeHierarchyDestroyed =
				partialRuntimeRoot->Destroy();

			Expect(
				context,
				partialRuntimeHierarchyDestroyed,
				"Partial Runtime hierarchy destruction succeeds");

			Expect(
				context,
				ObjectManager::FindObject(
					partialRuntimeMiddleHandle) ==
				nullptr &&
				ObjectManager::FindObject(
					partialRuntimeLeafHandle) ==
				nullptr,
				"Partial Runtime hierarchy destruction removes nested members");

			if (partialRuntimeHierarchyDestroyed)
			{
				partialRuntimeRoot = nullptr;
				partialRuntimeMiddle = nullptr;
				partialRuntimeLeaf = nullptr;
			}
		}

		if (partialRuntimeRoot != nullptr &&
			ObjectManager::FindObject(
				partialRuntimeRoot->GetHandle()) ==
			partialRuntimeRoot)
		{
			partialRuntimeRoot->Destroy();
		}

		if (partialRuntimeMiddle != nullptr &&
			ObjectManager::FindObject(
				partialRuntimeMiddle->GetHandle()) ==
			partialRuntimeMiddle)
		{
			partialRuntimeMiddle->Destroy();
		}

		if (partialRuntimeLeaf != nullptr &&
			ObjectManager::FindObject(
				partialRuntimeLeaf->GetHandle()) ==
			partialRuntimeLeaf)
		{
			partialRuntimeLeaf->Destroy();
		}

		GameObject* componentAttachmentOwner =
			Object::NewObject<GameObject>();

		Sprite* attachedComponent =
			Object::NewObject<Sprite>(
				resourceIds[0]);

		Object* attachedComponentObject =
			Object::NewObject<Object>();

		GameObject* attachedComponentGameObject =
			Object::NewObject<GameObject>();

		Expect(
			context,
			componentAttachmentOwner != nullptr &&
			attachedComponent != nullptr &&
			attachedComponentObject != nullptr &&
			attachedComponentGameObject != nullptr,
			"Component attachment fixtures are created");

		if (componentAttachmentOwner != nullptr &&
			attachedComponent != nullptr &&
			attachedComponentObject != nullptr &&
			attachedComponentGameObject != nullptr)
		{
			Expect(
				context,
				attachedComponentObject->
				AttachSubObject(
					*attachedComponentGameObject),
				"Component attachment Object owns a GameObject");

			Expect(
				context,
				attachedComponent->AttachSubObject(
					*attachedComponentObject),
				"Detached Component owns its Object hierarchy");

			Expect(
				context,
				runtime.Admit(
					*componentAttachmentOwner),
				"GameRuntime admits the Component attachment owner");

			Expect(
				context,
				componentAttachmentOwner->
				AttachSubObject(
					*attachedComponent),
				"Participating GameObject accepts a Component hierarchy");

			Expect(
				context,
				attachedComponent->GetOwner() ==
				componentAttachmentOwner &&
				attachedComponentGameObject->
				GetGameRuntime() ==
				&runtime,
				"Component attachment propagates Runtime membership through its subtree");

			Expect(
				context,
				componentAttachmentOwner->
				DetachSubObject(
					*attachedComponent),
				"Participating GameObject detaches the Component hierarchy");

			Expect(
				context,
				attachedComponent->GetOwner() ==
				nullptr &&
				attachedComponentGameObject->
				GetGameRuntime() ==
				nullptr,
				"Component Detach removes nested Runtime membership");

			const ObjectHandle attachedComponentObjectHandle =
				attachedComponentObject->GetHandle();

			const ObjectHandle attachedComponentGameObjectHandle =
				attachedComponentGameObject->GetHandle();

			const bool componentAttachmentOwnerDestroyed =
				componentAttachmentOwner->Destroy();

			Expect(
				context,
				componentAttachmentOwnerDestroyed,
				"Component attachment owner destruction succeeds");

			if (componentAttachmentOwnerDestroyed)
			{
				componentAttachmentOwner = nullptr;
			}

			const bool attachedComponentHierarchyDestroyed =
				attachedComponent->Destroy();

			Expect(
				context,
				attachedComponentHierarchyDestroyed,
				"Detached Component hierarchy destruction succeeds");

			Expect(
				context,
				ObjectManager::FindObject(
					attachedComponentObjectHandle) ==
				nullptr &&
				ObjectManager::FindObject(
					attachedComponentGameObjectHandle) ==
				nullptr,
				"Component hierarchy destruction removes nested members");

			if (attachedComponentHierarchyDestroyed)
			{
				attachedComponent = nullptr;
				attachedComponentObject = nullptr;
				attachedComponentGameObject = nullptr;
			}
		}

		if (componentAttachmentOwner != nullptr &&
			ObjectManager::FindObject(
				componentAttachmentOwner->GetHandle()) ==
			componentAttachmentOwner)
		{
			componentAttachmentOwner->Destroy();
		}

		if (attachedComponent != nullptr &&
			ObjectManager::FindObject(
				attachedComponent->GetHandle()) ==
			attachedComponent)
		{
			attachedComponent->Destroy();
		}

		if (attachedComponentObject != nullptr &&
			ObjectManager::FindObject(
				attachedComponentObject->GetHandle()) ==
			attachedComponentObject)
		{
			attachedComponentObject->Destroy();
		}

		if (attachedComponentGameObject != nullptr &&
			ObjectManager::FindObject(
				attachedComponentGameObject->GetHandle()) ==
			attachedComponentGameObject)
		{
			attachedComponentGameObject->Destroy();
		}

		GameObject* hierarchyOwner =
			Object::NewObject<GameObject>();

		Sprite* hierarchyComponent =
			Object::NewObject<Sprite>(
				resourceIds[0]);

		Object* hierarchyObject =
			Object::NewObject<Object>();

		GameObject* hierarchyGameObject =
			Object::NewObject<GameObject>();

		Expect(
			context,
			hierarchyOwner != nullptr &&
			hierarchyComponent != nullptr &&
			hierarchyObject != nullptr &&
			hierarchyGameObject != nullptr,
			"Mixed hierarchy fixtures are created");

		if (hierarchyOwner != nullptr &&
			hierarchyComponent != nullptr &&
			hierarchyObject != nullptr &&
			hierarchyGameObject != nullptr)
		{
			Expect(
				context,
				hierarchyObject->AttachSubObject(
					*hierarchyGameObject),
				"Object accepts a GameObject SubObject");

			Expect(
				context,
				hierarchyComponent->AttachSubObject(
					*hierarchyObject),
				"Component accepts an Object SubObject");

			Expect(
				context,
				hierarchyOwner->AttachSubObject(
					*hierarchyComponent),
				"GameObject accepts a compatible Component hierarchy");

			Expect(
				context,
				runtime.Admit(
					*hierarchyOwner),
				"GameRuntime admits the prebuilt mixed hierarchy");

			Expect(
				context,
				hierarchyComponent->GetOwner() ==
				hierarchyOwner &&
				hierarchyGameObject->GetGameRuntime() ==
				&runtime,
				"Component hierarchy preserves Owner and Runtime relationships");

			Expect(
				context,
				hierarchyOwner->DetachSubObject(
					*hierarchyComponent),
				"GameObject detaches a Component hierarchy");

			Expect(
				context,
				hierarchyComponent->GetOwner() == nullptr &&
				hierarchyOwner->GetSubObjectCount() == 0 &&
				hierarchyGameObject->GetGameRuntime() ==
				nullptr,
				"Component Detach removes the nested GameObject Runtime");
		}

		if (hierarchyComponent != nullptr &&
			hierarchyComponent->GetOuter() != nullptr)
		{
			hierarchyComponent->GetOuter()->
				DetachSubObject(*hierarchyComponent);
		}

		if (hierarchyObject != nullptr &&
			hierarchyObject->GetOuter() != nullptr)
		{
			hierarchyObject->GetOuter()->
				DetachSubObject(*hierarchyObject);
		}

		if (hierarchyGameObject != nullptr &&
			hierarchyGameObject->GetOuter() != nullptr)
		{
			hierarchyGameObject->GetOuter()->
				DetachSubObject(*hierarchyGameObject);
		}

		if (hierarchyOwner != nullptr)
		{
			hierarchyOwner->Destroy();
		}

		if (hierarchyComponent != nullptr)
		{
			hierarchyComponent->Destroy();
		}

		if (hierarchyObject != nullptr)
		{
			hierarchyObject->Destroy();
		}

		if (hierarchyGameObject != nullptr)
		{
			hierarchyGameObject->Destroy();
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

	// ------------------------------------------------------------------------
	// 서브트리 경로 정합성
	//
	// 엔진이 만든 GameRuntime과 World 실환경을 대상으로
	// Runtime 참여 경로와 World anchor 경계 규칙을 검증합니다.
	// 이 구간의 fixture는 검증 후 모두 파괴합니다.
	// ------------------------------------------------------------------------
	World* secondWorld = runtime.GetWorld(1);
	World* thirdWorld = runtime.GetWorld(2);

	Expect(
		context,
		runtime.GetWorldCount() == 3 &&
		runtime.GetWorld(0) == &world &&
		secondWorld != nullptr &&
		thirdWorld != nullptr,
		"GameRuntime owns three debug Worlds");

	if (secondWorld != nullptr &&
		thirdWorld != nullptr)
	{
		// 1. 소유 GameObject가 없는 Component 아래의 GameObject는
		//    독립적인 Runtime 참여 루트가 될 수 없습니다.
		Component* orphanComponent = CreateTestComponent(0);
		Object* orphanBridge = Object::NewObject<Object>();
		GameObject* orphanGameObject = CreateTestRuntimeObject(0);

		Expect(
			context,
			orphanComponent != nullptr &&
			orphanBridge != nullptr &&
			orphanGameObject != nullptr,
			"Subtree path fixtures are created");

		if (orphanComponent != nullptr &&
			orphanBridge != nullptr &&
			orphanGameObject != nullptr)
		{
			Expect(
				context,
				orphanComponent->AttachSubObject(*orphanBridge) &&
				orphanBridge->AttachSubObject(*orphanGameObject),
				"Detached Component owns an Object and GameObject path");

			Expect(
				context,
				runtime.Admit(*orphanGameObject) == false,
				"GameObject below an unowned Component cannot join a Runtime");

			Expect(
				context,
				orphanComponent->Destroy(),
				"Subtree path fixture destruction succeeds");
		}

		// 2. 중첩 WorldObject는 같은 Runtime의 다른 World에 합류하여
		//    독립적인 World anchor 경계를 형성합니다.
		WorldObject* outerWorldObject = SpawnTestWorldObject(
			world,
			Transform::Identity(),
			8);

		WorldObject* nestedWorldObject = Object::NewObject<TestWorldObject_U_FU>();

		Expect(
			context,
			outerWorldObject != nullptr &&
			nestedWorldObject != nullptr,
			"Nested WorldObject fixtures are created");

		if (outerWorldObject != nullptr &&
			nestedWorldObject != nullptr)
		{
			Expect(
				context,
				outerWorldObject->AttachSubObject(*nestedWorldObject) &&
				nestedWorldObject->GetGameRuntime() == &runtime,
				"Nested WorldObject joins the Runtime through its outer");

			Expect(
				context,
				secondWorld->Join(*nestedWorldObject, Transform::Identity()),
				"Nested WorldObject joins a different World of the same Runtime");

			Expect(
				context,
				outerWorldObject->GetWorld() == &world &&
				nestedWorldObject->GetWorld() == secondWorld,
				"Nested WorldObjects keep independent World anchors");

			Expect(
				context,
				outerWorldObject->Destroy(),
				"Nested WorldObject anchor destruction succeeds");
		}

		// 3. World에 합류한 WorldObject는 구조적 Detach 이후에도
		//    자신의 World anchor와 Runtime 참여를 유지합니다.
		GameObject* anchorParent = CreateTestRuntimeObject(1);
		WorldObject* anchoredWorldObject = Object::NewObject<TestWorldObject_GU_U>();

		Expect(
			context,
			anchorParent != nullptr &&
			anchoredWorldObject != nullptr,
			"Anchor retention fixtures are created");

		if (anchorParent != nullptr &&
			anchoredWorldObject != nullptr)
		{
			Expect(
				context,
				runtime.Admit(*anchorParent) &&
				anchorParent->AttachSubObject(*anchoredWorldObject),
				"Anchor retention hierarchy joins the Runtime");

			Expect(
				context,
				thirdWorld->Join(*anchoredWorldObject, Transform::Identity()),
				"Attached WorldObject joins a World below its parent");

			Expect(
				context,
				anchorParent->DetachSubObject(*anchoredWorldObject),
				"Parent detaches the World-joined WorldObject");

			Expect(
				context,
				anchoredWorldObject->GetWorld() == thirdWorld &&
				anchoredWorldObject->GetGameRuntime() == &runtime,
				"Detached WorldObject keeps its World anchor and Runtime");

			Expect(
				context,
				anchoredWorldObject->Destroy() &&
				anchorParent->Destroy(),
				"Anchor retention fixtures destruction succeeds");
		}

		// 4. Runtime Leave는 World 탈퇴를 포함하며
		//    살아 있는 미소속 GameObject 상태로 되돌립니다.
		WorldObject* leavingWorldObject = SpawnTestWorldObject(
			*secondWorld,
			Transform::Identity(),
			15);

		Expect(
			context,
			leavingWorldObject != nullptr,
			"Runtime Leave fixture is created");

		if (leavingWorldObject != nullptr)
		{
			Expect(
				context,
				runtime.Leave(*leavingWorldObject),
				"Runtime Leave removes a joined WorldObject");

			Expect(
				context,
				leavingWorldObject->GetWorld() == nullptr &&
				leavingWorldObject->GetGameRuntime() == nullptr,
				"Left WorldObject loses World and Runtime membership");

			Expect(
				context,
				leavingWorldObject->Destroy(),
				"Left WorldObject destruction succeeds");
		}

		// --------------------------------------------------------------------
		// 최종 유지 인구 구성
		//
		// 유지된 Square / Sprite 두 쌍을 포함하여 최종 상태를
		// Total Object 10000 = WorldObject 2500 + GameObject 2500 +
		// Component 2500 + Object 2500으로 구성합니다.
		//
		// 각 클러스터는 WorldObject 루트 아래에 Object 다리와
		// 중첩 GameObject를 부착하고, Component는 anchor WorldObject와
		// 중첩 GameObject에 번갈아 부착하여 두 공급 경로를 모두 사용합니다.
		// Runtime이 소유한 세 World에 순환 배치하며
		// 16가지 UpdateParticipation 조합을 모두 사용합니다.
		// --------------------------------------------------------------------
		constexpr uint32 PopulationClusterCount = 2498;

		uint32 spawnedWorldObjectCount = 0;
		uint32 attachedObjectCount = 0;
		uint32 attachedGameObjectCount = 0;
		uint32 attachedComponentCount = 0;
		uint32 worldMembershipMatchCount = 0;
		uint32 transformMatchCount = 0;
		uint32 runtimeMembershipMatchCount = 0;
		uint32 componentOwnerMatchCount = 0;

		for (uint32 clusterIndex = 0; clusterIndex < PopulationClusterCount; ++clusterIndex)
		{
			World* targetWorld = runtime.GetWorld(clusterIndex % 3);

			if (targetWorld == nullptr)
			{
				continue;
			}

			const uint32 typeIndex = clusterIndex % TestParticipationTypeCount;

			const Transform clusterTransform = Transform::FromPlanar(
				Vector2(
					static_cast<float>(clusterIndex % 100),
					static_cast<float>(clusterIndex / 100)),
				0.0f,
				Vector2::One());

			const TestUpdateCallCounts* worldObjectUpdateCallCounts = nullptr;
			const TestUpdateCallCounts* runtimeObjectUpdateCallCounts = nullptr;
			const TestUpdateCallCounts* componentUpdateCallCounts = nullptr;

			WorldObject* worldObject = SpawnTestWorldObject(
				*targetWorld,
				clusterTransform,
				typeIndex,
				worldObjectUpdateCallCounts);

			if (worldObject == nullptr)
			{
				continue;
			}

			++spawnedWorldObjectCount;

			Object* bridgeObject = Object::NewObject<Object>();
			GameObject* nestedGameObject = CreateTestRuntimeObject(
				typeIndex,
				runtimeObjectUpdateCallCounts);

			Component* component = CreateTestComponent(
				typeIndex,
				componentUpdateCallCounts);

			if (bridgeObject == nullptr ||
				nestedGameObject == nullptr ||
				component == nullptr)
			{
				continue;
			}

			if (worldObject->AttachSubObject(*bridgeObject))
			{
				++attachedObjectCount;
			}

			if (bridgeObject->AttachSubObject(*nestedGameObject))
			{
				++attachedGameObjectCount;
			}

			GameObject* componentOwner =
				clusterIndex % 2 == 0 ?
				static_cast<GameObject*>(worldObject) :
				nestedGameObject;

			if (componentOwner->AttachSubObject(*component))
			{
				++attachedComponentCount;
			}

			if (worldObject->GetWorld() == targetWorld)
			{
				++worldMembershipMatchCount;
			}

			Transform storedTransform;

			if (targetWorld->TryGetTransform(worldObject->GetHandle(), storedTransform) &&
				storedTransform.position == clusterTransform.position)
			{
				++transformMatchCount;
			}

			if (worldObject->GetGameRuntime() == &runtime &&
				nestedGameObject->GetGameRuntime() == &runtime)
			{
				++runtimeMembershipMatchCount;
			}

			if (component->GetOwner() == componentOwner)
			{
				++componentOwnerMatchCount;
			}

			if (worldObjectExecutionFixtures[typeIndex].callCounts == nullptr)
			{
				worldObjectExecutionFixtures[typeIndex] =
				{
					worldObjectUpdateCallCounts,
					TestParticipations[typeIndex]
				};

				runtimeObjectExecutionFixtures[typeIndex] =
				{
					runtimeObjectUpdateCallCounts,
					TestParticipations[typeIndex]
				};

				componentExecutionFixtures[typeIndex] =
				{
					componentUpdateCallCounts,
					TestParticipations[typeIndex]
				};
			}
		}

		Expect(
			context,
			spawnedWorldObjectCount == PopulationClusterCount,
			"Population spawns every WorldObject cluster root");

		Expect(
			context,
			attachedObjectCount == PopulationClusterCount,
			"Population attaches an Object bridge per cluster");

		Expect(
			context,
			attachedGameObjectCount == PopulationClusterCount,
			"Population attaches a nested GameObject per cluster");

		Expect(
			context,
			attachedComponentCount == PopulationClusterCount,
			"Population attaches a Component per cluster");

		Expect(
			context,
			worldMembershipMatchCount == PopulationClusterCount,
			"Population WorldObjects join their round-robin World");

		Expect(
			context,
			transformMatchCount == PopulationClusterCount,
			"Population WorldObjects retain their initial Transform");

		Expect(
			context,
			runtimeMembershipMatchCount == PopulationClusterCount,
			"Population subtree shares the target GameRuntime");

		Expect(
			context,
			componentOwnerMatchCount == PopulationClusterCount,
			"Population Components reference their owner");

		// Square / Sprite 두 쌍이 WorldObject / Component 총계를 채우므로
		// GameObject / Object 총계를 2500으로 맞추는 여분 두 쌍은
		// World 없이 Runtime에만 참여시킵니다.
		uint32 admittedExtraPairCount = 0;

		for (uint32 extraIndex = 0; extraIndex < 2; ++extraIndex)
		{
			GameObject* extraGameObject = CreateTestRuntimeObject(extraIndex);
			Object* extraObject = Object::NewObject<Object>();

			if (extraGameObject == nullptr ||
				extraObject == nullptr)
			{
				continue;
			}

			if (runtime.Admit(*extraGameObject) &&
				extraGameObject->AttachSubObject(*extraObject) &&
				extraGameObject->GetGameRuntime() == &runtime)
			{
				++admittedExtraPairCount;
			}
		}

		Expect(
			context,
			admittedExtraPairCount == 2,
			"Population admits two world-less GameObject pairs");
	}

	runtimeExecutionVerificationPending = true;
}

void VerifyRuntimeUpdateTests()
{
	if (runtimeExecutionVerificationPending == false)
	{
		return;
	}

	for (uint32 typeIndex = 0;
		typeIndex < TestParticipationTypeCount;
		++typeIndex)
	{
		char testName[128] = {};

		std::snprintf(
			testName,
			sizeof(testName),
			"TestWorldObject combination %u executes only declared update phases",
			typeIndex);

		Expect(
			runtimeTestContext,
			MatchesExpectedUpdateCalls(
				worldObjectExecutionFixtures[typeIndex]),
			testName);

		std::snprintf(
			testName,
			sizeof(testName),
			"TestRuntimeObject combination %u executes only declared update phases",
			typeIndex);

		Expect(
			runtimeTestContext,
			MatchesExpectedUpdateCalls(
				runtimeObjectExecutionFixtures[typeIndex]),
			testName);

		std::snprintf(
			testName,
			sizeof(testName),
			"TestComponent combination %u executes only declared update phases",
			typeIndex);

		Expect(
			runtimeTestContext,
			MatchesExpectedUpdateCalls(
				componentExecutionFixtures[typeIndex]),
			testName);
	}

	char summary[128] = {};

	std::snprintf(
		summary,
		sizeof(summary),
		"[RuntimeTests] Summary PASS=%d FAIL=%d",
		runtimeTestContext.passCount,
		runtimeTestContext.failCount);

	WriteTestLine(summary);
	WriteTestLine("[RuntimeTests] End");

	runtimeExecutionVerificationPending = false;
}
