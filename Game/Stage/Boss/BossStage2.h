#pragma once
#include "boss.h"

namespace th5w{

class CBossStage2 : public CBoss
{
protected:
	bool m_bPhase5LaserRotationClockwise;

protected:
	void Phase0();
	void Phase1();
	void Phase2();
	void Phase2ShootType1();
	void Phase2ShootType2();
	void Phase3();
	void Phase4();
	void Phase4Shoot();
	void Phase5();
	void Phase5ShootType1();
	void Phase5ShootType2();
	void Phase6();
	void Phase7();
	void Phase7Shoot();

	int ShootAnimation(int endFrame);
	int BounceWalk(int nFrame,float speed);

public:
	virtual void Initialize();
	virtual void Step();
	virtual void DrawBoss();
	virtual void DrawStatistics();
	virtual void DrawBackground();

public:
	CBossStage2(void);
	virtual ~CBossStage2(void);
};

}