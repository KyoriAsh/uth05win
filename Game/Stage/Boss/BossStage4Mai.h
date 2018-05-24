#pragma once
#include "boss.h"
#include "../SpecialBullet/LargeBullet.h"

namespace th5w{

class CBossStage4Mai : public CBoss
{
friend class CStage;
friend class CBossStage4;

protected:
	__forceinline void SetX(float v) {m_curX=v;}
	__forceinline void SetY(float v) {m_curY=v;}
	__forceinline void SetStage(CStage *p) {m_pStage=p;}

protected:
	bool m_bDrawLargeBullet;
	float m_phase5LaserAngleSpeed;
	float m_phase5LaserRotationDir;
	CLargeBullet m_largeBullet;

protected:
	void Phase0();
	void Phase1();
	void Phase2();
	void Phase3();
	void Phase3ShootType1();
	void Phase3ShootType2();
	void Phase4();
	void Phase5();
	void Phase5Shoot();
	void Phase5ShootType1();
	void Phase5ShootType2();
	void Phase5ShootType3();
	void Phase6();
	void Phase7();
	void Phase7ShootType1();
	void Phase7ShootType2();
	void Phase8();
	void Phase9();
	void Phase9ShootType1();
	void Phase9ShootType2();
public:
	virtual void Initialize();
	virtual void Step();
	virtual void DrawBoss();
	virtual void DrawStatistics();
	virtual void DrawBackground();
	virtual void DrawSpecialBullet();

public:
	CBossStage4Mai(void);
	virtual ~CBossStage4Mai(void);
};

}
