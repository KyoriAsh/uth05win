#include ".\curvingbullet.h"
#include "../Stage.h"
#include "../Chara/Chara.h"

namespace th5w{

CCurvingBullet::CCurvingBullet(void)
{
}

CCurvingBullet::~CCurvingBullet(void)
{
}

void CCurvingBullet::Initialize()
{
	memset(m_bullet,0,sizeof(m_bullet));
}

void CCurvingBullet::AddBullet(float origX,float origY,float angle, float speed, unsigned char colorIdx)
{
	for (int i=0;i<CURVING_BULLET_N_RECORD;i++)
		if (m_bullet[i].flag==0)
		{
			m_bullet[i].flag=1;
			m_bullet[i].angle=angle;
			m_bullet[i].speed=speed;
			m_bullet[i].velX=(float)cos(m_bullet[i].angle/256.0f*2*PI)*m_bullet[i].speed;
			m_bullet[i].velY=(float)sin(m_bullet[i].angle/256.0f*2*PI)*m_bullet[i].speed;
			m_bullet[i].color[0]=m_pStage->m_pStageRes->m_palette[colorIdx*3]/255.0f;
			m_bullet[i].color[1]=m_pStage->m_pStageRes->m_palette[colorIdx*3+1]/255.0f;
			m_bullet[i].color[2]=m_pStage->m_pStageRes->m_palette[colorIdx*3+2]/255.0f;
			for (int j=0;j<16;j++)
			{
				m_bullet[i].nodeX[j]=origX;
				m_bullet[i].nodeY[j]=origY;
			}
			m_bullet[i].mode=1;
			break;
		}
}

void CCurvingBullet::Step()
{
	for (int i=0;i<CURVING_BULLET_N_RECORD;i++)
	{
		if (m_bullet[i].flag==0)
			continue;
		m_bullet[i].age++;
		for (int j=15;j>=1;j--)
		{
			m_bullet[i].nodeX[j]=m_bullet[i].nodeX[j-1];
			m_bullet[i].nodeY[j]=m_bullet[i].nodeY[j-1];
			if (m_pStage->m_pChara->TestCrash(m_bullet[i].nodeX[j],m_bullet[i].nodeY[j],6.0f*16.0f,6.0f*16.0f))
				m_pStage->m_pChara->m_bMiss=true;
		}
		if (m_bullet[i].nodeX[15]<=-16.0f*16.0f||m_bullet[i].nodeX[15]>=400.0f*16.0f
			||m_bullet[i].nodeY[15]<=-16.0f*16.0f||m_bullet[i].nodeY[15]>=384.0f*16.0f)
		{
			m_bullet[i].flag=0;
			continue;
		}
		m_bullet[i].nodeX[0]+=m_bullet[i].velX;
		m_bullet[i].nodeY[0]+=m_bullet[i].velY;
		if (m_pStage->m_pChara->TestCrash(m_bullet[i].nodeX[0],m_bullet[i].nodeY[0],8.0f*16.0f,8.0f*16.0f))
			m_pStage->m_pChara->m_bMiss=true;

		if (m_bullet[i].mode==1)
		{
			m_bullet[i].speed-=1;
			if (m_bullet[i].speed<=4)
				m_bullet[i].mode=2;
		}
		else
			m_bullet[i].speed+=(float)(m_pStage->m_curFrame%2+1);
		int deltaAngle=(int)(m_bullet[i].angle-
							 m_pStage->m_pChara->AngleToChara(m_bullet[i].nodeX[0],m_bullet[i].nodeY[0]));
		while(deltaAngle<-128)
			deltaAngle+=256;
		while(deltaAngle>=128)
			deltaAngle-=256;
		if (abs(deltaAngle)>2)
		{
			if (deltaAngle>0)
				m_bullet[i].angle-=1;
			else
				m_bullet[i].angle+=1;
		}
		else
			m_bullet[i].angle-=deltaAngle;
		m_bullet[i].velX=(float)cos(m_bullet[i].angle/256.0f*2*PI)*m_bullet[i].speed;
		m_bullet[i].velY=(float)sin(m_bullet[i].angle/256.0f*2*PI)*m_bullet[i].speed;
	}
}

void CCurvingBullet::Draw()
{
	for (int i=0;i<CURVING_BULLET_N_RECORD;i++)
	{
		if (m_bullet[i].flag==0)
			continue;
		for (int j=0;j<16;j++)
			m_pStage->m_pStageRes->DrawCircle(m_bullet[i].nodeX[j],m_bullet[i].nodeY[j],10.0f*16.0f,
								 m_bullet[i].color[0],m_bullet[i].color[1],m_bullet[i].color[2],true);
	}
}

}























