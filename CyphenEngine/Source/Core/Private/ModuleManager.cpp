#include "pch.h"

#include <map>
#include <set>
#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/ModuleManager.h"
#include "HAL/Private/ModuleLoader.h"

// ============================================================================
// ModuleRecord
// ----------------------------------------------------------------------------
// 현재 프로세스에 실제로 적재된 Module 구현 Binary의 상태입니다.
//
// Descriptor가 갱신되어도 이미 적재된 구현의 이름과 Binary는 Record에
// 유지됩니다. 기존 참조가 모두 해제된 뒤 다음 Acquire에서 새 Descriptor의
// 구현이 적재됩니다.
// ============================================================================

struct ModuleRecord
{
	CString implementationName;
	CString binaryName;
	void* nativeHandle = nullptr;
	uint32 referenceCount = 0;
};

static std::map<CString, ModuleDescriptor> gModuleDescriptors;
static std::map<CString, ModuleRecord> gModuleRecords;
static std::vector<CString> gModuleLoadOrder;

bool ModuleManager::Refresh(const std::vector<ModuleDescriptor>& moduleDescriptors)
{
	std::map<CString, ModuleDescriptor> refreshedDescriptors;
	std::set<CString> discoveredModuleNames;
	std::set<CString> rejectedModuleNames;

	bool isRefreshSuccessful = true;

	for (const ModuleDescriptor& moduleDescriptor : moduleDescriptors)
	{
		if (moduleDescriptor.moduleName.empty())
		{
			isRefreshSuccessful = false;
			continue;
		}

		auto discoveredResult =
			discoveredModuleNames.emplace(moduleDescriptor.moduleName);

		if (discoveredResult.second == false)
		{
			refreshedDescriptors.erase(moduleDescriptor.moduleName);
			rejectedModuleNames.emplace(moduleDescriptor.moduleName);
			isRefreshSuccessful = false;
			continue;
		}

		bool isDescriptorValid = true;

		if (moduleDescriptor.isEnabled)
		{
			isDescriptorValid =
				moduleDescriptor.implementationName.empty() == false &&
				moduleDescriptor.binaryName.empty() == false;
		}

		if (isDescriptorValid == false)
		{
			rejectedModuleNames.emplace(moduleDescriptor.moduleName);
			isRefreshSuccessful = false;
			continue;
		}

		refreshedDescriptors.emplace(
			moduleDescriptor.moduleName,
			moduleDescriptor);
	}

	for (const CString& rejectedModuleName : rejectedModuleNames)
	{
		refreshedDescriptors.erase(rejectedModuleName);
	}

	// Refresh는 Desired 상태만 교체합니다.
	// 실제 Load/Unload와 실행 중 구현 교체는 수행하지 않습니다.
	gModuleDescriptors = refreshedDescriptors;

	return isRefreshSuccessful;
}

bool ModuleManager::Shutdown()
{
	if (UnloadAll() == false)
	{
		return false;
	}

	gModuleDescriptors.clear();

	return true;
}

bool ModuleManager::Acquire(const CString& moduleName)
{
	if (moduleName.empty())
	{
		return false;
	}

	auto descriptorIterator = gModuleDescriptors.find(moduleName);

	if (descriptorIterator == gModuleDescriptors.end())
	{
		return false;
	}

	const ModuleDescriptor& moduleDescriptor = descriptorIterator->second;

	if (moduleDescriptor.isEnabled == false ||
		moduleDescriptor.implementationName.empty() ||
		moduleDescriptor.binaryName.empty())
	{
		return false;
	}

	auto moduleIterator = gModuleRecords.find(moduleName);

	if (moduleIterator != gModuleRecords.end())
	{
		ModuleRecord& moduleRecord = moduleIterator->second;

		// 실행 중 구현을 Refresh가 암묵적으로 교체하지 않습니다.
		// 기존 참조가 모두 해제된 뒤 다음 Acquire에서 새 구현을 적재합니다.
		if (moduleRecord.implementationName != moduleDescriptor.implementationName ||
			moduleRecord.binaryName != moduleDescriptor.binaryName)
		{
			return false;
		}

		++moduleRecord.referenceCount;

		return true;
	}

	void* nativeHandle = nullptr;

	if (ModuleLoader::Load(moduleDescriptor.binaryName, nativeHandle) == false)
	{
		return false;
	}

	ModuleRecord moduleRecord;
	moduleRecord.implementationName = moduleDescriptor.implementationName;
	moduleRecord.binaryName = moduleDescriptor.binaryName;
	moduleRecord.nativeHandle = nativeHandle;
	moduleRecord.referenceCount = 1;

	auto insertResult = gModuleRecords.emplace(moduleName, moduleRecord);

	if (insertResult.second == false)
	{
		ModuleLoader::Unload(nativeHandle);
		return false;
	}

	gModuleLoadOrder.push_back(moduleName);

	return true;
}

bool ModuleManager::Release(const CString& moduleName)
{
	if (moduleName.empty())
	{
		return false;
	}

	auto moduleIterator = gModuleRecords.find(moduleName);

	if (moduleIterator == gModuleRecords.end())
	{
		return false;
	}

	ModuleRecord& moduleRecord = moduleIterator->second;

	if (moduleRecord.referenceCount == 0)
	{
		return false;
	}

	if (moduleRecord.referenceCount > 1)
	{
		--moduleRecord.referenceCount;
		return true;
	}

	// 마지막 참조에서만 실제 Binary를 Unload합니다.
	return RemoveModuleRecord(moduleName);
}

ModuleSymbol ModuleManager::FindSymbol(const CString& moduleName, const char* symbolName)
{
	if (moduleName.empty())
	{
		return nullptr;
	}

	if (symbolName == nullptr || symbolName[0] == '\0')
	{
		return nullptr;
	}

	auto moduleIterator = gModuleRecords.find(moduleName);

	if (moduleIterator == gModuleRecords.end())
	{
		return nullptr;
	}

	return ModuleLoader::FindSymbol(
		moduleIterator->second.nativeHandle,
		symbolName);
}

bool ModuleManager::IsLoaded(const CString& moduleName)
{
	if (moduleName.empty())
	{
		return false;
	}

	return gModuleRecords.find(moduleName) != gModuleRecords.end();
}

void ModuleManager::GetLoadedModuleNames(std::vector<CString>& outModuleNames)
{
	outModuleNames = gModuleLoadOrder;
}

bool ModuleManager::RemoveModuleRecord(const CString& moduleName)
{
	auto moduleIterator = gModuleRecords.find(moduleName);

	if (moduleIterator == gModuleRecords.end())
	{
		return false;
	}

	if (ModuleLoader::Unload(moduleIterator->second.nativeHandle) == false)
	{
		return false;
	}

	gModuleRecords.erase(moduleIterator);

	for (auto loadOrderIterator = gModuleLoadOrder.begin();
		loadOrderIterator != gModuleLoadOrder.end();
		++loadOrderIterator)
	{
		if (*loadOrderIterator != moduleName)
		{
			continue;
		}

		gModuleLoadOrder.erase(loadOrderIterator);
		break;
	}

	return true;
}

bool ModuleManager::UnloadAll()
{
	std::vector<CString> loadedModuleNames = gModuleLoadOrder;
	bool isAllUnloaded = true;

	// Module 간 적재 순서의 역순으로 종료합니다.
	for (auto moduleIterator = loadedModuleNames.rbegin();
		moduleIterator != loadedModuleNames.rend();
		++moduleIterator)
	{
		if (RemoveModuleRecord(*moduleIterator) == false)
		{
			isAllUnloaded = false;
		}
	}

	return isAllUnloaded;
}
