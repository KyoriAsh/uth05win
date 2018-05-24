#include ".\midbossstage4.h"
#include <stdlib.h>
#include "../Stage.h"
#include "../../game.h"

namespace th5w{

CMidBossStage4::CMidBossStage4(void)
{
}

CMidBossStage4::~CMidBossStage4(void)
{
}

void CMidBossStage4::Initialize()
{
	m_curX=192.0f*16.0f;
	m_curY=-32.0f*16.0f;
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_velX=0;
	m_velY=8.0f;
	m_curHP=1100;
	m_curImage=208;
	m_bPhase1CanBeHit=true;
}

void CMidBossStage4::Phase0()
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
		m_bPhase1CanBeHit=true;
		m_phase1CurMode=1;
		m_phase1NModeChange=0;		//in stage 3 mid boss, it is initialized to 1 not 0.
									//This is because in CMidBossStage4::Phase1(), it is first increased
									//and then %3 to get current mode. But in CMidBossStage3::Phase1(),
									//the execution order of these two operation is inversed.
									//Here we just keep it the same as PC98 ver.
	}
}

int CMidBossStage4::FadeOutAndIn()
{
	if (m_curPhaseFrame<16)
		return 0;

	int frameIdx=m_curPhaseFrame-16;
	if (frameIdx<32)
	{
		if (frameIdx==0)
		{
			m_bPhase1CanBeHit=false;
			m_pStage->m_bPlaySound[13]=true;
			m_phase1FadeCenterX=m_curX;
		}
		int offset=frameIdx;
		if (frameIdx>20)
			offset=(frameIdx-20)/2+20;
		offset*=32;
		if (m_pStage->m_curFrame%2==0)
			offset=-offset;
		m_curX=m_phase1FadeCenterX+(float)offset;
		return 0;
	}
	else
	{
		if (frameIdx==32)
		{
			m_phase1FadeCenterX=(float)(64+rand()%256)*16;
			m_curY=(float)(64+rand()%32)*16;
			return 0;
		}
		frameIdx=64-frameIdx;
		int offset=frameIdx;
		if (frameIdx>20)
			offset=(frameIdx-20)/2+20;
		offset*=32;
		if (m_pStage->m_curFrame%2==0)
			offset=-offset;
		m_curX=m_phase1FadeCenterX+(float)offset;
		if (frameIdx==0)
		{
			m_bPhase1CanBeHit=true;
			return 1;
		}
		return 0;
	}
	return 0;
}

void CMidBossStage4::Phase1Mode1()
{
	if (m_curPhaseFrame==16)
	{
		m_pStage->m_bPlaySound[8]=true;
		m_curImage=212;
	}

	ENEMY_SHOOT_INFO shootInfo;
	shootInfo.bulletBornType=2;
	shootInfo.shootType=EST13_RANDOM_BASE_SPEED_32;
	shootInfo.nWay=7;
	m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
	if (m_curPhaseFrame>=48&&m_curPhaseFrame<=80&&m_curPhaseFrame%8==0)
	{
		shootInfo.bulletSpeed=8;
		for (int i=0;i<4;i++)
		{
			shootInfo.bulletImg=i%2*116;
			shootInfo.shootOrigX=m_curX+(float)(rand()%(48*16)-24*16);
			shootInfo.shootOrigY=m_curY+(float)(rand()%(48*16)-24*16);
			m_pStage->m_pEnemyBullet->Shoot(&shootInfo,false,false);
			shootInfo.bulletSpeed+=16;
		}
		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame>=96)
	{
		m_curImage=208;
		m_curPhaseFrame=0;
		m_phase1CurMode=0;
	}
}

void CMidBossStage4::Phase1Mode2()
{
	if (m_curPhaseFrame==16)
	{
		m_pStage->m_bPlaySound[8]=true;
		m_curImage=212;
	}

	ENEMY_SHOOT_INFO shootInfo;
	shootInfo.shootOrigX=m_curX;
	shootInfo.shootOrigY=m_curY;
	shootInfo.shootType=EST04_NWAY_ALLAROUND;
	shootInfo.nWay=16;
	m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
	shootInfo.bulletImg=84;

	if (m_curPhaseFrame>=48&&m_curPhaseFrame<=80&&m_curPhaseFrame%4==0)
	{
		if (m_curPhaseFrame%8==0)
		{
			shootInfo.bulletSpeed=56;
			shootInfo.bulletBornType=2;
		}
		else
		{
			shootInfo.bulletSpeed=40;
			shootInfo.bulletBornType=0;
		}
		shootInfo.shootAngle=m_phase1Mode2Angle;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_phase1Mode2Angle+=4;
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame>=96)
	{
		m_curImage=208;
		m_curPhaseFrame=0;
		m_phase1CurMode=0;
	}
}

void CMidBossStage4::Phase1Mode3()
{
	if (m_curPhaseFrame==16)
	{
		m_pStage->m_bPlaySound[8]=true;
		m_curImage=212;
	}

	ENEMY_SHOOT_INFO shootInfo;
	shootInfo.shootOrigX=m_curX;
	shootInfo.shootOrigY=m_curY;
	shootInfo.bulletBornType=18;
	shootInfo.shootType=EST09_NWAY_MULTI_TO_CHARA;
	shootInfo.nWay=3;
	shootInfo.nWayUnitAngle=8;
	shootInfo.nMulti=4;
	shootInfo.deltaSpeed=4;
	shootInfo.bulletImg=44;
	m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);

	if (m_curPhaseFrame>=48&&m_curPhaseFrame<=80&&m_curPhaseFrame%4==0)
	{
		shootInfo.bulletSpeed=(float)16+(m_curPhaseFrame-48)/4*12;
		shootInfo.shootAngle=(float)32-(m_curPhaseFrame-48)/4*8;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame>=96)
	{
		m_curImage=208;
		m_curPhaseFrame=0;
		m_phase1CurMode=0;
	}
}


void CMidBossStage4::Phase1()
{
	m_lastX=m_curX;
	m_lastY=m_curY;
	bool bPhaseEnd=false;

	switch(m_phase1CurMode)
	{
	case 0:
		if (FadeOutAndIn()!=0)
		{
			m_curPhaseFrame=0;
			m_phase1NModeChange++;
			m_phase1CurMode=m_phase1NModeChange%3+1;
			if (m_phase1NModeChange>=16)
				bPhaseEnd=true;
			if (m_phase1CurMode==2)
				m_phase1Mode2Angle=(float)(rand()%256);
		}
		break;
	case 1:
		Phase1Mode1();
		break;
	case 2:
		Phase1Mode2();
	    break;
	case 3:
		Phase1Mode3();
	    break;
	default:
		return;
	}
	if (bPhaseEnd==false)
	{
		if (m_bPhase1CanBeHit)
		{
			int dmg=GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,4);
			if (dmg!=0)
				m_bHitThisFrame=true;
			m_curHP-=dmg;
		}
		if (m_curHP<=0)
		{
			m_pStage->m_pEnemyBullet->m_noEnemyBullet=true;
			AddDefeatScoreAndPopupNumbers(15);
			m_pStage->m_pItem->AddItem(m_curX,m_curY,ITEM_BOMB,0,-48);
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


int CMidBossStage4::Step()
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
		StepHPGauge(m_curHP,1100);
		return ret;
	}
	StepHPGauge(m_curHP,1100);
	m_pStage->m_bHomingEnemyExist=true;
	m_pStage->m_homingEnemyX=m_curX;
	m_pStage->m_homingEnemyY=m_curY;

	return 0;
}

void CMidBossStage4::Draw()
{
	if (m_curPhase<0xfe)
	{
		int imgIdx;
		imgIdx=m_curImage+m_pStage->m_curFrame%16/4;
		C2DImage *pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
		if (m_bHitThisFrame||m_bPhase1CanBeHit==false)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImage,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImage,m_curX,m_curY);
	}
	else
		if (m_curPhase==0xfe)
			DrawExplode();
}

void CMidBossStage4::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(1100);
}

}

