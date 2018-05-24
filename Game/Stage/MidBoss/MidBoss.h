#pragma once
#include "../EnemyBullet/EnemyBullet.h"
#include "../Item/Item.h"
#include "../SparkEffect/SparkEffect.h"
#include "../GatherEffect/GatherEffect.h"
#include "../Chara/Chara.h"
#include "../PopupNumber/PopupNumber.h"
#include "../Enemy/Enemy.h"

namespace th5w{

class CStage;

class CMidBoss
{
friend class CStage;

protected:
	CStage *m_pStage;

protected:
	int m_curPhase;
	int m_curPhaseFrame;
	int m_curHP;
	int m_curImage;
	float m_curX,m_curY;
	float m_lastX,m_lastY;
	float m_velX,m_velY;

protected:
	int GetDamageAndPlaySoundWhenHit(float dmgAreaHalfWidth,float dmgAreaHalfHeight,int soundIdx);
	void AddDefeatScoreAndPopupNumbers(int n1280);
	int DeathSequence();

protected:
	float m_explodeAngle;
	void DrawExplode();
protected:
	int m_lastDrawnHP;
	void StepHPGauge(int curHP,int fullHP);
	void DrawHPGaugeAndExtStrEnemy(int fullHP);
public:
	virtual void Initialize();
	virtual int Step()=0;		//return 0 for continue, 1 for midboss end
	virtual void Draw()=0;
	virtual void DrawStatistics()=0;
public:
	CMidBoss(void);
	virtual ~CMidBoss(void);
};

}
