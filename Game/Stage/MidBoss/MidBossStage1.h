#pragma once
#include "midboss.h"

namespace th5w{

class CMidBossStage1 : public CMidBoss
{
protected:
	bool m_bHitThisFrame;
	float m_phase3ShootAngle;
	float m_phase3CircularMoveAngle;

protected:
	void Phase0();
	void Phase1_2();
	void Phase3();
	void Phase3Shoot();
public:
	virtual void Initialize();
	virtual int Step();
	virtual void Draw();
	virtual void DrawStatistics();

public:
	CMidBossStage1(void);
	virtual ~CMidBossStage1(void);
};

}