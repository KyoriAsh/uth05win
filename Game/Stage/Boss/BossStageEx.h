#pragma once
#include "boss.h"
#include "../SpecialBullet/CurvingBullet.h"

namespace th5w{

typedef struct strLINE_SEQ{
	float halfLen[19];
	float angle[19];
}LINE_SEQ;

class CBossStageEx : public CBoss
{
protected:
	int m_invulnerableTimeLeft;
	int m_curOuterImg;

protected:
	unsigned char m_bgColor[3];
	int m_bgTimer;
	LINE_SEQ m_lineSeq;

protected:
	float m_phase2Type2Angle;
	float m_phase4Angle;
	bool m_bPhase4AngleInc;
	float m_phase4AimX,m_phase4AimY,m_phase4AimAngle;
	float m_phase6Type2Angle;
	bool m_bPhase6Type2AngleInc;
	float m_phase8Angle;
	float m_phase12LaserAngle;
	bool m_bPhase12LaserAngleInc;
	float m_phase16ShootOrigX;
	int m_phase16Var3;
	int m_phase16Var4;
	int m_phase16Var5;
	float m_phase16ShootAngle;
	int m_phase17ColorChangeMode;
	float m_phase17ShootAngle;

protected:
	bool m_bFireWave[2];
	int m_fireWaveY[2];
	void StepFireWave();
	void DrawFireWave();

protected:
	int CommonShootOuter();
	void Phase0();
	void Phase1();
	void Phase2();
	int Phase2ShootType1();
	int Phase2ShootType2();
	void Phase3();
	void Phase4();
	int Phase4Shoot();
	void Phase5_6();
	int Phase6ShootType1();
	int Phase6ShootType2();
	void Phase7_11_15();
	void Phase8_12();
	int Phase8Shoot();
	int Phase12Shoot();
	void Phase9_10();
	int Phase10ShootType1();
	int Phase10ShootType2();
	void Phase13_14();
	int Phase14ShootType1();
	int Phase14ShootType2();
	void Phase16();
	int Phase16Shoot();
	void Phase17();
	void Phase17Shoot();

protected:
	void StepBackground();
	void DrawSet(float radius,float angle,unsigned char colorIdx);

	int GetDmgVerStageEx();
	void EndPhaseVerStageEx(int nextPhaseEndHP,int explodeEffectIdx);

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

public:
	CBossStageEx(void);
	virtual ~CBossStageEx(void);
};

}
