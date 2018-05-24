#pragma once
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

#define ENEMY_BULLET_N_RECORD_DOT 1800
#define ENEMY_BULLET_N_RECORD_IMAGED 2200
#define ENEMY_BULLET_N_RECORD (ENEMY_BULLET_N_RECORD_DOT+ENEMY_BULLET_N_RECORD_IMAGED)

#define EST00_SINGLE 0
#define EST01_SINGLE_TO_CHARA 1
#define EST02_NWAY 2
#define EST03_NWAY_TO_CHARA 3
#define EST04_NWAY_ALLAROUND 4
#define EST05_NWAY_ALLAROUND_TO_CHARA 5
#define EST06_MULTI 6
#define EST07_MULTI_TO_CHARA 7
#define EST08_NWAY_MULTI 8
#define EST09_NWAY_MULTI_TO_CHARA 9
#define EST10_NWAY_ALLAROUND_MULTI 10
#define EST11_NWAY_ALLAROUND_MULTI_TO_CHARA 11
#define EST12_RANDOM 12
#define EST13_RANDOM_BASE_SPEED_32 13
#define EST14_DEFAULT_SINGLE 14
#define EST15_DEFAULT_SINGLE_TO_CHARA 15

#define EBMT01_SLOW_DOWN_THEN_TURN_TO_CHARA 1
#define EBMT02_SLOW_DOWN_THEN_TURN 2
#define EBMT03_SPEED_UP 3
#define EBMT04_SLOW_DOWN_AND_TURN 4
#define EBMT05_BOUNCE_X 5
#define EBMT06_BOUNCE_Y 6
#define EBMT07_BOUNCE_X_Y 7
#define EBMT08_BOUNCE_X_YTOP 8
#define EBMT09_SPEED_UP_VERTICAL 9
#define EBMT10_STRAIGHT 10


typedef struct strENEMY_BULLET{
	unsigned char flag;
	unsigned char age;
	float curX;
	float curY;
	float lastX;
	float lastY;
	float velX;
	float velY;
	unsigned char bulletType;
	float curSpeed;
	float curAngle;
	unsigned char bornStageAndGrazeFlag;
	unsigned char dieStageAndMoveType;
	unsigned char specialMoveType;
	float origSpeed;
	unsigned char accumulator;
	float delta;
	unsigned char imgIdx;
	float origX;
	float origY;
	float totalDistanceTravelled;
	unsigned char specialMoveVar;			//this field is new, PC98 ver uses a global var to store it
}ENEMY_BULLET;

typedef struct strENEMY_SHOOT_INFO{
	unsigned char bulletBornType;
	unsigned char bulletImg;
	float shootOrigX;
	float shootOrigY;
	unsigned char shootType;
	unsigned char bulletMoveType;
	unsigned char nWay;
	float nWayUnitAngle;
	unsigned char nMulti;
	float deltaSpeed;
	float shootAngle;
	float bulletSpeed;
	float specialBulletDelta;				//these two fields are new, PC98 ver uses global vars to store them
	unsigned char specialMoveVar;
}ENEMY_SHOOT_INFO;

class CStage;

class CEnemyBullet
{
friend class CStage;

public:
	bool m_noEnemyBullet;
protected:
	int m_clearEnemyBulletTimeLeft;
public:
	bool m_bDropPointItemOnBonus;

protected:
	float m_clipXLow,m_clipXHigh,m_clipYLow,m_clipYHigh;
	float m_bounceXLow,m_bounceXHigh,m_bounceYLow,m_bounceYHigh;
public:
	float m_dotBulletShadowColor[3];

protected:
	ENEMY_BULLET m_bullets[ENEMY_BULLET_N_RECORD];
protected:
	CStage *m_pStage;
public:
	void Initialize();
	void Step();
	void Draw();
	void TuneEnemyShootInfo(ENEMY_SHOOT_INFO *pInfo);

	__forceinline int GetClearBulletTimeLeft() {return m_clearEnemyBulletTimeLeft;}
	__forceinline void SetClearBulletTimeLeft(int timeLeft)
	{
		if (m_clearEnemyBulletTimeLeft<timeLeft)
			m_clearEnemyBulletTimeLeft=timeLeft;
	}

protected:
	static int GetNRecordNeeded(ENEMY_SHOOT_INFO *shootInfo);
	int FindFreeRecord(int *retIdxArray,int startIdx,int nIdx,int nNeeded);
	void FillRecordSpeedAndAngle(int *freeRecIdx,ENEMY_SHOOT_INFO *shootInfo);
	void SpecialMove(int recordIdx);
public:
	static unsigned char AdjustBulletImageBasedOnAngle(unsigned char curImgIdx,float angle);

	__forceinline void UpdateVelXYFromSpeedAngle(int recordIdx)
	{
		m_bullets[recordIdx].velX=(float)cos(m_bullets[recordIdx].curAngle/256.0*2*PI)*m_bullets[recordIdx].curSpeed;
		m_bullets[recordIdx].velY=(float)sin(m_bullets[recordIdx].curAngle/256.0*2*PI)*m_bullets[recordIdx].curSpeed;

	}
public:
	void Shoot(ENEMY_SHOOT_INFO *shootInfo,bool bSpecialBullet=false,bool bAdjustSpeed=true);

public:
	CEnemyBullet(CStage *pStage);
	~CEnemyBullet(void);
};

}
