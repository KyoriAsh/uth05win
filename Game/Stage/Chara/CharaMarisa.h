#pragma once
#include "chara.h"
#define MARISA_BOMB_ANI_N_RECORD 8

namespace th5w{
typedef struct strMARISA_BOMB_ANI{
	float x,y;
	float width;
}MARISA_BOMB_ANI;

class CCharaMarisa : public CChara
{
protected:
	MARISA_BOMB_ANI m_bombAni[MARISA_BOMB_ANI_N_RECORD];
protected:
	virtual void ShootBullet();
	virtual void StepBomb();
	virtual void DrawBomb();
	virtual int GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss);
public:
	CCharaMarisa(void);
	virtual ~CCharaMarisa(void);
};

}