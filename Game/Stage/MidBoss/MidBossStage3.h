#pragma once
#include "midboss.h"

namespace th5w{

class CMidBossStage3 : public CMidBoss
{
protected:
	bool m_bHitThisFrame;
	int m_phase1CurMode;
	int m_phase1NModeChange;
	bool m_bPhase1FadingIn;

protected:
	void Phase0();
	void Phase1();
	void Phase1Mode1();
	void Phase1Mode2();
	void Phase1Mode3();

	void ChargeAnimation();
	void FadeOutAndIn();

public:
	virtual void Initialize();
	virtual int Step();
	virtual void Draw();
	virtual void DrawStatistics();
public:
	CMidBossStage3(void);
	virtual ~CMidBossStage3(void);
};

}
