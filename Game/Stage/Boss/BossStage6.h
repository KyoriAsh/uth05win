#pragma once
#include "boss.h"
#include "../SpecialBullet/CurvingBullet.h"
#include "../SpecialBullet/ShinkiBullet.h"

namespace th5w{

typedef struct strLINE_SET{
	float ctrX[20],ctrY[20];
	float halfLen[20];
	float angle[20];
	float customVar;
}LINE_SET;

typedef struct strBG_PARTICLE{
	bool bFlag;
	float curX,curY;
	float origX,origY;
	float velX,velY;
	int age;
	float angle;
	int imgIdx;
}BG_PARTICLE;

class CBossStage6 : public CBoss
{
protected:
	unsigned char m_bgColor[3];
	LINE_SET m_lineSet[2];
	BG_PARTICLE m_bgParticle[64];
	int m_bgTypeATimer;
	int m_bgTypeARotateCounter;
	float m_bgParticleColor[3];
	int m_bgTypeBTimer;
	int m_bgTypeBCFrame;
	int m_bgTypeCTimer;
	int m_bgTypeDTimer;

	void StepBackground();
	void StepBGParticles();
	void StepBGTypeA();
	void StepBGTypeB();
	void StepBGTypeC();
	void StepBGTypeD();
	void BGLineForwardCopy();

	void DrawBGParticles();
	void DrawLineSetMember(int setIdx,int memberIdx,unsigned char colorIdx);
	void DrawBGTypeA();
	void DrawBGTypeB();
	void DrawBGTypeC();
	void DrawBGTypeD();

protected:
	float m_phase2Type1DeltaAngle;
	float m_phase2Type1Angle;
	float m_phase2Type2Angle;
	int m_phase5ScrScale;
	int m_phase5Timer;
	int m_phase10Timer;
	int m_phase10Var0;
	int m_phase10Var1;
	int m_phase10Var2;
	int m_phase10Var3;
	int m_phase10Var4;
	int m_phase10Var5;
	int m_phase10VarI;
	int m_phase10VarS;
	int m_phase10VarD;

protected:
	void Phase0();
	void Phase1();
	void Phase2_3();
	void Phase2_3Shoot();
	int Phase2ShootType1();
	int Phase2ShootType2();
	int Phase3ShootType1();
	int Phase3ShootType2();
	void Phase4();
	void Phase5();
	int Phase5Effect();
	void Phase6_7();
	void Phase6Shoot();
	void Phase7Shoot();
	void Phase8();
	int Phase8Effect();
	void Phase9_10();
	void Phase9Shoot();
	void Phase10Shoot();
	void Phase11();
	void Phase12();
	void Phase12Shoot();

protected:
	bool m_bFloatingUp;
	void FloatMove();

public:
	virtual void Initialize();
	virtual void Step();
	virtual void DrawBoss();
	virtual void DrawStatistics();
	virtual void DrawBackground();
	virtual void DrawSpecialBullet();
protected:
	bool m_bDrawSpecialBullet;
	CCurvingBullet m_curvingBullet;
	CShinkiBullet m_shinkiBullet;

public:
	CBossStage6(void);
	virtual ~CBossStage6(void);
};

}
