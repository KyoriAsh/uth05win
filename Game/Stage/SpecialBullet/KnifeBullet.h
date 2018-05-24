#pragma once
#include "../EnemyBullet/EnemyBullet.h"

namespace th5w{

#define KNIFE_BULLET_N_RECORD 63

typedef struct strKNIFE_BULLET{
	unsigned char flag;
	float angle;
	float curX,curY;
	float lastX,lastY;
	float velX,velY;
	int cooldown;
	int img;
	int vanishAge;
	float speed;
}KNIFE_BULLET;

class CStage;

class CKnifeBullet
{
protected:
	KNIFE_BULLET m_bullet[KNIFE_BULLET_N_RECORD];

public:
	CStage *m_pStage;

	void Initialize();
	void AddBullet(float origX,float origY,float angle,float speed, int cooldown);
	void Step();
	void Draw();

public:
	CKnifeBullet(void);
	~CKnifeBullet(void);
};

}
