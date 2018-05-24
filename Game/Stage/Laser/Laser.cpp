#include ".\laser.h"
#include "../Stage.h"
#include "../Chara/Chara.h"
#include "../EnemyBullet/EnemyBullet.h"
#include <string.h>

namespace th5w{

CLaser::CLaser(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CLaser::~CLaser(void)
{
}

void CLaser::Initialize()
{
	memset(m_laser,0,sizeof(m_laser));
}

void CLaser::AddLaserFixed(int laserIdx,float srcX,float srcY,float color[3],float expectedWidth,float angle,
						   int ageThres,int shootAgeThres)
{
	if (m_laser[laserIdx].mode!=LASER_MODE_NONE)
		return;
	m_laser[laserIdx].mode=LASER_MODE_WAIT_TO_GROW;
	m_laser[laserIdx].srcX=srcX;
	m_laser[laserIdx].srcY=srcY;
	m_laser[laserIdx].lenLower=16.0f*16.0f;
	m_laser[laserIdx].lenUpper=550.0f*16.0f;
	m_laser[laserIdx].age=0;
	m_laser[laserIdx].ageThres=ageThres;
	m_laser[laserIdx].shootAgeThres=shootAgeThres;
	m_laser[laserIdx].color[0]=color[0];
	m_laser[laserIdx].color[1]=color[1];
	m_laser[laserIdx].color[2]=color[2];
	m_laser[laserIdx].width=1;
	m_laser[laserIdx].expectedWidth=expectedWidth;
	m_laser[laserIdx].angle=angle;
	m_pStage->m_bPlaySound[5]=true;
}

void CLaser::AddLaserShootOut(float srcX,float srcY,float color[3],float angle,int ageThres,float speed)
{
	for (int i=0;i<LASER_N_RECORD;i++)
		if (m_laser[i].mode==LASER_MODE_NONE)
		{
			m_laser[i].mode=LASER_MODE_SHOOT_OUT;
			m_laser[i].lenLower=16.0f*16.0f;
			m_laser[i].lenUpper=16.0f*16.0f;
			m_laser[i].age=0;
			m_laser[i].ageThres=ageThres;
			m_laser[i].color[0]=color[0];
			m_laser[i].color[1]=color[1];
			m_laser[i].color[2]=color[2];
			m_laser[i].speed=m_pStage->AdjustSpeedBasedOnPlayerPerformance(speed);
			m_laser[i].expectedWidth=6;
			m_laser[i].width=6;
			m_laser[i].angle=angle;
			m_laser[i].srcX=(float)(srcX+cos(angle/256.0f*2*PI)*16.0f*8.0f);
			m_laser[i].srcY=(float)(srcY+sin(angle/256.0f*2*PI)*16.0f*8.0f);
			m_pStage->m_bPlaySound[5]=true;
			break;
		}
}

void CLaser::StartGrow(int laserIdx)
{
	if (m_laser[laserIdx].mode==LASER_MODE_WAIT_TO_GROW)
	{
		m_laser[laserIdx].mode=LASER_MODE_GROW;
		m_pStage->m_bPlaySound[6]=true;
	}
}

void CLaser::SetMode(int laserIdx,unsigned char newMode)
{
	m_laser[laserIdx].mode=newMode;
}

void CLaser::RemoveLaser(int laserIdx)
{
	if (m_laser[laserIdx].mode==LASER_MODE_PRESENT)
	{
		m_laser[laserIdx].mode=LASER_MODE_SHRINK;
		return;
	}
	if (m_laser[laserIdx].mode==LASER_MODE_SHOOT_OUT)
	{
		m_laser[laserIdx].mode=LASER_MODE_SHOOT_OUT_END;
		return;
	}
	if (m_laser[laserIdx].mode==LASER_MODE_WAIT_TO_GROW||m_laser[laserIdx].mode==LASER_MODE_GROW
		||m_laser[laserIdx].mode==LASER_MODE_SHRINK_AND_WAIT_TO_GROW)
		m_laser[laserIdx].mode=LASER_MODE_NONE;
}

void CLaser::TestCrash(int laserIdx)
{
	for (float len=m_laser[laserIdx].lenUpper;len>=m_laser[laserIdx].lenLower;len-=16.0f*16.0f)
	{
		float testX=m_laser[laserIdx].srcX+(float)cos(m_laser[laserIdx].angle/256.0f*2*PI)*len;
		float testY=m_laser[laserIdx].srcY+(float)sin(m_laser[laserIdx].angle/256.0f*2*PI)*len;
		//PC98 ver uses 6.0f*16.0f as crash area
		//this makes it possible for the player to go through the laser at certain positions.
		//(adjacent centers are 16.0f*16.0f away. which is larger than 2*6.0f*16.0f)
		if (m_pStage->m_pChara->TestCrash(testX,testY,8.0f*16.0f,8.0f*16.0f))
			m_pStage->m_pChara->m_bMiss=true;
	}
}

void CLaser::Step()
{
	for (int i=0;i<LASER_N_RECORD;i++)
		if (m_laser[i].mode!=LASER_MODE_NONE)
		{
			if (m_pStage->m_pEnemyBullet->GetClearBulletTimeLeft()!=0||
				m_pStage->m_pEnemyBullet->m_noEnemyBullet)
			{
				if (m_laser[i].mode==LASER_MODE_SHOOT_OUT)
				{
					m_laser[i].mode=LASER_MODE_SHOOT_OUT_END;
					m_laser[i].speed/=2.0f;
				}
			}
			switch(m_laser[i].mode)
			{
			case LASER_MODE_SHOOT_OUT:
				if (m_laser[i].lenLower<550.0f*16.0f)			//not bug! 
					m_laser[i].lenUpper+=m_laser[i].speed;
				if (m_laser[i].age>=m_laser[i].ageThres)
					m_laser[i].lenLower+=m_laser[i].speed;
				TestCrash(i);
				break;
			case LASER_MODE_WAIT_TO_GROW:
				if (m_laser[i].ageThres>0&&m_laser[i].age>=m_laser[i].ageThres)
				{
					m_laser[i].mode=LASER_MODE_GROW;
					m_pStage->m_bPlaySound[6]=true;
				}
				break;
			case LASER_MODE_GROW:
				if (m_laser[i].age%2!=0)
					m_laser[i].width+=2;
				if (m_laser[i].width>=m_laser[i].expectedWidth)
					m_laser[i].mode=LASER_MODE_PRESENT;
			    break;
			case LASER_MODE_PRESENT:
				TestCrash(i);
				if (m_laser[i].shootAgeThres>0&&m_laser[i].age>=m_laser[i].shootAgeThres)
					m_laser[i].mode=LASER_MODE_SHRINK;
			    break;
			case LASER_MODE_SHRINK:
				if (m_laser[i].age%2!=0)
					m_laser[i].width-=2;
				if (m_laser[i].width<1)
					m_laser[i].mode=LASER_MODE_NONE;
				break;
			case LASER_MODE_SHRINK_AND_WAIT_TO_GROW:
				if (m_laser[i].age%2!=0)
					m_laser[i].width-=2;
				if (m_laser[i].width<=1)
				{
					m_laser[i].mode=LASER_MODE_WAIT_TO_GROW;
					m_laser[i].age=0;
				}
				break;
			case LASER_MODE_SHOOT_OUT_END:
				if (m_laser[i].lenLower<550.0f*16.0f)			//not bug! 
					m_laser[i].lenUpper+=m_laser[i].speed;
				if (m_laser[i].age>=m_laser[i].ageThres)
					m_laser[i].lenLower+=m_laser[i].speed;
				m_laser[i].width+=2;
				if (m_laser[i].width>=28)
					m_laser[i].mode=LASER_MODE_NONE;
			    break;
			}
			m_laser[i].age++;
			if (m_laser[i].lenLower>=550.0f*16.0f)
				m_laser[i].mode=LASER_MODE_NONE;
		}
}

void CLaser::Draw()
{
	for (int i=0;i<LASER_N_RECORD;i++)
	{
		if (m_laser[i].mode==LASER_MODE_NONE)
			continue;
		if (m_laser[i].mode!=LASER_MODE_SHOOT_OUT_END)
		{
			float srcX=m_laser[i].srcX+(float)cos(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenLower;
			float srcY=m_laser[i].srcY+(float)sin(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenLower;
			float tarX=m_laser[i].srcX+(float)cos(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenUpper;
			float tarY=m_laser[i].srcY+(float)sin(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenUpper;
			float cRadius=m_laser[i].width;
			if (cRadius<2)
				cRadius=2;
			float tWidth=m_laser[i].width;
			if (m_laser[i].width>=3)
			{
				if (m_laser[i].lenLower<=16.0f*16.0f)
				{
					m_pStage->m_pStageRes->DrawCircle(srcX,srcY,cRadius*16.0f,
										 m_laser[i].color[0],m_laser[i].color[1],m_laser[i].color[2],true);
					cRadius-=2;
				}
				if (m_laser[i].lenUpper>=2.0f*16.0f)
				{
					m_pStage->m_pStageRes->DrawLine(srcX,srcY,tarX,tarY,
									   m_laser[i].color[0],m_laser[i].color[1],m_laser[i].color[2],
									   m_laser[i].width);
				}
				if (tWidth>=5)
					tWidth-=4;
				else
					tWidth-=2;
			}
			if (m_laser[i].lenLower<=16.0f*16.0f)
				m_pStage->m_pStageRes->DrawCircle(srcX,srcY,cRadius*16.0f,1,1,1,true);
			m_pStage->m_pStageRes->DrawLine(srcX,srcY,tarX,tarY,1,1,1,tWidth);
		}
		else
		{
			float origX[]={m_laser[i].srcX+(float)cos((m_laser[i].angle+64.0f)/256.0f*2*PI)*m_laser[i].width*16.0f/2,
						   m_laser[i].srcX-(float)cos((m_laser[i].angle+64.0f)/256.0f*2*PI)*m_laser[i].width*16.0f/2};
			float origY[]={m_laser[i].srcY+(float)sin((m_laser[i].angle+64.0f)/256.0f*2*PI)*m_laser[i].width*16.0f/2,
						   m_laser[i].srcY-(float)sin((m_laser[i].angle+64.0f)/256.0f*2*PI)*m_laser[i].width*16.0f/2};
			float srcX_0=origX[0]+(float)cos(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenLower;
			float srcY_0=origY[0]+(float)sin(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenLower;
			float tarX_0=origX[0]+(float)cos(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenUpper;
			float tarY_0=origY[0]+(float)sin(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenUpper;
			float srcX_1=origX[1]+(float)cos(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenLower;
			float srcY_1=origY[1]+(float)sin(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenLower;
			float tarX_1=origX[1]+(float)cos(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenUpper;
			float tarY_1=origY[1]+(float)sin(m_laser[i].angle/256.0f*2*PI)*m_laser[i].lenUpper;
			m_pStage->m_pStageRes->DrawLine(srcX_0,srcY_0,tarX_0,tarY_0,1,1,1,1);
			m_pStage->m_pStageRes->DrawLine(srcX_1,srcY_1,tarX_1,tarY_1,1,1,1,1);
		}
	}
}

}













