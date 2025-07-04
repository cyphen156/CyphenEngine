#pragma once
#include <string>

class Path
{
private:
    static TSTRING _projectRoot;
    static TSTRING _currentDirectory;

public:
	static void Init();
    static TSTRING Root();
    static TSTRING Content();
    static TSTRING Saved();
    static TSTRING Logs();
    static TSTRING Config();
    static TSTRING Combine(const TSTRING& base, const TSTRING& relative);
    static void SetCurrentDir(const TSTRING& currentDirectory);
    static const TSTRING& GetCurrentDir() { return _currentDirectory; }
};

