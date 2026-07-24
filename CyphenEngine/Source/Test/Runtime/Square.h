#pragma once

#include "Runtime/Public/WorldObject.h"

class ObjectManager;

// ============================================================================
// Square
// ----------------------------------------------------------------------------
// World에 합류할 수 있는 2D 사각형 Runtime 예제 객체입니다.
//
// WorldObject를 통해 World Transform을 가지며,
// Sprite Component를 직접 SubObject로 구성할 수 있습니다.
//
// Sprite와의 관계는 Object의 Outer/SubObject 종속 수명 계약을 사용합니다.
//
// Update와 GlobalFinalUpdate에 참여하여 GameObject 기반 virtual 실행 경로를
// 검증합니다.
// ============================================================================

class Square final : public WorldObject
{
public:
	uint32 GetUpdateCount() const;
	uint32 GetGlobalFinalUpdateCount() const;

protected:
	explicit Square(ObjectHandle objectHandle);
	~Square() override;

private:
	friend class ObjectManager;

	void Update(double deltaSeconds) override;
	void GlobalFinalUpdate(double deltaSeconds) override;

	uint32 updateCount = 0;
	uint32 globalFinalUpdateCount = 0;
};
