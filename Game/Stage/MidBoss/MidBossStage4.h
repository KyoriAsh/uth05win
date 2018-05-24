#pragma once
#include "midboss.h"

namespace th5w{

class CMidBossStage4 : public CMidBoss
{
protected:
	bool m_bHitThisFrame;
	bool m_bPhase1CanBeHit;
	int m_phase1NModeChange;
	int m_phase1CurMode;
	float m_phase1Mode2Angle;
	float m_phase1FadeCenterX;

protected:
	void Phase0();
	void Phase1();
	void Phase1Mode1();
	void Phase1Mode2();
	void Phase1Mode3();

	int FadeOutAndIn();

public:
	virtual void Initialize();
	virtual int Step();
	virtual void Draw();
	virtual void DrawStatistics();
public:
	CMidBossStage4(void);
	virtual ~CMidBossStage4(void);
};

}