#pragma once
#include "chara.h"
#define REIMU_BOMB_ANI_N_RECORD 6

namespace th5w{

typedef struct strREIMU_BOMB_ANI{
	float x[8],y[8];
	float velX,velY;
}REIMU_BOMB_ANI;

class CCharaReimu : public CChara
{
protected:
	REIMU_BOMB_ANI m_bombAni[REIMU_BOMB_ANI_N_RECORD];
protected:
	virtual void ShootBullet();
	virtual void StepBomb();
	virtual void DrawBomb();
	virtual int GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss);
public:
	CCharaReimu(void);
	virtual ~CCharaReimu(void);
};

}
