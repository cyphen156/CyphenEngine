#include "pch.h"
#include "Core.h"

Core::Core()
	: m_hwnd()
	, m_hdc()
{ };

Core::~Core()
{

}

int Core::Init(HWND hwnd)
{
	m_hwnd = hwnd;
	
	if ()
	{
		return E_FAIL;
	}
}

void Core::Progress()
{
}
