#include ".\midbossstage1.h"
#include <stdlib.h>
#include "../Stage.h"

namespace th5w{

CMidBossStage1::CMidBossStage1(void)
{
}

CMidBossStage1::~CMidBossStage1(void)
{
}

void CMidBossStage1::Initialize()
{
	m_curX=192.0f*16.0f;
	m_curY=96.0f*16.0f;
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_curHP=1000;
	m_curImage=200;

	m_phase3CircularMoveAngle=(float)(rand()%256);	//PC98 does does not initialize this var, but it doens't matter much
}


void CMidBossStage1::Phase0()
{
	if (m_curPhaseFrame>=256)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curImage=204;
	}
}

void CMidBossStage1::Phase1_2()
{
	GetDamageAndPlaySoundWhenHit(12.0f*16.0f,12.0f*16.0f,10);
	if (m_curPhaseFrame>=64)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_phase3ShootAngle=224.0f;
	}
}

void CMidBossStage1::Phase3()
{
	Phase3Shoot();
	int dmg=GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,4);
	if (dmg!=0)
		m_bHitThisFrame=true;
	m_curHP-=dmg;
    float radius=(float)((1000-m_curHP)*1024/1000);
	m_curX=192.0f*16.0f+(float)cos(m_phase3CircularMoveAngle/256.0f*2*PI)*radius;
	m_curY=96.0f*16.0f+(float)sin(m_phase3CircularMoveAngle/256.0f*2*PI)*radius;
	m_phase3CircularMoveAngle-=2;
	if (m_curPhaseFrame>=1500||m_curHP<=0)
	{
		if (m_curHP<=0&&m_curPhaseFrame<1500)
		{
			m_pStage->m_pEnemyBullet->m_noEnemyBullet=true;
			AddDefeatScoreAndPopupNumbers(5);
			m_pStage->m_pItem->AddItem(m_curX,m_curY,ITEM_BIGPOWER,0,-48);
		}
		m_curPhase=0xfe;
		m_curImage=4;
		m_curPhaseFrame=0;
		m_pStage->m_pSparkEffect->AddEffect(m_curX,m_curY,128,48,true);
		m_pStage->m_bPlaySound[12]=true;
	}
}

void CMidBossStage1::Phase3Shoot()
{
	int shootInterval=16;
	if (m_curHP<=200)
		shootInterval=12;
	if (m_curPhaseFrame%shootInterval==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=0;
		shootInfo.bulletImg=0;
		shootInfo.bulletSpeed=32;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.bulletMoveType=EBMT10_STRAIGHT;
		shootInfo.nWay=4;
		shootInfo.nWayUnitAngle=2;
		shootInfo.shootAngle=m_phase3ShootAngle;
		m_phase3ShootAngle+=20;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true,true);

		shootInfo.shootType=EST00_SINGLE;
		shootInfo.bulletBornType=2;
		shootInfo.bulletSpeed=24;
		shootInfo.bulletImg=44;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		shootInfo.shootAngle=(float)(rand()%256);
		shootInfo.shootOrigX=m_curX+(float)cos(shootInfo.shootAngle/256.0f*2*PI)*32.0f*16.0f;
		shootInfo.shootOrigY=m_curY+(float)sin(shootInfo.shootAngle/256.0f*2*PI)*32.0f*16.0f;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
}

int CMidBossStage1::Step()
{
	m_curPhaseFrame++;
	m_bHitThisFrame=false;
	switch(m_curPhase)
	{
	case 0:
		Phase0();
		break;
	case 1:
	case 2:
		Phase1_2();
		break;
	case 3:
		Phase3();
	    break;
	default:				//0xfe or 0xff
		int ret=DeathSequence();
		StepHPGauge(m_curHP,1000);
		return ret;
	}
	StepHPGauge(m_curHP,1000);
	m_pStage->m_bHomingEnemyExist=true;
	m_pStage->m_homingEnemyX=m_curX;
	m_pStage->m_homingEnemyY=m_curY;

	return 0;
}

void CMidBossStage1::Draw()
{
	if (m_curPhase<0xfe)
	{
		C2DImage *pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage+m_pStage->m_curFrame%16/4);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImage,m_curX,m_curY);
		if (m_curPhase==2)
		{
			pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(208+m_pStage->m_curFrame%8/2);
			m_pStage->m_pStageRes->DrawStageObjectImage(pImage,m_curX,m_curY);
		}
		else
			if (m_curPhase==3)
			{
				pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(212+m_pStage->m_curFrame%8/2);
				if (m_bHitThisFrame)
					m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImage,m_curX,m_curY);
				else
					m_pStage->m_pStageRes->DrawStageObjectImage(pImage,m_curX,m_curY);
			}
	}
	else
		if (m_curPhase==0xfe)
			DrawExplode();
}

void CMidBossStage1::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(1000);
}

}






























