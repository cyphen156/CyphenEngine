#include "pch.h"

#include <map>
#include <vector>

#include "Core/Public/ModuleManager.h"
#include "HAL/Private/ModuleLoader.h"

struct ModuleRecord
{
	void* nativeHandle = nullptr;
};

static std::map<CString, ModuleDescriptor> gModuleDescriptors;
static std::map<CString, ModuleRecord> gModuleRecords;
static std::vector<CString> gModuleLoadOrder;

bool ModuleManager::Refresh(const std::vector<ModuleDescriptor>& moduleDescriptors)
{
	std::map<CString, ModuleDescriptor> refreshedDescriptors;
	bool isRefreshSuccessful = true;

	for (const ModuleDescriptor& moduleDescriptor : moduleDescriptors)
	{
		if (moduleDescriptor.moduleName.empty())
		{
			isRefreshSuccessful = false;
			continue;
		}

		auto insertResult =
			refreshedDescriptors.emplace(moduleDescriptor.moduleName, moduleDescriptor);

		if (insertResult.second == false)
		{
			isRefreshSuccessful = false;
		}
	}

	std::vector<CString> loadedModuleNames = gModuleLoadOrder;

	for (auto moduleIterator = loadedModuleNames.rbegin();
		moduleIterator != loadedModuleNames.rend();
		++moduleIterator)
	{
		auto descriptorIterator = refreshedDescriptors.find(*moduleIterator);

		bool shouldUnload =
			descriptorIterator == refreshedDescriptors.end() ||
			descriptorIterator->second.isEnabled == false;

		if (shouldUnload == false)
		{
			continue;
		}

		if (Unload(*moduleIterator) == false)
		{
			isRefreshSuccessful = false;
		}
	}

	gModuleDescriptors = refreshedDescriptors;

	for (const ModuleDescriptor& moduleDescriptor : moduleDescriptors)
	{
		auto descriptorIterator =
			gModuleDescriptors.find(moduleDescriptor.moduleName);

		if (descriptorIterator == gModuleDescriptors.end())
		{
			continue;
		}

		if (descriptorIterator->second.isEnabled == false)
		{
			continue;
		}

		if (IsLoaded(moduleDescriptor.moduleName) == true)
		{
			continue;
		}

		if (Load(moduleDescriptor.moduleName) == false)
		{
			isRefreshSuccessful = false;
		}
	}

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

bool ModuleManager::Load(const CString& moduleName)
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

	if (descriptorIterator->second.isEnabled == false)
	{
		return false;
	}

	if (gModuleRecords.find(moduleName) != gModuleRecords.end())
	{
		return false;
	}

	void* nativeHandle = nullptr;

	if (ModuleLoader::Load(moduleName, nativeHandle) == false)
	{
		return false;
	}

	ModuleRecord moduleRecord;
	moduleRecord.nativeHandle = nativeHandle;

	auto insertResult = gModuleRecords.emplace(moduleName, moduleRecord);

	if (insertResult.second == false)
	{
		ModuleLoader::Unload(nativeHandle);
		return false;
	}

	gModuleLoadOrder.push_back(moduleName);

	return true;
}

bool ModuleManager::Unload(const CString& moduleName)
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

	return ModuleLoader::FindSymbol(moduleIterator->second.nativeHandle, symbolName);
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

bool ModuleManager::UnloadAll()
{
	std::vector<CString> loadedModuleNames = gModuleLoadOrder;
	bool isAllUnloaded = true;

	for (auto moduleIterator = loadedModuleNames.rbegin();
		moduleIterator != loadedModuleNames.rend();
		++moduleIterator)
	{
		if (Unload(*moduleIterator) == false)
		{
			isAllUnloaded = false;
		}
	}

	return isAllUnloaded;
}
