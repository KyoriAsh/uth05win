#pragma once
#include "midboss.h"

namespace th5w{

class CMidBossStage5 : public CMidBoss
{
protected:
	bool m_bHitThisFrame;
	int m_phase1NModeChange;
	int m_phase1CurMode;
	float m_phase1ShootAngle;
	int m_phase1PositionIdx;

protected:
	void Phase0();
	void Phase1();
	int Phase1Move();
	void Phase1Common();
	int Phase1Mode1Shoot();
	int Phase1Mode2Shoot();
	int Phase1Mode3Shoot();

public:
	virtual void Initialize();
	virtual int Step();
	virtual void Draw();
	virtual void DrawStatistics();
public:
	CMidBossStage5(void);
	virtual ~CMidBossStage5(void);
};

}
