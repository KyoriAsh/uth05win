/*
	This class has the function of both hit and normal chara bullets
*/
#pragma once

namespace th5w{

#define CHARA_BULLET_N_RECORD 64
#define HIT_CHARA_BULLET_N_RECORD 24
#define CHARA_BULLET_TYPE_NORMAL 0
#define CHARA_BULLET_TYPE_HOMING 1
#define CHARA_BULLET_TYPE_MISSILE_LEFT 2
#define CHARA_BULLET_TYPE_MISSILE_RIGHT 3
#define CHARA_BULLET_TYPE_MISSILE_STRAIGHT 4
#define ADD_CHARA_BULLET_IGNORE_MOVEANGLE 10000.0f

typedef struct strCHARA_BULLET{
	unsigned char flag;
	unsigned char age;
	float curX;
	float curY;
	float lastX;
	float lastY;
	float velX;
	float velY;
	unsigned char imgIdx;
	unsigned char bulletType;	//0 for normal, 1 for homing, 2 for missileL, 3 for missileR, 4 for missileS
	unsigned char damage;
	float moveAngle;
}CHARA_BULLET;

typedef struct strHIT_CHARA_BULLET{
	unsigned char age;
	unsigned char imgIdx;
	float curX;
	float curY;
	float lastX;
	float lastY;
	float velX;
	float velY;
}HIT_CHARA_BULLET;

class CStage;

class CCharaBullet
{
friend class CStage;
friend class CChara;
friend class CCharaReimu;
friend class CCharaMarisa;
friend class CCharaMima;
friend class CCharaYuka;

protected:
	CStage *m_pStage;
	float m_clipXLow,m_clipXHigh,m_clipYLow,m_clipYHigh;

protected:
	CHARA_BULLET m_bullets[CHARA_BULLET_N_RECORD];
	HIT_CHARA_BULLET m_hitBullets[HIT_CHARA_BULLET_N_RECORD];
public:
	void Initialize();
public:
	void AddBullet(float bulletX, float bulletY, unsigned char imgIdx,unsigned char bulletType,
				   float velX, float velY, float moveAngle,unsigned char damage);
	void Step();
	void Draw();

public:
	int GetDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss,bool bReflectAndAddScore=false);
protected:
	void ConvertToHitRecord(int bulletRecordIdx);

protected:
	void StepBullets();
	void StepHitBullets();
	void DrawBullets();
	void DrawHitBullets();

public:
	CCharaBullet(CStage *pStage);
	~CCharaBullet(void);
};

}