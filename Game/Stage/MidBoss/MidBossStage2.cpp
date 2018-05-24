#include ".\midbossstage2.h"
#include <stdlib.h>
#include "../Stage.h"

namespace th5w{

CMidBossStage2::CMidBossStage2(void)
{
}

CMidBossStage2::~CMidBossStage2(void)
{
}

void CMidBossStage2::Initialize()
{
	m_curX=192.0f*16.0f;
	m_curY=-32.0f*16.0f;
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_velX=0;
	m_velY=8.0f;
	m_curHP=1400;
	m_curImage=202;
}

void CMidBossStage2::Phase0()
{
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_curX+=m_velX;
	m_curY+=m_velY;
	GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,10);
	if (m_curPhaseFrame>=256)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_phase1CtrX=192.0f*16.0f;
		m_phase1CtrY=m_curY;
		m_phase1VelX=32.0f;
		m_phase1Radius=0;
		m_phase1Angle=0;
		m_phase1DeltaRadius=1.0f*16.0f;
	}
}

void CMidBossStage2::Phase1()
{
	Phase1Shoot();
	Phase1Move();
	int dmg=GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,4);
	if (dmg!=0)
		m_bHitThisFrame=true;
	m_curHP-=dmg;
	if (m_curPhaseFrame>=1000||m_curHP<=400)
	{
		if (m_curHP<=400&&m_curPhaseFrame<1000)
		{
			AddDefeatScoreAndPopupNumbers(5);
			m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
		}
		m_curPhase++;
		m_curImage=206;
		m_curPhaseFrame=0;
		m_pStage->m_pSparkEffect->AddEffect(m_curX,m_curY,64,32,true);
		m_pStage->m_bPlaySound[15]=true;
		//PC98 ver tries to calculate velocity to move boss from current position to (192*64,64*64) in next phase
		//with speed 1, but due to the fact that it used integer velX and velY, they are set as 0.
		//(cos(any_angle0)*1<=1, sin(any_angle)<=1)
		//hence, actually it stays.
		//here, we ignore all the calculation and just let it stay.
		//(guess that setting speed to 1 was a miss, 16 was desired)
	}
}

void CMidBossStage2::Phase1Shoot()
{
	if (m_pStage->m_curFrame%16==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=16;
		shootInfo.bulletImg=48;
		shootInfo.shootType=EST06_MULTI;
		shootInfo.nMulti=3;
		shootInfo.deltaSpeed=8;
		shootInfo.bulletSpeed=24;
		shootInfo.shootAngle=(float)(rand()%256);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);

		shootInfo.bulletBornType=18;
		shootInfo.bulletImg=0;
		shootInfo.shootType=EST13_RANDOM_BASE_SPEED_32;
		shootInfo.nWay=3;
		if (m_curHP<=600)
			shootInfo.nWay=6;
		else
			if (m_curHP<=800)
				shootInfo.nWay=4;
		shootInfo.bulletSpeed=24;
		shootInfo.shootAngle=(float)(rand()%256);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
}

void CMidBossStage2::Phase1Move()
{
	m_curX=(float)cos(m_phase1Angle/256.0f*2*PI)*m_phase1Radius+m_phase1CtrX;
	m_curY=(float)sin(m_phase1Angle/256.0f*2*PI)*m_phase1Radius+m_phase1CtrY;
	m_phase1CtrX+=m_phase1VelX;
	if (m_phase1CtrX<=128.0f*16.0f)
		m_phase1VelX+=2;
	else
		if (m_phase1CtrX>=256.0f*16.0f)
			m_phase1VelX-=2;
	m_phase1Radius+=m_phase1DeltaRadius;
	if (m_phase1Radius>65.0f*16.0f)
		m_phase1DeltaRadius=-1.0f*16.0f;
	else
		if (m_phase1Radius<=1.0f*16.0f)
			m_phase1DeltaRadius=1.0f*16.0f;
	m_phase1Angle-=2;
}

void CMidBossStage2::Phase2()
{
	Phase2Shoot();
	int dmg=GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,4);
	if (dmg!=0)
		m_bHitThisFrame=true;
	m_curHP-=dmg;
	if (m_curPhaseFrame>=800||m_curHP<=0)
	{
		if (m_curHP<=0&&m_curPhaseFrame<800)
		{
			m_pStage->m_pEnemyBullet->m_noEnemyBullet=true;
			AddDefeatScoreAndPopupNumbers(15);
			m_pStage->m_pItem->AddItem(m_curX,m_curY,ITEM_BOMB,0,-48);
		}
		m_curPhase=0xfe;
		m_curImage=4;
		m_curPhaseFrame=0;
		m_pStage->m_pSparkEffect->AddEffect(m_curX,m_curY,128,48,true);
		m_pStage->m_bPlaySound[12]=true;
	}
}

void CMidBossStage2::Phase2Shoot()
{
	if (m_pStage->m_curFrame%4==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=16;
		shootInfo.bulletImg=44;
		shootInfo.shootType=EST06_MULTI;
		shootInfo.nMulti=3;
		shootInfo.deltaSpeed=15;
		shootInfo.bulletSpeed=24;
		shootInfo.shootAngle=(float)(rand()%256);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
}

int CMidBossStage2::Step()
{
	m_curPhaseFrame++;
	m_bHitThisFrame=false;

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
	default:
		int ret=DeathSequence();
		StepHPGauge(m_curHP,1400);
		return ret;
	}
	StepHPGauge(m_curHP,1400);
	m_pStage->m_bHomingEnemyExist=true;
	m_pStage->m_homingEnemyX=m_curX;
	m_pStage->m_homingEnemyY=m_curY;

	return 0;
}

void CMidBossStage2::Draw()
{
	if (m_curPhase<0xfe)
	{
		int imgIdx;
		if (m_curImage==202)
			imgIdx=202+m_pStage->m_curFrame%16/4;
		else
			imgIdx=206+m_pStage->m_curFrame%8/2;
		C2DImage *pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
		if (m_bHitThisFrame)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImage,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImage,m_curX,m_curY);
	}
	else
		if (m_curPhase==0xfe)
			DrawExplode();
}

void CMidBossStage2::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(1400);
}

}
