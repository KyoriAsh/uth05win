#include ".\circle.h"
#include "../Stage.h"
#include <string.h>
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

CCircle::CCircle(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CCircle::~CCircle(void)
{
}

void CCircle::Initialize()
{
	memset(m_circle,0,sizeof(m_circle));
}

void CCircle::AddCircle(float ctrX,float ctrY,float colorR,float colorG,float colorB,bool bExpanding)
{
	for (int i=0;i<CIRCLE_N_RECORD;i++)
		if (m_circle[i].flag==0)
		{
			m_circle[i].flag=1;
			m_circle[i].age=0;
			m_circle[i].ctrX=ctrX;
			m_circle[i].ctrY=ctrY;
			if (bExpanding)
			{
				m_circle[i].radius=4*16;
				m_circle[i].deltaRadius=8*16;
			}
			else
			{
				m_circle[i].radius=132*16;
				m_circle[i].deltaRadius=-8*16;

			}
			m_circle[i].color[0]=colorR;
			m_circle[i].color[1]=colorG;
			m_circle[i].color[2]=colorB;
			break;
		}
}

void CCircle::Step()
{
	for (int i=0;i<CIRCLE_N_RECORD;i++)
	{
		if (m_circle[i].flag!=1)
		{
			m_circle[i].flag=0;
			continue;
		}
		m_circle[i].radius+=m_circle[i].deltaRadius;
		m_circle[i].age++;
		if (m_circle[i].age>16)
			m_circle[i].flag=2;
	}
}

void CCircle::Draw()
{
	for (int i=0;i<CIRCLE_N_RECORD;i++)
		if (m_circle[i].flag==1)
			m_pStage->m_pStageRes->DrawCircle(m_circle[i].ctrX,m_circle[i].ctrY,m_circle[i].radius,
								 m_circle[i].color[0],m_circle[i].color[1],m_circle[i].color[2],false);
}

}









