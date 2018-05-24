#include ".\opparticle.h"
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

COpParticle::COpParticle(void)
{
}

COpParticle::~COpParticle(void)
{
}

void COpParticle::Initialize()
{
	for (int i=0;i<MAX_PARTICLE;i++)
		m_particle[i].bUsed=m_particle[i].age=0;
}

void COpParticle::Step(bool *pbPlaySound15)
{
	for (int i=0;i<MAX_PARTICLE;i++)
	{
		PARTICLE *pPar=&m_particle[i];
		if (pPar->bUsed!=1)
			continue;
		pPar->age++;
		if (pPar->age>=40)
		{
			pPar->bUsed=0;
			pPar->age=0;
			continue;
		}
		if (pPar->age<16)
			continue;
		if (pPar->age==16)
			*pbPlaySound15=true;
		pPar->lastR=pPar->curR;
		pPar->curR+=pPar->vel;
	}
}

void COpParticle::Draw(C2DImageArray *pImageArray)
{
	for (int i=0;i<MAX_PARTICLE;i++)
	{
		PARTICLE *pPar=&m_particle[i];
		if (pPar->bUsed!=1)
			continue;
		int drawImg=pPar->img+pPar->age/4;
		if (pPar->age<16)
		{
			C2DImage *pImage=pImageArray->GetImagePtr(drawImg);
			pImage->Draw((float)pPar->curX/16-8,(float)pPar->curY/16-8+40);
			continue;
		}
		int drawOffset;
		if (pPar->age<32)
			drawOffset=8;
		else
			drawOffset=16;
		double realAngle=pPar->angle/(double)256*2*PI;
		int drawX=((int)(cos(realAngle)*pPar->curR)+pPar->curX)/16-drawOffset;
		int drawY=((int)(sin(realAngle)*pPar->curR)+pPar->curY)/16-drawOffset;
		C2DImage *pImage=pImageArray->GetImagePtr(drawImg);
		pImage->Draw((float)drawX,(float)drawY+40);

	}
}

void COpParticle::AddParticles(int img,int nParticleToCreate,int originX,int originY)
{
	int nParticleCreated=0;
	int curSlot=0;
	while(curSlot<256)
	{
		if (m_particle[curSlot].bUsed)
		{
			curSlot++;
			continue;
		}
		m_particle[curSlot].bUsed=1;
		m_particle[curSlot].age=0;
		m_particle[curSlot].curX=originX*16;
		m_particle[curSlot].curY=originY*16;
		m_particle[curSlot].lastR=0;
		m_particle[curSlot].curR=0;
		m_particle[curSlot].vel=rand()%224+64;
		m_particle[curSlot].angle=rand()%256;
		m_particle[curSlot].img=img;
		nParticleCreated++;
		if (nParticleCreated>=nParticleToCreate)
			break;
	}
}

}