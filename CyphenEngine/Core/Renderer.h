#pragma once

class Renderer
{
	SINGLE(Renderer)

public:

private:
	virtual void Init(RendererType type , HWND hwnd);	// 메인 핸들을 받아서 초기화를 담당하는 함수
	

};