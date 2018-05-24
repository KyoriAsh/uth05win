#pragma once
#include "boss.h"
#include "../SpecialBullet/LargeBullet.h"

namespace th5w{

class CBossStage4Yuki : public CBoss
{
friend class CStage;
friend class CBossStage4;

protected:
	__forceinline void SetX(float v) {m_curX=v;}
	__forceinline void SetY(float v) {m_curY=v;}
	__forceinline void SetStage(CStage *p) {m_pStage=p;}

protected:
	bool m_bDrawLargeBullet;
	CLargeBullet m_largeBullet;
	float m_moveTarX,m_moveTarY;

	float m_phase3ShootType1Angle;
	float m_phase3ShootType1DeltaAngle;

	float m_phase9ShootType1Angle;

protected:
	void Phase0();
	void Phase1();
	void Phase2();
	void Phase3();
	void Phase3ShootType1();
	void Phase3ShootType2();
	void Phase4();
	void Phase5();
	void Phase5ShootType1();
	void Phase5ShootType2();
	void Phase6();
	void Phase7();
	void Phase7Shoot();
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
	CBossStage4Yuki(void);
	virtual ~CBossStage4Yuki(void);
};

}
