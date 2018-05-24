#pragma once

namespace th5w{

#define SPARK_EFFECT_N_RECORD 64

typedef struct strSPARK_EFFECT{
	unsigned char flag;
	unsigned char age;
	float curX,curY;
	float lastX,lastY;
	float velX,velY;
	float angle;
}SPARK_EFFECT;

class CStage;

class CSparkEffect
{
friend class CStage;

protected:
	float m_clipXLow,m_clipXHigh,m_clipYLow,m_clipYHigh;

protected:
	CStage *m_pStage;

protected:
	SPARK_EFFECT m_spark[SPARK_EFFECT_N_RECORD];

public:
	void AddEffect(float origX,float origY,float baseSpeed,int nTrail,bool isExplode);
	void Initialize();
	void Step();
	void Draw();

public:
	CSparkEffect(CStage *pStage);
	~CSparkEffect(void);
};

}
