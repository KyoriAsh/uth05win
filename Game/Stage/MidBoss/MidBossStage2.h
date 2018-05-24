#pragma once
#include "midboss.h"

namespace th5w{

class CMidBossStage2 : public CMidBoss
{
protected:
	bool m_bHitThisFrame;
	float m_phase1CtrX,m_phase1CtrY;
	float m_phase1VelX;
	float m_phase1Radius,m_phase1DeltaRadius,m_phase1Angle;

protected:
	void Phase0();
	void Phase1();
	void Phase1Shoot();
	void Phase1Move();
	void Phase2();
	void Phase2Shoot();

public:
	virtual void Initialize();
	virtual int Step();
	virtual void Draw();
	virtual void DrawStatistics();

public:
	CMidBossStage2(void);
	virtual ~CMidBossStage2(void);
};

}
