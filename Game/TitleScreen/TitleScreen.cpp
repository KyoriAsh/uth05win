#include <windows.h>
#include ".\titlescreen.h"
#include "../Game.h"
#include "../../gameLib/PMDPlayer/PMDPlayer.h"
#include "../../gameLib/CommonFunction/CommonFunctionMusicSE.h"
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "../../gameLib/Graphic/PC98Font.h"

namespace th5w{

#define MENU_ITEM_COLOR_NORMAL 8
#define MENU_ITEM_COLOR_HIGHLIGHTED 14
#define MENU_ITEM_COLOR_DISABLED 2

#define MAIN_MENU_START 0
#define MAIN_MENU_EXTRA_START 1
#define MAIN_MENU_REPLAY 2
#define MAIN_MENU_HISCORE 3
#define MAIN_MENU_MUSIC_ROOM 4
#define MAIN_MENU_OPTION 5
#define MAIN_MENU_QUIT 6


char mainMenuDesc[][100]={"",
						  "エキストラステージを開始します",
						  "リプレーを見ます",
						  "現在のハイスコアを表示します",
						  "音楽室に入ります(Not implemented)",
						  "各種設定を変更出来ます",
						  "ＷＩＮＤＯＷＳに戻ります"};
char startGameDesc[][100]={"ゲームを開始します（イージー）",
						   "ゲームを開始します（ノーマル）",
						   "ゲームを開始します（ハード）",
						   "ゲームを開始します（ルナティック）"};
char optionMenuDesc[][100]={"",
							"ゲームスタート時の人数を変更出来ます（除くエキストラ）",
							"ボムの使用回数を変更出来ます（除くエキストラ）",
							"ＢＧＭに８６音源互換を使用します(Via PMDWin and DirectSound, not changeable)",
							"効果音にＦＭ音源を使用します(Via DirectSound, not changeable)",
							"処理落ちをマシンのスピードに任せます（標準）(SlowMode not implemented)",
							"このオプションを全て標準に戻します(Not implemented)",
							"オプションを終了します"};
char difficultyDesc[][100]={"難易度をやさしくします　     （初心者向け）　 ",
							"難易度を標準にします　　　    （一般向け）　　",
							"難易度を難しくします　　　（アーケーダー向け）",
							"難易度を非常に難しくします　（シューター向け）"};


CTitleScreen::CTitleScreen(void)
{
	m_mainMenuNItem=7;
	m_optionMenuNItem=8;
	m_pBGImage=NULL;
	m_pMenuImgPractice=NULL;
	m_pMenuImgReplay=NULL;
}

CTitleScreen::~CTitleScreen(void)
{
	if (m_pBGImage!=NULL)
		m_pBGImage->Destroy();
	m_pBGImage=NULL;
	if (m_pMenuImgPractice!=NULL)
		m_pMenuImgPractice->Destroy();
	m_pMenuImgPractice=NULL;
	if (m_pMenuImgReplay!=NULL)
		m_pMenuImgReplay->Destroy();
	m_pMenuImgReplay=NULL;
}

void CTitleScreen::Initialize(bool bRollTama,int cursorInitialPos, bool bSwitchMusic)
{
	if (CGame::GVar().m_bUseMod)
	{
		int fileIdx=CGame::s_pCurGame->m_modDat.GetChildFileIndex("menudesc.txt");
		if (fileIdx!=-1)
		{
			CGame::s_pCurGame->m_modDat.Childfseek(fileIdx,0,SEEK_SET);
			for (int i=1;i<7;i++)
			{
				CGame::s_pCurGame->m_modDat.Childfgets(mainMenuDesc[i],100,fileIdx);
				if (mainMenuDesc[i][strlen(mainMenuDesc[i])-1]=='\n')
					mainMenuDesc[i][strlen(mainMenuDesc[i])-1]=0;
			}
			for (int i=0;i<4;i++)
			{
				CGame::s_pCurGame->m_modDat.Childfgets(startGameDesc[i],100,fileIdx);
				if (startGameDesc[i][strlen(startGameDesc[i])-1]=='\n')
					startGameDesc[i][strlen(startGameDesc[i])-1]=0;
			}
			for (int i=1;i<8;i++)
			{
				CGame::s_pCurGame->m_modDat.Childfgets(optionMenuDesc[i],100,fileIdx);
				if (optionMenuDesc[i][strlen(optionMenuDesc[i])-1]=='\n')
					optionMenuDesc[i][strlen(optionMenuDesc[i])-1]=0;
			}
			for (int i=0;i<4;i++)
			{
				CGame::s_pCurGame->m_modDat.Childfgets(difficultyDesc[i],100,fileIdx);
				if (difficultyDesc[i][strlen(difficultyDesc[i])-1]=='\n')
					difficultyDesc[i][strlen(difficultyDesc[i])-1]=0;
			}
		}
	}

	m_bMenuOn=false;
	m_curMenu=0;

	m_bQuit=false;
	m_quitCode=0;

	m_flagRollingTama=0;
	CGame::s_pCurGame->m_fpsLimit=60;
	m_curScrFade=0;

	m_bInitWithRollTama=true;
	if (bRollTama==false)
	{
		m_bInitWithRollTama=false;
		m_flagRollingTama=2;
		OnRollTamaEnd();
		if (bSwitchMusic)
		{
			CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat1,"OP.M2");
			th5w::CPMDPlayer::Play();
		}
	}

	m_mainMenuCursorPosition=cursorInitialPos;
	for (int i=0;i<m_mainMenuNItem;i++)
		m_bMainMenuItemEnabled[i]=true;

	m_bMainMenuItemEnabled[1]=false;

	for (int i=0;i<4;i++)
		if (CGame::GVar().m_bExtraPlayable[i])
			m_bMainMenuItemEnabled[1]=true;

	m_optionMenuCursorPosition=0;
	for (int i=0;i<m_optionMenuNItem;i++)
		m_bOptionMenuItemEnabled[i]=true;

	unsigned char pc[]={64,64,64};
	CCommonFunctionGraphic::LoadBinaryImageFile(&m_pMenuImgPractice,pc,"practice.bmp");
	CCommonFunctionGraphic::LoadBinaryImageFile(&m_pMenuImgReplay,pc,"replay.bmp");

	m_lastKeyState=0;
}

void CTitleScreen::OnRollTamaEnd()
{
	CCommonFunctionGraphic::LoadPIFromDat(&m_pLastTamaWithAlpha,m_globalPalette,&CGame::s_pCurGame->m_th5Dat1,"OP2H.PI");

	CCommonFunctionGraphic::LoadPIFromDat(&m_pBGImage,m_globalPalette,&CGame::s_pCurGame->m_th5Dat1,"OP1.PI");
	m_bgFade=0;

	unsigned char tempPalette[3];
	tempPalette[0]=tempPalette[1]=tempPalette[2]=255;
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,
													   "SFT1.CD2",tempPalette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,
													   "SFT2.CD2",tempPalette);
	CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat1,
													   "CAR.CD2",m_globalPalette);
}

int CTitleScreen::Step()
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

	if (m_flagRollingTama!=2)
	{
		if (m_flagRollingTama==0)
		{
			m_tama.Initialize();
			m_flagRollingTama=1;
		}
		if (m_tama.Step()==1)
		{
			m_flagRollingTama=2;
			OnRollTamaEnd();
			CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat1,"OP.M2");
			th5w::CPMDPlayer::Play();
			m_curScrFade=100;
		}
		return 0;
	}

	if (m_curScrFade<100)
	{
		m_curScrFade+=6;
		if (m_curScrFade>100)
			m_curScrFade=100;
	}

	if (m_bgFade<100)
	{
		m_bgFade+=2;
		if (m_bgFade>100)
			m_bgFade=100;
	}

	if (m_bgFade==100&&m_curScrFade==100)
		m_bMenuOn=true;

	ParseKeyEvent();

	return 0;
}

void CTitleScreen::Draw()
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (m_flagRollingTama==1)
	{
		m_tama.Draw();
		return;
	}

	DrawBackground();
	if (m_bMenuOn)
	{
		switch(m_curMenu)
		{
		case 0:
			DrawMainMenuItems();
			DrawMainMenuCursor();
			break;
		case 1:
			DrawOptionMenuItems();
			DrawOptionMenuCursor();
			break;
		default:
		    break;
		}
	}

	CCommonFunctionGraphic::ScreenFade((float)m_curScrFade);
}

void CTitleScreen::DrawBackground()
{
	if (m_bInitWithRollTama)
		glClearColor(m_globalPalette[3]/255.0f,m_globalPalette[4]/255.0f,m_globalPalette[5]/255.0f,1);
	else
		glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	if (m_bgFade<100&&m_bInitWithRollTama)
		m_pLastTamaWithAlpha->Draw(0,278+40);
	m_pBGImage->Draw(0,40,m_bgFade/100.0f);
	if (m_bInitWithRollTama)
		CCommonFunctionGraphic::DrawRectangle(0,0,639,39,
											  m_globalPalette[3]*(float)(100-m_bgFade)/100.0f/255.0f,
											  m_globalPalette[4]*(float)(100-m_bgFade)/100.0f/255.0f,
											  m_globalPalette[5]*(float)(100-m_bgFade)/100.0f/255.0f);
	else
		CCommonFunctionGraphic::DrawRectangle(0,440,639,479,
											  m_globalPalette[3]*(float)(m_bgFade)/100.0f/255.0f,
											  m_globalPalette[4]*(float)(m_bgFade)/100.0f/255.0f,
											  m_globalPalette[5]*(float)(m_bgFade)/100.0f/255.0f);
}

void CTitleScreen::DrawMainMenuItems()
{
	for (int i=0;i<m_mainMenuNItem;i++)
	{
		float menuItemColor[3];
		int itemColorIdx;
		if (i==m_mainMenuCursorPosition)
			itemColorIdx=MENU_ITEM_COLOR_HIGHLIGHTED;
		else
			itemColorIdx=MENU_ITEM_COLOR_NORMAL;
		if (m_bMainMenuItemEnabled[i]==false)
			itemColorIdx=MENU_ITEM_COLOR_DISABLED;
		menuItemColor[0]=m_globalPalette[itemColorIdx*3]/255.0f;
		menuItemColor[1]=m_globalPalette[itemColorIdx*3+1]/255.0f;
		menuItemColor[2]=m_globalPalette[itemColorIdx*3+2]/255.0f;
		if (i==MAIN_MENU_REPLAY)
		{
			if (m_pMenuImgReplay!=NULL)
				m_pMenuImgReplay->DrawColorScaled((float)272,(float)230+i*20+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
		}
		else
		{
			int idx=10+i;
			if (i>MAIN_MENU_REPLAY)
				idx--;
			m_spriteArray.GetImagePtr(idx)->DrawColorScaled((float)272,(float)230+i*20+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
		}
	}

	if (m_mainMenuCursorPosition!=0)
		DrawDescString(mainMenuDesc[m_mainMenuCursorPosition]);
	else
		DrawDescString(startGameDesc[CGame::GVar().m_initDifficulty]);
}

void CTitleScreen::DrawDescString(char *descStr)
{
	CPC98Font::DrawString(descStr,1000,624-(int)strlen(descStr)*8,384+40,
						  m_globalPalette[9*3]/255.0f,m_globalPalette[9*3+1]/255.0f,m_globalPalette[9*3+2]/255.0f);
	CPC98Font::DrawString(descStr,1000,624-(int)strlen(descStr)*8-1,384+40,
						  m_globalPalette[9*3]/255.0f,m_globalPalette[9*3+1]/255.0f,m_globalPalette[9*3+2]/255.0f);
}

void CTitleScreen::DrawOptionMenuItems()
{
	for (int i=0;i<m_optionMenuNItem;i++)
	{
		float menuItemColor[3];
		int itemColorIdx;
		if (i==m_optionMenuCursorPosition)
			itemColorIdx=MENU_ITEM_COLOR_HIGHLIGHTED;
		else
			itemColorIdx=MENU_ITEM_COLOR_NORMAL;
		if (m_bOptionMenuItemEnabled[i]==false)
			itemColorIdx=MENU_ITEM_COLOR_DISABLED;
		menuItemColor[0]=m_globalPalette[itemColorIdx*3]/255.0f;
		menuItemColor[1]=m_globalPalette[itemColorIdx*3+1]/255.0f;
		menuItemColor[2]=m_globalPalette[itemColorIdx*3+2]/255.0f;
		switch(i)
		{
		case 0:
			m_spriteArray.GetImagePtr(16)->DrawColorScaled((float)224,(float)250+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			{
			int dif=CGame::GVar().m_initDifficulty;
			m_spriteArray.GetImagePtr(21+dif)->DrawColorScaled((float)320,(float)250+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			}
			break;
		case 1:
			m_spriteArray.GetImagePtr(17)->DrawColorScaled((float)224,(float)266+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			{
			int life=CGame::GVar().m_nInitLife;
			m_spriteArray.GetImagePtr(life)->DrawColorScaled((float)320,(float)266+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			}
			break;
		case 2:
			m_spriteArray.GetImagePtr(18)->DrawColorScaled((float)224,(float)282+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			{
			int bomb=CGame::GVar().m_nInitBomb;
			m_spriteArray.GetImagePtr(bomb)->DrawColorScaled((float)320,(float)282+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			}
		    break;
		case 3:
			m_spriteArray.GetImagePtr(19)->DrawColorScaled((float)224,(float)298+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			m_spriteArray.GetImagePtr(26)->DrawColorScaled((float)320,(float)298+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
		    break;
		case 4:
			m_spriteArray.GetImagePtr(20)->DrawColorScaled((float)224,(float)314+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			m_spriteArray.GetImagePtr(30)->DrawColorScaled((float)320,(float)314+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			break;
		case 5:
			m_spriteArray.GetImagePtr(32)->DrawColorScaled((float)272,(float)330+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
			break;
		case 6:
			m_spriteArray.GetImagePtr(31)->DrawColorScaled((float)272,(float)346+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
		    break;
		case 7:
			m_spriteArray.GetImagePtr(15)->DrawColorScaled((float)272,(float)366+40,menuItemColor[0],menuItemColor[1],menuItemColor[2]);
		    break;
		default:
		    break;
		}
	}

	if (m_optionMenuCursorPosition!=0)
		DrawDescString(optionMenuDesc[m_optionMenuCursorPosition]);
	else
		DrawDescString(difficultyDesc[CGame::GVar().m_initDifficulty]);
}

void CTitleScreen::DrawMainMenuCursor()
{
	m_spriteArray.GetImagePtr(35)->Draw((float)256,(float)230+m_mainMenuCursorPosition*20+40);
	m_spriteArray.GetImagePtr(36)->Draw((float)352,(float)230+m_mainMenuCursorPosition*20+40);
}

void CTitleScreen::DrawOptionMenuCursor()
{
	int drawX=224;
	if (m_optionMenuCursorPosition>=5)
		drawX=256;

	int drawY=250+16*m_optionMenuCursorPosition+40;
	if (m_optionMenuCursorPosition==7)
		drawY+=4;

	m_spriteArray.GetImagePtr(35)->Draw((float)drawX,(float)drawY);

	if (m_optionMenuCursorPosition>=5)
		drawX=352;
	else
		drawX=384;
	m_spriteArray.GetImagePtr(36)->Draw((float)drawX,(float)drawY);
}

void CTitleScreen::MoveCursorInMenu(int *cursorPos,int deltaPos,int nItem,bool *bItemMask)
{
	(*cursorPos)+=deltaPos;
	if ((*cursorPos)<0)
		(*cursorPos)+=nItem;
	if ((*cursorPos)>=nItem)
		(*cursorPos)-=nItem;
	while(bItemMask[(*cursorPos)]==false)
	{
		(*cursorPos)+=deltaPos;
		if ((*cursorPos)<0)
			(*cursorPos)+=nItem;
		if ((*cursorPos)>=nItem)
			(*cursorPos)-=nItem;
	}
}

void CTitleScreen::ParseKeyEvent()
{
	if (m_bMenuOn)
	{
		if (m_curMenu==0)
		{
			unsigned short curKeyState=CCommonFunctionInput::GetAllKeyState();
			if (CCommonFunctionInput::UpPressed(curKeyState,m_lastKeyState))
			{
				MoveCursorInMenu(&m_mainMenuCursorPosition,-1,m_mainMenuNItem,m_bMainMenuItemEnabled);
				CGame::s_pCurGame->m_soundEffect.PlaySound(1);
				m_lastKeyState=curKeyState;
				return;
			}
			if (CCommonFunctionInput::DownPressed(curKeyState,m_lastKeyState))
			{
				MoveCursorInMenu(&m_mainMenuCursorPosition,1,m_mainMenuNItem,m_bMainMenuItemEnabled);
				CGame::s_pCurGame->m_soundEffect.PlaySound(1);
				m_lastKeyState=curKeyState;
				return;
			}
			if (CCommonFunctionInput::ZPressed(curKeyState,m_lastKeyState))
			{
				CGame::s_pCurGame->m_soundEffect.PlaySound(11);
				switch(m_mainMenuCursorPosition)
				{
				case MAIN_MENU_START:
					CGame::GVar().m_playDifficulty=CGame::GVar().m_initDifficulty;
					CGame::GVar().m_playStage=0;
					CGame::GVar().m_bPracticeMode=false;
					CGame::GVar().m_bReplayMode=false;
					m_bQuit=true;
					m_quitCode=TITLESCREEN_END_START_GAME;
					break;
				case MAIN_MENU_EXTRA_START:
					CGame::GVar().m_playDifficulty=4;
					CGame::GVar().m_playStage=6;
					CGame::GVar().m_bPracticeMode=false;
					CGame::GVar().m_bReplayMode=false;
					m_bQuit=true;
					m_quitCode=TITLESCREEN_END_START_EXTRA;
					break;
				case MAIN_MENU_OPTION:
					m_curMenu=1;
					m_optionMenuCursorPosition=0;
				    break;
				case MAIN_MENU_QUIT:
					m_bQuit=true;
					m_quitCode=TITLESCREEN_END_QUIT_GAME;
				    break;
				case MAIN_MENU_REPLAY:
					m_bQuit=true;
					m_quitCode=TITLESCREEN_END_CHOOSE_REPLAY;
					break;
				case MAIN_MENU_HISCORE:
					m_bQuit=true;
					m_quitCode=TITLESCREEN_END_VIEW_HIGHSCORE;
					break;
				}
				m_lastKeyState=curKeyState;
				return;
			}
			if (CCommonFunctionInput::ESCPressed(curKeyState,m_lastKeyState))
			{
				m_bQuit=true;
				m_quitCode=TITLESCREEN_END_QUIT_GAME;
				m_lastKeyState=curKeyState;
				return;
			}
			m_lastKeyState=curKeyState;
			return;
		}
		if (m_curMenu==1)
		{
			unsigned short curKeyState=CCommonFunctionInput::GetAllKeyState();
			if (CCommonFunctionInput::UpPressed(curKeyState,m_lastKeyState))
			{
				MoveCursorInMenu(&m_optionMenuCursorPosition,-1,m_optionMenuNItem,m_bOptionMenuItemEnabled);
				CGame::s_pCurGame->m_soundEffect.PlaySound(1);
				m_lastKeyState=curKeyState;
				return;
			}
			if (CCommonFunctionInput::DownPressed(curKeyState,m_lastKeyState))
			{
				MoveCursorInMenu(&m_optionMenuCursorPosition,1,m_optionMenuNItem,m_bOptionMenuItemEnabled);
				CGame::s_pCurGame->m_soundEffect.PlaySound(1);
				m_lastKeyState=curKeyState;
				return;
			}
			if (CCommonFunctionInput::ZPressed(curKeyState,m_lastKeyState))
			{
				if (m_optionMenuCursorPosition!=m_optionMenuNItem-1)
				{
					switch(m_optionMenuCursorPosition)
					{
					case 0:
						ChangeValue(&CGame::GVar().m_initDifficulty,1,0,3);
						break;
					case 1:
						ChangeValue(&CGame::GVar().m_nInitLife,1,1,6);
						break;
					case 2:
						ChangeValue(&CGame::GVar().m_nInitBomb,1,0,3);
						break;
					}
				}
				else
				{
					CGame::s_pCurGame->m_soundEffect.PlaySound(11);
					m_curMenu=0;
				}
				m_lastKeyState=curKeyState;
				return;
			}
			if (CCommonFunctionInput::ESCPressed(curKeyState,m_lastKeyState))
			{
				m_curMenu=0;
				m_lastKeyState=curKeyState;
				return;
			}
			if (CCommonFunctionInput::LeftPressed(curKeyState,m_lastKeyState))
			{
				switch(m_optionMenuCursorPosition)
				{
				case 0:
					ChangeValue(&CGame::GVar().m_initDifficulty,-1,0,3);
					break;
				case 1:
					ChangeValue(&CGame::GVar().m_nInitLife,-1,1,6);
					break;
				case 2:
					ChangeValue(&CGame::GVar().m_nInitBomb,-1,0,3);
					break;
				}
				m_lastKeyState=curKeyState;
				return;
			}
			if (CCommonFunctionInput::RightPressed(curKeyState,m_lastKeyState))
			{
				switch(m_optionMenuCursorPosition)
				{
				case 0:
					ChangeValue(&CGame::GVar().m_initDifficulty,1,0,3);
					break;
				case 1:
					ChangeValue(&CGame::GVar().m_nInitLife,1,1,6);
					break;
				case 2:
					ChangeValue(&CGame::GVar().m_nInitBomb,1,0,3);
					break;
				}
				m_lastKeyState=curKeyState;
				return;
			}
			m_lastKeyState=curKeyState;
			return;
		}
	}
	unsigned short curKeyState=CCommonFunctionInput::GetAllKeyState();
	m_lastKeyState=curKeyState;
}

}























