#pragma once

#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
//#pragma comment(lib,"dinput.lib")
#pragma comment(lib,"dxguid.lib")

namespace th5w{

class CKeyInputDI
{
protected:
	LPDIRECTINPUT8 m_pDInput8;
	LPDIRECTINPUTDEVICE8  m_pDInputKeyboardDevice; 
	BYTE m_keyState[256];

public:
	bool Initialize();
	bool UpdateKeyState();
	__forceinline unsigned char GetKeyState(int keyScanCode) const
	{
		return m_keyState[keyScanCode]>>7;
	}
public:
	CKeyInputDI(void);
	~CKeyInputDI(void);
};

}
