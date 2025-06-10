#pragma once
class Core
{
	SINGLE(Core)

private:
	HWND	m_hwnd;
	HDC		m_hdc;
	Status EngineStatus;

public:
	int Init(HWND hwnd);
	void Progress();
	void Pause();
	void SetEngineStatus(Status status);
	Status GetEngineStatus();

protected:


};

