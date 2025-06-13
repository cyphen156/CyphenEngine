#pragma once
class Core
{
	SINGLE(Core)

private:
	HWND	m_hwnd;
	HDC		m_hdc;
	EngineStatus engineStatus;

public:
	int Init(HWND hwnd);
	void Progress();
	void Pause();
	void SetEngineStatus(EngineStatus status);
	EngineStatus GetEngineStatus();

protected:


};

