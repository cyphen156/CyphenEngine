#include "pch.h"
#include "Launch/Public/Path.h"

TSTRING Path::_projectRoot = TSLASH_STR;
TSTRING Path::_currentDirectory = TSLASH_STR;

void Path::Init()
{
    bool bSuccess = false;

#if defined(PLATFORM_WINDOWS)
    TCHAR rootBuffer[255] = { 0 };
    DWORD length = ::GetCurrentDirectory(255, rootBuffer);
    if (length > 0 && length < 255)
    {
        _projectRoot = rootBuffer;
        _currentDirectory = _projectRoot;
        bSuccess = true;
    }

#elif defined(PLATFORM_LINUX)
    char cwd[PATH_MAX] = { 0 };
    if (getcwd(cwd, sizeof(cwd)) != nullptr)
    {
        _projectRoot = cwd;
        _currentDirectory = _projectRoot;
        bSuccess = true;
    }
#endif

    if (!bSuccess)
    {
        _projectRoot = TSLASH_STR;
        _currentDirectory = TSLASH_STR;
    }
}

TSTRING Path::Root() 
{
    return _projectRoot; 
}

TSTRING Path::Content()
{
    return _projectRoot + TTEXT("Content") + TSLASH_STR;
}

TSTRING Path::Saved()
{
    return _projectRoot + TTEXT("Saved") + TSLASH_STR;
}

TSTRING Path::Logs()
{
    return _projectRoot + TTEXT("Logs") + TSLASH_STR;
}

TSTRING Path::Config()
{
    return _projectRoot + TTEXT("Config") + TSLASH_STR;
}

TSTRING Path::Combine(const TSTRING& base, const TSTRING& relative)
{
    return base + TSLASH_STR + relative;
}

void Path::SetCurrentDir(const TSTRING& currentDirectory)
{
    _currentDirectory = currentDirectory;
}
