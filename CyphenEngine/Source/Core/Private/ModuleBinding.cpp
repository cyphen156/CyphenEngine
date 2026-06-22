#include "pch.h"

#include "Core/Public/ModuleBinding.h"

bool ModuleBinding::Bind(const CString& moduleName)
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

bool ModuleBinding::Release()
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

ModuleSymbol ModuleBinding::FindSymbol(const char* symbolName) const
{
	if (IsBound() == false)
	{
		return nullptr;
	}

	return ModuleManager::FindSymbol(boundModuleName, symbolName);
}

bool ModuleBinding::IsBound() const
{
	return boundModuleName.empty() == false;
}
