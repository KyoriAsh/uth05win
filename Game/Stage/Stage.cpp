#include ".\stage.h"
#include "../Game.h"
#include "Chara/Chara.h"
#include "CharaBullet/CharaBullet.h"
#include "EnemyBullet/EnemyBullet.h"
#include "GatherEffect/GatherEffect.h"
#include "SparkEffect/SparkEffect.h"
#include "PopupNumber/PopupNumber.h"
#include "Circle/Circle.h"
#include "Item/Item.h"
#include "FloatingText/FloatingText.h"
#include "Background/Background.h"
#include "Enemy/Enemy.h"
#include "MidBoss/MidBoss.h"
#include "TextOverlay/TextOverlay.h"
#include "Dialogue/Dialogue.h"
#include "boss/Boss.h"
#include "Laser/Laser.h"
#include "Summary/Summary.h"
#include "Stage2BG/Stage2BG.h"

#include "Chara/CharaReimu.h"
#include "Chara/CharaMarisa.h"
#include "Chara/CharaMima.h"
#include "Chara/CharaYuka.h"
#include "MidBoss/MidBossStage1.h"
#include "MidBoss/MidBossStage2.h"
#include "MidBoss/MidBossStage3.h"
#include "MidBoss/MidBossStage4.h"
#include "MidBoss/MidBossStage5.h"
#include "MidBoss/MidBossStageEx.h"
#include "Boss/BossStage1.h"
#include "Boss/BossStage2.h"
#include "Boss/BossStage3.h"
#include "Boss/BossStage4.h"
#include "Boss/BossStage5.h"
#include "Boss/BossStage6.h"
#include "Boss/BossStageEx.h"
#include "../../gameLib/PMDPlayer/PMDPlayer.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include "../../gameLib/Graphic/PC98Font.h"

#define SAFE_DELETE(x) {if (x) delete x;x=NULL;}

namespace th5w{

CStage::CStage(void)
{
	m_pChara=NULL;
	m_pCharaBullet=NULL;
	m_pEnemyBullet=NULL;
	m_pGatherEffect=NULL;
	m_pSparkEffect=NULL;
	m_pPopupNumber=NULL;
	m_pCircle=NULL;
	m_pItem=NULL;
	m_pFloatingText=NULL;
	m_pBackground=NULL;
	m_pEnemy=NULL;
	m_pTextOverlay=NULL;
	m_pDialogue=NULL;
	m_pLaser=NULL;
	m_pSummary=NULL;
	m_pStage2BG=NULL;

	m_pMidBoss=NULL;
	m_pBoss=NULL;

	m_pStageRes=NULL;

	m_pauseCursorPos=0;
}

CStage::~CStage(void)
{
	SAFE_DELETE(m_pChara);
	SAFE_DELETE(m_pCharaBullet);
	SAFE_DELETE(m_pEnemyBullet);
	SAFE_DELETE(m_pGatherEffect);
	SAFE_DELETE(m_pSparkEffect);
	SAFE_DELETE(m_pPopupNumber);
	SAFE_DELETE(m_pCircle);
	SAFE_DELETE(m_pItem);
	SAFE_DELETE(m_pFloatingText);
	SAFE_DELETE(m_pBackground);
	SAFE_DELETE(m_pEnemy);
	SAFE_DELETE(m_pTextOverlay);
	SAFE_DELETE(m_pDialogue);
	SAFE_DELETE(m_pLaser);
	SAFE_DELETE(m_pSummary);
	SAFE_DELETE(m_pStage2BG);

	SAFE_DELETE(m_pMidBoss);
	SAFE_DELETE(m_pBoss);
	SAFE_DELETE(m_pStageRes);
}

void CStage::UploadVarToGlobal()
{
	CGame::GVar().m_nCurLife=m_nCurLife;
	CGame::GVar().m_nCurBomb=m_nCurBomb;
	CGame::GVar().m_powerOverflowLevel=m_powerOverflowLevel;
	CGame::GVar().m_curDream=m_curDream;
	CGame::GVar().m_curScore=m_curScore;
	CGame::GVar().m_curHighScore=m_highScore;
	CGame::GVar().m_totalNPointItem=m_totalNPointItem;
	CGame::GVar().m_playerPerformance=m_playerPerformance;
	CGame::GVar().m_nContinueUsed=m_nContinueUsed;
	CGame::GVar().m_curPower=m_pChara->GetPower();
}

void CStage::DownloadVarFromGlobal()
{
	//make sure that after creation of CStage, no rand() calls before this statement,
	//otherwise, replays may work incorrectly!
	srand(CGame::GVar().m_randomSeed[CGame::GVar().m_playStage]);
	//printf("------------------\n");
	//printf("%d\n",CGame::GVar().m_randomSeed[CGame::GVar().m_playStage]);
	//printf("------------------\n");
	m_nCurLife=CGame::GVar().m_nCurLife;
	m_nCurBomb=CGame::GVar().m_nCurBomb;
	m_powerOverflowLevel=CGame::GVar().m_powerOverflowLevel;
	m_curDream=CGame::GVar().m_curDream;
	m_curScore=CGame::GVar().m_curScore;
	m_highScore=CGame::GVar().m_curHighScore;
	if (m_highScore<m_curScore)			//this may occur if we play a replay file from stage 2+
		m_highScore=m_curScore;
	m_curNPointItem=0;
	m_totalNPointItem=CGame::GVar().m_totalNPointItem;
	m_curNGraze=0;
	m_playerPerformance=CGame::GVar().m_playerPerformance;
	m_nContinueUsed=CGame::GVar().m_nContinueUsed;

	m_nContinueLimit=CGame::GVar().m_nContinueLimit;
	m_pointItemMaxScore=CGame::GVar().m_pointItemMaxScore;
	m_grazeBonus=CGame::GVar().m_grazeBonus;
	m_playerPerformanceLowerBound=CGame::GVar().m_playerPerformanceLowerBound;
	m_playerPerformanceUpperBound=CGame::GVar().m_playerPerformanceUpperBound;

	m_nLifeOnContinue=CGame::GVar().m_nLifeOnContinue;
	m_nBombOnMiss=CGame::GVar().m_nBombOnMiss;
	if (CGame::GVar().m_playDifficulty==4)
	{
		m_nLifeOnContinue=3;
		m_nBombOnMiss=3;
	}

	//curPower is given to m_pChara in InitChara
}

void CStage::InitChara()
{
	switch(CGame::GVar().m_playChara)
	{
	case 0:
		m_pChara=new CCharaReimu;
		break;
	case 1:
		m_pChara=new CCharaMarisa;
		break;
	case 2:
		m_pChara=new CCharaMima;
		break;
	case 3:
		m_pChara=new CCharaYuka;
		break;
	}
	m_pChara->m_pStage=this;

	m_pChara->m_moveAreaXLow=128.0f;
	m_pChara->m_moveAreaXHigh=6016.0f;
	m_pChara->m_moveAreaYLow=128.0f;
	m_pChara->m_moveAreaYHigh=5632.0f;

	m_pChara->m_curX=3072;
	m_pChara->m_curY=5120;

	m_pChara->m_bBombDisabled=false;
	m_pChara->m_bCharaCanShoot=true;
	m_pChara->m_bCharaCanMove=true;
	m_pChara->m_bMiss=false;
	m_pChara->m_invulnerableTimeLeft=64;
	m_pChara->m_cannotControlTimeLeft=0;
	m_pChara->m_nextLifeComeTimeLeft=0;
	m_pChara->m_charaShootTimer=0;
	m_pChara->m_power=CGame::GVar().m_curPower;		//assign twice to avoid floating text "FullPower"
	m_pChara->SetPower(CGame::GVar().m_curPower);
//	m_pChara->m_power=128;					//TODO: remove these two lines which are only for test use
//	m_pChara->SetPower(128);
	m_pChara->m_getPointItemPerformance=0;
}

void CStage::Initialize()
{
	if (CGame::GVar().m_bReplayMode==false)
		CGame::GVar().m_replay.OpenStage(CGame::GVar().m_playStage,false);
	else
		CGame::GVar().m_replay.OpenStage(CGame::GVar().m_playStage,true);

	m_bQuit=false;

	m_bLogo=true;
	m_logoFrame=0;

	m_bPaused=false;
	m_bCanPause=true;
	m_bContinueScr=false;

	m_curFrame=-1;					//assign -1 so that when Step() is called the first time,
									//its first statement, which is m_curFrame++ will make it 0

	m_bInCharaBomb=false;
	m_bInCharaBombForItem=false;
	m_bInBossBattle=false;

	m_nMiss=0;
	m_nBomb=0;

	DownloadVarFromGlobal();
	m_drawScore=m_curScore;
	m_drawScoreDelta=0;
	m_drawHighScore=m_highScore;

	m_playAreaUpperLeftX=CGame::s_pCurGame->m_playArea.left;
	m_playAreaUpperLeftY=CGame::s_pCurGame->m_playArea.top;

	m_pStageRes=new CStageResource(this);

	InitChara();
	m_pCharaBullet=new CCharaBullet(this);
	m_pEnemyBullet=new CEnemyBullet(this);
	m_pGatherEffect=new CGatherEffect(this);
	m_pSparkEffect=new CSparkEffect(this);
	m_pPopupNumber=new CPopupNumber(this);
	m_pCircle=new CCircle(this);

	m_pItem=new CItem(this);
	m_pFloatingText=new CFloatingText(this);
	m_pBackground=new CBackground(this);
	m_pEnemy=new CEnemy(this);
	m_pTextOverlay=new CTextOverlay(this);
	m_pDialogue=new CDialogue(this);
	m_pLaser=new CLaser(this);
	m_pSummary=new CSummary(this);
	if (CGame::GVar().m_playStage==1)
		m_pStage2BG=new CStage2BG(this);

	m_curScrFade=100;

	memset(m_bPlaySound,0,sizeof(m_bPlaySound));

	//CPMDPlayer::Pause();

	CGame::s_pCurGame->m_fpsLimit=56;

	m_lastGameKeyState=0;
	m_curGameKeyState=0;
}

void CStage::StepScore()
{
	if (m_drawScore>m_curScore)
	{
		m_drawScore=m_curScore;
		return;
	}
	if (m_drawScore==m_curScore)
		return;
	if (m_drawScoreDelta>m_curScore-m_drawScore)
		m_drawScoreDelta=m_curScore-m_drawScore;
	int curDelta=(m_curScore-m_drawScore)/32;
	if (curDelta==0) curDelta=1;
	if (curDelta>6111) curDelta=6111;
	if (m_drawScoreDelta<curDelta)
		m_drawScoreDelta=curDelta;
	m_drawScore+=m_drawScoreDelta;
	if (m_drawScore>m_drawHighScore)
		m_drawHighScore=m_drawScore;

	//unlike PC98 ver, we don't trigger "hiscore" floating text here, because this may cause incorrect "hiscore"
	//during replay.
}

void CStage::CheckMidBoss()
{
	//for stage 6(index 5), PC98 ver uses 30000. However, 30000 frame just lasts 9 minutes.
	//To be safe, it is changed to 300000 here.
	const int midBossAppearFrame[]={2500,2750,5750,3900,4800,300000,5800};
	if (m_curFrame==midBossAppearFrame[CGame::GVar().m_playStage])
	{
		switch(CGame::GVar().m_playStage)
		{
		case 0:
			m_pMidBoss=new CMidBossStage1;
			break;
		case 1:
			m_pMidBoss=new CMidBossStage2;
			break;
		case 2:
			m_pMidBoss=new CMidBossStage3;
		    break;
		case 3:
			m_pMidBoss=new CMidBossStage4;
		    break;
		case 4:
			m_pMidBoss=new CMidBossStage5;
			break;
		case 6:
			m_pMidBoss=new CMidBossStageEx;
			break;
		}
		if (m_pMidBoss!=NULL)
		{
			m_pMidBoss->m_pStage=this;
			m_pMidBoss->Initialize();
			m_pEnemy->m_noAddEnemy=true;
		}
	}
}

void CStage::CheckBoss()
{
	if (m_pBackground->GetCurScrollSpeed()==0)
	{
		if (m_bInBossBattle==false)
		{
			m_bInBossBattle=true;
			m_pDialogue->Run();
		}
		else
			if (m_pDialogue->IsRunning()==false&&m_pBoss==NULL)
			{
				switch(CGame::GVar().m_playStage)
				{
				case 0:
					m_pBoss=new CBossStage1;
					break;
				case 1:
					m_pBoss=new CBossStage2;
					break;
				case 2:
					m_pBoss=new CBossStage3;
					break;
				case 3:
					m_pBoss=new CBossStage4;
					break;
				case 4:
					m_pBoss=new CBossStage5;
					break;
				case 5:
					m_pBoss=new CBossStage6;
					break;
				case 6:
					m_pBoss=new CBossStageEx;
					break;
				}
				if (m_pBoss!=NULL)
				{
					m_pBoss->m_pStage=this;
					m_pBoss->Initialize();
				}
				m_pTextOverlay->SetMode(TEXT_OVERLAY_DRAW_STAGE_NAME_BGM_NAME,NULL,m_pStageRes->m_pStageStd->m_stageBossBGMName);
			}
	}
}

int CStage::Step()
{
	CGame::s_pCurGame->m_fpsLimit=56;
	CGame::s_pCurGame->SetVSYNC(true);
	if (m_bQuit)
	{
		if (m_nQuitFrameLeft>0)
		{
			m_nQuitFrameLeft--;
			CPMDPlayer::SetVolume((float)(m_nQuitFrameLeft*2));
			m_curScrFade-=2;
			if (m_curScrFade<=0)
				m_curScrFade=0;
			return 0;
		}
		CPMDPlayer::Pause();
		CPMDPlayer::SetVolume(100);
		UploadVarToGlobal();
		//make sure to upload var before close stage
		//otherwise, replay will not get the correct score
		if (m_quitCode==STAGE_END_CLEAR)
			CGame::GVar().m_replay.CloseStage(true);
		else
			CGame::GVar().m_replay.CloseStage(false);
		if (CGame::GVar().m_bReplayMode)
			if (m_quitCode!=STAGE_END_CLEAR||CGame::GVar().m_playStage>=5)
				m_quitCode=STAGE_END_REPLAY_END;
		return m_quitCode;
	}

	if (m_bLogo)
	{
		if (CGame::GVar().m_bReplayMode)
			if (CCommonFunctionInput::LCtrlKey(CCommonFunctionInput::GetAllKeyState()))
				CGame::s_pCurGame->m_fpsLimit=1000;
		if (StepLogo()==0)
			return 0;
		m_bLogo=false;

		char bgmFileName[]="ST00.M2";
		bgmFileName[3]=CGame::GVar().m_playStage+48;
		CPMDPlayer::UnloadPMDData();
		CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat2,bgmFileName);
		CPMDPlayer::SetVolume(100);
		CPMDPlayer::Play();

		m_bCanPause=true;
	}

	if (m_bPaused)
	{
		StepPause();
		return 0;
	}

	if (m_bContinueScr)
	{
		StepContinue();
		return 0;
	}

	if (CGame::GVar().m_bReplayMode==false)
	{
		m_lastGameKeyState=m_curGameKeyState;
		m_curGameKeyState=CCommonFunctionInput::GetAllKeyState();
		m_lastMenuKeyState=m_lastGameKeyState;
		m_curMenuKeyState=m_curGameKeyState;
		if (CCommonFunctionInput::ESCPressed(m_curMenuKeyState,m_lastMenuKeyState))
		{
			if (Pause())
				return 0;
		}
	}
	else
	{
		m_lastMenuKeyState=m_curMenuKeyState;
		m_curMenuKeyState=CCommonFunctionInput::GetAllKeyState();
		if (CCommonFunctionInput::ESCPressed(m_curMenuKeyState,m_lastMenuKeyState))
		{
			if (Pause())
				return 0;
		}

		m_lastGameKeyState=m_curGameKeyState;
		if (CGame::GVar().m_replay.GetNextData(&m_curGameKeyState)==false)
		{
			SetQuit(STAGE_END_REPLAY_END);
			return 0;
		}
	}

	if (CGame::GVar().m_bReplayMode==false)
		CGame::GVar().m_replay.PutNextdata(m_curGameKeyState);

	if (CGame::GVar().m_bReplayMode)
		if (CCommonFunctionInput::LCtrlKey(m_curMenuKeyState))
		{
			CGame::s_pCurGame->SetVSYNC(false);
			CGame::s_pCurGame->m_fpsLimit=1000;
		}

	if (m_pDialogue->Step()==0)
		return 0;

	m_curFrame++;

	if (m_curFrame>0)
	{
		m_pBackground->Step();

		for (int i=1;i<=15;i++)
			if (m_bPlaySound[i])
				CGame::s_pCurGame->m_soundEffect.PlaySound(i);
		memset(m_bPlaySound,0,sizeof(m_bPlaySound));

		if (m_curFrame%4096==0)
			ChangePlayerPerformance(1);

		StepScore();
	}

	CheckBoss();
	CheckMidBoss();

	m_pEnemy->StepEnemyTable();

	m_pPopupNumber->Step();
	m_pCircle->Step();
	m_pSparkEffect->Step();
	m_pChara->Step();
	m_pCharaBullet->Step();
	m_pLaser->Step();
	m_pEnemyBullet->Step();
	m_pEnemy->Step();
	if (m_pMidBoss)
		if (m_pMidBoss->Step()!=0)
		{
			delete m_pMidBoss;
			m_pMidBoss=NULL;
		}
	if (m_pBoss)
		m_pBoss->Step();
	m_pSummary->Step();
	m_pItem->Step();
	m_pGatherEffect->Step();
	if (m_pStage2BG)
		m_pStage2BG->Step();
	if (m_bInCharaBomb)
		m_pChara->StepBomb();
	m_pTextOverlay->Step();
	m_pFloatingText->Step();

	return 0;
}

void CStage::Draw()
{
	if (m_bLogo)
	{
		DrawLogo();
		return;
	}

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (m_pBoss==NULL)
	{
		m_pBackground->Draw();
		if (m_pStage2BG)
			m_pStage2BG->Draw();
	}
	else
		m_pBoss->DrawBackground();

	if (m_bInCharaBomb)
		m_pChara->DrawBomb();

	if (m_pBoss)
		m_pBoss->Draw();
	if (m_pMidBoss)
		m_pMidBoss->Draw();
	m_pEnemy->Draw();

	m_pCharaBullet->Draw();
	m_pChara->Draw();
	if (m_pBoss)
		m_pBoss->DrawSpecialBullet();
	m_pLaser->Draw();
	m_pGatherEffect->Draw();
	m_pSparkEffect->Draw();
	m_pItem->Draw();
	m_pPopupNumber->Draw();
	m_pEnemyBullet->Draw();
	m_pCircle->Draw();
	m_pChara->DrawCollisionArea();
	m_pTextOverlay->Draw();
	m_pDialogue->Draw();

	//cut pixels drawn outside play area
	CCommonFunctionGraphic::DrawRectangle(0,0,639,(float)m_playAreaUpperLeftY-1,0,0,0);
	CCommonFunctionGraphic::DrawRectangle(0,(float)m_playAreaUpperLeftY+368,639,479,0,0,0);
	CCommonFunctionGraphic::DrawRectangle(0,0,(float)m_playAreaUpperLeftX-1,479,0,0,0);
	CCommonFunctionGraphic::DrawRectangle((float)m_playAreaUpperLeftX+384,0,639,479,0,0,0);

	PlayAreaFade((float)m_curScrFade);

	m_pFloatingText->Draw();
	m_pSummary->Draw();

	DrawStatistics();

	if (m_bPaused)
		DrawPause();

	if (m_bContinueScr)
		DrawContinue();

	//CCommonFunctionGraphic::ScreenFade((float)m_curScrFade);
}

void CStage::EnableContinueScreen()
{
	if (m_bContinueScr==false)
	{
		m_bContinueScr=true;
		m_continueScrCurPart=0;
		m_continueScrTimer=32;
	}
}

void CStage::StepContinue()
{
	m_lastMenuKeyState=m_curMenuKeyState;
	m_curMenuKeyState=CCommonFunctionInput::GetAllKeyState();
	switch(m_continueScrCurPart)
	{
	case 0:
		if (m_continueScrTimer>0)
			m_continueScrTimer--;
		else
			m_continueScrCurPart++;
		break;
	case 1:
		if (m_continueScrTimer==0)
			m_curScrFade=50;
		if (m_continueScrTimer<31)
			m_continueScrTimer++;
		else
		{
			m_continueScrCurPart++;
			m_continueStrX=52;
		}
		break;
	case 2:
		if (m_continueStrX>8)
			m_continueStrX-=2;
		else
		{
			m_continueScrCurPart++;
			m_continueStrX=8;
		}
		break;
	case 3:
		if (m_continueStrX<20)
			m_continueStrX+=2;
		else
		{
			m_continueScrCurPart++;
			m_continueStrX=20;
		}
		break;
	case 4:
		if (CCommonFunctionInput::ZPressed(m_curMenuKeyState,m_lastMenuKeyState))
		{
			m_continueScrCurPart++;
			m_continueCursorPos=0;
			m_continueScrTimer=32;
			if (CGame::GVar().m_playStage==6||m_nContinueLimit-m_nContinueUsed<=0)
			{
				m_continueScrCurPart++;
				m_bContinued=false;
			}
		}
		break;
	case 5:
		if (CCommonFunctionInput::UpPressed(m_curMenuKeyState,m_lastMenuKeyState)
			||CCommonFunctionInput::DownPressed(m_curMenuKeyState,m_lastMenuKeyState))
			m_continueCursorPos=1-m_continueCursorPos;
		if (CCommonFunctionInput::ZPressed(m_curMenuKeyState,m_lastMenuKeyState))
		{
			m_bContinued=(m_continueCursorPos==0);
			m_continueScrCurPart++;
		}
		break;
	case 6:
		if (m_continueScrTimer==32)
			if (m_bContinued==false)
			{
				SetQuit(STAGE_END_ALL_MISSED);
				m_bContinueScr=false;
			}
		if (m_continueScrTimer>0)
			m_continueScrTimer--;
		else
			m_continueScrCurPart++;
		break;
	case 7:
		if (m_continueScrTimer==0)
			m_curScrFade=100;
		if (m_continueScrTimer<31)
			m_continueScrTimer++;
		else
		{
			m_bContinueScr=false;
			m_pChara->SetPower(1);
			SetDream(1);
			m_nCurLife=m_nLifeOnContinue;
			m_nBomb=m_nBombOnMiss;
			m_curScore=0;
			//unlike PC98 ver, we do not update global highscore here.
			//like PC98 ver, we do not update graze here
			m_nContinueUsed++;
			m_pChara->EnableDraw();

			CGame::GVar().m_bCanSaveReplay=false;
		}
		break;
	}
}

void CStage::DrawContinue()
{
	switch(m_continueScrCurPart)
	{
	case 0:
	case 1:
	case 6:
	case 7:
		{
			int fontIdx=0x40-1-m_continueScrTimer/4;
			unsigned char s[25];
			for (int i=0;i<24;i++)
				s[i]=fontIdx;
			s[24]=0;
			for (int i=0;i<23;i++)
				CTh5ExtFont::DrawExtString(s,24,m_playAreaUpperLeftX,i*16+m_playAreaUpperLeftY,0,0,0);
		}
		break;
	case 2:
	case 3:
		CTh5ExtFont::DrawExtChar(0xb0,m_continueStrX*8-32+m_playAreaUpperLeftX,0x0c*16-16+m_playAreaUpperLeftY,1,1,1);
		break;
	case 4:
		{
			unsigned char extGameOver[]={0xb0,0xaa,0xb6,0xae,0xb8,0xbf,0xae,0xbb,0};
			CTh5ExtFont::DrawExtString(extGameOver,100,0x14*8-32+m_playAreaUpperLeftX,0x0c*16-16+m_playAreaUpperLeftY,1,1,1);
		}
		break;
	case 5:
		{
			unsigned char extContinue[]={0xac,0xb8,0xb7,0xbd,0xb2,0xb7,0xbe,0xae,0x8,0};
			CTh5ExtFont::DrawExtString(extContinue,100,0x13*8-32+m_playAreaUpperLeftX,0x0a*16-16+m_playAreaUpperLeftY,1,1,1);
			unsigned char extYes[]={0xc2,0xae,0xbc,0};
			unsigned char extNo[]={0xb7,0xb8,0};
			if (m_continueCursorPos==0)
			{
				CTh5ExtFont::DrawExtString(extYes,100,0x18*8-32+m_playAreaUpperLeftX,0x0d*16-16+m_playAreaUpperLeftY,0,1,0,1.0,true,true);
				CTh5ExtFont::DrawExtString(extNo,100,0x19*8-32+m_playAreaUpperLeftX,0x0f*16-16+m_playAreaUpperLeftY,1,1,1);
			}
			else
			{
				CTh5ExtFont::DrawExtString(extYes,100,0x18*8-32+m_playAreaUpperLeftX,0x0d*16-16+m_playAreaUpperLeftY,1,1,1);
				CTh5ExtFont::DrawExtString(extNo,100,0x19*8-32+m_playAreaUpperLeftX,0x0f*16-16+m_playAreaUpperLeftY,0,1,0,1.0,true,true);
			}
			unsigned char extCredit[]={0xac,0xbb,0xae,0xad,0xb2,0xbd,0};
			CTh5ExtFont::DrawExtString(extCredit,100,0x13*8-32+m_playAreaUpperLeftX,0x16*16-16+m_playAreaUpperLeftY,0,1,0);
			CTh5ExtFont::DrawExtChar(0xa0+m_nContinueLimit-m_nContinueUsed,0x21*8-32+m_playAreaUpperLeftX,0x16*16-16+m_playAreaUpperLeftY,0,1,0);
		}
		break;
	}
}

int CStage::StepLogo()
{
	m_logoFrame++;
	if (m_logoFrame>=128+17)
		return 1;
	CPMDPlayer::SetVolume(100.0f-m_logoFrame);
	return 0;
}

void CStage::DrawLogo()
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	m_pStageRes->m_pImgLogo->Draw((float)80-32+m_playAreaUpperLeftX,(float)112-16+m_playAreaUpperLeftY);
	if (m_logoFrame<17)
        PlayAreaFade((float)100-(16-m_logoFrame)*6);
	if (m_logoFrame>=128)
		PlayAreaFade((float)100-(m_logoFrame-128)*6);
	DrawStatistics();
}

void CStage::StepPause()
{
	m_lastMenuKeyState=m_curMenuKeyState;
	m_curMenuKeyState=CCommonFunctionInput::GetAllKeyState();
	if (CCommonFunctionInput::ESCPressed(m_curMenuKeyState,m_lastMenuKeyState))
	{
		m_bPaused=false;
		return;
	}
	if (CCommonFunctionInput::ZPressed(m_curMenuKeyState,m_lastMenuKeyState)&&m_pauseCursorPos==0)
	{
		m_bPaused=false;
		return;
	}
	if (CCommonFunctionInput::QPressed(m_curMenuKeyState,m_lastMenuKeyState))
	{
		if (CGame::GVar().m_bReplayMode)
			SetQuit(STAGE_END_REPLAY_END);
		else
			SetQuit(STAGE_END_PLAYER_QUIT);
		m_bPaused=false;
		return;
	}
	if (CCommonFunctionInput::ZPressed(m_curMenuKeyState,m_lastMenuKeyState)&&m_pauseCursorPos==1)
	{
		if (CGame::GVar().m_bReplayMode)
			SetQuit(STAGE_END_REPLAY_END);
		else
			SetQuit(STAGE_END_PLAYER_QUIT);
		m_bPaused=false;
		return;
	}
	if (CCommonFunctionInput::UpPressed(m_curMenuKeyState,m_lastMenuKeyState))
		m_pauseCursorPos=1-m_pauseCursorPos;
	if (CCommonFunctionInput::DownPressed(m_curMenuKeyState,m_lastMenuKeyState))
		m_pauseCursorPos=1-m_pauseCursorPos;
}

void CStage::DrawPause()
{
	if (m_bPaused==false)
		return;

	const unsigned char extPause[]={0xf0,0xf1,0};
	const unsigned char extResume[]={0xf2,0xf3,0};
	const unsigned char extExit[]={0xf4,0xf5,0};
	CTh5ExtFont::DrawExtString(extPause,100,0x1a*8-32+m_playAreaUpperLeftX,0xc*16-16+m_playAreaUpperLeftY,1,1,0);
	if (m_pauseCursorPos==0)
	{
		CTh5ExtFont::DrawExtString(extResume,100,0x1a*8-32+m_playAreaUpperLeftX,0xe*16-16+m_playAreaUpperLeftY,1,1,1);
		CTh5ExtFont::DrawExtString(extExit,100,0x1a*8-32+m_playAreaUpperLeftX,0xf*16-16+m_playAreaUpperLeftY,1,1,0);
	}
	else
	{
		CTh5ExtFont::DrawExtString(extResume,100,0x1a*8-32+m_playAreaUpperLeftX,0xe*16-16+m_playAreaUpperLeftY,1,1,0);
		CTh5ExtFont::DrawExtString(extExit,100,0x1a*8-32+m_playAreaUpperLeftX,0xf*16-16+m_playAreaUpperLeftY,1,1,1);
	}
	float x=(float)(212-32+m_playAreaUpperLeftX);
	float y=(float)(239-16+m_playAreaUpperLeftY+m_pauseCursorPos*16);
	CCommonFunctionGraphic::DrawRectangle(x,y,x+31,y,1,1,1);
}

bool CStage::Pause()
{
	if (m_bCanPause)
	{
		m_bPaused=true;
		m_pauseCursorPos=0;
		return true;
	}
	return false;
}

void CStage::PlayAreaFade(float fadeScale)
{
	if (fadeScale<0) fadeScale=0;
	if (fadeScale>200) fadeScale=200;
	float overlayColor;
	if (fadeScale<100)
		overlayColor=0;
	else
		overlayColor=1;
	float overlayAlpha=fabs(fadeScale-100)/100;
	CCommonFunctionGraphic::DrawRectangle((float)(m_playAreaUpperLeftX),(float)(m_playAreaUpperLeftY),
										  (float)(m_playAreaUpperLeftX+384),(float)(m_playAreaUpperLeftY+368),
										  overlayColor,overlayColor,overlayColor,overlayAlpha);
}


void CStage::DrawStatistics()
{
	//draw score and highscore
	unsigned char extHiScore[]={0xd6,0xd7,0xd8,0xd9,0};
	CTh5ExtFont::DrawExtString(extHiScore,100,0x3c*8,3*16+40,1,1,0);
	unsigned char extScore[]={0xd7,0xd8,0xd9,0};
	CTh5ExtFont::DrawExtString(extScore,100,0x3d*8,5*16+40,1,1,0);
	m_pStageRes->DrawNumber(0x38*8,4*16+40,m_drawHighScore*10,8,true,false,1,1,1);
	m_pStageRes->DrawNumber(0x38*8,6*16+40,m_drawScore*10+m_nContinueUsed,8,true,false,1,1,1);

	if (CGame::GVar().m_playChara==0)
	{
		unsigned char extReigeki[]={0xda,0xdb,0};
		CTh5ExtFont::DrawExtString(extReigeki,100,0x39*8,0xb*16+40,1,1,0);
		unsigned char extReimu[]={0xdc,0xdd,0};
		CTh5ExtFont::DrawExtString(extReimu,100,0x39*8,0xd*16+40,1,1,0);
		unsigned char extReiryoku[]={0xde,0xdf,0};
		CTh5ExtFont::DrawExtString(extReiryoku,100,0x3e*8,0x15*16+40,1,1,0);
	}
	else
	{
		unsigned char extBomb[]={0xe0,0xe1,0};
		CTh5ExtFont::DrawExtString(extBomb,100,0x39*8,0xb*16+40,1,1,0);
		unsigned char extPlayer[]={0xe2,0xe3,0};
		CTh5ExtFont::DrawExtString(extPlayer,100,0x39*8,0xd*16+40,1,1,0);
		unsigned char extPower[]={0xe4,0xe5,0};
		CTh5ExtFont::DrawExtString(extPower,100,0x3e*8,0x15*16+40,1,1,0);
	}

	//draw bomb row
	if (m_nCurBomb>5)
	{
		CPC98Font::DrawString((char*)m_pStageRes->m_crossRow,100,0x3e*8,0xb*16+40,1,1,1);
		m_pStageRes->DrawNumber(0x44*8,0xb*16+40,m_nCurBomb,2,false,true,1,1,1);
	}
	else
	{
		for (int i=0;i<m_nCurBomb;i++)
			CTh5ExtFont::DrawExtChar(0xd3,0x3e*8+i*16,0xb*16+40,1,1,1);
	}
	//draw life row
	if (m_nCurLife>6)
	{
		CPC98Font::DrawString((char*)m_pStageRes->m_crossRow,100,0x3e*8,0xd*16+40,1,1,1);
		m_pStageRes->DrawNumber(0x44*8,0xd*16+40,m_nCurLife-1,2,false,true,1,1,1);
	}
	else
	{
		for (int i=0;i<m_nCurLife-1;i++)
			CTh5ExtFont::DrawExtChar(0xd4,0x3e*8+i*16,0xd*16+40,1,1,1);
	}

	CTh5ExtFont::DrawExtChar(0xe6,0x3a*8,0x10*16+40,1,1,0);
	unsigned char extRuikei[]={0xed,0xee,0};
	CTh5ExtFont::DrawExtString(extRuikei,100,0x39*8,0xf*16+40,0,1,1);
	m_pStageRes->DrawNumber(0x3e*8,0x10*16+40,m_curNPointItem,5,false,true,1,1,1);
	m_pStageRes->DrawNumber(0x3e*8,0xf*16+40,m_totalNPointItem,5,false,true,0,1,1);
	CTh5ExtFont::DrawExtChar(0xe7,0x3f*8,0x13*16+40,1,1,0);			//Ten
	CTh5ExtFont::DrawExtChar(0xe8,0x3a*8,0x12*16+40,1,1,0);			//Dan
	m_pStageRes->DrawNumber(0x3e*8,0x12*16+40,m_curNGraze,5,false,true,1,1,1);

	//draw dream and power gauge
	float powerGaugeColor[][3]={{1,0,0},{1,0,0},{1,0,0},{1,0,1},{1,0,1},{0,0,1},{0,0,1},{0,1,1},{1,1,0},{1,1,1}};
	float dreamGaugeColor[][3]={{1,0,0},{1,0,0},{1,0,1},{1,0,1},{0,0,1},{0,0,1},{0,1,1},{1,1,0},{1,1,1}};
	m_pStageRes->DrawGauge(m_curDream,128,0x38*8,0x14*16+40,dreamGaugeColor[m_curDream/16][0],
			  dreamGaugeColor[m_curDream/16][1],dreamGaugeColor[m_curDream/16][2]);
	m_pStageRes->DrawGauge(m_pChara->GetPower(),128,0x38*8,0x16*16+40,powerGaugeColor[m_pChara->m_powerLevel][0],
			  powerGaugeColor[m_pChara->m_powerLevel][1],powerGaugeColor[m_pChara->m_powerLevel][2]);

	//draw difficulty
	unsigned char difficultyExtStr[][8]={{0xae,0xaa,0xbc,0xc2,2,2,2,0},				//"EASY"
										 {0xb7,0xb8,0xbb,0xb6,0xaa,0xb5,2,0},		//"NORMAL"
										 {0xb1,0xaa,0xbb,0xad,2,2,2,0},				//"HARD"
										 {0xb5,0xbe,0xb7,0xaa,0xbd,0xb2,0xac,0},	//"LUNATIC"
										 {0xae,0xc1,0xbd,0xbb,0xaa,2,2,0}};			//"EXTRA"
	float color[][3]={{0,1,0},{0,1,1},{1,0,1},{1,0,0},{1,0,0}};
	int dif=CGame::GVar().m_playDifficulty;
	CTh5ExtFont::DrawExtString(difficultyExtStr[dif],100,0x39*8,0x17*16+40,color[dif][0],color[dif][1],color[dif][2]);

	if (m_pMidBoss)
		m_pMidBoss->DrawStatistics();
	if (m_pBoss)
		m_pBoss->DrawStatistics();

	//draw fps
	char str[20];
	if (CGame::s_pCurGame->m_fps>100)
        sprintf(str,"FPS:%.1lf",CGame::s_pCurGame->m_fps);
	else
		sprintf(str,"FPS: %.1lf",CGame::s_pCurGame->m_fps);
	CPC98Font::DrawString(str,9,564,460,1,1,1);
}

int CStage::AdjustDamageFromChara(int origDmg, bool isBoss)
{
	//special adjust taken from original PC98 version
	if (isBoss)
	{
		if (CGame::GVar().m_playChara==0||CGame::GVar().m_playChara==3)
		{
			//reimu and yuka's damage to stage extra boss is increased to 5/4
			if (CGame::GVar().m_playStage==6)
				origDmg=(origDmg*5)/4;
			else
			{
				//reimu's damage to normal stage boss is increased to 8/7
				if (CGame::GVar().m_playChara==0)
					origDmg=(origDmg*8)/7;
			}
		}
		else
		{
			//marisa and mima's damage to boss in all stages is reduced to 4/5
			origDmg=(origDmg*4)/5;
		}
	}
	else
	{
		//reimu's damage to non-boss in stage extra is reduced to 4/5
		if (CGame::GVar().m_playStage==6&&CGame::GVar().m_playChara==0)
			origDmg=(origDmg*4)/5;
		//yuka's damage to non-boss in all stages is reduced to 4/5
		if (CGame::GVar().m_playChara==3)
			origDmg=(origDmg*4)/5;
	}
	return origDmg;
}

void CStage::SetQuit(int quitCode)
{
	m_bQuit=true;
	m_nQuitFrameLeft=50;
	m_quitCode=quitCode;
	m_bCanPause=false;
}

void CStage::SetDream(int newDream)
{
	if (newDream>128) newDream=128;
	if (newDream<1) newDream=1;
	if (newDream==128&&m_curDream<=127)
	{
		m_pEnemyBullet->SetClearBulletTimeLeft(20);
		m_pFloatingText->SetTextType(FLOATING_TEXT_DREAMMAX);
	}
	m_curDream=newDream;
}

int CStage::GetDamageFromChara(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss)
{
	int totalDmg=m_pCharaBullet->GetDamage(ctrX,ctrY,halfWidth,halfHeight,isBoss);
	if (m_bInCharaBomb)
		totalDmg+=m_pChara->GetBombDamage(ctrX,ctrY,halfWidth,halfHeight,isBoss);
	totalDmg=AdjustDamageFromChara(totalDmg,isBoss);
	AddScore(totalDmg);
	return totalDmg;//*10;
}

}
















