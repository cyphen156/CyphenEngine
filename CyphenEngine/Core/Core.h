#pragma once
class Core
{
	SINGLE(Core);

private:
	HWND	m_hwnd;
	HDC		m_hdc;

public:
	int Init(HWND hwnd);
	void Progress();


protected:


};

