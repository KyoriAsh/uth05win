#pragma once
#include "../../../gameLib/Graphic/2DImageArray.h"
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

class CStage;
class CCharaBullet;

class CChara
{
friend class CStage;

protected:
	float m_curX,m_curY;
	float m_lastX,m_lastY;
	float m_deltaX,m_deltaY;
public:
	__forceinline float GetCurX() {return m_curX;}
	__forceinline float GetCurY() {return m_curY;}

//params that must be initialized by child class constructor
protected:
	float m_moveSpeed,m_moveSpeedDiagonal;
	int m_bombInvulnerableTime;
	int m_bombClearEnemyBulletTime;

//params that must be initialized by the instance creator which new the instance
protected:
	float m_moveAreaXLow,m_moveAreaXHigh,m_moveAreaYLow,m_moveAreaYHigh;
protected:
	CStage *m_pStage;

protected:
	int m_power;
	int m_powerLevel;
public:
	void SetPower(int newPower);
	__forceinline int GetPower() {return m_power;}

public:
	bool m_bCharaCanMove;
	bool m_bCharaCanShoot;
	bool m_bBombDisabled;
	bool m_bMiss;
protected:
	int m_nextLifeComeTimeLeft;
public:
	__forceinline int GetNextLifeComeTimeLeft() {return m_nextLifeComeTimeLeft;}
protected:
	int m_cannotControlTimeLeft;
	float m_missExplodeRadius;
	float m_missExplodeBaseAngle;
public:
	int m_invulnerableTimeLeft;

protected:
	int m_bombTimer;

	unsigned int m_charaShootTimer;

protected:
	int m_getPointItemPerformance;

protected:
	C2DImage* GetStageSprite(int spriteIdx);
	C2DImage* GetStagePattern(int patternIdx);
	CCharaBullet* GetCharaBullet();

protected:
	void MoveChara(unsigned short keyState);
	void DropBomb();
	void DecreaseDreamInBomb();
	void OnMissDropItem();

	bool m_bDraw;
	__forceinline void DisableDraw() {m_bDraw=false;}
	__forceinline void EnableDraw() {m_bDraw=true;}

protected:
	int m_collisionAreaAlpha;
	void DrawCollisionArea();

public:
	virtual void Step();
	virtual void Draw();
	virtual void StepBomb();
	virtual void DrawBomb();
	virtual int GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss);
	virtual bool TestCrash(float testX,float testY, float testHalfWidth,float testHalfHeight);
	__forceinline float AngleToChara(float srcX,float srcY)
	{
		return (float)(atan2(m_curY-srcY,m_curX-srcX)/2.0/PI*256);
	}
	void CaughtItem(float itemX,float itemY,unsigned char itemType);
protected:
	virtual void ShootBullet();
	void ShootPowerLevel0Common();
	void ShootPowerLevel1Common();

protected:
	CChara(void);
	virtual ~CChara(void);
};

}
