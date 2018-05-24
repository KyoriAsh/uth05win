#pragma once
#include "../scheme.h"
#include "../../gameLib/Graphic/2DImageArray.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "../../gameLib/CommonFunction/CommonFunctionMusicSE.h"
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/th5DatFile/bb/BBFile.h"
#include "../../gameLib/th5DatFile/mpn/MPNFile.h"
#include "../../gameLib/th5DatFile/std/STDFile.h"
#include "StageResource.h"

#include <math.h>

#ifdef PI
#undef PI
#endif
#define PI 3.1415926535897932384626433832795

#define STAGE_END_CLEAR 1
#define STAGE_END_PLAYER_QUIT 2
#define STAGE_END_ALL_MISSED 3
#define STAGE_END_REPLAY_END 4

namespace th5w{

class CStage : public CScheme
{
friend class CChara;
friend class CCharaReimu;
friend class CCharaMarisa;
friend class CCharaMima;
friend class CCharaYuka;
friend class CCharaBullet;
friend class CEnemyBullet;
friend class CGatherEffect;
friend class CSparkEffect;
friend class CPopupNumber;
friend class CCircle;
friend class CItem;
friend class CFloatingText;
friend class CBackground;
friend class CEnemy;
friend class CTextOverlay;
friend class CDialogue;
friend class CMidBoss;
friend class CMidBossStage1;
friend class CMidBossStage2;
friend class CMidBossStage3;
friend class CMidBossStage4;
friend class CMidBossStage5;
friend class CMidBossStageEx;
friend class CBoss;
friend class CBossStage1;
friend class CBossStage2;
friend class CBossStage3;
friend class CBossStage4;
friend class CBossStage4Mai;
friend class CBossStage4Yuki;
friend class CBossStage5;
friend class CBossStage6;
friend class CBossStageEx;
friend class CLaser;
friend class CSummary;
friend class CStage2BG;
friend class CCurvingBullet;
friend class CLargeBullet;
friend class CKnifeBullet;
friend class CShinkiBullet;

friend class CStageResource;

protected:
	bool m_bInBossBattle;
	bool m_bInCharaBomb;
	bool m_bInCharaBombForItem;		//used by PC98 ver as flag of inBomb, mostly with item functions

//common component that lives along with stage

protected:
	CChara *m_pChara;
	CCharaBullet *m_pCharaBullet;
	CEnemyBullet *m_pEnemyBullet;
	CGatherEffect *m_pGatherEffect;
	CSparkEffect *m_pSparkEffect;
	CPopupNumber *m_pPopupNumber;
	CCircle *m_pCircle;
	CItem *m_pItem;
	CFloatingText *m_pFloatingText;
	CBackground *m_pBackground;
	CEnemy *m_pEnemy;
	CTextOverlay *m_pTextOverlay;
	CDialogue *m_pDialogue;
	CLaser *m_pLaser;
	CSummary *m_pSummary;
	CStage2BG *m_pStage2BG;

//component that are created when needed
protected:
	CMidBoss *m_pMidBoss;
	void CheckMidBoss();

	CBoss *m_pBoss;
	void CheckBoss();

protected:
	CStageResource *m_pStageRes;
	__forceinline CStageResource* GetRes() {return m_pStageRes;}

public:
	bool m_bPlaySound[16];

protected:
	bool m_bLogo;
	int m_logoFrame;
	int StepLogo();
	void DrawLogo();

protected:
	bool m_bCanPause;
	bool m_bPaused;
	int m_pauseCursorPos;
	bool Pause();
	void StepPause();
	void DrawPause();

protected:
	bool m_bContinueScr;
	bool m_bContinued;
	int m_continueCursorPos;
	int m_continueScrTimer;
	int m_continueScrCurPart;
	int m_continueStrX;
	void EnableContinueScreen();
	void StepContinue();
	void DrawContinue();

//stage statistics
protected:
	//changeable
	int m_nCurLife;
	int m_nCurBomb;
	int m_powerOverflowLevel;
	int m_curDream;
	int m_curScore;
	int m_highScore;
	int m_curNPointItem;
	int m_totalNPointItem;
	int m_curNGraze;
	int m_playerPerformance;
	int m_nContinueUsed;

	//constant during game
	int m_nContinueLimit;
	int m_pointItemMaxScore;
	int m_grazeBonus;
	int m_playerPerformanceLowerBound;
	int m_playerPerformanceUpperBound;
	int m_nLifeOnContinue;
	int m_nBombOnMiss;

	//local var
	int m_nMiss;
	int m_nBomb;

protected:
	int m_drawScore;
	int m_drawScoreDelta;
	int m_drawHighScore;

protected:
	int m_curFrame;

public:
	__forceinline int GetCurFrameCount(){return m_curFrame;}

protected:
	unsigned short m_lastMenuKeyState;
	unsigned short m_curMenuKeyState;
	unsigned short m_lastGameKeyState;
	unsigned short m_curGameKeyState;
	int m_curScrFade;

protected:
	int m_playAreaUpperLeftX,m_playAreaUpperLeftY;

public:
	__forceinline unsigned short GetCurKeyState() {return m_curGameKeyState;}
	__forceinline unsigned short GetLastKeyState() {return m_lastGameKeyState;}

public:
	void Initialize();
protected:
	void DownloadVarFromGlobal();
	void UploadVarToGlobal();
	void InitChara();

protected:
	bool m_bHomingEnemyExist;
	float m_homingEnemyX,m_homingEnemyY;

public:
	__forceinline void ChangePlayerPerformance(int deltaPerformance)
	{
		m_playerPerformance+=deltaPerformance;
		if (m_playerPerformance>m_playerPerformanceUpperBound)
			m_playerPerformance=m_playerPerformanceUpperBound;
		if (m_playerPerformance<m_playerPerformanceLowerBound)
			m_playerPerformance=m_playerPerformanceLowerBound;
	}

protected:
	int AdjustDamageFromChara(int origDmg, bool isBoss);
	__forceinline float AdjustSpeedBasedOnPlayerPerformance(float origSpeed)
	{
		float ret=origSpeed*(0.5f+(float)m_playerPerformance/64.0f);
		if (ret>128) ret=128;
		if (ret<8) ret=8;
		return ret;
	}
public:
	int GetDamageFromChara(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss);

public:
	__forceinline int GetDream() {return m_curDream;}
	void SetDream(int newDream);
public:
	__forceinline void AddScore(int deltaScore)
	{
		if (m_curScore<m_highScore&&m_curScore+deltaScore>m_highScore)
		{
			//add floating text type 0
		}
		m_curScore+=deltaScore;
		if (m_highScore<m_curScore)
			m_highScore=m_curScore;
	}
protected:
	void StepScore();

public:
	int Step();
	void Draw();
protected:
	void DrawStatistics();
	void PlayAreaFade(float fadeScale);

protected:
	bool m_bQuit;
	int m_quitCode;
	int m_nQuitFrameLeft;
	void SetQuit(int quitCode);

public:
	CStage(void);
	~CStage(void);
};

}
