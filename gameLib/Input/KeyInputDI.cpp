#include ".\keyinputdi.h"

namespace th5w{

CKeyInputDI::CKeyInputDI(void)
{
	m_pDInput8=NULL;
	m_pDInputKeyboardDevice=NULL;
}

CKeyInputDI::~CKeyInputDI(void)
{
	if (m_pDInputKeyboardDevice!=NULL)
	{
		m_pDInputKeyboardDevice->Release();
		m_pDInputKeyboardDevice=NULL;
	}
	if (m_pDInput8!=NULL)
	{
		m_pDInput8->Release();
		m_pDInput8=NULL;
	}

}

bool CKeyInputDI::Initialize()
{
	if (m_pDInput8!=NULL)
		return false;

	if (DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(LPVOID *)&m_pDInput8,NULL)!=DI_OK)
		return false;

	if (m_pDInput8->CreateDevice(GUID_SysKeyboard, &m_pDInputKeyboardDevice, NULL)!=DI_OK)
	{
		m_pDInput8->Release();
		m_pDInput8=NULL;
		return false;
	}

	if (m_pDInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
	{
		m_pDInputKeyboardDevice->Release();
		m_pDInputKeyboardDevice=NULL;
		m_pDInput8->Release();
		m_pDInput8=NULL;
		return false;
	}

	if (m_pDInputKeyboardDevice->Acquire()!=DI_OK)
	{
		m_pDInputKeyboardDevice->Release();
		m_pDInputKeyboardDevice=NULL;
		m_pDInput8->Release();
		m_pDInput8=NULL;
		return false;
	}

	return true;
}

bool CKeyInputDI::UpdateKeyState()
{
	HRESULT getDevStateRes;
	getDevStateRes=m_pDInputKeyboardDevice->GetDeviceState(256,m_keyState);
	if (getDevStateRes!=DI_OK)
	{
		if (getDevStateRes==DIERR_INPUTLOST)
		{
			if (m_pDInputKeyboardDevice->Acquire()==DI_OK)
				getDevStateRes=m_pDInputKeyboardDevice->GetDeviceState(256,m_keyState);
		}
	}
	if (getDevStateRes!=DI_OK)
		return false;

	return true;
}

}