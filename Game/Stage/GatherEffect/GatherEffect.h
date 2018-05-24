#pragma once
#include "../EnemyBullet/EnemyBullet.h"
#include <stdio.h>

namespace th5w{
#define GATHER_EFFECT_N_RECORD 8

typedef struct strGATHER_EFFECT{
	unsigned char flag;
	float colorR,colorG,colorB;
	float curX;
	float curY;
	float lastX;
	float lastY;
	float velX;
	float velY;
	float radius;
	int nDots;
	float baseAngle;
	float deltaAngle;
	float lastRadius;
	float deltaRadius;
	ENEMY_SHOOT_INFO shootInfo;
}GATHER_EFFECT;

class CStage;

class CGatherEffect
{
friend class CStage;

protected:
	CStage *m_pStage;

protected:
	GATHER_EFFECT m_gather[GATHER_EFFECT_N_RECORD];
public:
	void AddEffect(float ctrX,float ctrY,float initR,float deltaAngle,float colorR,float colorG,float colorB,
				   int nDots,ENEMY_SHOOT_INFO *pShootInfo=NULL,float velX=0.0f,float velY=0.0f);
	void AddEffectTrailed(float ctrX,float ctrY,unsigned char bodyColorIdx,unsigned char headColorIdx,
						  int frameIdx,float initR=1024.0f,float deltaAngle=2.0f,int nDots=8,
						  float velX=0.0f,float velY=0.0f);
	void Initialize();
	void Step();
	void Draw();
public:
	CGatherEffect(CStage *pStage);
	~CGatherEffect(void);
};

}
