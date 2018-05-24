#pragma once

namespace th5w{

#define CURVING_BULLET_N_RECORD 8

typedef struct strCURVING_BULLET{
	unsigned char flag;
	float angle;
	float nodeX[16],nodeY[16];
	float velX,velY;
	int age;
	float speed;
	float color[3];
	int mode;			//1 for speed dec, 2 for speed inc
}CURVING_BULLET;

class CStage;

class CCurvingBullet
{
protected:
	CURVING_BULLET m_bullet[CURVING_BULLET_N_RECORD];
public:
	CStage *m_pStage;

	void Initialize();
	void AddBullet(float origX,float origY,float angle, float speed, unsigned char colorIdx);
	void Step();
	void Draw();

public:
	CCurvingBullet(void);
	~CCurvingBullet(void);
};

}
