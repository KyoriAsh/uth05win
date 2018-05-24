#pragma once
#include "../EnemyBullet/EnemyBullet.h"
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

#define ENEMY_N_RECORD 32
typedef struct strENEMY{
	unsigned char flag;
	int age;
	float curX,curY;
	float lastX,lastY;
	float velX,velY;
	int life;
	int score;
	unsigned char *pScript;
	float curSpeed;
	unsigned char imgIdx;
	unsigned char accumulator;
	unsigned char loopCounter;
	float curAngle;
	float deltaAngle;
	unsigned char nImage;
	unsigned char imgChangeInterval;
	//unsigned char idx30;
	unsigned char canLeave;			//bit 0 for horizontal leave, bit 4 for vertical leave
	unsigned char dropItemType;
	bool hitThisFrame;
	bool bTakeDamage;
	bool bAutoShoot;
	bool bCrashChara;
	bool atLeftHalfOfScreen;
	unsigned char autoShootTimer;
	unsigned char autoShootInterval;
	ENEMY_SHOOT_INFO shootInfo;
	unsigned char subType;
}ENEMY;

class CStage;

class CEnemy
{
friend class CStage;
protected:
	CStage *m_pStage;

public:
	bool m_noAddEnemy;

protected:
	ENEMY m_enemy[ENEMY_N_RECORD];
	unsigned char *m_pEnemyTableReadPtr;
	static const unsigned char s_enemyDropTable[64];
	int m_enemyDropTableIdx;					//this value is not carried over stages, as in PC98 ver

protected:
	void AddEnemy(unsigned char enemyType,unsigned char subType,float enemyX,float enemyY,unsigned char dropItemType);
	void StepEnemyScript(ENEMY *pEnemy);
	void EnemyDie(ENEMY *pEnemy);

protected:
	__forceinline static void UpdateVelXYFromSpeedAngle(ENEMY *pEnemy)
	{
		pEnemy->velX=(float)cos(pEnemy->curAngle/256.0f*2*PI)*pEnemy->curSpeed;
		pEnemy->velY=(float)sin(pEnemy->curAngle/256.0f*2*PI)*pEnemy->curSpeed;
	}
	__forceinline static bool MoveAndTestLeave(ENEMY *pEnemy)
	{
		pEnemy->lastX=pEnemy->curX;
		pEnemy->lastY=pEnemy->curY;
		pEnemy->curX+=pEnemy->velX;
		pEnemy->curY+=pEnemy->velY;
		if ((pEnemy->canLeave&1)&&(pEnemy->curX<=-16.0f*16.0f||pEnemy->curX>=400.0f*16.0f))
			return true;
		if ((pEnemy->canLeave&0x10)&&(pEnemy->curY<=-16.0f*16.0f||pEnemy->curY>=384.0f*16.0f))
			return true;
		return false;
	}

public:
	void Initialize();
	void StepEnemyTable();
	void Step();
	void Draw();
public:
	CEnemy(CStage *pStage);
	~CEnemy(void);
};

}