#include ".\bossstage4yuki.h"
#include "../Stage.h"
#include "../../Game.h"
#include <math.h>

namespace th5w{

CBossStage4Yuki::CBossStage4Yuki(void)
{
	m_bDrawLargeBullet=false;
}

CBossStage4Yuki::~CBossStage4Yuki(void)
{
}

void CBossStage4Yuki::Initialize()
{
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_velX=0;
	m_velY=0;
	m_bossDmgAreaHalfWidth=24.0f*16.0f;
	m_bossDmgAreaHalfHeight=24.0f*16.0f;

	m_largeBullet.m_pStage=m_pStage;
	m_largeBullet.Initialize();
}

void CBossStage4Yuki::Phase0()
{
	if (m_curPhaseFrame==1)
	{
		m_curHP=7900;
		m_curPhaseEndHP=4600;
		m_curImage=196;
		m_imgGoLeft=198;
		m_imgGoRight=197;
		m_imgStay=196;
	}
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame==64)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_pStage->m_bPlaySound[13]=true;
	}
}

void CBossStage4Yuki::Phase1()
{
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame==64)
	{
		m_curImage=196;			//PC98 ver sets to 204 here, which is not correct.
		m_curPhase++;
		m_curPhaseFrame=0;
		m_bDrawLargeBullet=true;
	}
}

void CBossStage4Yuki::Phase2()
{
	GetDmgOffAndCrashChara();
	if (MoveTo(192.0f*16.0f,96.0f*16.0f)!=0)
	{
		m_curImage=204;
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=1;
		m_nModeChange=0;
		m_moveTarX=m_curX;
		m_moveTarY=m_curY+16.0f*16.0f;
		m_pStage->m_pCircle->AddCircle(m_curX,m_curY,1,14.0f/15.0f,12.0f/15.0f,true);
		AddExplodeEffect(4,m_curX,m_curY);
		m_imgGoLeft=206;
		m_imgGoRight=205;
		m_imgStay=204;
	}
}

void CBossStage4Yuki::Phase3ShootType1()
{
	if (m_curPhaseFrame<48)
		return;
	if (m_curPhaseFrame==48)
	{
		m_curImage=208;
		m_phase3ShootType1Angle=(float)(rand()%256);
		m_phase3ShootType1DeltaAngle=1;
		return;
	}
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.bulletMoveType=EBMT10_STRAIGHT;
		si.bulletImg=50;
		si.nWay=5+CGame::GVar().m_playDifficulty;
		si.bulletSpeed=54;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		si.shootAngle=m_phase3ShootType1Angle;
		m_phase3ShootType1Angle+=m_phase3ShootType1DeltaAngle;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);
		m_pStage->m_bPlaySound[3]=true;
		if (m_curPhaseFrame%16==0)
			m_phase3ShootType1Angle+=m_phase3ShootType1DeltaAngle*16;
	}
	if (m_curPhaseFrame==160)
		m_phase3ShootType1DeltaAngle=-m_phase3ShootType1DeltaAngle;
	if (m_curPhaseFrame==256)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
	if (m_nModeChange>=2&&m_curPhaseFrame>=64)
		RandomWalk(m_curPhaseFrame%64);
}

void CBossStage4Yuki::Phase3ShootType2()
{
	if (m_curPhaseFrame<8)
		return;
	if (m_curPhaseFrame==8)
	{
		m_curImage=208;
		return;
	}
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.shootType=EST02_NWAY;
		si.bulletImg=48;
		si.nWay=rand()%8;
		si.nWayUnitAngle=6;
		si.bulletSpeed=(float)(8+rand()%32);
		si.shootAngle=(float)(128+(m_curPhaseFrame-10)/2*6);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		m_pStage->m_pEnemyBullet->Shoot(&si,true);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame==256)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
	if (m_nModeChange>=2&&m_curPhaseFrame>=64)
		RandomWalk(m_curPhaseFrame%64);
}

void CBossStage4Yuki::Phase3()
{
	if (m_curMode==0)
	{
		if (MoveTo(m_moveTarX,m_moveTarY)!=0)
		{
			m_moveTarX=m_curX;
			m_moveTarY=96.0f*16.0f;
			m_curPhaseFrame=0;
			m_curMode++;
			m_nModeChange++;
			if (m_nModeChange>=12)
			{
				EndPhase(3200,1);
				return;
			}
		}
	}
	else
	{
		if (m_nModeChange%2==0)
			Phase3ShootType1();
		else
			Phase3ShootType2();
	}
	if (GetDmgOnAndCrashChara()!=0)
	{
		AddDefeatScoreAndPopupNumbers(10);
		EndPhase(3200,1);
	}
}

void CBossStage4Yuki::Phase4()
{
	GetDmgOnAndCrashChara();
	if (MoveTo(192.0f*16.0f,96.0f*16.0f)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=1;
	}
}

void CBossStage4Yuki::Phase5ShootType1()
{
	if (m_curPhaseFrame<8)
		return;
	if (m_curPhaseFrame==8)
	{
		m_curImage=208;
		return;
	}
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.bulletImg=0;
		si.nWay=24;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		si.shootAngle=(float)(rand()%256);
		si.bulletSpeed=(float)(32+rand()%32);
		m_pStage->m_pEnemyBullet->Shoot(&si);
		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame==160)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
	if (m_nModeChange>=2&&m_curPhaseFrame>=64)
		RandomWalk(m_curPhaseFrame%64-32);
}

void CBossStage4Yuki::Phase5ShootType2()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,8,9,m_curPhaseFrame-16);
		m_curImage=208;
		return;
	}
	if (m_curPhaseFrame%32==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.shootType=EST03_NWAY_TO_CHARA;
		si.bulletImg=48;
		si.nWay=5;
		si.nWayUnitAngle=12;
		si.shootAngle=0;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		si.bulletSpeed=(float)(16+rand()%32);
		m_pStage->m_pEnemyBullet->Shoot(&si);
		m_pStage->m_bPlaySound[15]=true;

		float angle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
		m_largeBullet.AddBullet(m_curX,m_curY,216,angle,64,0,false,NULL);
		m_largeBullet.AddBullet(m_curX,m_curY,216,angle-12.0f,64,0,false,NULL);
		m_largeBullet.AddBullet(m_curX,m_curY,216,angle-24.0f,64,0,false,NULL);
		m_largeBullet.AddBullet(m_curX,m_curY,216,angle+12.0f,64,0,false,NULL);
		m_largeBullet.AddBullet(m_curX,m_curY,216,angle+24.0f,64,0,false,NULL);
	}
	if (m_curPhaseFrame==160)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
	if (m_nModeChange>=2&&m_curPhaseFrame>=64)
		RandomWalk(m_curPhaseFrame%64-32);
}

void CBossStage4Yuki::Phase5()
{
	if (m_curMode==0)
	{
		if (MoveTo(m_curX,96.0f*16.0f)!=0)
		{
			m_curPhaseFrame=0;
			m_curMode++;
			m_nModeChange++;
			if (m_nModeChange>=24)
			{
				EndPhase(1200,2);
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
		EndPhase(1200,2);
	}
}

void CBossStage4Yuki::Phase6()
{
	GetDmgOnAndCrashChara();
	if (MoveTo(192.0f*16.0f,96.0f*16.0f)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
	}
}

void CBossStage4Yuki::Phase7Shoot()
{
	if (m_curPhaseFrame<=8)
		return;
	if (m_curPhaseFrame==8)
	{
		m_curImage=208;
		return;
	}
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.shootType=EST12_RANDOM;
		si.bulletMoveType=EBMT03_SPEED_UP;
		si.bulletImg=84;
		si.nWay=4+CGame::GVar().m_playDifficulty*2;
		si.bulletSpeed=32;
		si.specialMoveVar=1;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		m_pStage->m_pEnemyBullet->Shoot(&si,true);
	}
	if (m_curPhaseFrame>=128)
		if (m_curPhaseFrame>=320)
			RandomWalk(m_curPhaseFrame%64);
		else
			RandomWalk(m_curPhaseFrame%64-32);
}

void CBossStage4Yuki::Phase7()
{
	bool bEndPhase=false;
	Phase7Shoot();
	if (m_curPhaseFrame>=2000)
		bEndPhase=true;
	if (!bEndPhase)
	{
		if (GetDmgOnAndCrashChara()!=0)
		{
			bEndPhase=true;
			AddDefeatScoreAndPopupNumbers(10);
		}
	}
	if (bEndPhase)
		EndPhase(0,3);
}

void CBossStage4Yuki::Phase8()
{
	GetDmgOnAndCrashChara();
	if (MoveTo(192.0f*16.0f,96.0f*16.0f)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=1;
	}
}

void CBossStage4Yuki::Phase9ShootType1()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,8,9,m_curPhaseFrame-16);
		m_curImage=208;
		return;
	}
	if (m_curPhaseFrame==32)
	{
		m_pStage->m_bPlaySound[15]=true;
		m_phase9ShootType1Angle=(float)(rand()%256);
	}
	if (m_curPhaseFrame%4==0)
	{
		m_largeBullet.AddBullet(m_curX,m_curY,216,m_phase9ShootType1Angle,32,0,false,NULL);
		m_largeBullet.AddBullet(m_curX,m_curY,216,m_phase9ShootType1Angle+64.0f,32,0,false,NULL);
		m_largeBullet.AddBullet(m_curX,m_curY,216,m_phase9ShootType1Angle+128.0f,32,0,false,NULL);
		m_largeBullet.AddBullet(m_curX,m_curY,216,m_phase9ShootType1Angle+192.0f,32,0,false,NULL);
		if (m_nModeChange/5%2==0)
			m_phase9ShootType1Angle-=8;
		else
			m_phase9ShootType1Angle+=8;
	}
	if (m_curPhaseFrame==64)
	{
		m_curMode=0;
		m_curPhaseFrame=0;
	}
}

void CBossStage4Yuki::Phase9ShootType2()
{
	if (m_curPhaseFrame<24)
		return;
	if (m_curPhaseFrame==24)
	{
		m_curImage=208;
		return;
	}
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.shootType=EST06_MULTI;
		si.bulletImg=84;
		si.nMulti=8;
		si.deltaSpeed=6;
		si.bulletSpeed=32;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		if ((m_nModeChange%5-1)%2==0)
			si.shootAngle=(float)(16+(m_curPhaseFrame-26)/2*8);
		else
			si.shootAngle=(float)(112-(m_curPhaseFrame-26)/2*8);
		m_pStage->m_pEnemyBullet->Shoot(&si);
		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame==64)
	{
		m_curMode=0;
		m_curPhaseFrame=0;
		return;
	}
	if (m_nModeChange>2)
		RandomWalk(m_curPhaseFrame%64-25);
}

void CBossStage4Yuki::Phase9()
{
	bool bPhaseEnd=false;
	if (m_curMode==0)
	{
		if (m_curPhaseFrame>=8)
		{
			m_curPhaseFrame=0;
			m_curMode++;
			m_nModeChange++;
			if (m_nModeChange>=36)
			{
				bPhaseEnd=true;
				m_bSuccessDefeat=false;
			}
		}
	}
	else
	{
		switch(m_nModeChange%5)
		{
		case 0:
			Phase9ShootType1();
		    break;
		case 1:
		case 2:
		case 3:
		case 4:
			Phase9ShootType2();
		    break;
		}
	}
	if (bPhaseEnd==false)
		if (GetDmgOnAndCrashChara()!=0)
		{
			bPhaseEnd=true;
			m_bSuccessDefeat=true;
		}
	if (bPhaseEnd)
	{
		AddExplodeEffect(4,m_curX,m_curY);
		m_curPhaseFrame=0;
		m_curPhase=0xfd;
		m_bDrawLargeBullet=false;
	}
}

void CBossStage4Yuki::Step()
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
	case 8:
		Phase8();
		break;
	case 9:
		Phase9();
		break;
	default:
		DeathSequence(50);
		break;
	}
	m_largeBullet.Step();
	StepHPGauge(m_curHP,7900);
}

void CBossStage4Yuki::DrawBoss()
{
	//exactly the same CBossStage4Mai::DrawBoss()
	if (m_curPhase==0xfe)
	{
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
	else
	{
		int imgIdx;
		if (m_curImage==192||m_curImage==208)
			imgIdx=m_curImage+m_pStage->m_curFrame%8/2;
		else
			imgIdx=m_curImage;
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
		if (m_bHitThisFrame)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImg,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
}

void CBossStage4Yuki::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(7900);
}

void CBossStage4Yuki::DrawBackground()
{
	//exactly the same as CBossStage4::DrawBackground()
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
			pImage->Draw((float)96+m_pStage->m_playAreaUpperLeftX-32,(float)72+m_pStage->m_playAreaUpperLeftY-16);
		}
		m_pStage->m_pStageRes->DrawPlayAreaMask(m_pStage->m_pStageRes->m_pStageBB->GetMaskPtr(m_curPhaseFrame/4),0.6f,0.6f,1);
		return;
	}
	m_pStage->m_pStageRes->ClearPlayArea(0,0,0);
	C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
	pImage->Draw((float)96+m_pStage->m_playAreaUpperLeftX-32,(float)72+m_pStage->m_playAreaUpperLeftY-16);
}

void CBossStage4Yuki::DrawSpecialBullet()
{
	if (m_bDrawLargeBullet)
		m_largeBullet.Draw();
}

}
























