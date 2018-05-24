#include ".\bossstage4mai.h"
#include "../Stage.h"
#include "../../Game.h"
#include <math.h>

namespace th5w{

CBossStage4Mai::CBossStage4Mai(void)
{
	m_bDrawLargeBullet=false;
}

CBossStage4Mai::~CBossStage4Mai(void)
{
}

void CBossStage4Mai::Initialize()
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

void CBossStage4Mai::Phase0()
{
	if (m_curPhaseFrame==1)
	{
		m_curHP=7800;
		m_curPhaseEndHP=5800;
		m_curImage=180;
		m_imgGoLeft=182;
		m_imgGoRight=181;
		m_imgStay=180;
	}
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame==64)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_pStage->m_bPlaySound[13]=true;
	}
}

void CBossStage4Mai::Phase1()
{
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame==64)
	{
		m_curImage=180;			//PC98 ver sets to 204 here, which is obviously a bug. Actually, setting it to 188
								//is also not correct. it is best to use the stay image here.
		m_curPhase++;
		m_curPhaseFrame=0;
		m_bDrawLargeBullet=true;
	}
}

void CBossStage4Mai::Phase2()
{
	GetDmgOffAndCrashChara();
	if (MoveTo(192.0f*16.0f,96.0f*16.0f)!=0)
	{
		m_curImage=188;
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=1;
		m_nModeChange=0;
		AddExplodeEffect(4,m_curX,m_curY);
		m_pStage->m_pCircle->AddCircle(m_curX,m_curY,1,14.0f/15.0f,12.0f/15.0f,true);
		m_imgGoLeft=190;
		m_imgGoRight=189;
		m_imgStay=188;
	}
}

void CBossStage4Mai::Phase3ShootType1()
{
	if (m_curPhaseFrame<32)
		return;
	if (m_curPhaseFrame==32)
	{
		m_curImage=192;
		return;
	}
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.shootType=EST02_NWAY;
		si.bulletImg=46;
		si.nWay=2;
		si.shootAngle=64;
		si.nWayUnitAngle=12;
		si.bulletSpeed=48;
		for (int i=0;i<8;i++)
		{
			m_pStage->m_pEnemyBullet->Shoot(&si);
			si.nWayUnitAngle+=8;
			si.bulletSpeed+=6;
		}
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame==160)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
	if (m_nModeChange>=2&&m_curPhaseFrame>=64)
		RandomWalk(m_curPhaseFrame%64);
}

void CBossStage4Mai::Phase3ShootType2()
{
	if (m_curPhaseFrame<32)
		return;
	if (m_curPhaseFrame==32)
	{
		m_curImage=192;
		return;
	}
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.shootType=EST06_MULTI;
		si.bulletMoveType=EBMT02_SLOW_DOWN_THEN_TURN;
		si.specialMoveVar=1;
		si.bulletImg=116;
		si.nMulti=CGame::GVar().m_playDifficulty+4;
		si.deltaSpeed=16;
		si.bulletSpeed=32;
		si.shootAngle=(float)(96+(m_curPhaseFrame-48)/16*9);
		si.specialBulletDelta=-64;
		if (m_curPhaseFrame%32!=0)
		{
			si.shootAngle=128.0f-si.shootAngle;
			si.specialBulletDelta=64;
		}
		m_pStage->m_pEnemyBullet->Shoot(&si,true,false);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame==192)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
	if (m_nModeChange>=2&&m_curPhaseFrame>=64)
		RandomWalk(m_curPhaseFrame%64);
}

void CBossStage4Mai::Phase3()
{
	if (m_curMode==0)
	{
		if (RandomWalk(m_curPhaseFrame-32)!=0)
		{
			m_curPhaseFrame=0;
			m_curMode++;
			m_nModeChange++;
			if (m_nModeChange>=36)
			{
				EndPhase(2800,1);
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
		EndPhase(2800,1);
	}
}

void CBossStage4Mai::Phase4()
{
	GetDmgOnAndCrashChara();
	if (MoveTo(192.0f*16.0f,128.0f*16.0f)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;					//does not set m_curMode to 1 here, same as PC98 ver
	}
}

void CBossStage4Mai::Phase5ShootType1()
{
	int cycleFrame=(m_curPhaseFrame-32)%256;
	if (cycleFrame<=128&&cycleFrame%4==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.shootType=EST02_NWAY;
		si.bulletImg=116;
		si.nWay=3;
		si.nWayUnitAngle=12;
		si.bulletSpeed=48;
		si.shootAngle=(float)(cycleFrame/4*6);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		m_pStage->m_pEnemyBullet->Shoot(&si);
	}
}

void CBossStage4Mai::Phase5ShootType2()
{
	int cycleFrame=(m_curPhaseFrame-32)%256;
	if (cycleFrame<=128&&cycleFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.bulletImg=116;
		si.nWay=24;
		si.bulletSpeed=48;
		si.shootAngle=(float)(cycleFrame/16);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		m_pStage->m_pEnemyBullet->Shoot(&si);
	}
}

void CBossStage4Mai::Phase5ShootType3()
{
	int cycleFrame=(m_curPhaseFrame-32)%256;
	if (cycleFrame<=128&&cycleFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.bulletBornType=18;
		si.shootType=EST12_RANDOM;
		si.bulletImg=0;
		si.nWay=4;
		si.bulletSpeed=32;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		float angle=(float)(128-cycleFrame/16*12);
		m_largeBullet.AddBullet(m_curX,m_curY,212,angle,48,24,true,&si);
		m_largeBullet.AddBullet(m_curX,m_curY,212,angle-16,48,24,true,&si);
		m_largeBullet.AddBullet(m_curX,m_curY,212,angle+16,48,24,true,&si);
	}
}

void CBossStage4Mai::Phase5Shoot()
{
	if (m_curPhaseFrame<32)
		return;

	int nLaser=5+CGame::GVar().m_playDifficulty;
	if (m_curPhaseFrame==32)
	{
		float c[]={m_pStage->m_pStageRes->m_palette[8*3]/255.0f,m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f,
				   m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f};
		float angle=(float)(rand()%256);
		for (int i=0;i<nLaser;i++)
			m_pStage->m_pLaser->AddLaserFixed(i,m_curX,m_curY,c,8,angle+i*256.0f/nLaser);
		m_phase5LaserAngleSpeed=1;
		m_phase5LaserRotationDir=1;
	}
	int cycleFrame=(m_curPhaseFrame-32)%256;
	for (int i=0;i<nLaser;i++)
		m_pStage->m_pLaser->SetLaserSrc(i,m_curX,m_curY);

	if (cycleFrame==160)
		for (int i=0;i<nLaser;i++)
			m_pStage->m_pLaser->StartGrow(i);
	else
		if (cycleFrame==212)
			for (int i=0;i<nLaser;i++)
				m_pStage->m_pLaser->SetMode(i,LASER_MODE_SHRINK_AND_WAIT_TO_GROW);
		else
			if (cycleFrame==180)
				m_phase5LaserRotationDir=-m_phase5LaserRotationDir;

	if (cycleFrame<=112)
	{
		m_phase5LaserAngleSpeed+=(float)(m_pStage->m_curFrame%2);
		if (m_phase5LaserAngleSpeed>128)
			m_phase5LaserAngleSpeed=128;
	}
	else
		if (cycleFrame<=192)
		{
			m_phase5LaserAngleSpeed-=2;
			if (m_phase5LaserAngleSpeed<0)
				m_phase5LaserAngleSpeed=0;
		}
		else
		{
			m_phase5LaserAngleSpeed+=(float)(m_pStage->m_curFrame%2);
			if (cycleFrame>212&&rand()%32==0)
				m_curPhaseFrame+=255-cycleFrame;
		}

	if (m_curPhaseFrame>=288)
	{
		switch((m_curPhaseFrame/256+2)%3)
		{
		case 0:
			Phase5ShootType1();
			break;
		case 1:
			Phase5ShootType2();
			break;
		case 2:
			Phase5ShootType3();
		    break;
		}
		if (m_curPhaseFrame>=544&&cycleFrame<=96)
			RandomWalk(cycleFrame-32);
	}
	for (int i=0;i<nLaser;i++)
		m_pStage->m_pLaser->LaserAngle(i)+=m_phase5LaserAngleSpeed*m_phase5LaserRotationDir/16.0f;
}

void CBossStage4Mai::Phase5()
{
	Phase5Shoot();
	if (m_curPhaseFrame<5000)
		if (GetDmgOnAndCrashChara()==0)
			return;
		else
			AddDefeatScoreAndPopupNumbers(10);
	for (int i=0;i<10;i++)
		m_pStage->m_pLaser->RemoveLaser(i);
	EndPhase(1200,3);
}

void CBossStage4Mai::Phase6()
{
	GetDmgOnAndCrashChara();
	if (MoveTo(192.0f*16.0f,96.0f*16.0f)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;					//does not set m_curMode to 1 here, same as PC98 ver
	}
}

void CBossStage4Mai::Phase7ShootType1()
{
	if (m_curPhaseFrame<32)
		return;
	if (m_curPhaseFrame==32)
	{
		m_curImage=192;
		return;
	}
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.shootType=EST02_NWAY;
		si.bulletImg=45;
		si.nWay=2;
		si.shootAngle=64;
		si.nWayUnitAngle=12;
		si.bulletSpeed=48;
		for (int i=0;i<8;i++)
		{
			m_pStage->m_pEnemyBullet->Shoot(&si);
			si.nWayUnitAngle+=8;
			si.bulletSpeed+=6;
		}
		m_pStage->m_bPlaySound[3]=true;
		if (m_curPhaseFrame%32==0)
		{
			si.shootType=EST12_RANDOM;
			si.bulletImg=0;
			si.nWay=3;
			si.bulletSpeed=32;
			float angle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
			m_largeBullet.AddBullet(m_curX,m_curY,212,angle,48,24,true,&si);
			m_largeBullet.AddBullet(m_curX,m_curY,212,angle-32,48,24,true,&si);
			m_largeBullet.AddBullet(m_curX,m_curY,212,angle+32,48,24,true,&si);
		}
	}

	if (m_curPhaseFrame==224)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
	if (m_nModeChange>=2&&m_curPhaseFrame>=64)
		RandomWalk(m_curPhaseFrame%64);
}

void CBossStage4Mai::Phase7ShootType2()
{
	if (m_curPhaseFrame<32)
		return;
	if (m_curPhaseFrame==32)
	{
		m_curImage=192;
		return;
	}
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.shootType=EST08_NWAY_MULTI;
		si.specialMoveVar=1;
		si.bulletMoveType=EBMT02_SLOW_DOWN_THEN_TURN;
		si.bulletImg=116;
		si.nWay=2;
		si.nWayUnitAngle=8;
		si.nMulti=2;
		si.deltaSpeed=8;
		si.bulletSpeed=48;
		si.shootAngle=128;
		si.specialBulletDelta=(float)(182+(m_curPhaseFrame-48)/16*8);
		m_pStage->m_pEnemyBullet->Shoot(&si,true,false);
		si.shootAngle=0;
		si.specialBulletDelta=-si.specialBulletDelta;
		m_pStage->m_pEnemyBullet->Shoot(&si,true,false);
		m_pStage->m_bPlaySound[3]=true;
	}

	if (m_curPhaseFrame==192)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
	if (m_nModeChange>=2&&m_curPhaseFrame>=64)
		RandomWalk(m_curPhaseFrame%64);
}

void CBossStage4Mai::Phase7()
{
	if (m_curMode==0)
	{
		if (RandomWalk(m_curPhaseFrame-32)!=0)
		{
			m_curPhaseFrame=0;
			m_curMode++;
			m_nModeChange++;
			if (m_nModeChange>=36)
			{
				EndPhase(0,1);
				return;
			}
		}
	}
	else
	{
		if (m_nModeChange%2==0)
			Phase7ShootType1();
		else
			Phase7ShootType2();
	}
	if (GetDmgOnAndCrashChara()!=0)
	{
		AddDefeatScoreAndPopupNumbers(10);
		EndPhase(0,1);
	}
}

void CBossStage4Mai::Phase8()
{
	GetDmgOnAndCrashChara();
	if (MoveTo(192.0f*16.0f,96.0f*16.0f)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;					//does not set m_curMode to 1 here, same as PC98 ver
	}
}

void CBossStage4Mai::Phase9ShootType1()
{
	if (m_curPhaseFrame<32)
		return;
	if (m_curPhaseFrame==32)
	{
		m_curImage=192;
		return;
	}
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.shootType=EST13_RANDOM_BASE_SPEED_32;
		si.bulletImg=46;
		si.nWay=8;
		si.bulletSpeed=24;
		float angle;
		if (m_nModeChange/2%2==1)
			angle=(float)(128-(m_curPhaseFrame-32)/4*4);
		else
			angle=(float)((m_curPhaseFrame-32)/4*4);
		m_largeBullet.AddBullet(m_curX,m_curY,212,angle,28,6,true,&si);
	}

	if (m_curPhaseFrame>128)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
}

void CBossStage4Mai::Phase9ShootType2()
{
	if (m_curPhaseFrame==16)
	{
		m_curImage=192;
		float deltaAngle=(float)(16+rand()%16);
		float c[]={m_pStage->m_pStageRes->m_palette[8*3]/255.0f,m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f,
				   m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f};
		m_pStage->m_pLaser->AddLaserFixed(0,m_curX,m_curY,c,8,64.0f-deltaAngle/2.0f,32,90);
		m_pStage->m_pLaser->AddLaserFixed(1,m_curX,m_curY,c,8,64.0f-deltaAngle/2.0f*3.0f,32,90);
		m_pStage->m_pLaser->AddLaserFixed(2,m_curX,m_curY,c,8,64.0f+deltaAngle/2.0f,32,90);
		m_pStage->m_pLaser->AddLaserFixed(3,m_curX,m_curY,c,8,64.0f+deltaAngle/2.0f*3.0f,32,90);
	}
	if (m_curPhaseFrame<=32)
		return;
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.shootType=EST06_MULTI;
		si.bulletImg=116;
		si.nMulti=8;
		si.deltaSpeed=6;
		si.bulletSpeed=24;
		si.bulletMoveType=EBMT08_BOUNCE_X_YTOP;
		si.specialMoveVar=1;
		si.shootAngle=(float)((-8+(m_curPhaseFrame-40)/8*8+8)%64-8);
		m_pStage->m_pEnemyBullet->Shoot(&si,true);
		si.shootAngle=128.0f-si.shootAngle;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);
	}

	if (m_curPhaseFrame>128)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return;
	}
}

void CBossStage4Mai::Phase9()
{
	if (m_curMode==0)
	{
		if (RandomWalk(m_curPhaseFrame-4)!=0)
		{
			m_curPhaseFrame=0;
			m_curMode++;
			m_nModeChange++;
			if (m_nModeChange>=20)
			{
				m_bSuccessDefeat=false;
				AddExplodeEffect(4,m_curX,m_curY);
				m_bDrawLargeBullet=false;
				m_curPhaseFrame=0;
				m_curPhase=0xfd;
				return;
			}
		}
	}
	else
	{
		if (m_nModeChange%2==0)
			Phase9ShootType1();
		else
			Phase9ShootType2();
	}
	if (GetDmgOnAndCrashChara()!=0)
	{
		m_bSuccessDefeat=true;
		AddExplodeEffect(4,m_curX,m_curY);
		m_bDrawLargeBullet=false;
		m_curPhaseFrame=0;
		m_curPhase=0xfd;
		return;
	}
}

void CBossStage4Mai::Step()
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
		DeathSequence(70);
		return;
	}
	m_largeBullet.Step();
	StepHPGauge(m_curHP,7800);
}

void CBossStage4Mai::DrawBoss()
{
	//exactly the same CBossStage4Yuki::DrawBoss()
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

void CBossStage4Mai::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(7800);
}

void CBossStage4Mai::DrawBackground()
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

void CBossStage4Mai::DrawSpecialBullet()
{
	if (m_bDrawLargeBullet)
		m_largeBullet.Draw();
}

}

























