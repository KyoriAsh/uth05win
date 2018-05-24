#include ".\glcontext.h"
#include <stdio.h>
#include <windows.h>

namespace th5w{
	namespace gl{

CGLContext::CGLContext(void)
{
	m_hWnd=NULL;
	m_hDC=NULL;
	m_hRC=NULL;
}

CGLContext::~CGLContext(void)
{
	if (m_hRC)
	{
		::wglDeleteContext(m_hRC);
		::ReleaseDC(m_hWnd,m_hDC);
	}
}

bool CGLContext::Init(HWND hWnd)
{
	HDC lastDC=::wglGetCurrentDC();
	HGLRC lastRC=::wglGetCurrentContext();
	if (m_hRC!=NULL)
		return false;

	HDC hDC=::GetDC(hWnd);
	if (hDC==NULL)
	{
		printf("[CGLContext::Init] error getting hDC from hWnd.\n");
		return false;
	}


	static PIXELFORMATDESCRIPTOR pfd= 
	{
		sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd
			1,                                // version number
			PFD_DRAW_TO_WINDOW |              // support window
			PFD_SUPPORT_OPENGL |              // support OpenGL
			PFD_DOUBLEBUFFER,                 // double buffered
			PFD_TYPE_RGBA,                    // RGBA type
			24,                               // 24-bit color depth
			0, 0, 0, 0, 0, 0,                 // color bits ignored
			8,                                // 8-bit alpha buffer
			0,                                // shift bit ignored
			0,                                // no accumulation buffer
			0, 0, 0, 0,                       // accumulation bits ignored
			32,                               // 32-bit z-buffer
			8,                                // 8-bit stencil buffer
			0,                                // no auxiliary buffer
			PFD_MAIN_PLANE,                   // main layer
			0,                                // reserved
			0, 0, 0                           // layer masks ignored
	};

	int pixelformat=::ChoosePixelFormat(hDC,&pfd);

	if (pixelformat==0)
	{
		printf("[CGLContext::Init] error finding proper pixel format.\n");
		::ReleaseDC(hWnd,hDC);
		return false;
	}

	if (::SetPixelFormat(hDC,pixelformat,&pfd)==FALSE)
	{
		printf("[CGLContext::Init] error setting DC's pixel format to %d.\n",pixelformat);
		::ReleaseDC(hWnd,hDC);
		return false;
	}

	//Create Rendering Context
	HGLRC hRC=::wglCreateContext(hDC);
	if (hRC==NULL)
	{
		printf("[CGLContext::Init] error creating rendering context.\n");
		::ReleaseDC(hWnd,hDC);
		return false;
	}

	//Make the RC Current
	if(::wglMakeCurrent(hDC,hRC)==FALSE)
	{
		printf("[CGLContext::Init] error making the rendering context current.\n");
		::wglDeleteContext(hRC);
		::ReleaseDC(hWnd,hDC);
		return false;
	}

	//Init extension lib
	glewInit();

	// Usual OpenGL stuff
	glShadeModel(GL_SMOOTH);
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glReadBuffer(GL_BACK_LEFT);

	::wglMakeCurrent(lastDC,lastRC);

	m_hWnd=hWnd;
	m_hDC=hDC;
	m_hRC=hRC;

	return true;
}

bool CGLContext::MakeCurrent()
{
	if (m_hRC==NULL)
	{
		printf("[CGLContext::MakeCurrent] error. Context not created yet.\n");
		return false;
	}

	if (::wglMakeCurrent(m_hDC,m_hRC)==FALSE)
	{
		printf("[CGLContext::MakeCurrent] error. Cannot make the context current.\n");
		return false;
	}

	return true;
}

bool CGLContext::SwapBuffers()
{
	if (m_hRC==NULL)
	{
		printf("[CGLContext::SwapBuffers] error. Context not created yet.\n");
		return false;
	}

	::SwapBuffers(m_hDC);
	return true;
}
	}
}