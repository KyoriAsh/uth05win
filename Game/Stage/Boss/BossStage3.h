#pragma once
#include "boss.h"

namespace th5w{

typedef struct strPUPPET{
	unsigned char flag;
	float angle;
	float curX,curY;
	float tarX,tarY;
	float velX,velY;
	int shootAge;
	float radius;
	int image;
	int life;
	bool bHitThisFrame;
	int mode;
}PUPPET;

class CBossStage3 : public CBoss
{
protected:
	int m_shieldState;
	int m_shieldImg;
	int m_shieldFrame;
	PUPPET m_puppet[2];

protected:
	void PuppetFlyIn();
	int PuppetNoShoot(int idx);
	int PuppetPrepareEnter(int idx);
	int PuppetShootType0(int idx);
	int PuppetShootType1(int idx);
	int PuppetShootType2(int idx);
	int PuppetShootType3(int idx);
	int PuppetShootPhase14(int idx);
protected:
	void Phase0();
	void Phase1();
	void Phase2();
	void Phase2Shoot();
	void Phase3_6_9_12();
	void Phase3_6_9_12Shoot();
	void Phase3ShootType1();
	void Phase3ShootType2_6_1();
	void Phase3ShootType3_6_2__9_1();
	void Phase6ShootType3_9_3__12_2();
	void Phase9ShootType2_12_1();
	void Phase12ShootType3();
	void Phase4_7_10();
	void Phase5_8_11();
	void Phase13();
	void Phase14();
	void Phase14Shoot();

protected:
	void StepPuppet();
	void StepShieldAndShoot();
	void DrawPuppet();
	void DrawShield();
public:
	virtual void Initialize();
	virtual void Step();
	virtual void DrawBoss();
	virtual void DrawStatistics();
	virtual void DrawBackground();

public:
	CBossStage3(void);
	virtual ~CBossStage3(void);
};

}