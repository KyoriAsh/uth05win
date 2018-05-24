#include ".\bossstage5.h"
#include "../Stage.h"
#include "../../Game.h"

namespace th5w{

CBossStage5::CBossStage5(void)
{
	m_bDrawKnives=false;
}

CBossStage5::~CBossStage5(void)
{
}

void CBossStage5::Initialize()
{
	m_curX=96.0f*16.0f;
	m_curY=64.0f*16.0f;
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_velX=0;
	m_velY=0;
	m_shinkiX=192.0f*16.0f;
	m_shinkiY=64.0f*16.0f;
	m_curImage=180;
	m_bossDmgAreaHalfWidth=24.0f*16.0f;
	m_bossDmgAreaHalfHeight=24.0f*16.0f;

	m_knifeBullet.m_pStage=m_pStage;
	m_knifeBullet.Initialize();
}

void CBossStage5::Phase0()
{
	if (m_curPhaseFrame==1)
	{
		m_curHP=8300;
		m_curPhaseEndHP=7500;
		m_curImage=180;
		m_velX=4.0f*16.0f;
		m_imgGoLeft=180;
		m_imgGoRight=180;
		m_imgStay=180;
	}
	if (m_shinkiY>=-32.0f*16.0f)
	{
		m_shinkiY-=1.0f*16.0f;
		if (m_pStage->GetDamageFromChara(m_shinkiX,m_shinkiY,24.0f*16.0f,24.0f*16.0f,false)!=0)
			m_pStage->m_bPlaySound[10]=true;
	}
	if (m_curPhaseFrame<64)
		GetDmgOffAndCrashChara();
	else
	{
		m_curImage=188;
		m_curX+=2.0f*16.0f;
		if (m_curX>=192.0f*16.0f)
		{
			m_curImage=180;
			m_curPhase++;
			m_curPhaseFrame=0;
			m_pStage->m_bPlaySound[13]=true;
		}
	}
}

void CBossStage5::Phase1()
{
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=64)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=1;
		m_nModeChange=0;
		m_bDrawKnives=true;
	}
}

int CBossStage5::RandomWalkVerStage5(int nFrame,float speed)
{
	if (m_curPhaseFrame==1)
	{
		float angle=(float)(rand()%256);
		m_velX=(float)cos(angle/256.0f*2*PI)*speed;
		m_velY=(float)sin(angle/256.0f*2*PI)*speed;
		m_curImage=188;
	}
	m_curX+=m_velX;
	m_curY+=m_velY;
	if (m_curX<=48.0f*16.0f)
		m_velX=fabs(m_velX);
	if (m_curX>=336.0f*16.0f)
		m_velX=-fabs(m_velX);
	if (m_curY<=48.0f*16.0f)
		m_velY=fabs(m_velY);
	if (m_curY>=128.0f*16.0f)
		m_velY=-fabs(m_velY);

	if (m_curPhaseFrame<nFrame)
		return 0;
	m_curImage=180;
	return 1;
}

void CBossStage5::Phase2ShootType1()
{
	if (m_curPhaseFrame==16)
	{
		m_curImage=184;
		m_bPhase2KnifeClockwise=(rand()%2==0);
		return;
	}
	if (m_curPhaseFrame<=32)
		return;
	if (m_curPhaseFrame%2==0)
	{
		float angle=(float)(112-(m_curPhaseFrame-34)/2*6);
		if (m_bPhase2KnifeClockwise)
			angle=128.0f-angle;
		float origX=m_curX+(float)cos(angle/256.0f*2*PI)*48.0f*16.0f;
		float origY=m_curY+(float)sin(angle/256.0f*2*PI)*48.0f*16.0f;
		m_knifeBullet.AddBullet(origX,origY,angle,80,48);
		if (m_bPhase2KnifeClockwise)
			angle=128.0f-angle;
		if (angle<=12.001f)
		{
			m_curMode=0;
			m_curPhaseFrame=0;
		}
	}
}

void CBossStage5::Phase2ShootType2()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,6,7,m_curPhaseFrame-16);
		if (m_curPhaseFrame==16)
		{
			m_curImage=184;
			m_pStage->m_bPlaySound[8]=true;
		}
		return;
	}
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO si;
		si.bulletBornType=19;
		si.bulletImg=48;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.bulletSpeed=60;
		si.nWay=16;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		si.shootAngle=(float)(rand()%256);
		float len=(float)(rand()%(32*16));
		si.shootOrigX=m_curX+(float)cos(si.shootAngle/256.0f*2*PI)*len;
		si.shootOrigY=m_curY+(float)sin(si.shootAngle/256.0f*2*PI)*len;
		m_pStage->m_pEnemyBullet->Shoot(&si);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame==80)
	{
		m_curMode=0;
		m_curPhaseFrame=0;
	}
}

void CBossStage5::Phase5ShootType1()
{
	if (m_curPhaseFrame<16)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,6,7,m_curPhaseFrame-1);
		if (m_curPhaseFrame==4)
		{
			m_curImage=184;
			m_pStage->m_bPlaySound[8]=true;
		}
		return;
	}
	if (m_curPhaseFrame%8==0)
	{
		float a=(float)64-(m_curPhaseFrame-16)/8*6;
		float b=(float)96-(m_curPhaseFrame-16)/8*8;

		ENEMY_SHOOT_INFO si;
		si.bulletBornType=16;
		si.shootType=EST02_NWAY;
		si.bulletMoveType=EBMT10_STRAIGHT;
		si.bulletSpeed=56;
		si.nWay=5;
		si.nWayUnitAngle=2;

		si.bulletImg=44;

		si.shootOrigX=m_curX+(float)cos(b/256.0f*2*PI)*48.0f*16.0f;
		si.shootOrigY=m_curY+(float)sin(b/256.0f*2*PI)*48.0f*16.0f;
		si.shootAngle=b+a;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);

		si.shootOrigX=m_curX+(float)cos((b+128.0f)/256.0f*2*PI)*48.0f*16.0f;
		si.shootOrigY=m_curY+(float)sin((b+128.0f)/256.0f*2*PI)*48.0f*16.0f;
		si.shootAngle=b+a+128.0f;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);

		b=-b;

		si.bulletImg=0;

		si.shootOrigX=m_curX+(float)cos(b/256.0f*2*PI)*32.0f*16.0f;		//why PC98 ver change to 32 here
		si.shootOrigY=m_curY+(float)sin(b/256.0f*2*PI)*32.0f*16.0f;
		si.shootAngle=b-a;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);

		si.shootOrigX=m_curX+(float)cos((b+128.0f)/256.0f*2*PI)*32.0f*16.0f;
		si.shootOrigY=m_curY+(float)sin((b+128.0f)/256.0f*2*PI)*32.0f*16.0f;
		si.shootAngle=b-a+128.0f;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);

		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame==256)
	{
		m_curMode=0;
		m_curPhaseFrame=0;
	}
}

void CBossStage5::Phase5ShootType2()
{
	if (m_curPhaseFrame<16)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,6,7,m_curPhaseFrame-1);
		if (m_curPhaseFrame==4)
		{
			m_curImage=184;
			m_pStage->m_bPlaySound[8]=true;
		}
		return;
	}
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO si;
		si.bulletBornType=16;
		si.shootType=EST03_NWAY_TO_CHARA;
		si.bulletSpeed=56;
		si.nWayUnitAngle=20;
		si.shootAngle=0;
		si.bulletImg=68;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);

		float angle=(float)128-(m_curPhaseFrame-16)/8*8;
		si.shootOrigX=m_curX+(float)cos(angle/256.0f*2*PI)*48.0f*16.0f;
		si.shootOrigY=m_curY+(float)sin(angle/256.0f*2*PI)*48.0f*16.0f;
		si.nWay=5;
		m_pStage->m_pEnemyBullet->Shoot(&si);

		angle=128.0f-angle;
		si.shootOrigX=m_curX+(float)cos(angle/256.0f*2*PI)*48.0f*16.0f;
		si.shootOrigY=m_curY+(float)sin(angle/256.0f*2*PI)*48.0f*16.0f;
		si.nWay=4;
		m_pStage->m_pEnemyBullet->Shoot(&si);

		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame==192)
	{
		m_curMode=0;
		m_curPhaseFrame=0;
	}
}

void CBossStage5::Phase2_5()
{
	bool bPhaseEnd=false;
	if (m_curMode==0)
	{
		if (RandomWalkVerStage5(64,32)!=0)
		{
			m_curPhaseFrame=0;
			m_curMode++;
			m_nModeChange++;
			if (m_nModeChange>=20)
				bPhaseEnd=true;
		}
	}
	else
	{
		if (m_curPhase==2)
		{
			if (m_nModeChange%2==0)
				Phase2ShootType1();
			else
				Phase2ShootType2();
		}
		else
		{
			if (m_nModeChange%2==0)
				Phase5ShootType1();
			else
				Phase5ShootType2();
		}

	}
	if (!bPhaseEnd)
	{
		if (m_curMode!=0)
			if (GetDmgOnAndCrashChara()!=0)
			{
				AddDefeatScoreAndPopupNumbers(10);
				bPhaseEnd=true;
			}
	}
	if (bPhaseEnd)
	{
		if (m_curPhase==2)
			EndPhase(5700,0);
		else
			EndPhase(2700,3);
	}
}

void CBossStage5::Phase3_6()
{
	GetDmgOnAndCrashChara();
	if (MoveTo(192.0f*16.0f,64.0f*16.0f)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
	}
}

void CBossStage5::Phase4Shoot()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,6,7,m_curPhaseFrame-16);
		if (m_curPhaseFrame==16)
		{
			m_curImage=184;
			m_pStage->m_bPlaySound[8]=true;
			m_phase4ShootEnd=32;
			int interval[]={20,10,6,6};
			m_phase4ShootKnifeInterval=interval[CGame::GVar().m_playDifficulty];
			m_phase4CrazyMode=false;
		}
		return;
	}
	if (m_curPhaseFrame%64<m_phase4ShootEnd&&m_curPhaseFrame%2==0)
	{
		if (m_curPhaseFrame%64==0)
		{
			m_phase4ShootAngle[0]=m_pStage->m_pChara->AngleToChara(m_curX-32.0f*16.0f,m_curY-16.0f*16.0f);
			m_phase4ShootAngle[1]=m_pStage->m_pChara->AngleToChara(m_curX+32.0f*16.0f,m_curY-16.0f*16.0f);
		}
		ENEMY_SHOOT_INFO si;
		si.bulletBornType=18;
		si.bulletImg=84;
		si.shootType=EST00_SINGLE;
		si.bulletSpeed=96;

		si.shootOrigX=m_curX-32.0f*16.0f;
		si.shootOrigY=m_curY-16.0f*16.0f;
		si.shootAngle=m_phase4ShootAngle[0];
		m_pStage->m_pEnemyBullet->Shoot(&si);

		si.shootOrigX=m_curX+32.0f*16.0f;
		si.shootOrigY=m_curY-16.0f*16.0f;
		si.shootAngle=m_phase4ShootAngle[1];
		m_pStage->m_pEnemyBullet->Shoot(&si);

		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame%m_phase4ShootKnifeInterval==0)
	{
		float origX=(float)(16*16+rand()%(352*16));
		float origY=(float)(rand()%(96*16));
		float angle=m_pStage->m_pChara->AngleToChara(origX,origY)+(float)(rand()%32-15);
		m_knifeBullet.AddBullet(origX,origY,angle,76,32);
	}
	if (m_curHP<m_curPhaseEndHP+600&&m_phase4CrazyMode==false)
	{
		m_phase4CrazyMode=true;
		int interval[]={16,8,4,4};
		m_phase4ShootKnifeInterval=interval[CGame::GVar().m_playDifficulty];
		int shootEnd[]={40,48,52,52};
		m_phase4ShootEnd=shootEnd[CGame::GVar().m_playDifficulty];
		AddExplodeEffect(0,m_curX,m_curY);
		m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
	}
}

void CBossStage5::Phase7Shoot()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,6,7,m_curPhaseFrame-16);
		if (m_curPhaseFrame==16)
		{
			m_curImage=184;
			m_pStage->m_bPlaySound[8]=true;
			m_phase7ShootEnd=32;
			int interval[]={48,32,24,24};
			m_phase7ShootKnifeInterval=interval[CGame::GVar().m_playDifficulty];
			m_phase7CrazyMode=false;
		}
		return;
	}
	if (m_curPhaseFrame%64<m_phase7ShootEnd&&m_curPhaseFrame%2==0)
	{
		if (m_curPhaseFrame%64==0)
		{
			m_phase7ShootAngle[0]=m_pStage->m_pChara->AngleToChara(m_curX-32.0f*16.0f,m_curY-16.0f*16.0f);
			m_phase7ShootAngle[1]=m_pStage->m_pChara->AngleToChara(m_curX+32.0f*16.0f,m_curY-16.0f*16.0f);
		}
		ENEMY_SHOOT_INFO si;
		si.bulletBornType=18;
		si.bulletImg=116;
		si.shootType=EST00_SINGLE;
		si.bulletSpeed=96;

		si.shootOrigX=m_curX-32.0f*16.0f;
		si.shootOrigY=m_curY-16.0f*16.0f;
		si.shootAngle=m_phase7ShootAngle[0];
		m_pStage->m_pEnemyBullet->Shoot(&si);

		si.shootOrigX=m_curX+32.0f*16.0f;
		si.shootOrigY=m_curY-16.0f*16.0f;
		si.shootAngle=m_phase7ShootAngle[1];
		m_pStage->m_pEnemyBullet->Shoot(&si);

		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame%m_phase7ShootKnifeInterval==0)
	{
		float laserX=(float)(64*16+rand()%256*16);
		float laserY=32.0f*16.0f;
		float laserAngle=m_pStage->m_pChara->AngleToChara(laserX,laserY);
		float color[]={m_pStage->m_pStageRes->m_palette[8*3]/255.0f,m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f,
					   m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f};
		m_pStage->m_pLaser->AddLaserShootOut(laserX,laserY,color,laserAngle,28,100);
		for (int i=0;i<2;i++)
		{
			float origX=(float)(16*16+rand()%(352*16));
			float origY=(float)(rand()%(96*16));
			float angle=m_pStage->m_pChara->AngleToChara(origX,origY)+(float)(rand()%32-15);
			m_knifeBullet.AddBullet(origX,origY,angle,76,32);
		}
	}
	if (m_curHP<m_curPhaseEndHP+500&&m_phase7CrazyMode==false)
	{
		m_phase7CrazyMode=true;
		int interval[]={34,28,20,20};
		m_phase7ShootKnifeInterval=interval[CGame::GVar().m_playDifficulty];
		int shootEnd[]={40,48,52,48};		//PC98 ver makes the one for lunatic smaller than hard, should be bug
		m_phase7ShootEnd=shootEnd[CGame::GVar().m_playDifficulty];
		AddExplodeEffect(0,m_curX,m_curY);
		m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
	}
}

void CBossStage5::Phase4_7()
{
	if (m_curPhase==4)
		Phase4Shoot();
	else
		Phase7Shoot();
	if (m_curPhaseFrame<2000)
		if (GetDmgOnAndCrashChara()==0)
			return;
		else
			AddDefeatScoreAndPopupNumbers(10);

	if (m_curPhase==4)
		EndPhase(4500,1);
	else
		EndPhase(1200,1);
	m_curMode=1;
}

void CBossStage5::Phase8Shoot()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,6,7,m_curPhaseFrame-16);
		if (m_curPhaseFrame==16)
		{
			m_curImage=184;
			m_pStage->m_bPlaySound[8]=true;
			m_phase8KnifeY=(float)(rand()%(32*16));
			m_phase8KnifeLeft=true;
			m_phase8KnifePart=true;
			m_phase8ShootPartSpeed=128.0f;
		}
		return;
	}
	if (m_phase8KnifePart)
	{
		if (m_curPhaseFrame%8==0)
		{
			float angle;
			float knifeX;
			if (m_phase8KnifeLeft==true)
			{
				angle=0;
				knifeX=16.0f*16.0f;
			}
			else
			{
				angle=128;
				knifeX=368.0f*16.0f;
			}
			m_knifeBullet.AddBullet(knifeX,m_phase8KnifeY,angle,64,32);
			float deltaY[]={40.0f*16.0f,30.0f*16.0f,24.0f*16.0f,16.0f*16.0f};
			m_phase8KnifeY+=deltaY[CGame::GVar().m_playDifficulty];
			m_phase8KnifeLeft=!m_phase8KnifeLeft;
			if (m_phase8KnifeY>=376.0f*16.0f)
			{
				m_phase8KnifeY=(float)(rand()%(32*16));
				m_phase8KnifePart=false;
				m_phase8ShootPartNReset=1;
			}
		}
	}
	else
	{
		if (m_curPhaseFrame%4==0)
		{
			ENEMY_SHOOT_INFO si;
			si.shootOrigX=m_curX;
			si.shootOrigY=m_curY;
			si.bulletBornType=16;
			si.bulletImg=52;
			si.shootType=EST02_NWAY;
			si.nWay=5;
			float unitAngle[]={24,16,12,10};
			si.nWayUnitAngle=unitAngle[CGame::GVar().m_playDifficulty];
			if (m_phase8ShootPartSpeed>96)
			{
				m_phase8ShootPartSpeed=8;
				m_phase8ShootPartNReset++;
				m_pStage->m_bPlaySound[15]=true;
				if (m_phase8ShootPartNReset>5)
				{
					m_phase8KnifePart=true;
					return;
				}
				m_phase8ShootPartAngle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
			}
			si.bulletSpeed=m_phase8ShootPartSpeed;
			si.shootAngle=m_phase8ShootPartAngle;
			m_pStage->m_pEnemyBullet->Shoot(&si);
			m_phase8ShootPartSpeed+=8;
		}
	}
}

void CBossStage5::Phase8()
{
	Phase8Shoot();
	if (m_curPhaseFrame<2000)
		if (GetDmgOnAndCrashChara()==0)
			return;
		else
			AddDefeatScoreAndPopupNumbers(10);

	EndPhase(0,1);
}

void CBossStage5::Phase9()
{
	GetDmgOnAndCrashChara();
	if (MoveTo(192.0f*16.0f,96.0f*16.0f)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_nModeChange=0;
	}
}

void CBossStage5::Phase10Shoot()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,6,7,m_curPhaseFrame-16);
		if (m_curPhaseFrame==16)
		{
			m_curImage=184;
			m_pStage->m_bPlaySound[8]=true;
		}
		return;
	}
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.bulletImg=116;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.nWay=18;
		si.bulletMoveType=EBMT02_SLOW_DOWN_THEN_TURN;
		si.bulletSpeed=38;
		si.specialMoveVar=2;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		si.shootAngle=(float)((m_curPhaseFrame-32)/16);
		if ((m_curPhaseFrame-32)/16%2==0)
		{
			si.shootAngle=128.0f-si.shootAngle;
			si.specialBulletDelta=64.0f;
		}
		else
			si.specialBulletDelta=192.0f;
		m_pStage->m_pEnemyBullet->Shoot(&si,true,false);
		m_pStage->m_bPlaySound[3]=true;
	}
}

void CBossStage5::Phase10()
{
	Phase10Shoot();
	if (m_curPhaseFrame<1200)
	{
		if (GetDmgOnAndCrashChara()==0)
			return;
		else
			m_bSuccessDefeat=true;
	}
	else
		m_bSuccessDefeat=false;
	AddExplodeEffect(4,m_curX,m_curY);
	m_curPhaseFrame=0;
	m_curPhase=0xfd;
	m_bDrawKnives=false;
}

void CBossStage5::Step()
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
	case 5:
		Phase2_5();
		break;
	case 3:
	case 6:
		Phase3_6();
		break;
	case 4:
	case 7:
		Phase4_7();
		break;
	case 8:
		Phase8();
		break;
	case 9:
		Phase9();
		break;
	case 10:
		Phase10();
		break;
	default:
		DeathSequence(65);
		return;
	}
	m_knifeBullet.Step();
	StepHPGauge(m_curHP,9600);
}

void CBossStage5::DrawBoss()
{
	if (m_curPhase==0xfe)
	{
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
	else
	{
		C2DImage *pImg;
		if (m_curPhase==0)
		{
			pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(192);
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_shinkiX,m_shinkiY);
		}
		pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage+m_pStage->m_curFrame%16/4);
		if (m_bHitThisFrame)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImg,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
}

void CBossStage5::DrawBackground()
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
			pImage->Draw((float)32+m_pStage->m_playAreaUpperLeftX-32,(float)16+m_pStage->m_playAreaUpperLeftY-16);
		}
		m_pStage->m_pStageRes->DrawPlayAreaMask(m_pStage->m_pStageRes->m_pStageBB->GetMaskPtr(m_curPhaseFrame/4),1,1,1);
		return;
	}
	m_pStage->m_pStageRes->ClearPlayArea(0,0,0);
	C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
	pImage->Draw((float)32+m_pStage->m_playAreaUpperLeftX-32,(float)16+m_pStage->m_playAreaUpperLeftY-16);
}

void CBossStage5::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(8300);
}

void CBossStage5::DrawSpecialBullet()
{
	if (m_bDrawKnives)
		m_knifeBullet.Draw();
}

}




























