// CyphenEngine.cpp : 애플리케이션에 대한 공통 진입점을 정의합니다.
//
#include "pch.h"
#include "Launch/Public/CyphenEngine.h"

/**
 * @ 애플리케이션 진입점
 *
 * 플랫폼 독립적으로 실행될 핵심 코드는 이곳에서 정의합니다.
 */
CyphenEngine* GEngine = nullptr;

#if defined(PLATFORM_WINDOWS)
CyphenEngine::CyphenEngine() : 
	_engineStatus(EngineStatus::Initializing)
{
}
CyphenEngine::~CyphenEngine()
{

}
// Windows용 초기화 함수
bool CyphenEngine::InitEngine(HWND g_hMainWindow)  
{  
	_engineStatus = Ready;
	return true;
}  
#elif defined(PLATFORM_LINUX)
// Linux용 초기화 함수
bool CyphenEngine::InitEngine()  
{  
	return true;  
}  
#endif

void CyphenEngine::Run()
{
}

void CyphenEngine::HotReload()
{
}

void CyphenEngine::Shutdown()
{
	_engineStatus = EngineStatus::Terminated;
}
