#include "pch.h"

#include "Modules/Public/ModuleBinder.h"

bool ModuleBinder::Bind(const CString& moduleName)
{
	if (moduleName.empty())
	{
		return false;
	}

	if (IsBound())
	{
		return boundModuleName == moduleName;
	}

	if (ModuleManager::Acquire(moduleName) == false)
	{
		return false;
	}

	boundModuleName = moduleName;

	return true;
}

bool ModuleBinder::Release()
{
	if (IsBound() == false)
	{
		return true;
	}

	if (ModuleManager::Release(boundModuleName) == false)
	{
		return false;
	}

	boundModuleName.clear();

	return true;
}

ModuleSymbol ModuleBinder::FindSymbol(const char* symbolName) const
{
	if (IsBound() == false)
	{
		return nullptr;
	}

	return ModuleManager::FindSymbol(boundModuleName, symbolName);
}

bool ModuleBinder::IsBound() const
{
	return boundModuleName.empty() == false;
}
