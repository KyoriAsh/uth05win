#pragma once
#include "midboss.h"

namespace th5w{

class CMidBossStageEx : public CMidBoss
{
protected:
	bool m_bHitThisFrame;
	float m_movingAngle;
	int m_shootType;
	int m_phase1CurSubphase;
	int m_phase1NModeChange;
	float m_shootType1DeltaAngle;
	float m_shootType1Angle;

protected:
	void Phase0();
	void Phase1();
	int Move(int frameIdx);
	int Shoot();
	int ShootType1();
	int ShootType2();
	int ShootType3_4();

public:
	virtual void Initialize();
	virtual int Step();
	virtual void Draw();
	virtual void DrawStatistics();

public:
	CMidBossStageEx(void);
	virtual ~CMidBossStageEx(void);
};

}