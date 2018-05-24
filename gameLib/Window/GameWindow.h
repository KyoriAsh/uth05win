#pragma once
#include <windows.h>
#include "../Graphic/GLContext.h"

namespace th5w{

class CGameWindow
{
public:
	static CGameWindow* s_pCurrentGameWindow;		//window pointer used by static WndProc to call the actual window proc
	static LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);	//static WndProc entry

protected:
	DWORD m_wndStyleFull,m_wndExStyleFull;			//window style and exStyle of full screen mode
	DWORD m_wndStyleWnded,m_wndExStyleWnded;		//window style and exStyle of windowed mode
protected:
	int m_lastWindowModeX,m_lastWindowModeY;		//window position of window mode, used to restore window position
													//when switch from full screen to windowed
public:
	bool m_bActive;									//whether window is active
public:
	bool m_bFullScr;								//whether window is in fullscreen mode
	HWND m_hWnd;									//window handle
	int m_width,m_height;							//window client area size
public:
	gl::CGLContext *m_pGLContext;					//pointer to OpenGL context object of this window

public:
	bool Create(bool bCreateFullScr,int width, int height);						//create window
	void Destroy();																//destroy window
	void ToggleFullScreen();													//toggle full screen mode
	LRESULT WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);		//actual window proc
public:
	CGameWindow(void);
	~CGameWindow(void);
};

}