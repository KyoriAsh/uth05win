#include ".\selectcharascreen.h"
#include "../Game.h"
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"

namespace th5w{

CSelectCharaScreen::CSelectCharaScreen(void)
{
}

CSelectCharaScreen::~CSelectCharaScreen(void)
{
	if (m_pBGImage!=NULL)
		m_pBGImage->Destroy();
	m_pBGImage=NULL;
}

void CSelectCharaScreen::Initialize()
{
	m_bQuit=false;
	m_quitCode=0;
	m_lastKeyState=0;

	m_cursorPos=0;

	int difficulty=CGame::GVar().m_playDifficulty;
	for (int i=0;i<4;i++)
		m_bCharaClearFlag[i]=CGame::GVar().m_bClear[i][difficulty];
	if (difficulty!=4)
		for (int i=0;i<4;i++)
			m_bCharaSelectable[i]=true;
	else
		for (int i=0;i<4;i++)
			m_bCharaSelectable[i]=CGame::GVar().m_bExtraPlayable[i];
	for (int i=0;i<4;i++)
		if (m_bCharaSelectable[i])
			m_charaDisplayImg[i]=i;
		else
			m_charaDisplayImg[i]=4;

	CCommonFunctionGraphic::LoadPIFromDat(&m_pBGImage,m_palette,&CGame::s_pCurGame->m_th5Dat1,"SLB1.PI");

	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL00.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL01.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL02.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL03.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SL04.CDG",m_palette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,"SLCL.CDG",m_palette);

	m_curScrFade=0;
}

int CSelectCharaScreen::Step()
{
	if (m_bQuit)
	{
		if (m_curScrFade>0)
		{
			m_curScrFade-=6;
			if (m_curScrFade<0)
				m_curScrFade=0;
			return 0;
		}
		else
			return m_quitCode;
	}

	if (m_curScrFade<100)
	{
		m_curScrFade+=6;
		if (m_curScrFade>100)
			m_curScrFade=100;
		return 0;
	}

	ParseKeyEvent();

	return 0;
}

void CSelectCharaScreen::Draw()
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	m_pBGImage->Draw(0,40);

	int charaX[]={16,272,160,400};
	int charaY[]={48+40,48+40,224+40,224+40};

	for (int i=0;i<4;i++)
	{
		if (m_cursorPos==i)
		{
			CCommonFunctionGraphic::DrawRectangle((float)charaX[i],(float)charaY[i],
												  (float)charaX[i]+m_spriteArray.GetImagePtr(m_charaDisplayImg[i])->m_width-1,
												  (float)charaY[i]+m_spriteArray.GetImagePtr(m_charaDisplayImg[i])->m_height-1,
												  0,0,0);
			m_spriteArray.GetImagePtr(m_charaDisplayImg[i])->Draw((float)charaX[i]-8,(float)charaY[i]-8);
			if (m_bCharaClearFlag[i])
				m_spriteArray.GetImagePtr(5)->Draw((float)charaX[i]-8+160,(float)charaY[i]-8+144);
		}
		else
		{
			m_spriteArray.GetImagePtr(m_charaDisplayImg[i])->DrawColorScaled((float)charaX[i],(float)charaY[i],0.5,0.5,0.5);
			if (m_bCharaClearFlag[i])
				m_spriteArray.GetImagePtr(5)->DrawColorScaled((float)charaX[i]+160,(float)charaY[i]+144,0.5,0.5,0.5);
		}
	}

	CCommonFunctionGraphic::ScreenFade((float)m_curScrFade);
}

void CSelectCharaScreen::ParseKeyEvent()
{
	unsigned short curState=CCommonFunctionInput::GetAllKeyState();
	if (CCommonFunctionInput::LeftPressed(curState,m_lastKeyState)||CCommonFunctionInput::RightPressed(curState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_cursorPos=m_cursorPos/2*2+(1-m_cursorPos%2);
		m_lastKeyState=curState;
		return;
	}
	if (CCommonFunctionInput::UpPressed(curState,m_lastKeyState)||CCommonFunctionInput::DownPressed(curState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_cursorPos=m_cursorPos%2+(2-m_cursorPos+m_cursorPos%2);
		m_lastKeyState=curState;
		return;
	}
	if (CCommonFunctionInput::ZPressed(curState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(11);
		if (m_bCharaSelectable[m_cursorPos]==true)
		{
			CGame::GVar().m_playChara=m_cursorPos;
			CGame::GVar().OnBeginGame();
			m_bQuit=true;
			m_quitCode=SELECTCHARASCREEN_END_SELECTED_CHARA;
		}
		m_lastKeyState=curState;
		return;
	}
	if (CCommonFunctionInput::ESCPressed(curState,m_lastKeyState))
	{
		m_bQuit=true;
		m_quitCode=SELECTCHARASCREEN_END_BACK;
		m_lastKeyState=curState;
		return;
	}

	m_lastKeyState=curState;
	return;
}

}












