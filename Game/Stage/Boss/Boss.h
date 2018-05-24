#pragma once
#include "../EnemyBullet/EnemyBullet.h"
#include "../Item/Item.h"
#include "../SparkEffect/SparkEffect.h"
#include "../GatherEffect/GatherEffect.h"
#include "../Chara/Chara.h"
#include "../PopupNumber/PopupNumber.h"
#include "../Enemy/Enemy.h"
#include "../Dialogue/Dialogue.h"
#include "../Summary/Summary.h"
#include "../TextOverlay/TextOverlay.h"
#include "../Laser/Laser.h"
#include "../Background/Background.h"
#include "../Circle/Circle.h"

namespace th5w{

#define EXPLODE_EFFECT_N_RECORD 2

typedef struct strEXPLODE_EFFECT{
	unsigned char flag;
	unsigned char age;
	float ctrX,ctrY;
	float xLen,yLen;
	float deltaXLen,deltaYLen;
	float angleOffset;
}EXPLODE_EFFECT;

class CStage;

class CBoss
{
friend class CStage;

protected:
	CStage *m_pStage;

protected:
	EXPLODE_EFFECT m_explodeEffect[EXPLODE_EFFECT_N_RECORD];

protected:
	int m_curPhase;
	int m_curPhaseFrame;
	int m_curHP;
	int m_curPhaseEndHP;
	int m_curImage;
	float m_curX,m_curY;
	float m_lastX,m_lastY;
	float m_velX,m_velY;
	float m_bossDmgAreaHalfWidth;
	float m_bossDmgAreaHalfHeight;
	int m_imgStay;
	int m_imgGoLeft;
	int m_imgGoRight;

	bool m_bHitThisFrame;
	int m_HPLostThisFrame;

	float m_deathExplodeCenterX,m_deathExplodeCenterY;

protected:
	bool m_bTimeoutPhaseEnd;
	int m_curMode;
	int m_nModeChange;

public:
	bool m_bSuccessDefeat;

protected:
	int m_lastDrawnHP;
	void StepHPGauge(int curHP,int fullHP);
	void DrawHPGaugeAndExtStrEnemy(int fullHP);
	void GetDmgOffAndCrashChara();
	int GetDmgOnAndCrashChara();
	int GetDmgAndTestCrash(float dmgAreaHalfWidth,float dmgAreaHalfHeight,int soundIdx);
	void AddDefeatScoreAndPopupNumbers(int n1000);
	void EndPhase(int nextPhaseEndHP,int explodeEffectIdx);
	void DropItem();
	void AddExplodeEffect(int explodeEffectIdx,float x,float y);
	void StepExplodeEffect();
	void DrawExplodeEffect();
	void DeathSequence(int n1000);

protected:
	float m_xLowerBound;
	float m_xUpperBound;
	float m_yLowerBound;
	float m_yUpperBound;
	float m_randomWalkMovingAngle;
	int m_forceGoUpOrDown;		//1 up, 2 down, 0 don't care
	int RandomWalk(int frameIdx);
	int MoveTo(float tarX,float tarY);

protected:
	bool m_bDrawBoss;
public:
	virtual void Initialize();
	virtual void Step()=0;
	void Draw();
	virtual void DrawSpecialBullet();
	virtual void DrawBoss()=0;
	virtual void DrawStatistics()=0;
	virtual void DrawBackground()=0;
public:
	__forceinline int GetCurPhase(){return m_curPhase;}
	__forceinline int GetCurPhaseFrame(){return m_curPhaseFrame;}

public:
	CBoss(void);
	virtual ~CBoss(void);
};

}
