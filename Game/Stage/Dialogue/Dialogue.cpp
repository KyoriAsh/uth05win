#include ".\dialogue.h"
#include "../Stage.h"
#include "../../Game.h"
#include "../../../gameLib/Graphic/Th5ExtFont.h"
#include "../../../gameLib/Graphic/PC98Font.h"

namespace th5w{

CDialogue::CDialogue(CStage *pStage)
{
	m_pStage=pStage;
	m_pTranScript=NULL;
	Initialize();
}

CDialogue::~CDialogue(void)
{
	if (m_bTranScript&&m_pTranScript!=NULL)
		delete m_pTranScript;
}

void CDialogue::Initialize()
{
	m_pScript=NULL;
	m_bRunning=false;

	m_bTranScript=false;
	char tx2File[]="_DM00.TX2";
	tx2File[4]=48+CGame::GVar().m_playStage;
	Cth5DatFile *pPackFile=CGame::s_pCurGame->FindResource(tx2File);
	int idx=pPackFile->GetChildFileIndex(tx2File);
	unsigned char *pContent=pPackFile->GetChildFileContentPointer(idx);
	int offset=*(unsigned short *)(pContent+CGame::GVar().m_playChara*2);
	SetDialogueScript(pContent+offset);
}

int CDialogue::StepScript()
{
	while(1)
	{
		switch(m_pScript[0])
		{
		case 0:
		case 1:
			m_activeChara=m_pScript[0];
			m_pScript++;
			break;
		case 2:
			if (m_pScript[1]==0xff)
				m_face[m_activeChara]=-1;
			else
				m_face[m_activeChara]=m_pScript[1]+m_activeChara*6+2;
			m_pScript+=2;
			break;
		case 3:
			CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_pStage->m_pStageRes->m_patternArray,NULL,
															 &CGame::s_pCurGame->m_th5Dat2,(char *)(m_pScript+1),
															 0,m_pStage->m_pStageRes->m_palette);
			m_pScript+=2+(int)strlen((char *)(m_pScript+1));
		    break;
		case 4:
			if (CGame::GVar().m_playStage==6)
			{
				m_pStage->m_pStageRes->m_patternArray.UnloadRange(36);
				CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_pStage->m_pStageRes->m_patternArray,NULL,
																 &CGame::s_pCurGame->m_th5Dat2,"ST06_16.BFT",
																 0,m_pStage->m_pStageRes->m_palette);
				char charaBombBFT[]="BOMB0.BFT";
				if (CGame::GVar().m_playChara!=2)
					charaBombBFT[4]=48+CGame::GVar().m_playChara;
				CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_pStage->m_pStageRes->m_patternArray,NULL,
																 &CGame::s_pCurGame->m_th5Dat2,charaBombBFT,
																 0,m_pStage->m_pStageRes->m_palette);
			}
			else
				m_pStage->m_pStageRes->m_patternArray.UnloadRange(180);
			m_pScript++;
			break;
		case 5:
			if (m_pScript[1]==36)
				CPMDPlayer::UnloadPMDData();
			else
			{
				char s[100];
				strcpy(s,(char *)(m_pScript+1));
				strcat(s,".M2");
				CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat2,s);
				CPMDPlayer::Play();
			}
			m_pScript+=2+(int)strlen((char *)(m_pScript+1));
			break;
		case 6:
			m_bossX[m_nBoss]=m_pScript[1]+m_pScript[2]*256;
			m_bossY[m_nBoss]=m_pScript[3]+m_pScript[4]*256;
			m_bossImg[m_nBoss]=m_pScript[5];
			m_nBoss++;
			m_pScript+=6;
		    break;
		case 7:
		case 8:
			m_pScript++;
			break;
		case 9:
			m_bFading=true;
			m_bFadeToWhite=true;
			m_bInstantFade=(m_pScript[1]==0?true:false);
			m_pScript+=2;
			return 0;
		case 10:
			m_bFading=true;
			m_bFadeToWhite=false;
			m_bInstantFade=(m_pScript[1]==0?true:false);
			m_pScript+=2;
			return 0;
		case 13:
			if (m_words[m_activeChara]!=m_pScript+1)
			{
				m_words[m_activeChara]=m_pScript+1;
				m_wordNChar[m_activeChara]=0;
				m_curWordTotalChar=0;
				unsigned char *ptr=m_pScript+1;
				while(ptr[0]!=0xff)
				{
					if (ptr[0]==11)
						ptr++;
					else
					{
						ptr+=2;
						m_curWordTotalChar++;
					}
				}
			}
			else
			{
				if (CCommonFunctionInput::ZPressed(m_pStage->m_curGameKeyState,m_pStage->m_lastGameKeyState)
					||CCommonFunctionInput::LCtrlKey(m_pStage->m_curGameKeyState))
				{
					if (m_wordNChar[m_activeChara]<m_curWordTotalChar)
						m_wordNChar[m_activeChara]=m_curWordTotalChar;
					else
					{
						m_pScript++;
						while(m_pScript[0]!=0xff)
						{
							if (m_pScript[0]==11)
								m_pScript++;
							else
								m_pScript+=2;
						}
						m_pScript++;
					}
				}
				else
					if (m_wordNChar[m_activeChara]<m_curWordTotalChar)
						m_wordNChar[m_activeChara]++;
			}
			return 0;
		case 14:
			m_pScript++;
			break;
		case 0xff:
			m_pScript++;
			m_bRunning=false;
			return 1;
		}
	}
}

int CDialogue::Step()
{
	if (m_bRunning==false)
		return 1;
	if (m_pScript==NULL)
	{
		m_bRunning=false;
		return 1;
	}
	if (m_dialogBGFadeTimer<35)
	{
		m_dialogBGFadeTimer++;
		return 0;
	}
	if (m_bFading)
	{
		if (m_bFadeToWhite)
		{
			if (m_bInstantFade)
				m_pStage->m_curScrFade=200;
			else
				m_pStage->m_curScrFade+=6;
			if (m_pStage->m_curScrFade>=200)
			{
				m_pStage->m_curScrFade=200;
				m_bFading=false;
			}
		}
		else
		{
			if (m_bInstantFade)
				m_pStage->m_curScrFade=100;
			else
				m_pStage->m_curScrFade-=6;
			if (m_pStage->m_curScrFade<=100)
			{
				m_pStage->m_curScrFade=100;
				m_bFading=false;
			}
		}
		return 0;
	}
	return StepScript();
}

void CDialogue::Draw()
{
	if (m_bRunning==false)
		return;

	float scale=(m_dialogBGFadeTimer/12+1)*0.25f;
	float xLow,yLow;
	xLow=(float)48-32+m_pStage->m_playAreaUpperLeftX;
	yLow=(float)192-16+m_pStage->m_playAreaUpperLeftY;
	CCommonFunctionGraphic::DrawRectangle(xLow,yLow,xLow+320,yLow+48,0,0,0,scale);
	xLow=(float)80-32+m_pStage->m_playAreaUpperLeftX;
	yLow=(float)320-16+m_pStage->m_playAreaUpperLeftY;
	CCommonFunctionGraphic::DrawRectangle(xLow,yLow,xLow+320,yLow+48,0,0,0,scale);

	for (int i=0;i<2;i++)
		if (m_face[i]!=-1)
		{
			C2DImage *pImg=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(m_face[i]);
			float drawX=(float)32+i*(288-32)-32+m_pStage->m_playAreaUpperLeftX;
			float drawY=(float)240+i*(112-240)-16+m_pStage->m_playAreaUpperLeftY;
			pImg->Draw(drawX,drawY);
		}
	for (int i=0;i<2;i++)
	{
		if (m_words[i]==NULL)
			continue;

		float color[]={1,1,1};
		if (m_activeChara!=i)
			color[0]=color[1]=0;

		int nCharLeft=m_wordNChar[i];
		unsigned char *ptr=m_words[i];
		int drawX=(i==0?20:6)*8-32+m_pStage->m_playAreaUpperLeftX;
		int drawY=(i==0?20:12)*16-16+m_pStage->m_playAreaUpperLeftY;
		while (nCharLeft>0)
		{
			if (ptr[0]==255)
				break;
			if (ptr[0]==13)
			{
				ptr++;
				continue;
			}
			if (ptr[0]==12)
			{
				CTh5ExtFont::DrawExtChar(ptr[1],drawX,drawY,color[0],color[1],color[2]);
				ptr+=2;
				drawX+=16;
				nCharLeft--;
				continue;
			}
			if (ptr[0]==11)
			{
				drawX=(i==0?20:6)*8-32+m_pStage->m_playAreaUpperLeftX;
				drawY+=16;
				ptr++;
				continue;
			}
			unsigned char buf[3];
			buf[0]=ptr[0];buf[1]=ptr[1];buf[2]=0;
			CPC98Font::DrawString((char *)buf,100,drawX,drawY,color[0],color[1],color[2]);
			ptr+=2;
			drawX+=16;
			nCharLeft--;
		}
	}
	for (int i=0;i<m_nBoss;i++)
	{
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_bossImg[i]);
		pImg->Draw((float)m_bossX[i]-32+m_pStage->m_playAreaUpperLeftX,
				   (float)m_bossY[i]-16+m_pStage->m_playAreaUpperLeftY);
	}
}

void CDialogue::Run()
{
	m_bRunning=true;
	m_dialogBGFadeTimer=0;
	m_face[0]=m_face[1]=-1;
	m_words[0]=m_words[1]=NULL;
	m_wordNChar[0]=m_wordNChar[1]=0;
	m_nBoss=0;
	//m_activeChara=0;
	m_bFading=false;
}

}










