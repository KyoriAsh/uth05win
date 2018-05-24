#pragma once
#include "boss.h"
#include "../SpecialBullet/KnifeBullet.h"

namespace th5w{

class CBossStage5 : public CBoss
{
protected:
	bool m_bDrawKnives;
	CKnifeBullet m_knifeBullet;

protected:
	float m_shinkiX,m_shinkiY;

protected:
	bool m_bPhase2KnifeClockwise;
	int m_phase4ShootEnd;
	int m_phase4ShootKnifeInterval;
	bool m_phase4CrazyMode;
	float m_phase4ShootAngle[2];
	int m_phase7ShootEnd;
	int m_phase7ShootKnifeInterval;
	bool m_phase7CrazyMode;
	float m_phase7ShootAngle[2];
	float m_phase8KnifeY;
	bool m_phase8KnifeLeft;
	bool m_phase8KnifePart;
	int m_phase8ShootPartNReset;
	float m_phase8ShootPartSpeed;
	float m_phase8ShootPartAngle;

protected:
	int RandomWalkVerStage5(int nFrame,float speed);
protected:
	void Phase0();
	void Phase1();
	void Phase2_5();
	void Phase2ShootType1();
	void Phase2ShootType2();
	void Phase5ShootType1();
	void Phase5ShootType2();
	void Phase3_6();
	void Phase4_7();
	void Phase4Shoot();
	void Phase7Shoot();
	void Phase8();
	void Phase8Shoot();
	void Phase9();
	void Phase10();
	void Phase10Shoot();

public:
	virtual void Initialize();
	virtual void Step();
	virtual void DrawBoss();
	virtual void DrawStatistics();
	virtual void DrawBackground();
	virtual void DrawSpecialBullet();

public:
	CBossStage5(void);
	virtual ~CBossStage5(void);
};

}
