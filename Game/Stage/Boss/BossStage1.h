#pragma once
#include "boss.h"

namespace th5w{

class CBossStage1 : public CBoss
{
protected:
	int m_phase2NFrameBeforeRandomWalk;
	int m_phase3ShootNFrame;
	int m_lastMode;

protected:
	void Phase0();
	void Phase1();
	void Phase2();
	void Phase2Shoot();
	void Phase2ShootType1();
	void Phase2ShootType2();
	void Phase2ShootType3();
	void Phase2ShootType4();
	void Phase3();
	void Phase3Shoot();
	void Phase3ShootType1();
	void Phase3ShootType2();
	void Phase3ShootType3();
	void Phase3ShootType4();
	void Phase4();
	void Phase4Shoot();

public:
	virtual void Initialize();
	virtual void Step();
	virtual void DrawBoss();
	virtual void DrawStatistics();
	virtual void DrawBackground();

public:
	CBossStage1(void);
	virtual ~CBossStage1(void);
};

}
