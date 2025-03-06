#include "pch.h"
#include <typeinfo>
#include "Core.h"
#include "InputMgr.h"

Core::Core()
	: m_hwnd()
	, m_hdc()
{
	SetEngineStatus(Status::Running);
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

	InputMgr::GetInstance();
	return S_OK;
}

void Core::Progress()
{
}

void Core::Pause()
{
	
}

Status Core::GetEngineStatus()
{
	return EngineStatus;
}

void Core::SetEngineStatus(Status status)
{
	EngineStatus = status;
}
