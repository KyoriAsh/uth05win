#include ".\highscorescreen.h"
#include "../Game.h"
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "../../gameLib/CommonFunction/CommonFunctionMusicSE.h"
#include "../../gameLib/Graphic/PC98Font.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include <time.h>
#include <math.h>
#define PI 3.1415926535897932384626433832795

namespace th5w{

CHighScoreScreen::CHighScoreScreen(void)
{
	m_pBGImage=NULL;
	for (int i=0;i<17;i++)
		m_charTable[0][i]=0xaa+i;
	for (int i=0;i<11;i++)
		m_charTable[1][i]=0xbb+i;
	m_charTable[1][11]=0xd0;
	m_charTable[1][12]=0xc9;
	m_charTable[1][13]=0xc6;
	m_charTable[1][14]=0xc7;
	m_charTable[1][15]=0xc8;
	m_charTable[1][16]=0xcc;
	for (int i=0;i<10;i++)
		m_charTable[2][i]=0xa0+i;
	m_charTable[2][10]=0x70;
	m_charTable[2][11]=0x71;
	m_charTable[2][12]=0x72;
	m_charTable[2][13]=0xce;
	m_charTable[2][14]=0xcf;
	m_charTable[2][15]=0xcd;
	m_charTable[2][16]=0xd5;
}

CHighScoreScreen::~CHighScoreScreen(void)
{
	if (m_pBGImage)
		m_pBGImage->Destroy();
	m_pBGImage=NULL;
}

void CHighScoreScreen::Initialize(bool bViewMode,bool clearFlag)
{
	m_bViewMode=bViewMode;
	if (bViewMode)
		m_curPage=0;
	else
		m_curPage=CGame::GVar().m_playDifficulty;

	if (bViewMode==false)
	{
		int curScore=CGame::GVar().m_curScore;
		m_opChara=CGame::GVar().m_playChara;
		if (curScore<=CGame::GVar().m_highScore[m_opChara][m_curPage][4])
		{
			m_bQuit=true;
			m_quitCode=HIGHSCORESCREEN_END_ENTER_SKIPPED;
			m_curScrFade=0;
			return;
		}
		m_opRank=4;
		for (int i=4;i>=0;i--)
		{
			if (curScore>CGame::GVar().m_highScore[m_opChara][m_curPage][i])
			{
				if (i<4)
				{
					CGame::GVar().m_highScore[m_opChara][m_curPage][i+1]=CGame::GVar().m_highScore[m_opChara][m_curPage][i];
					memcpy(CGame::GVar().m_highScoreName[m_opChara][m_curPage][i+1],CGame::GVar().m_highScoreName[m_opChara][m_curPage][i],10);
					CGame::GVar().m_highScoreFlag[m_opChara][m_curPage][i+1]=CGame::GVar().m_highScoreFlag[m_opChara][m_curPage][i];
				}
				m_opRank=i;
				CGame::GVar().m_highScore[m_opChara][m_curPage][i]=curScore;
				CGame::GVar().m_highScoreName[m_opChara][m_curPage][i][0]=0;
				CGame::GVar().m_highScoreFlag[m_opChara][m_curPage][i]=CGame::GVar().m_playStage+1;
				if (CGame::GVar().m_playStage==6)
					CGame::GVar().m_highScoreFlag[m_opChara][m_curPage][i]=1;
				else
					if (clearFlag)
						CGame::GVar().m_highScoreFlag[m_opChara][m_curPage][i]=128;
			}
			else
				break;
		}
		m_curCursorPos=0;
		m_curCharX=0;
		m_curCharY=0;
	}

	m_bQuit=false;
	m_curScrFade=0;

	CCommonFunctionGraphic::LoadPIFromDat(&m_pBGImage,m_palette,&CGame::s_pCurGame->m_th5Dat1,"HI01.PI");
	CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_patternArray,NULL,&CGame::s_pCurGame->m_th5Dat1,"HI_M.BFT",0,NULL);
	CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_numberArray,NULL,&CGame::s_pCurGame->m_th5Dat1,"SCNUM.BFT",0,m_palette);

	CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat1,"NAME.M2");
	CPMDPlayer::Play();

	m_curFrame=0;
	m_lastKeyState=0;
	m_curKeyState=0;

	memcpy(m_curEnterName,CGame::GVar().m_defaultHighScoreName,10);
	m_curCursorPos=(int)strlen((char*)m_curEnterName);
	if (m_curCursorPos>7)
		m_curCursorPos=7;
}

int CHighScoreScreen::Step()
{
	m_curFrame++;

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

	m_lastKeyState=m_curKeyState;
	m_curKeyState=CCommonFunctionInput::GetAllKeyState();

	if (m_bViewMode)
	{
		if (CCommonFunctionInput::LeftPressed(m_curKeyState,m_lastKeyState)&&m_curPage>0)
			m_curPage--;
		if (CCommonFunctionInput::RightPressed(m_curKeyState,m_lastKeyState)&&m_curPage<4)
			m_curPage++;
		if (CCommonFunctionInput::ESCPressed(m_curKeyState,m_lastKeyState))
		{
			m_bQuit=true;
			m_quitCode=HIGHSCORESCREEN_END_VIEW_END;
		}
		return 0;
	}
	else
	{
		if (CCommonFunctionInput::LeftPressed(m_curKeyState,m_lastKeyState))
		{
			CGame::s_pCurGame->m_soundEffect.PlaySound(1);
			m_curCharX=(m_curCharX+16)%17;
		}
		if (CCommonFunctionInput::RightPressed(m_curKeyState,m_lastKeyState))
		{
			CGame::s_pCurGame->m_soundEffect.PlaySound(1);
			m_curCharX=(m_curCharX+1)%17;
		}
		if (CCommonFunctionInput::UpPressed(m_curKeyState,m_lastKeyState))
		{
			CGame::s_pCurGame->m_soundEffect.PlaySound(1);
			m_curCharY=(m_curCharY+2)%3;
		}
		if (CCommonFunctionInput::DownPressed(m_curKeyState,m_lastKeyState))
		{
			CGame::s_pCurGame->m_soundEffect.PlaySound(1);
			m_curCharY=(m_curCharY+1)%3;
		}
		if (CCommonFunctionInput::ESCPressed(m_curKeyState,m_lastKeyState))
		{
			CGame::s_pCurGame->m_soundEffect.PlaySound(11);
			m_curCharX=16;
			m_curCharY=2;
		}
		if (CCommonFunctionInput::ZPressed(m_curKeyState,m_lastKeyState))
		{
			CGame::s_pCurGame->m_soundEffect.PlaySound(11);
			switch(m_curCharY*17+m_curCharX)
			{
			case 47:
				if (m_curCursorPos>0)
					m_curCursorPos--;
				break;
			case 48:
				if (m_curCursorPos<7)
					m_curCursorPos++;
				break;
			case 49:
				m_curEnterName[m_curCursorPos]=0x02;
				if (m_curCursorPos<7)
					m_curCursorPos++;
				break;
			case 50:
				m_bQuit=true;
				m_quitCode=HIGHSCORESCREEN_END_ENTER_END;
				memcpy(CGame::GVar().m_highScoreName[m_opChara][m_curPage][m_opRank],m_curEnterName,10);
				CGame::GVar().SaveHighScore();
				memcpy(CGame::GVar().m_defaultHighScoreName,m_curEnterName,10);
				CGame::GVar().SaveConfig();
				break;
			default:
				m_curEnterName[m_curCursorPos]=m_charTable[m_curCharY][m_curCharX];
				if (m_curCursorPos<7)
					m_curCursorPos++;
				break;
			}
		}
		if (CCommonFunctionInput::XPressed(m_curKeyState,m_lastKeyState))
		{
			CGame::s_pCurGame->m_soundEffect.PlaySound(11);
			if (m_curCursorPos>0)
				m_curCursorPos--;
		}
		return 0;
	}

	return 0;
}

void CHighScoreScreen::DrawItem(int x,int y,unsigned char name[10],int score,unsigned char flag,bool bHighLight)
{
	float c[3];
	unsigned char drawName[10];
	memcpy(drawName,name,10);
	for (int i=0;i<8;i++)
		if (drawName[i]==0)
		{
			for (int j=i;j<=8;j++)
				drawName[j]=0xc4;
			break;
		}
	if (bHighLight)
		for (int i=0;i<3;i++)
			c[i]=m_palette[7*3+i]/255.0f;
	else
		for (int i=0;i<3;i++)
			c[i]=m_palette[2*3+i]/255.0f;
	CTh5ExtFont::DrawExtString(drawName,8,x,y,c[0],c[1],c[2]);

	x+=134;
	int digit[9];
	score*=10;
	for (int i=0;i<9;i++,score/=10)
		digit[8-i]=score%10;
	int baseIdx=0;
	if (bHighLight&&!m_bViewMode)
		baseIdx=10;
	for (int i=0;i<9;i++)
	{
		C2DImage *pImg=m_numberArray.GetImagePtr(baseIdx+digit[i]);
		pImg->Draw((float)x+i*16,(float)y);
	}

	x+=152;
	if (bHighLight||m_bViewMode)
		for (int i=0;i<3;i++)
			c[i]=m_palette[7*3+i]/255.0f;
	else
		for (int i=0;i<3;i++)
			c[i]=m_palette[2*3+i]/255.0f;
	unsigned char drawFlag;
	if (flag==128)
		drawFlag=0xe9;
	else
		drawFlag=0xa0+flag;
	CTh5ExtFont::DrawExtChar(drawFlag,x,y,c[0],c[1],c[2]);
}

void CHighScoreScreen::Draw()
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (m_curScrFade==0)
		return;

	m_pBGImage->Draw(0,40);
	CCommonFunctionGraphic::DrawRectangle(0,0,639,39,1.0f,14.0f/15.0f,14.0f/15.0f);
	CCommonFunctionGraphic::DrawRectangle(0,440,639,479,1.0f,12.0f/15.0f,13.0f/15.0f);

	for (int i=0;i<4;i++)
	{
		int x=8+i%2*320;
		int y=88+i/2*136+40;
		for (int j=0;j<5;j++)
		{
			unsigned char *pName=CGame::GVar().m_highScoreName[i][m_curPage][j];
			if (!m_bViewMode&&i==m_opChara&&j==m_opRank)
				pName=m_curEnterName;
			DrawItem(x,y,pName,
					 CGame::GVar().m_highScore[i][m_curPage][j],
					 CGame::GVar().m_highScoreFlag[i][m_curPage][j],
					 (m_bViewMode&&j==0)||(!m_bViewMode&&i==m_opChara&&j==m_opRank));
			if (!m_bViewMode&&i==m_opChara&&j==m_opRank)
			{
				float c[3];
				for (int i=0;i<3;i++)
					c[i]=m_palette[7*3+i]/255.0f;
				CCommonFunctionGraphic::DrawRectangle((float)x-2,(float)y-1,(float)x-2+308,(float)y-1+17,c[0],c[1],c[2],1.0f,false);
				CCommonFunctionGraphic::DrawRectangle((float)x+m_curCursorPos*16,(float)y+15,(float)x+m_curCursorPos*16+16,(float)y+15,c[0],c[1],c[2],1.0f,false);
			}
			y+=(j==0?24:16);
		}
	}

	if (!m_bViewMode)
	{
		int x=184;
		int y=336+40;
		for (int i=0;i<3;i++)
			for (int j=0;j<17;j++)
				if (i==m_curCharY&&j==m_curCharX)
					CTh5ExtFont::DrawExtString(m_charTable[i]+j,1,x+16*j,y+16*i,0,1,0,1.0f,true,true);
				else
					CTh5ExtFont::DrawExtString(m_charTable[i]+j,1,x+16*j,y+16*i,1,1,1);
	}

	if (m_bViewMode)
	{
		C2DImage *pImg=m_patternArray.GetImagePtr(m_curPage*2);
		pImg->Draw(496.0f,376.0f+40.0f);
		pImg=m_patternArray.GetImagePtr(m_curPage*2+1);
		pImg->Draw(560.0f,376.0f+40.0f);
	}

	CCommonFunctionGraphic::ScreenFade((float)m_curScrFade);
}

}































