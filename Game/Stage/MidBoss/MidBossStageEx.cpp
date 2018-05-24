#include ".\midbossstageex.h"
#include "../Stage.h"
#include "../../game.h"

namespace th5w{

CMidBossStageEx::CMidBossStageEx(void)
{
}

CMidBossStageEx::~CMidBossStageEx(void)
{
}

void CMidBossStageEx::Initialize()
{
	m_curX=192.0f*16.0f;
	m_curY=-16.0f*16.0f;
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_velX=0;
	m_velY=8.0f;
	m_curHP=3000;
	m_curImage=220;
	m_shootType=0;
}

int CMidBossStageEx::ShootType1()
{
	if (m_curPhaseFrame==94)
	{
		m_shootType1DeltaAngle=(float)(rand()%2*2-1);
		m_shootType1Angle=(float)(rand()%256);
	}
	if (m_curPhaseFrame<=114)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=16;
		shootInfo.specialMoveVar=2;
		shootInfo.bulletMoveType=EBMT03_SPEED_UP;
		shootInfo.nWay=18;
		shootInfo.nWayUnitAngle=16;				//no need to set here, kept just to follow PC98 ver
		shootInfo.shootType=EST04_NWAY_ALLAROUND;
		shootInfo.bulletSpeed=8;
		shootInfo.bulletImg=47;
		shootInfo.shootAngle=m_shootType1Angle;
		m_shootType1Angle+=m_shootType1DeltaAngle;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true);
	}
	if (m_curPhaseFrame>=128)
		return 1;
	return 0;
}

int CMidBossStageEx::ShootType2()
{
	if (m_pStage->m_curFrame%2!=0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST13_RANDOM_BASE_SPEED_32;
		shootInfo.bulletSpeed=16;
		shootInfo.nWay=12;
		shootInfo.nWayUnitAngle=10;				//no need to set here, kept just to follow PC98 ver
		shootInfo.bulletImg=0;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
	if (m_curPhaseFrame>=128)
		return 1;
	return 0;
}

int CMidBossStageEx::ShootType3_4()
{
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=19;
		shootInfo.specialMoveVar=1;
		shootInfo.bulletMoveType=EBMT02_SLOW_DOWN_THEN_TURN;
		shootInfo.shootType=EST08_NWAY_MULTI;
		shootInfo.nWay=5;
		shootInfo.nWayUnitAngle=8;
		shootInfo.nMulti=4;
		shootInfo.deltaSpeed=16;
		shootInfo.bulletSpeed=24;
		shootInfo.bulletImg=47;
		shootInfo.shootAngle=128;
		shootInfo.specialBulletDelta=-56;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true);
		shootInfo.shootAngle=0;
		shootInfo.specialBulletDelta=56;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame>=128)
		return 1;
	return 0;
}

int CMidBossStageEx::Shoot()
{
	switch(m_shootType)
	{
	case 0:
		return m_curPhaseFrame>=112?1:0;
	case 1:
		return ShootType1();
	case 2:
		return ShootType2();
	case 3:
	case 4:
		return ShootType3_4();
	}
	return 1;
}

int CMidBossStageEx::Move(int frameIdx)
{
	if (frameIdx<-8)
		return 0;
	if (frameIdx<0)
	{
		m_curImage=221;
		return 0;
	}
	if (frameIdx<30)
	{
		if (frameIdx==0)
			m_pStage->m_bPlaySound[8]=true;
		m_velX=(float)cos(m_movingAngle/256.0f*2*PI)*(64-frameIdx*2);
		m_velY=(float)sin(m_movingAngle/256.0f*2*PI)*(64-frameIdx*2);
		m_lastX=m_curX;
		m_lastY=m_curY;
		m_curX+=m_velX;
		m_curY+=m_velY;
		if (frameIdx*2<16||frameIdx*2>48)
			m_curImage=222;
		else
			m_curImage=223;
		return 0;
	}
	if (frameIdx<40)
	{
		if (frameIdx==30)
			m_pStage->m_bPlaySound[15]=1;
		m_lastX=m_curX;
		m_lastY=m_curY;
		m_curImage=221;
		Shoot();
		return 0;
	}
	m_curImage=220;

	return Shoot();
}

void CMidBossStageEx::Phase0()
{
	GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,10);
	m_movingAngle=64;
	if (Move(m_curPhaseFrame)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_movingAngle=0;
		m_phase1CurSubphase=0;
		m_phase1NModeChange=0;
		m_shootType=1;
	}
}

void CMidBossStageEx::Phase1()
{
	const float movingAngle[]={16,112,128,128,144,240,0,0};
	if (Move(m_curPhaseFrame-64)!=0)
	{
		if (m_phase1CurSubphase==0)
			if (m_curHP<1000)
			{
				AddDefeatScoreAndPopupNumbers(10);
				m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
				m_pStage->m_bPlaySound[15]=true;
				m_phase1CurSubphase++;
			}
		m_curPhaseFrame=0;
		m_movingAngle=movingAngle[m_phase1NModeChange%8];
		m_phase1NModeChange++;
		m_shootType=m_phase1CurSubphase*2+m_phase1NModeChange%2+1;
	}

	int dmg=GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,4);
	if (dmg!=0)
		m_bHitThisFrame=true;
	m_curHP-=dmg;

	bool bPhaseEnd=false;
	if (m_phase1NModeChange>=20)
		bPhaseEnd=true;
	if (!bPhaseEnd)
	{
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

int CMidBossStageEx::Step()
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
		StepHPGauge(m_curHP,3000);
		return ret;
	}
	StepHPGauge(m_curHP,3000);
	m_pStage->m_bHomingEnemyExist=true;
	m_pStage->m_homingEnemyX=m_curX;
	m_pStage->m_homingEnemyY=m_curY;

	return 0;
}

void CMidBossStageEx::Draw()
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

void CMidBossStageEx::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(3000);
}

}





























