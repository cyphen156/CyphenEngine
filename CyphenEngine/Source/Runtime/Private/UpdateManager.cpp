#include "pch.h"

#include "Runtime/Public/UpdateManager.h"
#include "Core/Public/CPrimitiveTypes.h"

UpdateManager::~UpdateManager()
{
	Clear();
}

bool UpdateManager::RegisterGlobalUpdate(const UpdateFunction& updateFunction)
{
	return Register(globalUpdateGroups, updateFunction);
}

bool UpdateManager::RegisterGlobalFinalUpdate(const UpdateFunction& updateFunction)
{
	return Register(globalFinalUpdateGroups, updateFunction);
}

bool UpdateManager::RegisterUpdate(const World& targetWorld, const UpdateFunction& updateFunction)
{
	WorldUpdateFunctionGroups* worldFunctionGroups = GetWorldFunctionGroups(targetWorld);

	if (worldFunctionGroups == nullptr)
	{
		return false;
	}

	return Register(worldFunctionGroups->updateGroups, updateFunction);
}

bool UpdateManager::RegisterFinalUpdate(const World& targetWorld, const UpdateFunction& updateFunction)
{
	WorldUpdateFunctionGroups* worldFunctionGroups = GetWorldFunctionGroups(targetWorld);

	if (worldFunctionGroups == nullptr)
	{
		return false;
	}

	return Register(worldFunctionGroups->finalUpdateGroups, updateFunction);
}

bool UpdateManager::UnregisterGlobalUpdate(const UpdateFunction& updateFunction)
{
	return Unregister(globalUpdateGroups, updateFunction);
}

bool UpdateManager::UnregisterGlobalFinalUpdate(const UpdateFunction& updateFunction)
{
	return Unregister(globalFinalUpdateGroups, updateFunction);
}

bool UpdateManager::UnregisterUpdate(const World& targetWorld, const UpdateFunction& updateFunction)
{
	WorldUpdateFunctionGroups* worldFunctionGroups = GetWorldFunctionGroups(targetWorld);

	if (worldFunctionGroups == nullptr)
	{
		return false;
	}

	return Unregister(worldFunctionGroups->updateGroups, updateFunction);
}

bool UpdateManager::UnregisterFinalUpdate(const World& targetWorld, const UpdateFunction& updateFunction)
{
	WorldUpdateFunctionGroups* worldFunctionGroups = GetWorldFunctionGroups(targetWorld);

	if (worldFunctionGroups == nullptr)
	{
		return false;
	}

	return Unregister(worldFunctionGroups->finalUpdateGroups, updateFunction);
}

const std::vector<UpdateFunctionGroup>& UpdateManager::GetGlobalUpdateGroups() const
{
	return globalUpdateGroups;
}

const std::vector<UpdateFunctionGroup>& UpdateManager::GetGlobalFinalUpdateGroups() const
{
	return globalFinalUpdateGroups;
}

const WorldUpdateFunctionGroups* UpdateManager::CreateWorldFunctionGroups(World& targetWorld)
{
	if (GetWorldFunctionGroups(targetWorld) != nullptr)
	{
		return nullptr;
	}

	WorldUpdateFunctionGroups* createdFunctionGroups = new WorldUpdateFunctionGroups();

	createdFunctionGroups->targetWorld = &targetWorld;
	worldFunctionGroups.push_back(createdFunctionGroups);

	return createdFunctionGroups;
}

bool UpdateManager::DestroyWorldFunctionGroups(World& targetWorld)
{
	std::vector<WorldUpdateFunctionGroups*>::iterator iterator;
	for (iterator = worldFunctionGroups.begin(); iterator != worldFunctionGroups.end(); ++iterator)
	{
		if (*iterator != nullptr && (*iterator)->targetWorld == &targetWorld)
		{
			break;
		}
	}

	if (iterator == worldFunctionGroups.end())
	{
		return false;
	}

	WorldUpdateFunctionGroups* destroyedFunctionGroups = *iterator;

	worldFunctionGroups.erase(iterator);
	delete destroyedFunctionGroups;

	return true;
}

void UpdateManager::Clear()
{
	globalUpdateGroups.clear();
	globalFinalUpdateGroups.clear();

	for (WorldUpdateFunctionGroups* functionGroups : worldFunctionGroups)
	{
		delete functionGroups;
	}

	worldFunctionGroups.clear();
}

bool UpdateManager::Register(std::vector<UpdateFunctionGroup>& functionGroups, const UpdateFunction& updateFunction)
{
	if (updateFunction.target == nullptr || updateFunction.execute == nullptr)
	{
		return false;
	}

	for (UpdateFunctionGroup& functionGroup : functionGroups)
	{
		if (functionGroup.execute != updateFunction.execute)
		{
			continue;
		}

		for (Object* target : functionGroup.targets)
		{
			if (target == updateFunction.target)
			{
				return false;
			}
		}

		functionGroup.targets.push_back(updateFunction.target);

		return true;
	}

	UpdateFunctionGroup functionGroup;
	functionGroup.execute = updateFunction.execute;
	functionGroup.targets.push_back(updateFunction.target);

	functionGroups.push_back(std::move(functionGroup));

	return true;
}

bool UpdateManager::Unregister(std::vector<UpdateFunctionGroup>& functionGroups, const UpdateFunction& updateFunction)
{
	if (updateFunction.target == nullptr || updateFunction.execute == nullptr)
	{
		return false;
	}

	const uint32 functionGroupCount = static_cast<uint32>(functionGroups.size());

	for (uint32 groupIndex = 0; groupIndex < functionGroupCount; ++groupIndex)
	{
		UpdateFunctionGroup& functionGroup = functionGroups[groupIndex];

		if (functionGroup.execute != updateFunction.execute)
		{
			continue;
		}

		const uint32 targetCount = static_cast<uint32>(functionGroup.targets.size());

		for (uint32 targetIndex = 0; targetIndex < targetCount; ++targetIndex)
		{
			if (functionGroup.targets[targetIndex] != updateFunction.target)
			{
				continue;
			}

			functionGroup.targets[targetIndex] = functionGroup.targets.back();

			functionGroup.targets.pop_back();

			if (functionGroup.targets.empty())
			{
				functionGroups.erase(functionGroups.begin() + groupIndex);
			}

			return true;
		}

		// 실행 함수 그룹은 있지만 대상이 없다면 이미 해제된 상태이므로 true를 반환합니다.
		return true;
	}

	// 실행 함수 그룹 자체가 없다면 해당 대상 역시 등록되어 있지 않습니다.
	// 해제 요청의 최종 상태가 이미 충족되었으므로 성공으로 처리합니다.
	return true;
}

WorldUpdateFunctionGroups* UpdateManager::GetWorldFunctionGroups(const World& targetWorld)
{
	for (WorldUpdateFunctionGroups* functionGroups : worldFunctionGroups)
	{
		if (functionGroups != nullptr && functionGroups->targetWorld == &targetWorld)
		{
			return functionGroups;
		}
	}

	return nullptr;
}
