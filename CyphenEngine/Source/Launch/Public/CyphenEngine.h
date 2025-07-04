#pragma once


class CyphenEngine
{
public:
	EngineStatus _engineStatus;
	
	CyphenEngine();
	~CyphenEngine();

#if defined(PLATFORM_WINDOWS)
	bool InitEngine(HWND g_hMainWindow);
#elif defined(PLATFORM_LINUX)
	bool InitEngine();
#endif
	
	void Run();
	void HotReload();
	void Shutdown();
};

extern CyphenEngine* GEngine;
