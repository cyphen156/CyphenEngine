#pragma once

#include "Runtime/Public/GameObject.h"
#include "Core/Math/Public/Transform.h"

class World;
class ObjectManager;

// ============================================================================
// WorldObject
// ----------------------------------------------------------------------------
// World에 합류하여 Transform을 갖는 OOP 공간 객체입니다.
//
// WorldObject는 ObjectHandle을 부여받아 먼저 생성할 수 있습니다.
// 생성 직후에는 아직 World에 합류하지 않았으며 world는 nullptr입니다.
//
// World에 합류한 WorldObject는 반드시 Transform을 가집니다.
// 아직 합류하지 않은 WorldObject는 Transform을 조회할 수 없습니다.
//
// Destroy는 현재 World에 합류한 상태라면 먼저 Leave를 요청합니다.
// 이후 GameObject의 Runtime 소속 해제와 공통 Object 파괴 요청 경로에 합류합니다.
//
// World membership과 Update 참여 선언은 별개의 관계입니다.
// World에 합류한 WorldObject는 World-local Update / FinalUpdate의
// 실행 anchor입니다.
// World::Join은 자신을 anchor로 하는 실행 범위의 참여를 등록하고,
// 하위 WorldObject 가지는 각자의 anchor 범위에서 처리합니다.
//
// WorldObject는 소속 World를 비소유 포인터로 저장합니다.
// World의 수명을 소유하거나 연장하지 않습니다.
// ============================================================================

class WorldObject : public GameObject
{
public:
	Transform GetTransform() const;
	
	World* GetWorld();
	const World* GetWorld() const;

protected:
	explicit WorldObject(ObjectHandle objectHandle);
	WorldObject(ObjectHandle objectHandle, UpdateParticipation updateParticipation);
	~WorldObject() override;

	bool IsUpdateExecutionAnchor(UpdateParticipation participation) const override;
	
	bool OnDestroy() override;

private:
	friend class ObjectManager;
	friend class World;

	World* world = nullptr;
};
