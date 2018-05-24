#pragma once
#include "boss.h"
#include "../SpecialBullet/CurvingBullet.h"

namespace th5w{

class CBossStage4 : public CBoss
{
protected:
	float m_2curX,m_2curY;
	float m_2lastX,m_2lastY;
	float m_2velX,m_2velY;
	int m_2curHP;
	int m_2curPhaseEndHP;
	int m_2curImage;
	bool m_b2HitThisFrame;
	int m_2curMode;
	int m_whichDie;

	bool m_bDrawCurvingBullet;
	CCurvingBullet m_curvingBullet;

	float m_phase2MaiShootType2_3__2_4_baseAngle;
	float m_phase2MaiShootType2_3__2_4_deltaAngle;
	float m_phase2YukiShootType2_2__2_3_baseAngle;
	float m_phase2YukiShootType2_2__2_3_deltaAngle;

protected:
	void Phase0();
	void Phase1();
	void Phase2();
	void Phase2ShootMai();
	void Phase2ShootYuki();

	int MaiShootType0_1();
	int MaiShootType0_2__2_2();
	int MaiShootType0_3();
	int MaiShootType0_4();
	int MaiShootType1();
	int MaiShootType2_1();
	int MaiShootType2_3__2_4();

	int YukiShootType0_1__1_4();
	int YukiShootType0_2__2_4();
	int YukiShootType0_3__1_1();
	int YukiShootType0_4();
	int YukiShootType1_2();
	int YukiShootType1_3();
	int YukiShootType2_1();
	int YukiShootType2_2__2_3();

protected:
	void GetDmgOffAndCrashChara2();
	int GetDmgOnAndCrashChara2();
	int GetDmgAndTestCrash2(float dmgAreaHalfWidth,float dmgAreaHalfHeight,int soundIdx);
	void DrawBossImage(int imgIdx,float x,float y,bool bHit);

	float m_2randomWalkMovingAngle;
	int m_2forceGoUpOrDown;		//1 up, 2 down, 0 don't care
	int Move(int frameIdx);
	int RandomWalkVerStage4(int frameIdx,float &angle,int &forceMove,float &x,float &y,float &vx,float &vy,int &img);
	void DeathSequenceVerStage4();
public:
	virtual void Initialize();
	virtual void Step();
	virtual void DrawBoss();
	virtual void DrawStatistics();
	virtual void DrawBackground();
	virtual void DrawSpecialBullet();

public:
	CBossStage4(void);
	virtual ~CBossStage4(void);
};

}
