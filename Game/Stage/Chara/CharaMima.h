#pragma once
#include "chara.h"
#define MIMA_BOMB_ANI_N_RECORD 8

namespace th5w{

//in PC98 ver, there're x and y fields but they are never used, except x is for flag of used.
//hence, they are removed here, and radius is used for flag of used instead
typedef struct strMIMA_BOMB_ANI{
	float radius,angle;
}MIMA_BOMB_ANI;

class CCharaMima : public CChara
{
protected:
	MIMA_BOMB_ANI m_bombAni[MIMA_BOMB_ANI_N_RECORD];
protected:
	virtual void ShootBullet();
	virtual void StepBomb();
	virtual void DrawBomb();
	virtual int GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss);
public:
	CCharaMima(void);
	virtual ~CCharaMima(void);
};

}