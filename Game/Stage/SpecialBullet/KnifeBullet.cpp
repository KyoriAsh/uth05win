#include ".\knifebullet.h"
#include "../Stage.h"
#include "../Chara/Chara.h"
#include "../Circle/Circle.h"

namespace th5w{

CKnifeBullet::CKnifeBullet(void)
{
}

CKnifeBullet::~CKnifeBullet(void)
{
}

void CKnifeBullet::Initialize()
{
	memset(m_bullet,0,sizeof(m_bullet));
}

void CKnifeBullet::AddBullet(float origX,float origY,float angle,float speed, int cooldown)
{
	float speed1=m_pStage->AdjustSpeedBasedOnPlayerPerformance(speed);
	for (int i=0;i<KNIFE_BULLET_N_RECORD;i++)
		if (m_bullet[i].flag==0)
		{
			m_bullet[i].flag=1;
			m_bullet[i].curX=origX;
			m_bullet[i].curY=origY;
			m_bullet[i].velX=(float)cos(angle/256.0f*2*PI)*speed1;
			m_bullet[i].velY=(float)sin(angle/256.0f*2*PI)*speed1;
			m_bullet[i].angle=angle;
			m_bullet[i].img=m_pStage->m_pEnemyBullet->AdjustBulletImageBasedOnAngle(193,angle);
			m_bullet[i].speed=speed1;
			m_bullet[i].cooldown=cooldown;
			m_pStage->m_pCircle->AddCircle(origX,origY,0.6f,0.6f,1.0f,false);
			break;
		}
}

void CKnifeBullet::Step()
{
	for (int i=0;i<KNIFE_BULLET_N_RECORD;i++)
	{
		if (m_bullet[i].flag==0)
			continue;
		if (m_pStage->m_pEnemyBullet->GetClearBulletTimeLeft()!=0&&m_bullet[i].flag==1)
		{
			m_bullet[i].flag=2;
			m_bullet[i].cooldown=0;
		}
		if (m_bullet[i].cooldown>0)
		{
			m_bullet[i].cooldown--;
			if (m_bullet[i].cooldown==0)
			{
				m_bullet[i].img=m_pStage->m_pEnemyBullet->AdjustBulletImageBasedOnAngle(193,m_bullet[i].angle);
				m_pStage->m_bPlaySound[3]=true;
			}
			else
			{
				if (i%2==0)
				{
					m_bullet[i].img+=2;
					if (m_bullet[i].img>=225)
						m_bullet[i].img-=32;
				}
				else
				{
					m_bullet[i].img-=2;
					if (m_bullet[i].img<193)
						m_bullet[i].img+=32;
				}
				continue;
			}
		}
		m_bullet[i].lastX=m_bullet[i].curX;
		m_bullet[i].lastY=m_bullet[i].curY;
		m_bullet[i].curX+=m_bullet[i].velX;
		m_bullet[i].curY+=m_bullet[i].velY;
		if (m_bullet[i].curX<=-16.0f*16.0f||m_bullet[i].curX>=400.0f*16.0f
			||m_bullet[i].curY<=-16.0f*16.0f||m_bullet[i].curY>=384.0f*16.0f)
		{
			m_bullet[i].flag=0;
			continue;
		}
		if (m_bullet[i].flag==2)
		{
			if (m_bullet[i].img<225)
			{
				m_bullet[i].img=225;
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
					if (m_bullet[i].img>=229)
						m_bullet[i].flag=0;
				}
			}
			continue;
		}
		if (m_pStage->m_pChara->TestCrash(m_bullet[i].curX,m_bullet[i].curY,7.0f*16.0f,7.0f*16.0f))
		{
			m_pStage->m_pChara->m_bMiss=true;
			m_bullet[i].flag=2;
		}
	}
}

void CKnifeBullet::Draw()
{
	for (int i=0;i<KNIFE_BULLET_N_RECORD;i++)
		if (m_bullet[i].flag!=0)
		{
			C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_bullet[i].img);
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_bullet[i].curX,m_bullet[i].curY);
		}
}

}























