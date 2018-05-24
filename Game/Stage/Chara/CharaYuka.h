#pragma once
#include "chara.h"

namespace th5w{

class CCharaYuka : public CChara
{
protected:
	float m_bombAniRadius;
	float m_bombAniBaseAngle;
protected:
	virtual void ShootBullet();
	virtual void StepBomb();
	virtual void DrawBomb();
	virtual int GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss);
public:
	CCharaYuka(void);
	virtual ~CCharaYuka(void);
};

}