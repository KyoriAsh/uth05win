#pragma once

#include "mygl.h"

namespace th5w{
	namespace gl{

class CGLContext
{
protected:
	HWND m_hWnd;
	HDC m_hDC;
	HGLRC m_hRC;

public:
	bool Init(HWND hWnd);
	bool MakeCurrent();
	bool SwapBuffers();
public:
	CGLContext(void);
	~CGLContext(void);
};

	}
}