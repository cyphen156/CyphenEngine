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
				&runtime,
				"Detached Component hierarchy preserves nested Runtime membership");

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
				&runtime,
				"Detached Component hierarchy preserves nested GameObject Runtime");
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
