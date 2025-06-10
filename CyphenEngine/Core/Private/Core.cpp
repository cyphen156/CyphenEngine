#include "pch.h"
#include <typeinfo>
#include "Core/Public/Core.h"
#include "Core/Public/Input.h"

Core::Core()
	: m_hwnd()
	, m_hdc()
{
	SetEngineStatus(EngineStatus::None);
};

Core::~Core()
{

}

int Core::Init(HWND hwnd)
{
	m_hwnd = hwnd;
	
	if (!hwnd)
	{
		// 메인 윈도우 못받아왔음
		return E_FAIL;
	}

	Input::GetInstance();
	return S_OK;
}

void Core::Progress()
{
}

void Core::Pause()
{
	
}

EngineStatus Core::GetEngineStatus()
{
	return EngineStatus;
}

void Core::SetEngineStatus(EngineStatus status)
{
	EngineStatus = status;
}
