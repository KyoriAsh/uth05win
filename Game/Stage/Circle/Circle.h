#pragma once

namespace th5w{

#define CIRCLE_N_RECORD 8

typedef struct strCIRCLE{
	unsigned char flag;
	unsigned char age;
	float ctrX;
	float ctrY;
	float radius;
	float deltaRadius;
	float color[3];
}CIRCLE;

class CStage;

class CCircle
{
friend class CStage;

protected:
	CStage *m_pStage;

protected:
	CIRCLE m_circle[CIRCLE_N_RECORD];
public:
	void Initialize();
	void AddCircle(float ctrX,float ctrY,float colorR,float colorG,float colorB,bool bExpanding);
	void Step();
	void Draw();

public:
	CCircle(CStage *pStage);
	~CCircle(void);
};

}
