#pragma once

#include <vector>

#include "Runtime/Public/UpdateFunction.h"

class World;

// ============================================================================
// UpdateFunctionGroup
// ----------------------------------------------------------------------------
// 동일한 execute를 사용하는 Object 실행 대상들을 하나의 그룹으로 관리합니다.
//
// UpdateFunction은 등록과 해제 요청에 사용하는 POD 전달값입니다.
// UpdateFunctionGroup은 std::vector의 수명과 그룹 불변식을 소유하는
// 비-POD 관리 객체입니다.
//
// Runtime과 World는 UpdateManager가 제공한 const 그룹 목록을 읽어
// 자신의 실행 단계에서 직접 호출합니다.
// ============================================================================

struct UpdateFunctionGroup
{
	UpdateExecuteFunction execute = nullptr;
	std::vector<Object*> targets;
};

// ============================================================================
// WorldUpdateFunctionGroups
// ----------------------------------------------------------------------------
// 하나의 World에 귀속되는 World-local 실행 목록입니다.
//
// 이 객체의 수명은 GameRuntime의 UpdateManager가 소유합니다.
// targetWorld는 이 목록이 귀속된 World를 식별하는 비소유 포인터입니다.
//
// World는 생성 시 UpdateManager가 반환한 안정된 const 포인터를 보유하고
// Update와 FinalUpdate 실행에 사용합니다.
// ============================================================================

struct WorldUpdateFunctionGroups
{
	const World* targetWorld = nullptr;

	std::vector<UpdateFunctionGroup> updateGroups;
	std::vector<UpdateFunctionGroup> finalUpdateGroups;
};

// ============================================================================
// UpdateManager
// ----------------------------------------------------------------------------
// 하나의 GameRuntime에 귀속되는 모든 Update Function Group을 관리합니다.
//
// GameRuntime마다 UpdateManager 하나를 생성합니다.
// Runtime-global 실행 목록과 각 World의 World-local 실행 목록을 같은
// Runtime 세계선 안에서 관리합니다.
//
// UpdateManager는 GameRuntime 타입을 알지 않습니다.
// World는 World-local 실행 목록을 구분하는 직접적인 관리 키로 사용합니다.
// World는 생성 시 World-local 그룹을 요청하고,
// 반환된 그룹을 자신의 실행 목록으로 연결합니다.
//
// UpdateManager는 실행하지 않습니다.
// GameRuntime과 World가 각자 자신에게 귀속된 읽기 전용 그룹을 순회합니다.
//
// Scheduler와 Execution Priority는 현재 구현하지 않습니다.
// 우선순위 계약이 확정되면 UpdateManager가 소유한 그룹 벡터를 Scheduler에
// 전달하여 실행 순서를 재구성합니다.
// ============================================================================

class UpdateManager final
{
public:
	UpdateManager() = default;
	~UpdateManager();

	bool RegisterGlobalUpdate(const UpdateFunction& updateFunction);
	bool RegisterGlobalFinalUpdate(const UpdateFunction& updateFunction);
	bool RegisterUpdate(const World& targetWorld, const UpdateFunction& updateFunction);
	bool RegisterFinalUpdate(const World& targetWorld, const UpdateFunction& updateFunction);

	bool UnregisterGlobalUpdate(const UpdateFunction& updateFunction);
	bool UnregisterGlobalFinalUpdate(const UpdateFunction& updateFunction);
	bool UnregisterUpdate(const World& targetWorld, const UpdateFunction& updateFunction);
	bool UnregisterFinalUpdate(const World& targetWorld, const UpdateFunction& updateFunction);

	const std::vector<UpdateFunctionGroup>& GetGlobalUpdateGroups() const;
	const std::vector<UpdateFunctionGroup>& GetGlobalFinalUpdateGroups() const;

	const WorldUpdateFunctionGroups* CreateWorldFunctionGroups(World& targetWorld);
	bool DestroyWorldFunctionGroups(World& targetWorld);

	void Clear();

private:
	UpdateManager(const UpdateManager&) = delete;
	UpdateManager& operator=(const UpdateManager&) = delete;
	UpdateManager(UpdateManager&&) = delete;
	UpdateManager& operator=(UpdateManager&&) = delete;

	bool Register(std::vector<UpdateFunctionGroup>& functionGroups, const UpdateFunction& updateFunction);
	bool Unregister(std::vector<UpdateFunctionGroup>& functionGroups, const UpdateFunction& updateFunction);
	WorldUpdateFunctionGroups* GetWorldFunctionGroups(const World& targetWorld);

	std::vector<UpdateFunctionGroup> globalUpdateGroups;
	std::vector<UpdateFunctionGroup> globalFinalUpdateGroups;

	std::vector<WorldUpdateFunctionGroups*> worldFunctionGroups;
};
