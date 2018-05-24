#pragma once
#include "../EnemyBullet/EnemyBullet.h"

namespace th5w{

#define SHINKI_BULLET_N_RECORD 63

typedef struct strSHINKI_BULLET{
	unsigned char flag;
	float curX,curY;
	float velX,velY;
	int img;
	float radius;
	int vanishAge;
}SHINKI_BULLET;

class CStage;

class CShinkiBullet
{
protected:
	SHINKI_BULLET m_bullet[SHINKI_BULLET_N_RECORD];

public:
	CStage *m_pStage;

	void Initialize();
	void AddBullet(float origX,float origY,int imgIdx,float angle, float speed);
	void Step();
	void Draw();

public:
	CShinkiBullet(void);
	~CShinkiBullet(void);
};

}
