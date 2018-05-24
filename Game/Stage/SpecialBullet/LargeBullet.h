#pragma once
#include "../EnemyBullet/EnemyBullet.h"

namespace th5w{

#define LARGE_BULLET_N_RECORD 63

typedef struct strLARGE_BULLET{
	unsigned char flag;
	float angle;
	float curX,curY;
	float lastX,lastY;
	float velX,velY;
	int age;
	bool bSplit;
	int img;
	int life;
	bool bHit;
	float speed;
	ENEMY_SHOOT_INFO shootInfo;
}LARGE_BULLET;

class CStage;

class CLargeBullet
{
protected:
	LARGE_BULLET m_bullet[LARGE_BULLET_N_RECORD];

public:
	CStage *m_pStage;

	void Initialize();
	void AddBullet(float origX,float origY,int imgIdx,float angle, float speed,
				   int life,bool bSplit,ENEMY_SHOOT_INFO *shootInfo);
	void Step();
	void Draw();

public:
	CLargeBullet(void);
	~CLargeBullet(void);
};

}
