#include ".\gamewindow.h"
#include "../Graphic/include/glew.h"
#include "../../Game/Game.h"
#include <gl/gl.h>
#include <tchar.h>
#include <stdio.h>

namespace th5w{

CGameWindow* CGameWindow::s_pCurrentGameWindow=NULL;

CGameWindow::CGameWindow(void)
{
	m_lastWindowModeX=0;
	m_lastWindowModeY=0;
	m_bFullScr=false;
	m_hWnd=NULL;
	m_pGLContext=NULL;
	m_bActive=true;

	m_wndStyleFull=WS_POPUP;
	m_wndExStyleFull=WS_EX_APPWINDOW;
	m_wndStyleWnded=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
	m_wndExStyleWnded=WS_EX_APPWINDOW|WS_EX_WINDOWEDGE;
}

CGameWindow::~CGameWindow(void)
{
}

LRESULT CALLBACK CGameWindow::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return s_pCurrentGameWindow->WindowProc(hWnd,uMsg,wParam,lParam); 
}

LRESULT CGameWindow::WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_PAINT:
		if (m_bActive==false)
			CGame::s_pCurGame->OnPaintInactivate();
		break;
	case WM_ACTIVATE:								// Watch For Window Activate Message
		{
			if (LOWORD(wParam)==WA_INACTIVE)
			{
				if (m_bFullScr==true)
				{
					ShowWindow(m_hWnd,SW_MINIMIZE);
					ChangeDisplaySettings(NULL,0);
				}
				m_bActive=false;					// Program Is No Longer Active
				CGame::s_pCurGame->OnDeactivate();
			}
			else
			{
				if (m_bFullScr==true)
				{
					ShowWindow(m_hWnd,SW_SHOW);
					DEVMODE dmScreenSettings;								// Device Mode
					memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
					dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
					dmScreenSettings.dmPelsWidth	= m_width;				// Selected Screen Width
					dmScreenSettings.dmPelsHeight	= m_height;				// Selected Screen Height
					dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
					dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

					if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
						ShowWindow(m_hWnd,SW_MINIMIZE);

				}
				m_bActive=true;						// Program Is Active
				CGame::s_pCurGame->OnActivate();
			}

			return 0;								// Return To The Message Loop
		}

	case WM_SYSCOMMAND:								// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
			case SC_SCREENSAVE:						// Screensaver Trying To Start?
			case SC_MONITORPOWER:					// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case 0:
				ToggleFullScreen();
				break;
			}
			break;
		}

	case WM_CLOSE:									// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

bool CGameWindow::Create(bool bCreateFullScr,int width, int height)
{
	s_pCurrentGameWindow=this;

	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	m_bFullScr=bCreateFullScr;
	m_width=width;
	m_height=height;

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) CGameWindow::WndProc;		// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= GetModuleHandle(NULL);				// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= _T("MyGameWindowClass");				// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
		return false;

	if (bCreateFullScr)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			return false;
	}

	if (bCreateFullScr)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=m_wndExStyleFull;									// Window Extended Style
		dwStyle=m_wndStyleFull;											// Windows Style
		ShowCursor(FALSE);											// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=m_wndExStyleWnded;								// Window Extended Style
		dwStyle=m_wndStyleWnded;									// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(m_hWnd=CreateWindowEx(dwExStyle,							// Extended Style For The Window
								_T("MyGameWindowClass"),			// Class Name
								_T("Game"),							// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								GetModuleHandle(NULL),				// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		if (bCreateFullScr)
		{
			ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
			ShowCursor(TRUE);								// Show Mouse Pointer
		}
		return false;								// Return FALSE
	}

	if (m_pGLContext!=NULL)
		delete m_pGLContext;
	m_pGLContext=new gl::CGLContext;
	if (m_pGLContext->Init(m_hWnd)==false)
		return false;

	m_pGLContext->MakeCurrent();
	glViewport(0,0,m_width,m_height);

	//ShowWindow(m_hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(m_hWnd);						// Slightly Higher Priority
	SetFocus(m_hWnd);									// Sets Keyboard Focus To The Window

	return true;
}

void CGameWindow::Destroy()
{
	if (m_hWnd!=NULL)
		DestroyWindow(m_hWnd);
	m_hWnd=NULL;
	UnregisterClass(_T("MyGameWindowClass"),GetModuleHandle(NULL));
}

void CGameWindow::ToggleFullScreen()
{
	if (m_bFullScr==false)
	{
		RECT windowRect;
		GetWindowRect(m_hWnd,&windowRect);
		m_lastWindowModeX=windowRect.left;
		m_lastWindowModeY=windowRect.top;

		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= m_width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= m_height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			return;
		SetWindowLong(m_hWnd,GWL_STYLE,m_wndStyleFull);
		SetWindowLong(m_hWnd,GWL_EXSTYLE,m_wndExStyleFull);
		SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
		ShowCursor(FALSE);
		m_bFullScr=true;
	}
	else
	{
		if (ChangeDisplaySettings(NULL,0)!=DISP_CHANGE_SUCCESSFUL)
			return;

		SetWindowPos(m_hWnd,HWND_NOTOPMOST,m_lastWindowModeX,m_lastWindowModeY,640,480,SWP_SHOWWINDOW);
		SetWindowLong(m_hWnd,GWL_STYLE,m_wndStyleWnded);
		SetWindowLong(m_hWnd,GWL_EXSTYLE,m_wndExStyleWnded);
		RECT WindowRect;					// Grabs Rectangle Upper Left / Lower Right Values
		WindowRect.left=(long)0;			// Set Left Value To 0
		WindowRect.right=(long)m_width;		// Set Right Value To Requested Width
		WindowRect.top=(long)0;				// Set Top Value To 0
		WindowRect.bottom=(long)m_height;		// Set Bottom Value To Requested Height
		AdjustWindowRectEx(&WindowRect, m_wndStyleWnded, FALSE, m_wndExStyleWnded);

		SetWindowPos(m_hWnd,HWND_NOTOPMOST,m_lastWindowModeX,m_lastWindowModeY,
					 WindowRect.right-WindowRect.left,WindowRect.bottom-WindowRect.top,SWP_SHOWWINDOW);
		ShowCursor(TRUE);
		m_bFullScr=false;
	}
}
}
























