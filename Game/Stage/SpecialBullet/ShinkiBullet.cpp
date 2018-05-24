#include ".\shinkibullet.h"
#include "../Stage.h"
#include "../Chara/Chara.h"

namespace th5w{

CShinkiBullet::CShinkiBullet(void)
{
}

CShinkiBullet::~CShinkiBullet(void)
{
}

void CShinkiBullet::Initialize()
{
	memset(m_bullet,0,sizeof(m_bullet));
}

void CShinkiBullet::AddBullet(float origX,float origY,int imgIdx,float angle, float speed)
{
	float speed1=m_pStage->AdjustSpeedBasedOnPlayerPerformance(speed);
	for (int i=0;i<SHINKI_BULLET_N_RECORD;i++)
		if (m_bullet[i].flag==0)
		{
			m_bullet[i].flag=1;
			m_bullet[i].curX=origX;
			m_bullet[i].curY=origY;
			m_bullet[i].velX=(float)cos(angle/256.0f*2*PI)*speed1;
			m_bullet[i].velY=(float)sin(angle/256.0f*2*PI)*speed1;
			m_bullet[i].img=imgIdx;
			m_bullet[i].radius=48.0f*16.0f;
			break;
		}
}

void CShinkiBullet::Step()
{
	for (int i=0;i<SHINKI_BULLET_N_RECORD;i++)
	{
		if (m_bullet[i].flag==0)
			continue;
		if (m_bullet[i].flag==1)
		{
			m_bullet[i].radius-=2.0f*16.0f;
			if (m_bullet[i].radius>16.001f*16.0f)
				continue;
			m_bullet[i].flag++;
		}
		if (m_pStage->m_pEnemyBullet->GetClearBulletTimeLeft()!=0||m_pStage->m_pEnemyBullet->m_noEnemyBullet!=0)
			m_bullet[i].flag=3;
		m_bullet[i].curX+=m_bullet[i].velX;
		m_bullet[i].curY+=m_bullet[i].velY;
		if (m_bullet[i].curX<=-16.0f*16.0f||m_bullet[i].curX>=400.0f*16.0f
			||m_bullet[i].curY<=-16.0f*16.0f||m_bullet[i].curY>=384.0f*16.0f)
		{
			m_bullet[i].flag=0;
			continue;
		}
		if (m_bullet[i].flag==3)
		{
			if (m_bullet[i].img<204)
			{
				m_bullet[i].img=204;
				m_bullet[i].velX/=2.0f;
				m_bullet[i].velY/=2.0f;
				m_bullet[i].vanishAge=0;
			}
			else
			{
				m_bullet[i].vanishAge++;
				if (m_bullet[i].vanishAge%4==0)
				{
					m_bullet[i].img++;
					if (m_bullet[i].img>=208)
						m_bullet[i].flag=0;
				}
			}
		}
		else
		{
			if (m_pStage->m_pChara->TestCrash(m_bullet[i].curX,m_bullet[i].curY,8.0f*16.0f,8.0f*16.0f))
			{
				m_pStage->m_pChara->m_bMiss=true;
				m_bullet[i].flag=3;
			}
		}
	}
}

void CShinkiBullet::Draw()
{
	for (int i=0;i<SHINKI_BULLET_N_RECORD;i++)
		if (m_bullet[i].flag!=0)
		{
			if (m_bullet[i].flag==1)
				m_pStage->m_pStageRes->DrawCircle(m_bullet[i].curX,m_bullet[i].curY,m_bullet[i].radius,1,1,1,true);
			else
			{
				C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_bullet[i].img);
				m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_bullet[i].curX,m_bullet[i].curY);
			}
		}
}

}


























