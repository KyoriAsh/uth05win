#include ".\largebullet.h"
#include "../Stage.h"
#include "../Chara/Chara.h"

namespace th5w{

CLargeBullet::CLargeBullet(void)
{
}

CLargeBullet::~CLargeBullet(void)
{
}

void CLargeBullet::Initialize()
{
	memset(m_bullet,0,sizeof(m_bullet));
}

void CLargeBullet::AddBullet(float origX,float origY,int imgIdx,float angle, float speed,
							 int life,bool bSplit,ENEMY_SHOOT_INFO *shootInfo)
{
	float speed1=m_pStage->AdjustSpeedBasedOnPlayerPerformance(speed);
	for (int i=0;i<LARGE_BULLET_N_RECORD;i++)
		if (m_bullet[i].flag==0)
		{
			m_bullet[i].flag=1;
			m_bullet[i].curX=origX;
			m_bullet[i].curY=origY;
			m_bullet[i].velX=(float)cos(angle/256.0f*2*PI)*speed1;
			m_bullet[i].velY=(float)sin(angle/256.0f*2*PI)*speed1;
			m_bullet[i].angle=angle;
			m_bullet[i].speed=speed;
			m_bullet[i].img=imgIdx;
			m_bullet[i].life=life;
			m_bullet[i].bHit=false;
			m_bullet[i].bSplit=bSplit;
			if (shootInfo!=NULL)
				memcpy(&m_bullet[i].shootInfo,shootInfo,sizeof(ENEMY_SHOOT_INFO));
			break;
		}
}

void CLargeBullet::Step()
{
	for (int i=0;i<LARGE_BULLET_N_RECORD;i++)
	{
		if (m_bullet[i].flag==0)
			continue;
		m_bullet[i].age++;
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
			if (m_bullet[i].age%4==0)
			{
				m_bullet[i].img++;
				if (m_bullet[i].img>=228)
					m_bullet[i].flag=0;
			}
			continue;
		}
		if (m_pStage->m_pChara->TestCrash(m_bullet[i].curX,m_bullet[i].curY,8.0f*16.0f,8.0f*16.0f))
			m_pStage->m_pChara->m_bMiss=true;

		int dmg=m_pStage->GetDamageFromChara(m_bullet[i].curX,m_bullet[i].curY,8.0f*16.0f,8.0f*16.0f,false);
		m_bullet[i].bHit=false;
		if (dmg>0)
		{
			if (m_bullet[i].img!=212)
			{
				m_pStage->m_bPlaySound[10]=true;
				continue;
			}
			m_bullet[i].bHit=true;
			m_bullet[i].life-=dmg;
			m_pStage->m_bPlaySound[4]=true;
			if (m_bullet[i].life<0)
			{
				m_bullet[i].flag++;
				m_bullet[i].age=0;
				m_bullet[i].img=224;
				m_bullet[i].velX/=4.0f;
				m_bullet[i].velY/=4.0f;
				m_pStage->AddScore(550);
				if (m_bullet[i].bSplit&&m_bullet[i].curY<=240.0f*16.0f)
				{
					m_bullet[i].shootInfo.shootOrigX=m_bullet[i].curX;
					m_bullet[i].shootInfo.shootOrigY=m_bullet[i].curY;
					m_pStage->m_pEnemyBullet->Shoot(&m_bullet[i].shootInfo);
				}
				m_pStage->m_bPlaySound[3]=true;
			}
		}
	}
}

void CLargeBullet::Draw()
{
	for (int i=0;i<LARGE_BULLET_N_RECORD;i++)
		if (m_bullet[i].flag!=0)
		{
			int imgIdx=m_bullet[i].img;
			if (imgIdx<220)
			{
				imgIdx+=m_bullet[i].age%4;
				if (m_bullet[i].bHit)
					imgIdx+=8;
			}
			C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_bullet[i].curX,m_bullet[i].curY);
		}
}

}




















