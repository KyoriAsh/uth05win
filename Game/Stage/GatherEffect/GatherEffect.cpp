#include ".\gathereffect.h"
#include <string.h>
#include "../Stage.h"
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

CGatherEffect::CGatherEffect(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CGatherEffect::~CGatherEffect(void)
{
}

void CGatherEffect::Initialize()
{
	memset(m_gather,0,sizeof(m_gather));
}

void CGatherEffect::AddEffect(float ctrX,float ctrY,float initR,float deltaAngle,float colorR,float colorG,float colorB,
							  int nDots,ENEMY_SHOOT_INFO *pShootInfo,float velX,float velY)
{
	for (int i=0;i<GATHER_EFFECT_N_RECORD;i++)
		if (m_gather[i].flag==0)
		{
			m_gather[i].flag=1;
			m_gather[i].shootInfo.bulletBornType=0xff;
			m_gather[i].curX=ctrX;
			m_gather[i].curY=ctrY;
			m_gather[i].radius=initR;
			m_gather[i].lastRadius=initR;
			m_gather[i].velX=velX;
			m_gather[i].velY=velY;
			m_gather[i].baseAngle=0;
			m_gather[i].deltaAngle=deltaAngle;
			m_gather[i].colorR=colorR;
			m_gather[i].colorG=colorG;
			m_gather[i].colorB=colorB;
			m_gather[i].nDots=nDots;
			m_gather[i].deltaRadius=initR/32;
			if (pShootInfo!=NULL)
				memcpy(&m_gather[i].shootInfo,pShootInfo,sizeof(ENEMY_SHOOT_INFO));
			return;
		}
}

void CGatherEffect::AddEffectTrailed(float ctrX,float ctrY,unsigned char bodyColorIdx,unsigned char headColorIdx,
									 int frameIdx,float initR,float deltaAngle,int nDots, float velX,float velY)
{
	if (frameIdx==0)
	{
		AddEffect(ctrX,ctrY,initR,deltaAngle,m_pStage->m_pStageRes->m_palette[headColorIdx*3]/255.0f,
				  m_pStage->m_pStageRes->m_palette[headColorIdx*3+1]/255.0f,m_pStage->m_pStageRes->m_palette[headColorIdx*3+2]/255.0f,
				  nDots,NULL,velX,velY);
	}
	if (frameIdx==2||frameIdx==4)
		AddEffect(ctrX,ctrY,initR,deltaAngle,m_pStage->m_pStageRes->m_palette[bodyColorIdx*3]/255.0f,
				  m_pStage->m_pStageRes->m_palette[bodyColorIdx*3+1]/255.0f,m_pStage->m_pStageRes->m_palette[bodyColorIdx*3+2]/255.0f,
				  nDots,NULL,velX,velY);
}

void CGatherEffect::Step()
{
	for (int i=0;i<GATHER_EFFECT_N_RECORD;i++)
	{
		if (m_gather[i].flag==0||m_gather[i].flag==2)
		{
			m_gather[i].flag=0;
			continue;
		}
		m_gather[i].lastX=m_gather[i].curX;
		m_gather[i].lastY=m_gather[i].curY;
		m_gather[i].curX+=m_gather[i].velX;
		m_gather[i].curY+=m_gather[i].velY;
		m_gather[i].lastRadius=m_gather[i].radius;
		m_gather[i].radius-=m_gather[i].deltaRadius;
		m_gather[i].baseAngle+=m_gather[i].deltaAngle;
		if (m_gather[i].radius<32)
		{
			m_gather[i].flag=2;
			if (m_gather[i].shootInfo.bulletBornType!=0xff)
			{
				if (m_gather[i].shootInfo.bulletBornType>=0xfe)
				{
					m_gather[i].shootInfo.bulletBornType=0;
					m_pStage->m_pEnemyBullet->Shoot(&m_gather[i].shootInfo,true);
				}
				else
					m_pStage->m_pEnemyBullet->Shoot(&m_gather[i].shootInfo,false);
			}
		}
	}
}

void CGatherEffect::Draw()
{
	for (int i=0;i<GATHER_EFFECT_N_RECORD;i++)
	{
		if (m_gather[i].flag!=1)
			continue;
		for (int j=0;j<m_gather[i].nDots;j++)
		{
			float angle=m_gather[i].baseAngle+j*256.0f/m_gather[i].nDots;
			float drawX=cos((float)(angle/256.0f*2*PI))*m_gather[i].radius+m_gather[i].curX;
			float drawY=sin((float)(angle/256.0f*2*PI))*m_gather[i].radius+m_gather[i].curY;
			//image of gather effect is a solid circle which.which is the same as dot bullet without shadow
			m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(m_pStage->m_pStageRes->m_pImgDotBullet,drawX,
													  drawY,m_gather[i].colorR,m_gather[i].colorG,m_gather[i].colorB);
		}
	}
}

}




























