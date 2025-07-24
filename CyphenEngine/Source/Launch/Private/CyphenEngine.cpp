// CyphenEngine.cpp : 애플리케이션에 대한 공통 진입점을 정의합니다.
//
#include "pch.h"
#include "Launch/Public/CyphenEngine.h"
#include "Runtime/Public/Path.h"
#include "Runtime/Public/Time.h"
#include "Modules/Renderer/Public/Renderer.h"

/**
 * @ 애플리케이션 진입점
 *
 * 플랫폼 독립적으로 실행될 핵심 코드는 이곳에서 정의합니다.
 */
CyphenEngine* GEngine = nullptr;

CyphenEngine::CyphenEngine() :
	_engineStatus(EngineStatus::Initializing)
{
}
CyphenEngine::~CyphenEngine()
{

}

#if defined(PLATFORM_WINDOWS)
// Windows용 초기화 함수
bool CyphenEngine::InitEngine(HWND g_hMainWindow)  
{  
	Path::Init();
	Time::Init();
	Renderer::Initialize();

	_engineStatus = Ready;
	return true;
}  

#elif defined(PLATFORM_LINUX)
// Linux용 초기화 함수
bool CyphenEngine::InitEngine()  
{  
	_engineStatus = Ready;
	return true;  
}

#endif

void CyphenEngine::Run()
{
}

void CyphenEngine::HotReload()
{
}

// 엔진 상태가 종료되었을 때 호출될 해제 함수
void CyphenEngine::ShutdownEngine()
{
#if defined(PLATFORM_WINDOWS)

#elif defined(PLATFORM_LINUX)

#endif
}
