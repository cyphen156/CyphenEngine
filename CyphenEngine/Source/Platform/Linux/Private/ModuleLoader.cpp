#include "pch.h"

#include <dlfcn.h>

#include "HAL/Private/ModuleLoader.h"
#include "Platform/Linux/Private/LinuxString.h"

bool ModuleLoader::Load(const CString& moduleName, void*& outNativeHandle)
{
	if (moduleName.empty())
	{
		return false;
	}

	if (outNativeHandle != nullptr)
	{
		return false;
	}

	CString modulePath = moduleName;
	modulePath.append(CTEXT(".so"));   // Windows ".dll" 대칭 (lib 접두 없음)

	std::string linuxModulePath;

	if (LinuxString::ToUtf8String(modulePath, linuxModulePath) == false)
	{
		return false;
	}

	void* moduleHandle = ::dlopen(linuxModulePath.c_str(), RTLD_NOW | RTLD_LOCAL);

	if (moduleHandle == nullptr)
	{
		return false;
	}

	outNativeHandle = moduleHandle;

	return true;
}

void* ModuleLoader::FindSymbol(void* nativeHandle, const char* symbolName)
{
	if (nativeHandle == nullptr)
	{
		return nullptr;
	}

	if (symbolName == nullptr || symbolName[0] == '\0')
	{
		return nullptr;
	}

	return ::dlsym(nativeHandle, symbolName);
}

bool ModuleLoader::Unload(void*& nativeHandle)
{
	if (nativeHandle == nullptr)
	{
		return false;
	}

	if (::dlclose(nativeHandle) != 0)
	{
		return false;
	}

	nativeHandle = nullptr;

	return true;
}
