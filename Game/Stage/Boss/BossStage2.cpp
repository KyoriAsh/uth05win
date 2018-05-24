#include ".\bossstage2.h"
#include "../Stage.h"
#include "../../Game.h"
#include "../Stage2BG/Stage2BG.h"

namespace th5w{

CBossStage2::CBossStage2(void)
{
}

CBossStage2::~CBossStage2(void)
{
}

void CBossStage2::Initialize()
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
	m_imgGoLeft=180;
	m_imgGoRight=180;
}

void CBossStage2::Phase0()
{
	if (m_curPhaseFrame==1)
	{
		m_curHP=4400;
		m_curPhaseEndHP=3000;
	}
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=128)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_pStage->m_bPlaySound[13]=true;
	}
}

void CBossStage2::Phase1()
{
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=64)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=1;
		m_nModeChange=0;
	}
}

int CBossStage2::ShootAnimation(int endFrame)
{
	if (m_curPhaseFrame<16)
		return 0;
	if (m_curPhaseFrame<32)
	{
		m_curImage=184;
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,8,9,m_curPhaseFrame-16);
		if (m_curPhaseFrame==16)
			m_pStage->m_bPlaySound[8]=true;
		return 0;
	}
	if (m_curPhaseFrame==32)
	{
		m_curImage=188;
		return 1;
	}
	
	return m_curPhaseFrame<endFrame?2:3;
}

int CBossStage2::BounceWalk(int nFrame,float speed)
{
	if (m_curPhaseFrame==1)
	{
		float angle=(float)(rand()%256);
		m_velX=(float)cos(angle/256.0f*2*PI)*speed;
		m_velY=(float)sin(angle/256.0f*2*PI)*speed;
	}
	m_curX+=m_velX;
	m_curY+=m_velY;
	if (m_curX<=48.0f*16.0f)
		m_velX=fabs(m_velX);
	if (m_curX>=336.0f*16.0f)
		m_velX=-fabs(m_velX);
	if (m_curY<=48.0f*16.0f)
		m_velY=fabs(m_velY);
	if (m_curY>=96.0f*16.0f)
		m_velY=-fabs(m_velY);

	return m_curPhaseFrame>=nFrame?1:0;
}

void CBossStage2::Phase2ShootType1()
{
	int res=ShootAnimation(64);
	if (res==0)
		return;
	if (res==3)
	{
		m_curMode=0;
		m_curPhaseFrame=0;
		m_curImage=180;
		return;
	}
	if (res==1)
		m_pStage->m_bPlaySound[15]=true;
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=0;
		shootInfo.bulletImg=44;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.bulletSpeed=(float)(24+m_nModeChange/2*4);
		if (shootInfo.bulletSpeed>36)
			shootInfo.bulletSpeed=36;
		shootInfo.nWay=5;
		shootInfo.nWayUnitAngle=6;
		shootInfo.bulletMoveType=EBMT10_STRAIGHT;		//no need to set, just keep the same as PC98 ver
		shootInfo.shootAngle=(float)(32+(m_nModeChange/2)%2*64);
		shootInfo.bulletSpeed+=(m_curPhaseFrame-32)/2*4;
		shootInfo.shootAngle+=7*(m_curPhaseFrame-32)/2*(1-(m_nModeChange/2)%2*2);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		shootInfo.shootAngle+=128;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
}

void CBossStage2::Phase2ShootType2()
{
	int res=ShootAnimation(96);
	if (res==0)
		return;
	if (res==3)
	{
		m_curMode=0;
		m_curPhaseFrame=0;
		m_curImage=180;
		return;
	}
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.bulletImg=84;
		shootInfo.shootType=EST06_MULTI;
		shootInfo.bulletSpeed=32;
		const int nMulti[]={10,12,12,14};
		const float deltaSpeed[]={5,5,6,6};
		shootInfo.nMulti=nMulti[CGame::GVar().m_playDifficulty];
		shootInfo.deltaSpeed=deltaSpeed[CGame::GVar().m_playDifficulty];
		shootInfo.shootAngle=(float)(192+(m_curPhaseFrame-32)/2*8*((m_nModeChange/2)%2*2-1));
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[15]=true;
	}
}

void CBossStage2::Phase2()
{
	if (m_curMode==0)
	{
		if (BounceWalk(64,16)!=0)
		{
			m_curPhaseFrame=0;
			m_nModeChange++;
			m_curMode=m_nModeChange%2+1;
			if (m_nModeChange>=20)
			{
				EndPhase(1900,1);
				return;
			}
		}
	}
	else
	{
		if (m_nModeChange%2==0)
			Phase2ShootType1();
		else
			Phase2ShootType2();
	}
	if (GetDmgOnAndCrashChara()!=0)
	{
		AddDefeatScoreAndPopupNumbers(10);
		EndPhase(1900,1);
		return;
	}
}

void CBossStage2::Phase3()
{
	GetDmgOnAndCrashChara();
	if (m_curPhaseFrame>=64)
	{
		m_curPhaseFrame=0;
		m_curPhase++;
		m_curImage=188;
	}
}

void CBossStage2::Phase4Shoot()
{
	if (m_curPhaseFrame%32==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=3;
		shootInfo.shootType=EST00_SINGLE;
		shootInfo.bulletImg=0;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		shootInfo.bulletSpeed=48;
		float symAngle;			//angle of the symmetric axis
		if (m_curPhaseFrame%64==0)
		{
			shootInfo.shootAngle=36;
			symAngle=64;
		}
		else
		{
			shootInfo.shootAngle=100;
			symAngle=192;
		}
		for (int i=0;i<8;i++)
		{
			float oldAngle=shootInfo.shootAngle;
			m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
			shootInfo.shootAngle+=128;
			m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
			shootInfo.shootAngle=symAngle-shootInfo.shootAngle;
			m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
			shootInfo.shootAngle+=128;
			m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
			shootInfo.bulletSpeed-=5;
			shootInfo.shootAngle=oldAngle;
			shootInfo.shootAngle+=8;
		}
	}
}

void CBossStage2::Phase4()
{
	Phase4Shoot();
	if (BounceWalk(128,14)!=0)
	{
		m_curPhaseFrame=0;
		m_nModeChange++;
		if (m_nModeChange>=20)
		{
			EndPhase(500,2);
			return;
		}
	}
	if (GetDmgOnAndCrashChara()!=0)
	{
		AddDefeatScoreAndPopupNumbers(10);
		EndPhase(500,2);
		return;
	}
}

void CBossStage2::Phase5ShootType1()
{
	int res=ShootAnimation(128);
	if (res==0)
		return;
	if (res==3)
	{
		m_curMode=0;
		m_curPhaseFrame=0;
		m_curImage=180;
		return;
	}
	if (res==1)
		m_bPhase5LaserRotationClockwise=(rand()%2==0);
	if (m_curPhaseFrame%4==0)
	{
		float angle=(float)(240+(m_curPhaseFrame-32)/4*10);
		while(angle<0)
			angle+=256.0f;
		while(angle>=256.0f)
			angle-=256.0f;
		if (angle>128.0f&&angle<240.0f)
			return;
		float color[]={m_pStage->m_pStageRes->m_palette[8*3]/255.0f,m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f,
					   m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f};
		if (m_bPhase5LaserRotationClockwise==false)
			angle=128.0f-angle;
		m_pStage->m_pLaser->AddLaserShootOut(m_curX,m_curY,color,angle,30,104);
	}
}

void CBossStage2::Phase5ShootType2()
{
	int res=ShootAnimation(64);
	if (res==0)
		return;
	if (res==3)
	{
		m_curMode=0;
		m_curPhaseFrame=0;
		m_curImage=180;
		return;
	}
	if (res==1)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.bulletImg=84;
		shootInfo.shootType=EST11_NWAY_ALLAROUND_MULTI_TO_CHARA;
		shootInfo.bulletSpeed=(float)(20+m_nModeChange/2*4);
		if (shootInfo.bulletSpeed>32)
			shootInfo.bulletSpeed=32;
		shootInfo.nWay=24;
		shootInfo.nWayUnitAngle=8;
		shootInfo.nMulti=5;
		shootInfo.deltaSpeed=9;
		shootInfo.shootAngle=0;
		shootInfo.bulletMoveType=EBMT02_SLOW_DOWN_THEN_TURN;
		shootInfo.specialMoveVar=1;
		shootInfo.specialBulletDelta=(rand()%2==0?32.0f:-32.0f);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true);
		m_pStage->m_bPlaySound[15]=true;
	}
}

void CBossStage2::Phase5()
{
	if (m_curMode==0)
	{
		if (BounceWalk(48,48)!=0)
		{
			m_curPhaseFrame=0;
			m_nModeChange++;
			m_curMode=m_nModeChange%2+1;
			if (m_nModeChange>=20)
			{
				EndPhase(0,1);
				return;
			}
		}
	}
	else
	{
		if (m_nModeChange%2==0)
			Phase5ShootType1();
		else
			Phase5ShootType2();
	}
	if (GetDmgOnAndCrashChara()!=0)
	{
		AddDefeatScoreAndPopupNumbers(10);
		EndPhase(0,1);
		return;
	}
}

void CBossStage2::Phase6()
{
	Phase3();		//phase 6 is exactly the same as phase 3
}

void CBossStage2::Phase7Shoot()
{
	if (m_curPhaseFrame%32==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST03_NWAY_TO_CHARA;
		shootInfo.bulletImg=0;
		shootInfo.bulletMoveType=EBMT10_STRAIGHT;
		shootInfo.bulletSpeed=32;
		shootInfo.nWay=32;
		shootInfo.nWayUnitAngle=2;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		if (m_curPhaseFrame%64==0)
			shootInfo.shootAngle=31;
		else
			shootInfo.shootAngle=225;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true);
		m_pStage->m_bPlaySound[3]=true;
	}
}

void CBossStage2::Phase7()
{
	Phase7Shoot();
	if (BounceWalk(128,8)!=0)
	{
		m_curPhaseFrame=0;
		m_nModeChange++;
		if (m_nModeChange>=10)
		{
			m_bSuccessDefeat=false;
			m_curPhase=0xfd;
			m_curPhaseFrame=0;
			return;
		}
	}
	if (GetDmgOnAndCrashChara()!=0)
	{
		m_bSuccessDefeat=true;
		m_curPhase=0xfd;
		m_curPhaseFrame=0;
		return;
	}
}

void CBossStage2::Step()
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
		Phase3();
		break;
	case 4:
		Phase4();
		break;
	case 5:
		Phase5();
		break;
	case 6:
		Phase6();
		break;
	case 7:
		Phase7();
		break;
	default:
		DeathSequence(10);
		break;
	}
	StepHPGauge(m_curHP,4400);
}

void CBossStage2::DrawBoss()
{
	if (m_curPhase==0xfe)
	{
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
	else
	{
		int imgIdx;
		imgIdx=m_curImage+m_pStage->m_curFrame%16/4;
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
		if (m_bHitThisFrame)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImg,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
}

void CBossStage2::DrawBackground()
{
	if (m_curPhase==0||m_curPhase>=0xfe)	//as from code of PC98 ver, should be m_curPhase>=0xff
	{										//but from replay videos, it behaves as if m_curPhase>=0xfe, don't know why
		m_pStage->m_pBackground->Draw();
		m_pStage->m_pStage2BG->Draw();
		return;
	}
	if (m_curPhase==1)
	{
		if (m_curPhaseFrame/4<8)
		{
			m_pStage->m_pBackground->Draw();
			m_pStage->m_pStage2BG->Draw();
		}
		else
		{
			m_pStage->m_pStageRes->ClearPlayArea(0,0,0);
			C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
			pImage->Draw((float)32+m_pStage->m_playAreaUpperLeftX-32,(float)16+m_pStage->m_playAreaUpperLeftY-16);
		}
		m_pStage->m_pStageRes->DrawPlayAreaMask(m_pStage->m_pStageRes->m_pStageBB->GetMaskPtr(m_curPhaseFrame/4),
								   m_pStage->m_pStageRes->m_palette[0*3]/255.0f,
								   m_pStage->m_pStageRes->m_palette[0*3+1]/255.0f,
								   m_pStage->m_pStageRes->m_palette[0*3+2]/255.0f);
		return;
	}
	m_pStage->m_pStageRes->ClearPlayArea(0,0,0);
	C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
	pImage->Draw((float)32+m_pStage->m_playAreaUpperLeftX-32,(float)16+m_pStage->m_playAreaUpperLeftY-16);
}

void CBossStage2::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(4400);
}

}






















