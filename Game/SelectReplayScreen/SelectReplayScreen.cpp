#include ".\selectreplayscreen.h"
#include "../Game.h"
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "../../gameLib/CommonFunction/CommonFunctionMusicSE.h"
#include "../../gameLib/Graphic/PC98Font.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include <time.h>
#include <math.h>
#include <shlwapi.h>
#include <tchar.h>
#define PI 3.1415926535897932384626433832795

namespace th5w{

	char charTable[7][20]={"ABCDEFGHIJKLM",
						   "NOPQRSTUVWXYZ",
						   "abcdefghijklm",
						   "nopqrstuvwxyz",
						   "0123456789-=\\",
						   "!@#$%^&*()_+|",
						   "[]{};':\"<>/~End"};

CSelectReplayScreen::CSelectReplayScreen(void)
{
	m_pBGImage=NULL;
	m_bEnterNameMode=false;
}

CSelectReplayScreen::~CSelectReplayScreen(void)
{
	if (m_pBGImage)
		m_pBGImage->Destroy();
	m_pBGImage=NULL;
}

void CSelectReplayScreen::Initialize(bool bLoadMode,bool bSwitchMusic)
{
	//make sure replay sub directory exists
	TCHAR subdir[1000];
	wsprintf(subdir,_T("%S%S\\"),CGame::GVar().m_workingPath,
								 CGame::GVar().m_replaySubDir);
	if (::PathFileExists(subdir)==FALSE)
		::CreateDirectory(subdir,NULL);

	m_curFrame=0;
	m_lastKeyState=0;
	m_curKeyState=0;
	m_bQuit=false;
	m_curScrFade=0;

	m_bLoadMode=bLoadMode;
	m_nFilePerPage=20;
	m_nPage=5;
	m_listUpperLeftX=10;
	m_listUpperLeftY=4;
	SwitchPage(0);
	m_curCursorPos=0;
	m_curCursorColPos=0;

	if (CGame::GVar().m_bCanSaveReplay==false&&m_bLoadMode==false)
	{
		m_quitCode=SELECTREPLAYSCREEN_END_SAVE_REPLAY_SKIPPED;
		m_bQuit=true;
		return;
	}

	CCommonFunctionGraphic::LoadPIFromDat(&m_pBGImage,NULL,&CGame::s_pCurGame->m_th5Dat1,"UDE.PI");
	CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_shineArray,NULL,&CGame::s_pCurGame->m_th5Dat1,"ZUN01.BFT",0);
	CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_shineArray,NULL,&CGame::s_pCurGame->m_th5Dat1,"ZUN02.BFT",0);

	if (bSwitchMusic)
	{
		if (bLoadMode)
			CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat1,"OP.M2");
		else
			CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat1,"NAME.M2");
		th5w::CPMDPlayer::Play();
	}
}

void CSelectReplayScreen::SwitchPage(int pageIdx)
{
	m_curPage=pageIdx;
	for (int i=0;i<m_nFilePerPage;i++)
	{
		int fileIdx=m_curPage*m_nFilePerPage+i;
		char repFileName[1000];
		sprintf(repFileName,"%s%s\\replay%02d.rpy",CGame::GVar().m_workingPath,
												   CGame::GVar().m_replaySubDir,
												   fileIdx);
		m_bCurPageRepFileExist[i]=CReplay::GetRepFileInfo(&m_repInfo[i],repFileName);
	}
}

void CSelectReplayScreen::StepEnterNameMode()
{
	if (abs(m_curRowY-(m_listUpperLeftY+4)*16)>8)
	{
		if (m_curRowY<(m_listUpperLeftY+4)*16)
			m_curRowY+=8;
		else
			m_curRowY-=8;
		if (abs(m_curRowY-(m_listUpperLeftY+4)*16)<8)
			m_curRowY=(m_listUpperLeftY+4)*16;
		return;
	}
	else
		m_curRowY=(m_listUpperLeftY+4)*16;

	if (CCommonFunctionInput::ESCPressed(m_curKeyState,m_lastKeyState))
		m_bEnterNameMode=false;
	if (CCommonFunctionInput::LeftPressed(m_curKeyState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_curCharX=(m_curCharX+12)%13;
	}
	if (CCommonFunctionInput::RightPressed(m_curKeyState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_curCharX=(m_curCharX+1)%13;
	}
	if (CCommonFunctionInput::UpPressed(m_curKeyState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_curCharY=(m_curCharY+6)%7;
	}
	if (CCommonFunctionInput::DownPressed(m_curKeyState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_curCharY=(m_curCharY+1)%7;
	}
	if (CCommonFunctionInput::ZPressed(m_curKeyState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(11);

		if (m_curCharX==12&&m_curCharY==6)
		{
			memcpy(CGame::GVar().m_replay.m_playerName,m_curEnterName,8);
			char repFileName[1000];
			sprintf(repFileName,"%s%s\\replay%02d.rpy",CGame::GVar().m_workingPath,
													   CGame::GVar().m_replaySubDir,
													   m_curPage*m_nFilePerPage+m_selectedSlot);
			CGame::GVar().m_replay.SaveFile(repFileName);
			memcpy(CGame::GVar().m_defaultReplayName,m_curEnterName,10);
			CGame::GVar().SaveConfig();
			m_bEnterNameMode=false;

			CGame::s_pCurGame->m_soundEffect.PlaySound(14);
			m_bCurPageRepFileExist[m_selectedSlot]=CReplay::GetRepFileInfo(&m_repInfo[m_selectedSlot],repFileName);
		}
		else
		{
			m_curEnterName[m_nameCursorPos]=charTable[m_curCharY][m_curCharX];
			if (m_nameCursorPos<7)
				m_nameCursorPos++;
		}
	}
	if (CCommonFunctionInput::XPressed(m_curKeyState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(11);
		if (m_curEnterName[m_nameCursorPos]!=0)
			m_curEnterName[m_nameCursorPos]=0;
		else
			if (m_nameCursorPos>0)
			{
				m_nameCursorPos--;
				m_curEnterName[m_nameCursorPos]=0;
			}
	}
}

int CSelectReplayScreen::Step()
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

	if (m_bEnterNameMode)
	{
		StepEnterNameMode();
		return 0;
	}

	if (CCommonFunctionInput::LeftPressed(m_curKeyState,m_lastKeyState))
	{
		if (!CCommonFunctionInput::LShiftKey(m_curKeyState))
		{
			if (m_bLoadMode)
			{
				m_curCursorColPos=(m_curCursorColPos+6)%7;
				CGame::s_pCurGame->m_soundEffect.PlaySound(1);
			}
		}
		else
		{
			SwitchPage((m_curPage+m_nPage-1)%m_nPage);
			CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		}
	}
	if (CCommonFunctionInput::RightPressed(m_curKeyState,m_lastKeyState))
	{
		if (!CCommonFunctionInput::LShiftKey(m_curKeyState))
		{
			if (m_bLoadMode)
			{
				m_curCursorColPos=(m_curCursorColPos+1)%7;
				CGame::s_pCurGame->m_soundEffect.PlaySound(1);
			}
		}
		else
		{
			SwitchPage((m_curPage+1)%m_nPage);
			CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		}
	}
	if (CCommonFunctionInput::UpPressed(m_curKeyState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_curCursorPos=(m_curCursorPos+m_nFilePerPage-1)%m_nFilePerPage;
	}
	if (CCommonFunctionInput::DownPressed(m_curKeyState,m_lastKeyState))
	{
		CGame::s_pCurGame->m_soundEffect.PlaySound(1);
		m_curCursorPos=(m_curCursorPos+1)%m_nFilePerPage;
	}
	if (CCommonFunctionInput::ESCPressed(m_curKeyState,m_lastKeyState)
		||CCommonFunctionInput::XPressed(m_curKeyState,m_lastKeyState))
	{
		m_bQuit=true;
		m_quitCode=SELECTREPLAYSCREEN_END_BACK;
		if (!m_bLoadMode)
			m_quitCode=SELECTREPLAYSCREEN_END_SAVE_REPLAY_END;
	}
	if (CCommonFunctionInput::ZPressed(m_curKeyState,m_lastKeyState))
	{
		if (m_bLoadMode)
		{
			CGame::s_pCurGame->m_soundEffect.PlaySound(11);
			if (m_repInfo[m_curCursorPos].stageFlag[m_curCursorColPos]!=0)
			{
				int fileIdx=m_curPage*m_nFilePerPage+m_curCursorPos;
				char repFileName[1000];
				sprintf(repFileName,"%s%s\\replay%02d.rpy",CGame::GVar().m_workingPath,
														   CGame::GVar().m_replaySubDir,
														   fileIdx);
				if (CGame::GVar().m_replay.LoadFile(repFileName))
				{
					CGame::GVar().m_bReplayMode=true;
					CGame::GVar().m_playStage=m_curCursorColPos;
					CGame::GVar().OnBeginGame();
					m_bQuit=true;
					m_quitCode=SELECTREPLAYSCREEN_END_SELECTED_REPLAY;
				}
			}
		}
		else
		{
			CGame::s_pCurGame->m_soundEffect.PlaySound(11);
			m_curCharX=0;
			m_curCharY=0;
			m_selectedSlot=m_curCursorPos;
			m_curRowY=(m_listUpperLeftY+m_selectedSlot)*16;
			memcpy(m_curEnterName,CGame::GVar().m_defaultReplayName,10);
			m_nameCursorPos=(int)strlen(m_curEnterName);
			if (m_nameCursorPos>7)
				m_nameCursorPos=7;
			m_bEnterNameMode=true;
		}
	}
	return 0;
}

void CSelectReplayScreen::DrawEnterNameMode()
{
	CReplay *pRep=&CGame::GVar().m_replay;
	int x=m_listUpperLeftX*8;
	int y=m_curRowY;
	char strBuf[100];

	float color[]={1.0f,1.0f,1.0f};
	float sc[]={0.2f,0.2f,0.2f};
	float salpha=0.5f;
	int soffx=2,soffy=2;

	sprintf(strBuf,"No.%02d ",m_curPage*m_nFilePerPage+m_selectedSlot);
	CPC98Font::DrawString(strBuf,100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
	CPC98Font::DrawString(strBuf,100,x,y,color[0],color[1],color[2]);
	x+=6*8;

	memcpy(strBuf,m_curEnterName,10);
	CPC98Font::DrawString(strBuf,100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
	CPC98Font::DrawString(strBuf,100,x,y,color[0],color[1],color[2]);

	CCommonFunctionGraphic::DrawRectangle((float)x+m_nameCursorPos*8,(float)y+16,(float)x+m_nameCursorPos*8+7,(float)y+17,1,1,1);
	x+=9*8;

	{
		time_t tt=time(NULL);
		tm *ptm=localtime(&tt);
		sprintf(strBuf,"%02d/%02d/%02d %02d:%02d",ptm->tm_year%100,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min);
		CPC98Font::DrawString(strBuf,100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
		CPC98Font::DrawString(strBuf,100,x,y,color[0],color[1],color[2]);
		x+=15*8;
	}

	{
		char charaName[][10]={"Reimu","Marisa","Mima","Yuka"};
		CPC98Font::DrawString(charaName[pRep->m_playChara],100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
		CPC98Font::DrawString(charaName[pRep->m_playChara],100,x,y,color[0],color[1],color[2]);
		x+=7*8;
	}
	{
		char difficultyName[][10]={"Easy","Normal","Hard","Lunatic","Extra"};
		CPC98Font::DrawString(difficultyName[pRep->m_playDifficulty],100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
		CPC98Font::DrawString(difficultyName[pRep->m_playDifficulty],100,x,y,color[0],color[1],color[2]);
		x+=8*8;
	}

	{
		char stgBriefName[][3]={"1","2","3","4","5","6","Ex"};
		char stgBriefNameNone[][3]={"-","-","-","-","-","-","--"};
		for (int j=0;j<7;j++)
		{
			char *name=pRep->m_stageFlag[j]!=0?stgBriefName[j]:stgBriefNameNone[j];
			CPC98Font::DrawString(name,100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
			CPC98Font::DrawString(name,100,x,y,color[0],color[1],color[2]);
			x+=((int)strlen(name)+1)*8;
		}
	}

	if (m_curRowY!=(m_listUpperLeftY+4)*16)
		return;

	int ulx=(m_listUpperLeftX+18)*8;
	int uly=(m_listUpperLeftY+12)*16;
	for (int i=0;i<7;i++)
		for (int j=0;j<13;j++)
		{
			int len=1;
			if (i==6&&j==12)
				len=100;
			CPC98Font::DrawString(&charTable[i][j],len,ulx+j*16+soffx,uly+i*16+soffy,sc[0],sc[1],sc[2],salpha);
			if (i==m_curCharY&&j==m_curCharX)
			{
				color[0]=1.0f;
				color[1]=1.0f;
				color[2]=0.5f;
			}
			else
			{
				color[0]=0.3f;
				color[1]=0.7f;
				color[2]=0.3f;
			}
			CPC98Font::DrawString(&charTable[i][j],len,ulx+j*16,uly+i*16,color[0],color[1],color[2]);
		}
}

void CSelectReplayScreen::Draw()
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (m_curScrFade==0)
		return;

	m_pBGImage->Draw(0,40);
	CCommonFunctionGraphic::DrawRectangle(0,0,639,39,1.0f,1.0f,1.0f);
	CCommonFunctionGraphic::DrawRectangle(0,440,639,479,1.0f,13.0f/15.0f,13.0f/15.0f);
	CCommonFunctionGraphic::ScreenFade(50);

	if (m_bLoadMode)
	{
		unsigned char extLoadReplay[]={0xb5,0xb8,0xaa,0xad,0x02,0xbb,0xae,0xb9,0xb5,0xaa,0xc2,0};
		CTh5ExtFont::DrawExtString(extLoadReplay,100,(m_listUpperLeftX-4)*8+4,(m_listUpperLeftY-2)*16+4,0,0,0,0.5);
		CTh5ExtFont::DrawExtString(extLoadReplay,100,(m_listUpperLeftX-4)*8,(m_listUpperLeftY-2)*16,1,1,1);
	}
	else
	{
		unsigned char extSaveReplay[]={0xbc,0xaa,0xbf,0xae,0x02,0xbb,0xae,0xb9,0xb5,0xaa,0xc2,0};
		CTh5ExtFont::DrawExtString(extSaveReplay,100,(m_listUpperLeftX-4)*8+4,(m_listUpperLeftY-2)*16+4,0,0,0,0.5);
		CTh5ExtFont::DrawExtString(extSaveReplay,100,(m_listUpperLeftX-4)*8,(m_listUpperLeftY-2)*16,1,1,1);
	}

	if (m_bEnterNameMode)
	{
		DrawEnterNameMode();
		return;
	}

	for (int i=0;i<m_nFilePerPage;i++)
	{
		int x=m_listUpperLeftX*8;
		int y=(m_listUpperLeftY+i)*16;
		char strBuf[100];

		float color[]={0.5f,1.0f,0.5f};
		float sc[]={0.2f,0.2f,0.2f};
		float salpha=0.5f;
		int soffx=2,soffy=2;
		if (i==m_curCursorPos)
		{
			color[0]=color[1]=1;
			x-=1;y-=1;
		}

		sprintf(strBuf,"No.%02d ",m_curPage*m_nFilePerPage+i);
		CPC98Font::DrawString(strBuf,100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
		CPC98Font::DrawString(strBuf,100,x,y,color[0],color[1],color[2]);
		x+=6*8;

		if (!m_bCurPageRepFileExist[i])
		{
			char *outInfo=NULL;
			char errorInfo[][100]={"-------- -------------- ------ ------- - - - - - - --",
								   "          Invalid replay file          - - - - - - --",
								   "    Replay under other game version    - - - - - - --",
								   "      Replay under other mod file      - - - - - - --",
									};
			switch(m_repInfo[i].infoRes)
			{
			case InfoRes_FileNotExist:
				outInfo=errorInfo[0];
				break;
			case InfoRes_InvalidReplay:
				outInfo=errorInfo[1];
				break;
			case InfoRes_GameVersionMismatch:
				outInfo=errorInfo[2];
			    break;
			case InfoRes_ModFileMisMatch:
				outInfo=errorInfo[3];
			    break;
			}
			CPC98Font::DrawString(outInfo,100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
			CPC98Font::DrawString(outInfo,100,x,y,color[0],color[1],color[2]);
		}
		else
		{
			memcpy(strBuf,m_repInfo[i].playerName,8);
			strBuf[8]=0;
			CPC98Font::DrawString(strBuf,100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
			CPC98Font::DrawString(strBuf,100,x,y,color[0],color[1],color[2]);
			x+=9*8;

			{
				time_t tt=(time_t)m_repInfo[i].saveTime;
				tm *ptm=localtime(&tt);
				sprintf(strBuf,"%02d/%02d/%02d %02d:%02d",ptm->tm_year%100,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min);
				CPC98Font::DrawString(strBuf,100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
				CPC98Font::DrawString(strBuf,100,x,y,color[0],color[1],color[2]);
				x+=15*8;
			}

			{
				char charaName[][10]={"Reimu","Marisa","Mima","Yuka"};
				CPC98Font::DrawString(charaName[m_repInfo[i].playChara],100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
				CPC98Font::DrawString(charaName[m_repInfo[i].playChara],100,x,y,color[0],color[1],color[2]);
				x+=7*8;
			}
			{
				char difficultyName[][10]={"Easy","Normal","Hard","Lunatic","Extra"};
				CPC98Font::DrawString(difficultyName[m_repInfo[i].playDifficulty],100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
				CPC98Font::DrawString(difficultyName[m_repInfo[i].playDifficulty],100,x,y,color[0],color[1],color[2]);
				x+=8*8;
			}

			{
				char stgBriefName[][3]={"1","2","3","4","5","6","Ex"};
				char stgBriefNameNone[][3]={"-","-","-","-","-","-","--"};
				for (int j=0;j<7;j++)
				{
					char *name=m_repInfo[i].stageFlag[j]!=0?stgBriefName[j]:stgBriefNameNone[j];
					CPC98Font::DrawString(name,100,x+soffx,y+soffy,sc[0],sc[1],sc[2],salpha);
					CPC98Font::DrawString(name,100,x,y,color[0],color[1],color[2]);
					x+=((int)strlen(name)+1)*8;
				}
			}
		}
		if (i==m_curCursorPos&&m_bLoadMode)
		{
			x=(m_listUpperLeftX+45)*8+m_curCursorColPos*16+4-1;
			if (m_curCursorColPos==6)
				x+=4;
			y+=8;		//no need to minus 1, as the original y already has 1 minused
			int imgIdx=1;
			C2DImage *pImg=m_shineArray.GetImagePtr(imgIdx);
			float angle=((float)(m_curFrame%64)/64.0f)*(float)PI*2.0f;
			float radius=fabs(8.0f-(float)(m_curFrame%64)/4.0f);
			float alpha=0.5f+radius/16.0f;
			radius+=4.0f;
			pImg->Draw((float)x-pImg->m_width/2+(float)cos(angle)*radius,(float)y-pImg->m_height/2+(float)sin(angle)*radius,alpha);
			angle+=(float)PI;
			pImg->Draw((float)x-pImg->m_width/2+(float)cos(angle)*radius,(float)y-pImg->m_height/2+(float)sin(angle)*radius,alpha);

			pImg=m_shineArray.GetImagePtr(imgIdx+8);
			angle+=(float)PI/2.0f;
			pImg->Draw((float)x-pImg->m_width/2+(float)cos(angle)*radius,(float)y-pImg->m_height/2+(float)sin(angle)*radius,alpha);
			angle+=(float)PI;
			pImg->Draw((float)x-pImg->m_width/2+(float)cos(angle)*radius,(float)y-pImg->m_height/2+(float)sin(angle)*radius,alpha);
		}
	}
	if (m_bCurPageRepFileExist[m_curCursorPos])
	{
		char str[100];
		char ss[7][20];
		for (int i=0;i<7;i++)
			if (m_repInfo[m_curCursorPos].stageFlag[i]!=0)
				sprintf(ss[i],"%08d0",m_repInfo[m_curCursorPos].stageScore[i]);
			else
				sprintf(ss[i],"---------");
		sprintf(str,"Stage 1  %s   Stage 2  %s   Stage 3  %s",ss[0],ss[1],ss[2]);
		CPC98Font::DrawString(str,100,80+2,390+2,0.2f,0.2f,0.2f,0.5f);
		CPC98Font::DrawString(str,100,80,390,1,1,1);
		sprintf(str,"Stage 4  %s   Stage 5  %s   Stage 6  %s",ss[3],ss[4],ss[5]);
		CPC98Font::DrawString(str,100,80+2,406+2,0.2f,0.2f,0.2f,0.5f);
		CPC98Font::DrawString(str,100,80,406,1,1,1);
		sprintf(str,"                     Stage Ex %s",ss[6]);
		CPC98Font::DrawString(str,100,80+2,422+2,0.2f,0.2f,0.2f,0.5f);
		CPC98Font::DrawString(str,100,80,422,1,1,1);
	}


	char desc1[]="Up/Down: Select file         Shift + Left/Right: Previous/Next page";
	char desc2[]="Left/Right: Select stage";

	CPC98Font::DrawString(desc1,100,52+2,440+2,0.2f,0.2f,0.2f,0.5f);
	CPC98Font::DrawString(desc1,100,52,440,1,1,1);
	if (m_bLoadMode)
	{
		CPC98Font::DrawString(desc2,100,52+2,456+2,0.2f,0.2f,0.2f,0.5f);
		CPC98Font::DrawString(desc2,100,52,456,1,1,1);
	}

	CCommonFunctionGraphic::ScreenFade((float)m_curScrFade);
}

}
























