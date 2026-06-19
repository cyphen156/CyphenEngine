#include "pch.h"

#include "HAL/Private/ModuleLoader.h"
#include "Platform/Windows/Private/WindowsString.h"

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
	modulePath.append(CTEXT(".dll"));

	std::wstring windowsModulePath;

	if (WindowsString::ToWideString(modulePath, windowsModulePath) == false)
	{
		return false;
	}

	HMODULE moduleHandle = ::LoadLibraryW(windowsModulePath.c_str());

	if (moduleHandle == nullptr)
	{
		return false;
	}

	outNativeHandle = static_cast<void*>(moduleHandle);

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

	FARPROC symbol = ::GetProcAddress(static_cast<HMODULE>(nativeHandle), symbolName);

	return reinterpret_cast<void*>(symbol);
}

bool ModuleLoader::Unload(void*& nativeHandle)
{
	if (nativeHandle == nullptr)
	{
		return false;
	}

	BOOL isUnloaded = ::FreeLibrary(static_cast<HMODULE>(nativeHandle));

	if (isUnloaded == FALSE)
	{
		return false;
	}

	nativeHandle = nullptr;

	return true;
}
