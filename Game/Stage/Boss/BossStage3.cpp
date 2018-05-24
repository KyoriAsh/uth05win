#include ".\bossstage3.h"
#include "../Stage.h"
#include "../../Game.h"
#include "../CharaBullet/CharaBullet.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

namespace th5w{

CBossStage3::CBossStage3(void)
{
}

CBossStage3::~CBossStage3(void)
{
}

void CBossStage3::Initialize()
{
	m_curX=192.0f*16.0f;
	m_curY=64.0f*16.0f;
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_velX=0;
	m_velY=0;
	m_curImage=180;
	m_bossDmgAreaHalfWidth=24.0f*16.0f;
	m_bossDmgAreaHalfHeight=24.0f*16.0f;
	m_imgStay=180;
	m_imgGoLeft=188;
	m_imgGoRight=186;

	memset(m_puppet,0,sizeof(m_puppet));
}

void CBossStage3::PuppetFlyIn()
{
	m_puppet[0].curX=96.0f*16.0f+(float)cos(m_puppet[0].angle/256.0f*2*PI)*m_puppet[0].radius;
	m_puppet[0].curY=96.0f*16.0f+(float)sin(m_puppet[0].angle/256.0f*2*PI)*m_puppet[0].radius;
	m_puppet[0].tarX=m_puppet[0].curX;
	m_puppet[0].tarY=m_puppet[0].curY;
	m_puppet[0].angle-=2;
	m_puppet[0].radius-=2*16.0f;
	m_puppet[1].curX=288.0f*16.0f+(float)cos(m_puppet[1].angle/256.0f*2*PI)*m_puppet[1].radius;
	m_puppet[1].curY=96.0f*16.0f+(float)sin(m_puppet[1].angle/256.0f*2*PI)*m_puppet[1].radius;
	m_puppet[1].tarX=m_puppet[1].curX;
	m_puppet[1].tarY=m_puppet[1].curY;
	m_puppet[1].angle+=2;
	m_puppet[1].radius-=2*16.0f;
}

int CBossStage3::PuppetNoShoot(int idx)
{
	return 0;
}

int CBossStage3::PuppetPrepareEnter(int idx)
{
	return m_puppet[idx].shootAge>=96?1:0;
}

int CBossStage3::PuppetShootType0(int idx)
{
	int age=m_puppet[idx].shootAge;
	if (age==16)
		m_pStage->m_pCircle->AddCircle(m_puppet[idx].curX,m_puppet[idx].curY,1,14.0f/15.0f,12.0f/15.0f,false);
	if (age<32)
		return 0;
	if (age<64&&m_pStage->m_curFrame%2!=0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletImg=0;
		shootInfo.shootOrigX=m_puppet[idx].curX;
		shootInfo.shootOrigY=m_puppet[idx].curY;
		shootInfo.bulletBornType=16;
		shootInfo.bulletSpeed=32;
		shootInfo.shootType=EST00_SINGLE;
		shootInfo.shootAngle=(float)(rand()%256);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_puppet[idx].image=194;
		return 0;
	}
	if (age<96)
		return 0;
	m_puppet[idx].image=190;
	return 1;
}

int CBossStage3::PuppetShootType1(int idx)
{
	int age=m_puppet[idx].shootAge;
	if (age==16)
		m_pStage->m_pCircle->AddCircle(m_puppet[idx].curX,m_puppet[idx].curY,1,14.0f/15.0f,12.0f/15.0f,false);
	if (age<32)
		return 0;
	m_puppet[idx].image=194;
	if (age==32)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletImg=0;
		shootInfo.shootOrigX=m_puppet[idx].curX;
		shootInfo.shootOrigY=m_puppet[idx].curY;
		shootInfo.bulletBornType=16;
		shootInfo.bulletSpeed=24;
		shootInfo.shootType=EST09_NWAY_MULTI_TO_CHARA;
		shootInfo.nWay=3;
		shootInfo.nWayUnitAngle=18;
		shootInfo.nMulti=5;
		shootInfo.deltaSpeed=5;
		shootInfo.shootAngle=0;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		return 0;
	}
	if (age<96)
		return 0;
	m_puppet[idx].image=190;
	return 1;
}

int CBossStage3::PuppetShootType2(int idx)
{
	int age=m_puppet[idx].shootAge;
	if (age==16)
		m_pStage->m_pCircle->AddCircle(m_puppet[idx].curX,m_puppet[idx].curY,1,14.0f/15.0f,12.0f/15.0f,false);
	if (age<32)
		return 0;
	m_puppet[idx].image=194;
	if (age<64&&age%8==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletImg=0;
		shootInfo.shootOrigX=m_puppet[idx].curX;
		shootInfo.shootOrigY=m_puppet[idx].curY;
		shootInfo.bulletBornType=16;
		shootInfo.bulletSpeed=32;
		shootInfo.shootType=EST03_NWAY_TO_CHARA;
		shootInfo.nWay=5;
		shootInfo.nWayUnitAngle=16;
		shootInfo.shootAngle=0;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		return 0;
	}
	if (age<96)
		return 0;
	m_puppet[idx].image=190;
	return 1;
}

int CBossStage3::PuppetShootType3(int idx)
{
	int age=m_puppet[idx].shootAge;
	if (age==16)
		m_pStage->m_pCircle->AddCircle(m_puppet[idx].curX,m_puppet[idx].curY,1,14.0f/15.0f,12.0f/15.0f,false);
	if (age<32)
		return 0;
	m_puppet[idx].image=194;
	if (age<64&&age%8==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletImg=0;
		shootInfo.shootOrigX=m_puppet[idx].curX;
		shootInfo.shootOrigY=m_puppet[idx].curY;
		shootInfo.bulletBornType=16;
		shootInfo.bulletSpeed=32;
		shootInfo.shootType=EST05_NWAY_ALLAROUND_TO_CHARA;
		shootInfo.nWay=12;
		shootInfo.shootAngle=0;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		return 0;
	}
	if (age<96)
		return 0;
	m_puppet[idx].image=190;
	return 1;
}

int CBossStage3::PuppetShootPhase14(int idx)
{
	int age=m_puppet[idx].shootAge;
	if (age==16)
		m_pStage->m_pCircle->AddCircle(m_puppet[idx].curX,m_puppet[idx].curY,1,14.0f/15.0f,12.0f/15.0f,false);
	if (age<32)
		return 0;
	m_puppet[idx].image=194;
	if (m_pStage->m_curFrame%32==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletImg=0;
		shootInfo.shootOrigX=m_puppet[idx].curX;
		shootInfo.shootOrigY=m_puppet[idx].curY;
		shootInfo.bulletBornType=16;
		shootInfo.bulletSpeed=24;
		shootInfo.shootType=EST03_NWAY_TO_CHARA;
		shootInfo.nWay=7;
		shootInfo.nWayUnitAngle=12;
		shootInfo.shootAngle=0;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
	return 0;
}

void CBossStage3::StepPuppet()
{
	for (int i=0;i<2;i++)
	{
		if (m_puppet[i].flag==0)
			return;
		if (m_puppet[i].flag!=1)
		{
			if (m_puppet[i].flag%4==0)
			{
				m_puppet[i].image++;
				if (m_puppet[i].image>=12)
				{
					m_puppet[i].flag=0;
					m_puppet[i].curX=192.0f*16.0f;
					m_puppet[i].curY=-256.0f*16.0f;
					m_puppet[i].tarX=192.0f*16.0f;
					m_puppet[i].tarY=-16.0f*16.0f;
					m_puppet[i].life=500;
					m_puppet[i].image=192;
					m_puppet[i].mode=-1;
					m_curHP-=300;
				}
			}
			m_puppet[i].flag++;
			continue;
		}
		if (m_puppet[i].curX!=m_puppet[i].tarX||m_puppet[i].curY!=m_puppet[i].tarY)
		{
			float deltaX=(m_puppet[i].tarX-m_puppet[i].curX);
            if (fabs(deltaX)>=32.0f)
				m_puppet[i].curX+=deltaX/32.0f;
			else
				if (fabs(deltaX)>=4.0f)
					m_puppet[i].curX+=deltaX/4.0f;
				else
					m_puppet[i].curX=m_puppet[i].tarX;
			float deltaY=(m_puppet[i].tarY-m_puppet[i].curY);
			if (fabs(deltaY)>=32.0f)
				m_puppet[i].curY+=deltaY/32.0f;
			else
				if (fabs(deltaY)>=4.0f)
					m_puppet[i].curY+=deltaY/4.0f;
				else
					m_puppet[i].curY=m_puppet[i].tarY;
			m_puppet[i].image=192;
		}
		else
		{
			m_puppet[i].image=190;
			int res;
			switch(m_puppet[i].mode)
			{
			case -2:
				res=PuppetNoShoot(i);
				break;
			case -1:
				res=PuppetPrepareEnter(i);
				break;
			case 0:
				res=PuppetShootType0(i);
				break;
			case 1:
				res=PuppetShootType1(i);
			    break;
			case 2:
				res=PuppetShootType2(i);
			    break;
			case 3:
				res=PuppetShootType3(i);
				break;
			case 4:
				res=PuppetShootPhase14(i);
				break;
			}
			if (res!=0)
			{
				m_puppet[i].tarX=32.0f*16.0f+(float)(rand()%(320*16));
				m_puppet[i].tarY=16.0f*16.0f+(float)(rand()%(160*16));
				m_puppet[i].mode=rand()%4;
				m_puppet[i].shootAge=0;
			}
			else
				m_puppet[i].shootAge++;
		}
		if (m_pStage->m_pChara->TestCrash(m_puppet[i].curX,m_puppet[i].curY,12.0f*16.0f,12.0f*16.0f))
			m_pStage->m_pChara->m_bMiss=true;
		if (m_shieldState!=3)
		{
			int dmg=m_pStage->GetDamageFromChara(m_puppet[i].curX,m_puppet[i].curY,20.0f*16.0f,20.0f*16.0f,false);
			if (dmg==0)
				m_puppet[i].bHitThisFrame=false;
			else
			{
				m_puppet[i].bHitThisFrame=true;
				m_pStage->m_bPlaySound[4]=true;
			}
			m_puppet[i].life-=dmg;
			if (m_puppet[i].life<0)				//PC98 ver uses <0, keep the same here
			{
				m_puppet[i].flag++;
				m_puppet[i].image=4;
				m_puppet[i].bHitThisFrame=false;
				m_pStage->m_bPlaySound[12]=true;
				m_pStage->AddScore(100);
			}
		}
		else
			m_puppet[i].bHitThisFrame=false;
	}
}

void CBossStage3::Phase0()
{
	if (m_curPhaseFrame==1)
	{
		m_curHP=9600;
		m_curPhaseEndHP=7400;
	}
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=128)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_pStage->m_bPlaySound[13]=true;
	}
}

void CBossStage3::Phase1()
{
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=64)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=1;
		m_nModeChange=0;
		m_shieldState=0;
	}
}

void CBossStage3::Phase2Shoot()
{
	if (m_curPhaseFrame<64)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY-8.0f*16.0f,8,9,m_curPhaseFrame-40);
		m_curImage=184;
		if (m_curPhaseFrame==40)
			m_pStage->m_bPlaySound[8]=true;
		return;
	}
	if (m_curPhaseFrame==64)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=2;
		shootInfo.shootType=EST13_RANDOM_BASE_SPEED_32;
		shootInfo.nWay=2;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		for (int i=0;i<32;i++)
		{
			shootInfo.bulletImg=44*(i%2);
			shootInfo.bulletSpeed=(float)(12+rand()%64);
			shootInfo.shootOrigX=(float)(m_curX+rand()%(48*16)-24*16);
			shootInfo.shootOrigY=(float)(m_curY+rand()%(48*16)-32*16);
			m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		}
		m_curImage=182;
		m_pStage->m_bPlaySound[15]=true;
	}
}

void CBossStage3::Phase2()
{
	Phase2Shoot();
	GetDmgOnAndCrashChara();
	if (m_curPhaseFrame==128)
	{
		m_puppet[0].flag=1;
		m_puppet[0].image=190;
		m_puppet[0].radius=256.0f*16.0f;
		m_puppet[0].angle=96;
		m_puppet[0].life=500;
		m_puppet[0].mode=0;
		m_puppet[1].flag=1;
		m_puppet[1].image=190;
		m_puppet[1].radius=256.0f*16.0f;
		m_puppet[1].angle=32;
		m_puppet[1].life=500;
		m_puppet[1].mode=0;
		return;
	}
	if (m_curPhaseFrame<=128)
		return;
	PuppetFlyIn();
	if (m_puppet[0].radius<=0)
	{
		m_curPhase++;
		m_curMode=0;
		m_curPhaseFrame=0;
		m_nModeChange=0;
	}
}

void CBossStage3::Phase3ShootType1()
{
	Phase2Shoot();
	if (m_curPhaseFrame==96)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
	}
}

void CBossStage3::Phase3ShootType2_6_1()
{
	if (m_curPhaseFrame<64)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY-8.0f*16.0f,8,9,m_curPhaseFrame-40);
		m_curImage=184;
		if (m_curPhaseFrame==40)
			m_pStage->m_bPlaySound[8]=true;
		return;
	}
	if (m_curPhaseFrame==64)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;			//not at m_curY!
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST11_NWAY_ALLAROUND_MULTI_TO_CHARA;
		shootInfo.bulletImg=48;
		shootInfo.nWay=28;
		shootInfo.nMulti=4;
		shootInfo.deltaSpeed=8;
		shootInfo.bulletSpeed=24;
		shootInfo.shootAngle=0;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_curImage=180;
		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame==96)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
	}
}

void CBossStage3::Phase3ShootType3_6_2__9_1()
{
	if (m_curPhaseFrame<64)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY-8.0f*16.0f,8,9,m_curPhaseFrame-40);
		m_curImage=184;
		if (m_curPhaseFrame==40)
			m_pStage->m_bPlaySound[8]=true;
		return;
	}
	if (m_curPhaseFrame==64)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;			//not at m_curY!
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST09_NWAY_MULTI_TO_CHARA;
		shootInfo.bulletImg=44;
		shootInfo.nWay=7;
		shootInfo.nWayUnitAngle=8;
		shootInfo.nMulti=5;
		shootInfo.deltaSpeed=7;
		shootInfo.bulletSpeed=24;
		shootInfo.shootAngle=0;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_curImage=180;
		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame==96)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
	}
}

void CBossStage3::Phase6ShootType3_9_3__12_2()
{
	if (m_curPhaseFrame<64)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY-8.0f*16.0f,8,9,m_curPhaseFrame-40);
		m_curImage=184;
		if (m_curPhaseFrame==40)
			m_pStage->m_bPlaySound[8]=true;
		return;
	}
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;			//not at m_curY!
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST07_MULTI_TO_CHARA;
		shootInfo.bulletImg=44;
		shootInfo.nMulti=8;
		shootInfo.deltaSpeed=7;
		shootInfo.bulletSpeed=24;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		shootInfo.shootAngle=(float)(8+(m_curPhaseFrame-64)/4*5);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		shootInfo.shootAngle=-shootInfo.shootAngle;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_curImage=180;
		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame==96)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
	}
}

void CBossStage3::Phase9ShootType2_12_1()
{
	if (m_curPhaseFrame<64)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY-8.0f*16.0f,8,9,m_curPhaseFrame-40);
		m_curImage=184;
		if (m_curPhaseFrame==40)
			m_pStage->m_bPlaySound[8]=true;
		return;
	}
	if (m_curPhaseFrame==64)
	{
		float angle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY-8.0f*16.0f);
		float c[3]={m_pStage->m_pStageRes->m_palette[6*3]/255.0f,m_pStage->m_pStageRes->m_palette[6*3+1]/255.0f,
					m_pStage->m_pStageRes->m_palette[6*3+2]/255.0f};
		m_pStage->m_pLaser->AddLaserShootOut(m_curX,m_curY-8.0f*16.0f,c,angle,40,80);
		m_pStage->m_pLaser->AddLaserShootOut(m_curX,m_curY-8.0f*16.0f,c,angle+16,40,80);
		m_pStage->m_pLaser->AddLaserShootOut(m_curX,m_curY-8.0f*16.0f,c,angle-16,40,80);
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;			//not at m_curY!
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST13_RANDOM_BASE_SPEED_32;
		shootInfo.bulletImg=44;
		shootInfo.nWay=20;
		shootInfo.bulletSpeed=24;
		shootInfo.shootAngle=0;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_curImage=180;
	}
	if (m_curPhaseFrame==96)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
	}
}

void CBossStage3::Phase12ShootType3()
{
	if (m_curPhaseFrame<64)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY-8.0f*16.0f,8,9,m_curPhaseFrame-40);
		m_curImage=184;
		if (m_curPhaseFrame==40)
			m_pStage->m_bPlaySound[8]=true;
		return;
	}
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;			//not at m_curY!
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST04_NWAY_ALLAROUND;
		shootInfo.bulletImg=48;
		shootInfo.nWay=20;
		shootInfo.bulletSpeed=40;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		shootInfo.shootAngle=(float)((m_curPhaseFrame-64)/4*2);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_curImage=180;
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame==96)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
	}
}

void CBossStage3::Phase3_6_9_12Shoot()
{
	switch(m_curPhase-3+m_curMode-1)
	{
	case 0:
		Phase3ShootType1();
		break;
	case 1:
	case 3:
		Phase3ShootType2_6_1();
		break;
	case 2:
	case 4:
	case 6:
		Phase3ShootType3_6_2__9_1();
		break;
	case 5:
	case 8:
	case 10:
		Phase6ShootType3_9_3__12_2();
		break;
	case 7:
	case 9:
		Phase9ShootType2_12_1();
		break;
	case 11:
		Phase12ShootType3();
		break;
	}
}

void CBossStage3::Phase3_6_9_12()
{
	bool bPhaseEnd=false;
	if (m_curMode==0)
	{
		if (RandomWalk(m_curPhaseFrame-8)!=0)
		{
			m_curMode=1;
			m_curPhaseFrame=0;
			m_nModeChange++;
			if (m_nModeChange>=24)
				bPhaseEnd=true;
			else
				m_curMode=rand()%3+1;
		}
	}
	else
		Phase3_6_9_12Shoot();
	if (!bPhaseEnd)
		if (GetDmgOnAndCrashChara()!=0)
		{
			AddDefeatScoreAndPopupNumbers(7);
			DropItem();
			bPhaseEnd=true;
		}
	if (bPhaseEnd)
	{
		m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
		AddExplodeEffect(1,m_curX,m_curY);
		m_curPhase++;
		m_curPhaseFrame=0;
		m_nModeChange=0;
		m_curMode=0;
		m_curHP=m_curPhaseEndHP;
		if (m_curPhase>=12)
			m_curPhaseEndHP=0;
		else
			m_curPhaseEndHP-=2200;
		m_shieldFrame=0;
	}
}

void CBossStage3::Phase4_7_10()
{
	m_puppet[0].tarX=128.0f*16.0f;
	m_puppet[0].tarY=128.0f*16.0f;
	m_puppet[1].tarX=256.0f*16.0f;
	m_puppet[1].tarY=128.0f*16.0f;
	m_puppet[0].life=500;
	m_puppet[1].life=500;
	m_puppet[0].mode=-2;
	m_puppet[1].mode=-2;
	if (MoveTo(192.0f*16.0f,64.0f*16.0f)!=0)
	{
		m_curPhaseFrame=0;
		m_curPhase++;
	}
	GetDmgOnAndCrashChara();
}

void CBossStage3::StepShieldAndShoot()
{
	if (m_puppet[0].curX!=m_puppet[0].tarX||m_puppet[0].curY!=m_puppet[0].tarY
		||m_puppet[1].curX!=m_puppet[1].tarX||m_puppet[1].curY!=m_puppet[1].tarY)
		return;
	m_shieldFrame++;
	if (m_shieldFrame<16)
		return;
	if (m_shieldFrame==16)
	{
		m_shieldState=1;
		m_puppet[0].radius=64.0f*16.0f;
		m_puppet[1].radius=64.0f*16.0f;
		m_shieldImg=196;
		return;
	}
	if (m_shieldFrame<32)
		return;
	if (m_shieldFrame<64)
	{
		m_shieldState=2;
		m_puppet[0].radius=(64.0f-(float)m_shieldFrame)*2*16.0f;
		m_puppet[1].radius=(64.0f-(float)m_shieldFrame)*2*16.0f;
		return;
	}
	if (m_shieldFrame<80)
	{
		if (m_shieldFrame==64)
			m_pStage->m_bPlaySound[8]=true;
		m_shieldState=3;
		if (m_shieldFrame==71||m_shieldFrame==79)
			m_shieldImg+=2;
		return;
	}
	int dmg=m_pStage->m_pCharaBullet->GetDamage(m_puppet[0].curX+64.0f*16.0f,m_puppet[0].curY,
												56.0f*16.0f,4.0f*16.0f,false,true);
	if (dmg>0)
		m_pStage->m_bPlaySound[9]=true;
	if (m_pStage->m_pChara->TestCrash(m_puppet[0].curX+64.0f*16.0f,m_puppet[0].curY,64.0f*16.0f,4.0f*16.0f))
		m_pStage->m_pChara->m_bMiss=true;
	if (m_curPhaseFrame%16==0)
		if (m_curPhaseFrame%256<160+m_curPhase/3*32)
		{
			ENEMY_SHOOT_INFO shootInfo;
			shootInfo.shootOrigX=m_curX;
			shootInfo.shootOrigY=m_curY-8.0f*16.0f;			//not at m_curY!
			shootInfo.bulletBornType=18;
			shootInfo.bulletImg=44;
			shootInfo.shootType=EST07_MULTI_TO_CHARA;
			shootInfo.shootAngle=0;
			shootInfo.nMulti=8;
			shootInfo.deltaSpeed=6;
			shootInfo.bulletSpeed=32;
			m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
			m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
			m_pStage->m_bPlaySound[3]=true;
		}
}

void CBossStage3::Phase5_8_11()
{
	m_puppet[0].tarX=128.0f*16.0f;
	m_puppet[0].tarY=128.0f*16.0f;
	m_puppet[1].tarX=256.0f*16.0f;
	m_puppet[1].tarY=128.0f*16.0f;
	m_puppet[0].life=500;
	m_puppet[1].life=500;
	m_puppet[0].mode=-2;
	m_puppet[1].mode=-2;
	StepShieldAndShoot();
	GetDmgOnAndCrashChara();
	if (m_curPhaseFrame>=600)
	{
		AddDefeatScoreAndPopupNumbers(5);
		AddExplodeEffect(1,m_curX,m_curY);
		m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
		m_curPhase++;
		m_curPhaseFrame=0;
		m_shieldState=0;
		m_puppet[0].tarX=320.0f*16.0f;
		m_puppet[0].tarY=96.0f*16.0f;
		m_puppet[1].tarX=64.0f*16.0f;
		m_puppet[1].tarY=96.0f*16.0f;
		m_puppet[0].shootAge=0;
		m_puppet[1].shootAge=0;
		m_puppet[0].mode=1;
		m_puppet[1].mode=1;
	}
}

void CBossStage3::Phase13()
{
	m_puppet[0].tarX=64.0f*16.0f;
	m_puppet[0].tarY=128.0f*16.0f;
	m_puppet[1].tarX=320.0f*16.0f;
	m_puppet[1].tarY=128.0f*16.0f;
	m_puppet[0].life=500;
	m_puppet[1].life=500;
	m_puppet[0].mode=4;
	m_puppet[1].mode=4;
	if (MoveTo(192.0f*16.0f,64.0f*16.0f)!=0)
	{
		m_curPhaseFrame=0;
		m_curPhase++;
	}
	GetDmgOnAndCrashChara();
}

void CBossStage3::Phase14Shoot()
{
	ENEMY_SHOOT_INFO shootInfo;
	shootInfo.shootOrigX=m_curX;
	shootInfo.shootOrigY=m_curY-8.0f*16.0f;
	shootInfo.bulletBornType=16;
	shootInfo.shootType=EST02_NWAY;
	shootInfo.shootAngle=64;
	shootInfo.bulletImg=84;
	if (m_curPhaseFrame%32==0)
	{
		shootInfo.bulletSpeed=32;
		shootInfo.nWay=13;
		shootInfo.nWayUnitAngle=10;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
}

void CBossStage3::Phase14()
{
	m_puppet[0].tarX=64.0f*16.0f;
	m_puppet[0].tarY=128.0f*16.0f;
	m_puppet[1].tarX=320.0f*16.0f;
	m_puppet[1].tarY=128.0f*16.0f;
	m_puppet[0].life=500;
	m_puppet[1].life=500;
	m_puppet[0].mode=4;
	m_puppet[1].mode=4;
	Phase14Shoot();
	if (m_curPhaseFrame>=1000)
		m_bSuccessDefeat=false;
	if (GetDmgOnAndCrashChara()==0)
		return;
	else
		m_bSuccessDefeat=true;
	m_curPhaseFrame=0;
	m_curPhase=0xfd;
	m_puppet[0].flag=2;
	m_puppet[1].flag=2;
}

void CBossStage3::Step()
{
	m_curPhaseFrame++;
	m_bHitThisFrame=false;
	m_pStage->m_bHomingEnemyExist=true;
	m_pStage->m_homingEnemyX=m_curX;
	m_pStage->m_homingEnemyY=m_curY;
	StepExplodeEffect();

	switch(m_curPhase)
	{
	case 0:
		Phase0();
		break;
	case 1:
		Phase1();
		break;
	case 2:
		Phase2();
		break;
	case 3:
	case 6:
	case 9:
	case 12:
		Phase3_6_9_12();
		break;
	case 4:
	case 7:
	case 10:
		Phase4_7_10();
		break;
	case 5:
	case 8:
	case 11:
		Phase5_8_11();
		break;
	case 13:
		Phase13();
		break;
	case 14:
		Phase14();
		break;
	default:
		DeathSequence(10);
		break;
	}
	if (m_curPhase>=3&&m_curPhase<0xfe)
		StepPuppet();
	StepHPGauge(m_curHP,9600);
}

void CBossStage3::DrawPuppet()
{
	for (int i=0;i<2;i++)
		if (m_puppet[i].flag!=0)
		{
			if (m_puppet[i].curY<=-15.999f*16.0f)
				continue;
			float drawY=m_puppet[i].curY+(float)(abs(m_pStage->m_curFrame%32-16)/2*16);
			int img=m_puppet[i].image;
			if (m_puppet[i].flag==1)
				img+=m_pStage->m_curFrame%2;
			C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(img);
			if (m_puppet[i].bHitThisFrame)
				m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImg,m_puppet[i].curX,drawY);
			else
				m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_puppet[i].curX,drawY);
			if (m_shieldState==1)
			{
				if (m_pStage->m_curFrame%2!=0)
					m_pStage->m_pStageRes->DrawCircle(m_puppet[i].curX,m_puppet[i].curY,m_puppet[i].radius,1,1,1,false);
			}
			if (m_shieldState==2)
				m_pStage->m_pStageRes->DrawCircle(m_puppet[i].curX,m_puppet[i].curY,m_puppet[i].radius,1,1,1,true);
		}
}

void CBossStage3::DrawShield()
{
	if (m_shieldState==2)
	{
		float c[3]={m_pStage->m_pStageRes->m_palette[9*3]/255.0f,m_pStage->m_pStageRes->m_palette[9*3+1]/255.0f,
					m_pStage->m_pStageRes->m_palette[9*3+2]/255.0f};
		m_pStage->m_pStageRes->DrawLine(m_puppet[0].curX,m_puppet[0].curY,m_puppet[1].curX,m_puppet[1].curY,c[0],c[1],c[2],1);
	}
	if (m_shieldState==3)
	{
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_shieldImg);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_puppet[0].curX+32.0f*16.0f,m_puppet[0].curY+8.0f*16.0f);
		pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_shieldImg+1);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_puppet[0].curX+96.0f*16.0f,m_puppet[0].curY+8.0f*16.0f);
	}
}

void CBossStage3::DrawBoss()
{
	if (m_curPhase==0xfe)
	{
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
	else
	{
		int imgIdx;
		if (m_curImage==180)
			imgIdx=m_pStage->m_curFrame%16/4;
		else
			if (m_curImage==184)
				imgIdx=m_pStage->m_curFrame%2;
			else
				imgIdx=m_pStage->m_curFrame%8/4;
		imgIdx+=m_curImage;
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
		if (m_bHitThisFrame)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImg,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
		DrawPuppet();
		DrawShield();
	}
}

void CBossStage3::DrawBackground()
{
	if (m_curPhase==0||m_curPhase>=0xfe)	//as from code of PC98 ver, should be m_curPhase>=0xff
	{										//but from replay videos, it behaves as if m_curPhase>=0xfe, don't know why
		m_pStage->m_pBackground->Draw();
		return;
	}
	if (m_curPhase==1)
	{
		if (m_curPhaseFrame/4<8)
			m_pStage->m_pBackground->Draw();
		else
		{
			m_pStage->m_pStageRes->ClearPlayArea(0,0,0);
			C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
			pImage->Draw((float)32+m_pStage->m_playAreaUpperLeftX-32,(float)221+m_pStage->m_playAreaUpperLeftY-16);
		}
		m_pStage->m_pStageRes->DrawPlayAreaMask(m_pStage->m_pStageRes->m_pStageBB->GetMaskPtr(m_curPhaseFrame/4),1,1,1);
		return;
	}
	m_pStage->m_pStageRes->ClearPlayArea(0,0,0);
	C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
	pImage->Draw((float)32+m_pStage->m_playAreaUpperLeftX-32,(float)221+m_pStage->m_playAreaUpperLeftY-16);
}

void CBossStage3::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(9600);
}

}

























