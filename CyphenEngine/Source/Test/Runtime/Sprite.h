#pragma once

#include "Resource/Public/Resource.h"
#include "Runtime/Public/Component.h"

class ObjectManager;

// ============================================================================
// Sprite
// ----------------------------------------------------------------------------
// Texture2D Resource를 사용하는 2D 표현 Component 예제입니다.
//
// CPU-side Texture2D 데이터는 소유하거나 복사하지 않습니다.
// Resource 업로드 이후 Renderer와 Runtime이 공유하는 ResourceId만 보관합니다.
//
// Sprite는 GameObject의 직접 SubObject로 부착할 수 있으며,
// Component의 공통 부착과 종속 수명 계약을 따릅니다.
//
// GlobalUpdate와 Update에 참여하여 Component 기반 virtual 실행 경로를
// 검증합니다.
// ============================================================================

class Sprite final : public Component
{
public:
	ResourceId GetTextureId() const;

	uint32 GetGlobalUpdateCount() const;
	uint32 GetUpdateCount() const;

protected:
	Sprite(ObjectHandle objectHandle, ResourceId textureId);
	~Sprite() override;

private:
	friend class ObjectManager;

	void GlobalUpdate(double deltaSeconds) override;
	void Update(double deltaSeconds) override;

	const ResourceId textureId;

	uint32 globalUpdateCount = 0;
	uint32 updateCount = 0;
};
