#include ".\midbossstage5.h"
#include "../Stage.h"
#include "../../game.h"

namespace th5w{

CMidBossStage5::CMidBossStage5(void)
{
}

CMidBossStage5::~CMidBossStage5(void)
{
}

void CMidBossStage5::Initialize()
{
	m_curX=192.0f*16.0f;
	m_curY=-32.0f*16.0f;
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_velX=0;
	m_velY=8.0f;
	m_curHP=1550;
	m_curImage=212;
}

void CMidBossStage5::Phase0()
{
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_curX+=m_velX;
	m_curY+=m_velY;
	GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,10);
	if (m_curPhaseFrame>=192)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_phase1CurMode=1;
		m_phase1NModeChange=0;
		m_phase1PositionIdx=0;
		m_velY=0;
	}
}

int CMidBossStage5::Phase1Move()
{
	if (m_curPhaseFrame==1)
	{
		if (m_phase1PositionIdx==0||m_phase1PositionIdx==3)
			m_velX=-2.0f*16.0f;
		else
			m_velX=2.0f*16.0f;
		m_phase1PositionIdx=(m_phase1PositionIdx+1)%4;
	}
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_curX+=m_velX;
	m_curY+=m_velY;
	if (m_curPhaseFrame>=32)
		return 1;
	return 0;
}

int CMidBossStage5::Phase1Mode1Shoot()
{
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.bulletImg=116;
		shootInfo.bulletSpeed=88;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.nWay=13;
		shootInfo.nWayUnitAngle=7;
		shootInfo.shootAngle=m_phase1ShootAngle;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame>=96)
		return 1;
	return 0;
}

int CMidBossStage5::Phase1Mode2Shoot()
{
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX+(float)(rand()%(64*16)-32*16);
		shootInfo.shootOrigY=m_curY+(float)(rand()%(64*16)-32*16);
		shootInfo.bulletBornType=16;
		shootInfo.bulletImg=68;
		shootInfo.bulletSpeed=32;
		shootInfo.shootType=EST04_NWAY_ALLAROUND;
		shootInfo.nWay=16;
		shootInfo.shootAngle=(float)(rand()%256);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame>=96)
		return 1;
	return 0;
}

int CMidBossStage5::Phase1Mode3Shoot()
{
	if (m_curPhaseFrame==32)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.bulletImg=0;
		shootInfo.shootType=EST09_NWAY_MULTI_TO_CHARA;
		shootInfo.shootAngle=0;
		shootInfo.nWay=5;
		shootInfo.nWayUnitAngle=10;
		shootInfo.nMulti=5;
		shootInfo.deltaSpeed=6;
		shootInfo.bulletSpeed=32;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);

		shootInfo.shootType=EST11_NWAY_ALLAROUND_MULTI_TO_CHARA;
		shootInfo.deltaSpeed=4;
		shootInfo.nWay=32;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);

		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame>=64)
		return 1;
	return 0;
}

void CMidBossStage5::Phase1Common()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,2,3,m_curPhaseFrame-16);
		if (m_curPhaseFrame==16)
			m_pStage->m_bPlaySound[8]=true;
		if (m_curPhaseFrame==20||m_curPhaseFrame==24||m_curPhaseFrame==28)
			m_curImage++;
		m_phase1ShootAngle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
		return;
	}
	if (m_curImage<219&&m_curPhaseFrame%4==0)
		m_curImage++;

	int shootRet;
	switch(m_phase1CurMode)
	{
	case 1:
		shootRet=Phase1Mode1Shoot();
		break;
	case 2:
		shootRet=Phase1Mode2Shoot();
	    break;
	case 3:
		shootRet=Phase1Mode3Shoot();
		break;
	}
	if (shootRet!=0)
	{
		m_curPhaseFrame=0;
		m_phase1CurMode=0;
		m_curImage=212;
	}
}

void CMidBossStage5::Phase1()
{
	bool bPhaseEnd=false;
	if (m_phase1CurMode==0)
	{
		if (Phase1Move()!=0)
		{
			m_curPhaseFrame=0;
			m_phase1NModeChange++;
			m_phase1CurMode=m_phase1NModeChange%3+1;
			if (m_phase1NModeChange>=16)
				bPhaseEnd=true;
		}
	}
	else
		Phase1Common();
	if (bPhaseEnd==false)
	{
		int dmg=GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,4);
		if (dmg!=0)
			m_bHitThisFrame=true;
		m_curHP-=dmg;
		if (m_curHP<=0)
		{
			m_pStage->m_pEnemyBullet->m_noEnemyBullet=true;
			AddDefeatScoreAndPopupNumbers(30);
			m_pStage->m_pItem->AddItem(m_curX,m_curY,ITEM_1UP,0,-48);
			bPhaseEnd=true;
		}
	}
	if (bPhaseEnd)
	{
		m_curPhase=0xfe;
		m_curImage=4;
		m_curPhaseFrame=0;
		m_pStage->m_pSparkEffect->AddEffect(m_curX,m_curY,128,48,true);
		m_pStage->m_bPlaySound[12]=true;
	}
}

int CMidBossStage5::Step()
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
	default:
		int ret=DeathSequence();
		StepHPGauge(m_curHP,1550);
		return ret;
	}
	StepHPGauge(m_curHP,1550);
	m_pStage->m_bHomingEnemyExist=true;
	m_pStage->m_homingEnemyX=m_curX;
	m_pStage->m_homingEnemyY=m_curY;

	return 0;
}

void CMidBossStage5::Draw()
{
	if (m_curPhase<0xfe)
	{
		C2DImage *pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
		if (m_bHitThisFrame)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImage,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImage,m_curX,m_curY);
	}
	else
		if (m_curPhase==0xfe)
			DrawExplode();
}

void CMidBossStage5::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(1550);
}

}





























