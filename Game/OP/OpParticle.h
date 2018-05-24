#pragma once
#include "../../gameLib/Graphic/2DImageArray.h"

namespace th5w{

typedef struct strPARTICLE{
	unsigned char bUsed;
	unsigned char age;
	unsigned short curX;
	unsigned short curY;
	unsigned short lastR;
	unsigned short curR;
	unsigned short vel;
	unsigned char angle;
	unsigned char img;
}PARTICLE;

#define MAX_PARTICLE 256

class COpParticle
{
protected:
	PARTICLE m_particle[MAX_PARTICLE];

public:
	void Initialize();
	void AddParticles(int img,int nParticleToCreate,int originX,int originY);
	void Step(bool *pbPlaySound15);
	void Draw(C2DImageArray *pImageArray);
public:
	COpParticle(void);
	~COpParticle(void);
};

}
