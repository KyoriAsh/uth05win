#include ".\commonfunctioninput.h"
#include "../../Game/Game.h"

namespace th5w{

CCommonFunctionInput::CCommonFunctionInput(void)
{
}

CCommonFunctionInput::~CCommonFunctionInput(void)
{
}

unsigned short CCommonFunctionInput::GetAllKeyState()
{
//key state map:
//low byte bits:
//	7				6			5		4		3		2		1		0
//	left control	left shift	z		x		right	left	down	up
//high byte bits:
//	7		6		5		4		3		2		1		0
//			q		enter	esc

	CGame::s_pCurGame->m_input.UpdateKeyState();
	unsigned short ret=0;
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_UP))<<0;
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_DOWN))<<1;
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_LEFT))<<2;
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_RIGHT))<<3;
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_X))<<4;
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_Z))<<5;
	//PC98 version does not include shift status in the key state but manage it in another variable
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_LSHIFT))<<6;
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_LCONTROL))<<7;

	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_ESCAPE))<<12;
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_RETURN))<<13;
	ret|=((unsigned short)CGame::s_pCurGame->m_input.GetKeyState(DIK_Q))<<14;

	return ret;
}

}