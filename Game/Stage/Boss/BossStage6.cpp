#include ".\bossstage6.h"
#include "../Stage.h"
#include "../../Game.h"

namespace th5w{

CBossStage6::CBossStage6(void)
{
	m_bDrawSpecialBullet=false;
	m_bFloatingUp=false;
	m_phase10Timer=false;
}

CBossStage6::~CBossStage6(void)
{
}

void CBossStage6::Initialize()
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

	m_bgColor[0]=0;
	m_bgColor[1]=0;
	m_bgColor[2]=0;

	m_bgTypeATimer=0xff;
	m_bgTypeARotateCounter=0;

	m_bgTypeBTimer=0;
	m_bgTypeBCFrame=0;
	m_bgTypeCTimer=0;
	m_bgTypeDTimer=0;

	m_curvingBullet.m_pStage=m_pStage;
	m_curvingBullet.Initialize();
	m_shinkiBullet.m_pStage=m_pStage;
	m_shinkiBullet.Initialize();
}

void CBossStage6::FloatMove()
{
	if (m_pStage->m_curFrame%4==0)
	{
		if (m_bFloatingUp)
		{
			m_velY-=1;
			if (m_velY<=-16)
				m_bFloatingUp=false;
		}
		else
		{
			m_velY+=1;
			if (m_velY>=16)
				m_bFloatingUp=true;
		}
		m_curY+=m_velY;
	}
}

void CBossStage6::Phase0()
{
	if (m_curPhaseFrame==1)
	{
		m_curHP=22800;
		m_curPhaseEndHP=20600;
		m_curImage=180;
		m_velY=0;
		m_imgGoLeft=183;
		m_imgGoRight=182;
		m_imgStay=180;
	}
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=192)
	{
		m_curPhaseFrame=0;
		m_curPhase++;
		m_pStage->m_bPlaySound[13]=true;
	}
}

void CBossStage6::Phase1()
{
	GetDmgOffAndCrashChara();
	if (m_curPhase==32)
		m_bgColor[0]=m_bgColor[1]=m_bgColor[2]=0;
	if (m_curPhaseFrame>=64)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=1;
		//PC98 ver does not set m_nCurMode to 0 here, but it doesn't matter.
		//In PC98 ver, it is initialized in common initialize function for all stages.
		//Here we've already set it in CBoss constructor
	}
}

int CBossStage6::Phase2ShootType1()
{
	if (m_curPhaseFrame==32)
	{
		m_phase2Type1DeltaAngle=(float)(rand()%2*4-2);
		m_phase2Type1Angle=(float)(rand()%256);
		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.bulletImg=45;
		si.bulletSpeed=(float)(32+(m_curPhaseFrame-32)/4*8);
		si.shootType=EST04_NWAY_ALLAROUND;
		si.bulletMoveType=EBMT10_STRAIGHT;
		si.shootAngle=m_phase2Type1Angle;
		si.nWay=16;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		m_pStage->m_pEnemyBullet->Shoot(&si,true);
		m_phase2Type1Angle+=m_phase2Type1DeltaAngle;
	}
	if (m_curPhaseFrame==60)
		return 1;
	return 0;
}

int CBossStage6::Phase2ShootType2()
{
	if (m_curPhaseFrame==32)
		m_phase2Type2Angle=(float)(rand()%256);
	int intervalA[]={16,12,8,4};
	int interval=intervalA[CGame::GVar().m_playDifficulty];
	if (m_curPhaseFrame%interval==0)
	{
		ENEMY_SHOOT_INFO si;
		si.bulletBornType=18;
		si.bulletImg=44;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.nWay=16;

		si.shootAngle=m_phase2Type1Angle+(m_curPhaseFrame-32)/interval*8;
		float len=(float)(rand()%(32*16));
		si.shootOrigX=m_curX+(float)cos(si.shootAngle/256.0f*2*PI)*len;
		si.shootOrigY=m_curY+(float)sin(si.shootAngle/256.0f*2*PI)*len;

		si.bulletSpeed=32;
		m_pStage->m_pEnemyBullet->Shoot(&si);
		si.shootAngle+=8;
		si.bulletSpeed=64;
		m_pStage->m_pEnemyBullet->Shoot(&si);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame==96)
		return 1;
	return 0;
}

int CBossStage6::Phase3ShootType1()
{
	if (m_curPhaseFrame==32)
		m_pStage->m_bPlaySound[15]=true;
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO si;
		si.bulletBornType=18;
		si.bulletSpeed=28;
		si.shootType=EST13_RANDOM_BASE_SPEED_32;
		si.nWay=3;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		si.shootOrigY=m_curY;

		si.bulletImg=52;
		si.shootOrigX=m_curX-16.0f*16.0f;
		m_pStage->m_pEnemyBullet->Shoot(&si);

		si.bulletImg=116;
		si.shootOrigX=m_curX+16.0f*16.0f;
		m_pStage->m_pEnemyBullet->Shoot(&si);
	}

	return RandomWalk(m_curPhaseFrame-64);
}

int CBossStage6::Phase3ShootType2()
{
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.bulletImg=44;
		si.shootType=EST09_NWAY_MULTI_TO_CHARA;
		si.shootAngle=0;
		si.nWay=2;
		si.nWayUnitAngle=4;
		int nMulti[]={5,6,6,7};
		si.nMulti=nMulti[CGame::GVar().m_playDifficulty];
		float deltaSpeed[]={12,12,13,13};
		si.deltaSpeed=deltaSpeed[CGame::GVar().m_playDifficulty];
		si.bulletSpeed=32;
		m_pStage->m_pEnemyBullet->Shoot(&si);
		m_pStage->m_bPlaySound[15]=true;
	}

	return RandomWalk(m_curPhaseFrame-64);
}

void CBossStage6::Phase2_3Shoot()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,6,7,m_curPhaseFrame-16);
		if (m_curPhaseFrame==2)
		{
			m_curImage=181;
			m_pStage->m_bPlaySound[8]=true;
		}
		return;
	}
	int res;
	if (m_curPhase==2)
	{
		if (m_nModeChange%2==0)
			res=Phase2ShootType1();
		else
			res=Phase2ShootType2();
	}
	else
	{
		if (m_nModeChange%2==0)
			res=Phase3ShootType1();
		else
			res=Phase3ShootType2();
	}
	if (res!=0)
	{
		m_curImage=180;
		m_curPhaseFrame=0;
		m_curMode=0;
	}
}

void CBossStage6::Phase2_3()
{
	bool bPhaseEnd=false;
	if (m_curMode==0)
	{
		if (RandomWalk(m_curPhaseFrame-32)!=0)
		{
			m_curPhaseFrame=0;
			m_nModeChange++;
			m_curMode++;
			if (m_nModeChange>=16)
				bPhaseEnd=true;
		}
	}
	else
		Phase2_3Shoot();
	if (bPhaseEnd==false)
		if (GetDmgOnAndCrashChara()!=0)
		{
			AddDefeatScoreAndPopupNumbers(10);
			bPhaseEnd=true;
		}
	if (bPhaseEnd)
	{
		if (m_curPhase==2)
		{
			EndPhase(18400,1);
			m_curMode=1;
		}
		else
			EndPhase(14600,1);
	}
}

void CBossStage6::Phase4()
{
	GetDmgOnAndCrashChara();
	if (MoveTo(192.0f*16.0f,80.0f*16.0f)!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
	}
}

int CBossStage6::Phase5Effect()
{
	if (m_curPhaseFrame==16)
	{
		float c[]={1,1,1};
		m_pStage->m_pLaser->AddLaserFixed(0,m_curX,m_curY,c,8,80);
		m_pStage->m_pLaser->AddLaserFixed(1,m_curX,m_curY,c,8,72);
		m_pStage->m_pLaser->AddLaserFixed(2,m_curX,m_curY,c,8,64);
		m_pStage->m_pLaser->AddLaserFixed(3,m_curX,m_curY,c,8,64);
		m_pStage->m_pLaser->AddLaserFixed(4,m_curX,m_curY,c,8,56);
		m_pStage->m_pLaser->AddLaserFixed(5,m_curX,m_curY,c,8,48);
		m_pStage->m_bPlaySound[8]=true;
		m_curImage=181;
		m_phase5ScrScale=100;
		m_phase5Timer=0;
	}
	if (m_curPhaseFrame<=16)
		return 0;
	if (m_pStage->m_pLaser->LaserAngle(2)<128.0f)
	{
		if (m_pStage->m_curFrame%2!=0)
		{
			m_pStage->m_pLaser->LaserAngle(0)+=1;
			m_pStage->m_pLaser->LaserAngle(1)+=1;
			m_pStage->m_pLaser->LaserAngle(2)+=1;
			m_pStage->m_pLaser->LaserAngle(3)-=1;
			m_pStage->m_pLaser->LaserAngle(4)-=1;
			m_pStage->m_pLaser->LaserAngle(5)-=1;
			m_pStage->m_curScrFade=m_phase5ScrScale;
			m_phase5ScrScale++;
		}
		return 0;
	}
	if (m_phase5Timer==0)
	{
		for (int i=0;i<50;i++)
		{
			ENEMY_SHOOT_INFO si;
			si.bulletBornType=18;
			si.bulletImg=44;
			si.shootType=EST00_SINGLE;
			m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
			si.shootOrigX=m_curX+(float)(rand()%(256*16)-128*16);
			si.shootOrigY=m_curY+(float)(16*16-rand()%(64*16));
			si.shootAngle=(float)(rand()%96+16);
			si.bulletSpeed=(float)(rand()%64+24);
			m_pStage->m_pEnemyBullet->Shoot(&si);
		}
		m_curImage=184;
		for (int i=0;i<6;i++)
			m_pStage->m_pLaser->StartGrow(i);
		m_pStage->m_bPlaySound[15]=true;
		m_phase5Timer++;
		return 0;
	}
	if (m_pStage->m_curFrame%2==0)
		m_pStage->m_curScrFade=100;
	else
		m_pStage->m_curScrFade=150;
	if (m_phase5Timer<8)
	{
		m_phase5Timer++;
		return 0;
	}
	m_pStage->m_curScrFade=100;
	for (int i=0;i<6;i++)
		m_pStage->m_pLaser->RemoveLaser(i);
	return 1;
}

void CBossStage6::Phase5()
{
	GetDmgOnAndCrashChara();
	if (Phase5Effect()!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_velY=0;
		m_bDrawSpecialBullet=true;
		float w[]={50.0f*16.0f,35.0f*16.0f,45.0f*16.0f,50.0f*16.0f};	//PC98 ver has lunatic area larger than hard
		m_bossDmgAreaHalfWidth=w[CGame::GVar().m_playDifficulty];
	}
}

void CBossStage6::Phase6Shoot()
{
	if (m_curPhaseFrame<=128)
		return;
	if (m_curPhaseFrame%8==0)
	{
		float sX=(float)(rand()%(384*16));
		float sY=(float)(rand()%(64*16)+32*16);
		float speed=(float)(rand()%32+48);
		m_shinkiBullet.AddBullet(sX,sY,200,64,speed);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame%24==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX+(float)(rand()%(256*16)-128*16);
		si.shootOrigY=m_curY-(float)(rand()%(64*16));
		si.bulletBornType=18;
		si.bulletImg=44;
		si.shootType=EST03_NWAY_TO_CHARA;
		si.nWay=5;
		float unitAngle[]={16,12,10,8};
		si.nWayUnitAngle=unitAngle[CGame::GVar().m_playDifficulty];
		si.shootAngle=0;
		si.bulletSpeed=48;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&si);
		m_pStage->m_pEnemyBullet->Shoot(&si);
	}
}

void CBossStage6::Phase7Shoot()
{
	if (m_curPhaseFrame<128)
		return;
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.bulletImg=45;
		si.shootType=EST02_NWAY;
		si.bulletSpeed=(float)(48+rand()%32);
		si.nWay=6;
		si.nWayUnitAngle=8;
		si.shootAngle=104;
		m_pStage->m_pEnemyBullet->Shoot(&si);
		si.shootAngle=24;
		m_pStage->m_pEnemyBullet->Shoot(&si);
	}
	int interval[]={128,48,32,24};
	if (m_curPhaseFrame%interval[CGame::GVar().m_playDifficulty]==0)
	{
		m_curvingBullet.AddBullet(m_curX,m_curY,(float)(rand()%3*64),64,11);
		m_pStage->m_bPlaySound[3]=true;
	}
}

void CBossStage6::Phase6_7()
{
	if (m_bgColor[2]<128&&m_pStage->m_curFrame%16==0)
		m_bgColor[2]++;
	FloatMove();
	if (m_curPhase==6)
		Phase6Shoot();
	else
		Phase7Shoot();
	if (m_curPhaseFrame<3000)
		if (GetDmgOnAndCrashChara()!=0)
			AddDefeatScoreAndPopupNumbers(25);
		else
			return;
	if (m_curPhase==6)
		EndPhase(11600,2);
	else
		EndPhase(8600,3);
}

int CBossStage6::Phase8Effect()
{
	if (m_curPhaseFrame<160)
		return 0;
	if (m_curPhaseFrame<192)
	{
		if (m_curPhaseFrame%2==0)
			m_curY-=2.0f*16.0f;
		else
			m_curY+=2.0f*16.0f;
		return 0;
	}
	if (m_curPhaseFrame==192)
	{
		for (int i=0;i<16;i++)
		{
			float sX=m_curX+(float)(rand()%(256*16)-128*16);
			float sY=m_curY+(float)(16*16-rand()%(64*16));
			float angle=(float)(rand()%64+32);
			float speed=(float)(rand()%64+32);
			m_shinkiBullet.AddBullet(sX,sY,201,angle,speed);
		}
		m_curImage=192;
		m_pStage->m_bPlaySound[15]=true;
		return 0;
	}
	if (m_curPhaseFrame==200)
		return 1;
	return 0;
}

void CBossStage6::Phase8()
{
	if (m_bgColor[2]!=0&&m_pStage->m_curFrame%8==0)
		m_bgColor[2]--;
	GetDmgOnAndCrashChara();
	if (Phase8Effect()!=0)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
	}
}

void CBossStage6::Phase9Shoot()
{
	if (m_curPhaseFrame<=128)
		return;
	int interval[]={96,32,28,24};
	if (m_curPhaseFrame%interval[CGame::GVar().m_playDifficulty]==0)
	{
		float angle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
		m_shinkiBullet.AddBullet(m_curX,m_curY,201,angle,64);
	}
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootType=EST02_NWAY;
		si.bulletMoveType=EBMT10_STRAIGHT;
		si.nWay=3;
		si.nWayUnitAngle=2;
		si.bulletBornType=18;
		si.bulletSpeed=40;
		si.bulletImg=84;
		si.shootOrigY=m_curY;
		if (m_curPhaseFrame%128<64)
			si.shootAngle=(float)(m_curPhaseFrame%128*3);
		else
			si.shootAngle=(float)(64-m_curPhaseFrame%128*3);

		si.shootOrigX=m_curX-128.0f*16.0f;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);

		si.shootAngle=128.0f-si.shootAngle;
		si.shootOrigX=m_curX+128.0f*16.0f;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);

		m_pStage->m_bPlaySound[15]=true;
	}
}

void CBossStage6::Phase10Shoot()
{
	if (m_curPhaseFrame<192)
		return;
	int idx=m_curPhaseFrame-192;
	if (m_curHP<=5600||m_curPhaseFrame>=1800)
		if (m_phase10Timer==0)
		{
			float c[]={m_pStage->m_pStageRes->m_palette[14*3]/255.0f,m_pStage->m_pStageRes->m_palette[14*3+1]/255.0f,
					   m_pStage->m_pStageRes->m_palette[14*3+2]/255.0f};
			m_pStage->m_pLaser->AddLaserFixed(0,m_curX+96.0f*16.0f,m_curY,c,6,64);
			m_pStage->m_pLaser->AddLaserFixed(1,m_curX+32.0f*16.0f,m_curY,c,6,64);
			m_pStage->m_pLaser->AddLaserFixed(2,m_curX-32.0f*16.0f,m_curY,c,6,64);
			m_pStage->m_pLaser->AddLaserFixed(3,m_curX-96.0f*16.0f,m_curY,c,6,64);
			m_phase10Timer++;
			AddExplodeEffect(0,m_curX,m_curY);
		}
	if (m_curPhaseFrame%4==0)
	{
		if (idx==0)
		{
			m_phase10Var0=0;
			m_phase10Var1=0;
			m_phase10Var2=0;
			int d[]={64,40,32,28};
			m_phase10Var3=d[CGame::GVar().m_playDifficulty];
			m_phase10Var4=32;
			m_phase10Var5=0;
			m_phase10VarI=64;
			m_phase10VarS=64;
			m_phase10VarD=0;
		}
		if (m_phase10Var5==64&&m_phase10Var2==0)
			m_phase10Var2++;

		ENEMY_SHOOT_INFO si;
		si.bulletBornType=18;
		si.bulletImg=84;
		si.shootType=EST02_NWAY;
		if (m_phase10Var2!=0)
			si.nWay=3;
		else
			si.nWay=1;
		si.nWayUnitAngle=48;
		si.bulletSpeed=112;
		si.shootOrigY=m_curY;

		si.shootOrigX=m_curX+96.0f*16.0f;
		si.shootAngle=(float)m_phase10Var5;
		m_pStage->m_pEnemyBullet->Shoot(&si,false,false);

		si.shootOrigX=m_curX+32.0f*16.0f;
		si.shootAngle=(float)m_phase10Var4;
		m_pStage->m_pEnemyBullet->Shoot(&si,false,false);

		si.shootOrigX=m_curX-32.0f*16.0f;
		si.shootAngle=128.0f-(float)m_phase10Var4;
		m_pStage->m_pEnemyBullet->Shoot(&si,false,false);

		si.shootOrigX=m_curX-96.0f*16.0f;
		si.shootAngle=128.0f-(float)m_phase10Var5;
		m_pStage->m_pEnemyBullet->Shoot(&si,false,false);

		if (m_phase10Var2!=0)
		{
			si.bulletImg=0;
			si.bulletBornType=16;
			si.nWay=3;
			si.nWayUnitAngle=48;

			si.shootOrigX=m_curX+96.0f*16.0f;
			si.shootAngle=(float)m_phase10Var5+128.0f;
			m_pStage->m_pEnemyBullet->Shoot(&si,false,false);

			si.shootOrigX=m_curX+32.0f*16.0f;
			si.shootAngle=(float)m_phase10Var4+128.0f;
			m_pStage->m_pEnemyBullet->Shoot(&si,false,false);

			si.shootOrigX=m_curX-32.0f*16.0f;
			si.shootAngle=128.0f-(float)m_phase10Var4+128.0f;
			m_pStage->m_pEnemyBullet->Shoot(&si,false,false);

			si.shootOrigX=m_curX-96.0f*16.0f;
			si.shootAngle=128.0f-(float)m_phase10Var5+128.0f;
			m_pStage->m_pEnemyBullet->Shoot(&si,false,false);

			if (m_phase10Var1!=0)
			{
				m_phase10Var5+=2;
				m_phase10Var4++;
				if (m_phase10Var5==64)
					m_phase10Var1--;
			}
			else
			{
				m_phase10Var5-=2;
				m_phase10Var4--;
				if (m_phase10Var5==48)
					m_phase10Var1++;
			}
		}
		else
		{
			m_phase10Var5+=4;
			m_phase10Var4+=2;
		}
		if (m_phase10Var2!=0&&m_curPhaseFrame%m_phase10Var3==0)
		{
			float x=m_curX+(float)(rand()%(256*16)-128*16);
			float y=m_curY+(float)(16*16-rand()%(64*16));
			float angle=m_pStage->m_pChara->AngleToChara(x,y);
			m_shinkiBullet.AddBullet(x,y,201,angle,60);
		}
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_phase10Timer!=0)
	{
		if (m_phase10Timer<64)
		{
			m_phase10Timer++;
			return;
		}
		if (m_phase10Timer==64)
		{
			for (int i=0;i<4;i++)
				m_pStage->m_pLaser->StartGrow(i);
			m_phase10Timer++;
			return;
		}
		if (m_phase10Var0!=0)
		{
			if (m_curPhaseFrame%8==0)
			{
				m_pStage->m_pLaser->LaserAngle(0)=(float)m_phase10VarS;
				m_pStage->m_pLaser->LaserAngle(1)=(float)m_phase10VarI;
				m_pStage->m_pLaser->LaserAngle(2)=128.0f-m_pStage->m_pLaser->LaserAngle(1);
				m_pStage->m_pLaser->LaserAngle(3)=128.0f-m_pStage->m_pLaser->LaserAngle(0);
				if (m_phase10VarD!=0)
				{
					m_phase10VarS--;
					m_phase10VarI++;
					if (m_phase10VarI==65)
						m_phase10VarD=0;
				}
				else
				{
					m_phase10VarS++;
					m_phase10VarI--;
					if (m_phase10VarI==56)
						m_phase10VarD=1;
				}
				if (m_curHP<=3800||m_curPhaseFrame>=2500)
				{
					if (m_phase10Timer==65)
					{
						int d[]={52,20,12,8};
						m_phase10Var3=d[CGame::GVar().m_playDifficulty];
						AddExplodeEffect(0,m_curX,m_curY);
					}
					m_phase10Timer++;
				}
			}
		}
		else
		{
			if (m_phase10Var5==48)
				m_phase10Var0++;
			return;
		}
	}
}

void CBossStage6::Phase9_10()
{
	if (m_bgColor[2]>0)
	{
		if (m_pStage->m_curFrame%8==0)
			m_bgColor[2]--;
	}
	else
	{
		if (m_bgColor[0]<128&&m_pStage->m_curFrame%16==0)
			m_bgColor[0]++;
	}
	FloatMove();
	if (m_curPhase==9)
		Phase9Shoot();
	else
		Phase10Shoot();
	if (m_curPhaseFrame<3000)
	{
		int res=GetDmgOnAndCrashChara();
		if (m_pStage->m_bInCharaBomb)
		{
			res=0;
			m_curHP+=m_HPLostThisFrame;
		}
		if (res!=0)
			AddDefeatScoreAndPopupNumbers(25);
		else
			return;
	}
	if (m_curPhase==9)
		EndPhase(2800,2);
	else
	{
		EndPhase(0,3);
		for (int i=0;i<4;i++)
			m_pStage->m_pLaser->RemoveLaser(i);
	}
}

void CBossStage6::Phase11()
{
	GetDmgOnAndCrashChara();
	if (m_curPhaseFrame%2==0)
		m_pStage->m_curScrFade=150;
	else
		m_pStage->m_curScrFade=100;
	if (m_curPhaseFrame>16)
	{
		m_curPhase++;
		m_curImage=180;
		m_bossDmgAreaHalfWidth=24.0f*16.0f;
		m_bgColor[0]=96;
		m_bgColor[2]=0;
		m_pStage->m_bPlaySound[13]=true;
	}
}

void CBossStage6::Phase12Shoot()
{
	if (m_curPhaseFrame<128)
		return;
	if (m_curPhaseFrame%8==0)
	{
		float angle=(float)((m_curPhaseFrame-128)/8*8);
		float x=m_curX+(float)cos(angle/256.0f*2*PI)*64.0f*16.0f;
		float y=m_curY+(float)sin(angle/256.0f*2*PI)*64.0f*16.0f;
		m_shinkiBullet.AddBullet(x,y,200,angle,60);

		angle+=128;
		x=m_curX+(float)cos(angle/256.0f*2*PI)*64.0f*16.0f;
		y=m_curY+(float)sin(angle/256.0f*2*PI)*64.0f*16.0f;
		m_shinkiBullet.AddBullet(x,y,200,angle,60);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame>=256&&m_curPhaseFrame%8==0&&m_curPhaseFrame%256<128)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.shootAngle=(float)((m_curPhaseFrame-256)/256*-32+m_curPhaseFrame%256/8*-2);
		si.bulletImg=84;
		si.bulletSpeed=32;
		si.nWay=12;
		m_pStage->m_pEnemyBullet->Shoot(&si);
	}
	if (m_curPhaseFrame>=512&&m_curPhaseFrame%8==0&&m_curPhaseFrame%256>=128)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.shootAngle=(float)((m_curPhaseFrame-256)/256*64+(m_curPhaseFrame%256-128)/8*4);
		si.bulletImg=0;
		si.bulletSpeed=32;
		si.nWay=12;
		m_pStage->m_pEnemyBullet->Shoot(&si);
	}
	if (m_curPhaseFrame>=720)
		RandomWalk((m_curPhaseFrame-720)%128-96);
}

void CBossStage6::Phase12()
{
	Phase12Shoot();
	if (m_curPhaseFrame<3000)
	{
		if (GetDmgOnAndCrashChara()!=0)
			m_bSuccessDefeat=true;
		else
			return;
	}
	else
		m_bSuccessDefeat=false;
	AddExplodeEffect(4,m_curX,m_curY);
	m_curPhaseFrame=0;
	m_curPhase=0xfd;
}


void CBossStage6::Step()
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
	case 3:
		Phase2_3();
		break;
	case 4:
		Phase4();
		break;
	case 5:
		Phase5();
		break;
	case 6:
	case 7:
		Phase6_7();
		break;
	case 8:
		Phase8();
		break;
	case 9:
	case 10:
		Phase9_10();
		break;
	case 11:
		Phase11();
		break;
	case 12:
		Phase12();
		break;
	default:
		DeathSequence(65);
		break;
	}
	m_shinkiBullet.Step();
	m_curvingBullet.Step();
	StepHPGauge(m_curHP,22800);
	StepBackground();
}

void CBossStage6::StepBGParticles()
{
	for (int i=0;i<64;i++)
	{
		if (m_bgParticle[i].bFlag==false)
			continue;
		m_bgParticle[i].curX+=m_bgParticle[i].velX;
		m_bgParticle[i].curY+=m_bgParticle[i].velY;
		if (m_bgParticle[i].curX<=-8.0f*16.0f||m_bgParticle[i].curX>=392.0f*16.0f
			||m_bgParticle[i].curY<=-8.0f*16.0f||m_bgParticle[i].curY>=376.0f*16.0f)
		{
			m_bgParticle[i].curX=m_bgParticle[i].origX;
			m_bgParticle[i].curY=m_bgParticle[i].origY;
			m_bgParticle[i].age=0;
		}
		else
			m_bgParticle[i].age++;
	}
}

void CBossStage6::BGLineForwardCopy()
{
	for (int i=0;i<2;i++)
		for (int j=18;j>0;j--)
		{
			m_lineSet[i].ctrX[j]=m_lineSet[i].ctrX[j-1];
			m_lineSet[i].ctrY[j]=m_lineSet[i].ctrY[j-1];
			m_lineSet[i].halfLen[j]=m_lineSet[i].halfLen[j-1];
			m_lineSet[i].angle[j]=m_lineSet[i].angle[j-1];
		}
}

void CBossStage6::StepBGTypeA()
{
	if (m_bgTypeATimer==0xff)
	{
		for (int i=0;i<20;i++)
		{
			m_lineSet[0].ctrX[i]=m_lineSet[1].ctrX[i]=192.0f*16.0f;
			m_lineSet[0].ctrY[i]=m_lineSet[1].ctrY[i]=184.0f*16.0f;
			m_lineSet[0].halfLen[i]=m_lineSet[1].halfLen[i]=1.0f*16.0f;
			m_lineSet[0].angle[i]=64;
			m_lineSet[1].angle[i]=-64;
		}
		for (int i=0;i<64;i++)
		{
			m_bgParticle[i].bFlag=false;
			m_bgParticle[i].imgIdx=0;
		}
		m_bgTypeATimer=0;
		m_bgParticleColor[0]=m_pStage->m_pStageRes->m_palette[8*3]/255.0f;
		m_bgParticleColor[1]=m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f;
		m_bgParticleColor[2]=m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f;
	}
	if (m_bgTypeATimer<64&&m_curPhaseFrame%4==0)
	{
		m_bgParticle[m_bgTypeATimer].bFlag=true;
		m_bgParticle[m_bgTypeATimer].curX=192.0f*16.0f;
		m_bgParticle[m_bgTypeATimer].curY=184.0f*16.0f;
		m_bgParticle[m_bgTypeATimer].origX=192.0f*16.0f;
		m_bgParticle[m_bgTypeATimer].origY=184.0f*16.0f;
		m_bgParticle[m_bgTypeATimer].angle=(float)(rand()%256);
		m_bgParticle[m_bgTypeATimer].age=0;
		float speed=(float)(rand()%64+32);
		m_bgParticle[m_bgTypeATimer].velX=(float)cos(m_bgParticle[m_bgTypeATimer].angle/256.0f*2*PI)*speed;
		m_bgParticle[m_bgTypeATimer].velY=(float)sin(m_bgParticle[m_bgTypeATimer].angle/256.0f*2*PI)*speed;
		m_bgTypeATimer++;
	}
	if (m_curPhase==3)
	{
		if (m_bgTypeARotateCounter%4>=2)
		{
			m_lineSet[0].angle[0]-=2;
			m_lineSet[1].angle[0]-=2;
		}
		else
		{
			m_lineSet[0].angle[0]+=2;
			m_lineSet[1].angle[0]+=2;
		}
	}
	BGLineForwardCopy();
	for (int i=0;i<2;i++)
	{
		m_lineSet[i].halfLen[0]+=4.0f*16.0f;
		float dis=m_lineSet[i].halfLen[0]*3.0f/4.0f;
		m_lineSet[i].ctrX[0]=(float)cos((m_lineSet[i].angle[0]-64.0f)/256.0f*2*PI)*dis+192.0f*16.0f;
		m_lineSet[i].ctrY[0]=(float)sin((m_lineSet[i].angle[0]-64.0f)/256.0f*2*PI)*dis+184.0f*16.0f;
		if (m_lineSet[i].halfLen[0]>=223.999f*16.0f)
		{
			m_lineSet[i].halfLen[0]=0.0f;
			m_bgTypeARotateCounter++;
		}
	}
	StepBGParticles();
}

void CBossStage6::StepBGTypeB()
{
	if (m_bgTypeBTimer==0)
	{
		for (int i=0;i<=18;i++)
		{
			m_lineSet[0].ctrX[i]=64.0f*16.0f;
			m_lineSet[1].ctrX[i]=320.0f*16.0f;
			m_lineSet[0].ctrY[i]=m_lineSet[1].ctrY[i]=216.0f*16.0f;
			m_lineSet[0].halfLen[i]=m_lineSet[1].halfLen[i]=256.0f*16.0f;
			m_lineSet[0].angle[i]=m_lineSet[1].angle[i]=64;
		}
		m_lineSet[0].customVar=m_lineSet[1].customVar=0;
		for (int i=0;i<64;i++)
		{
			m_bgParticle[i].bFlag=true;
			m_bgParticle[i].curX=(float)(rand()%(384*16));
			m_bgParticle[i].curY=(float)(rand()%(384*16));
			m_bgParticle[i].origX=m_bgParticle[i].curX;
			m_bgParticle[i].origY=-1.0f*16.0f;
			m_bgParticle[i].velX=m_bgParticle[i].velY=0;
			m_bgParticle[i].imgIdx=172+rand()%4;
		}
		m_bgParticleColor[0]=m_pStage->m_pStageRes->m_palette[8*3]/255.0f;
		m_bgParticleColor[1]=m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f;
		m_bgParticleColor[2]=m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f;
		m_bgTypeBTimer++;
		return;
	}
	for (int i=0;i<64;i++)
		if (m_bgParticle[i].velY<10.0f*16.0f)
			m_bgParticle[i].velY+=(float)(m_pStage->m_curFrame%2);
	BGLineForwardCopy();
	float delta=4;
	for (int i=0;i<2;i++)
	{
		m_lineSet[i].ctrY[0]+=m_lineSet[i].customVar;
		if (m_lineSet[i].customVar>-14.0f*16.0f&&m_pStage->m_curFrame%4==0)
			m_lineSet[i].customVar-=1;
		if (m_lineSet[i].halfLen[0]>96.0f*16.0f)
			m_lineSet[i].halfLen[0]-=2;
		if (m_bgTypeBCFrame%512>=256)
			m_lineSet[i].ctrX[0]-=delta;
		else
			m_lineSet[i].ctrX[0]+=delta;
		m_lineSet[i].angle[0]+=delta/2.0f;
		delta=-delta;
	}
	m_bgTypeBCFrame++;
	if (m_lineSet[0].ctrY[0]<80.0f*16.0f)
	{
		delta=80.0f*16.0f-m_lineSet[0].ctrY[0];
		for (int i=0;i<2;i++)
			for (int j=18;j>=0;j--)
				m_lineSet[i].ctrY[j]+=delta;
	}
	StepBGParticles();
}

void CBossStage6::StepBGTypeC()
{
	if (m_bgTypeCTimer==0)
	{
		if (m_bgParticle[0].velY<0)
		{
            for (int i=0;i<64;i++)
				m_bgParticle[i].origY=369.0f*16.0f;
			m_bgTypeBTimer++;
		}
		m_bgParticleColor[0]=m_pStage->m_pStageRes->m_palette[8*3]/255.0f;
		m_bgParticleColor[1]=m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f;
		m_bgParticleColor[2]=m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f;
	}
	for (int i=0;i<64;i++)
		if (m_bgParticle[i].velY>-10.0f*16.0f)
			m_bgParticle[i].velY-=(float)(m_pStage->m_curFrame%2);
	BGLineForwardCopy();
	float delta=4;
	for (int i=0;i<2;i++)
	{
		m_lineSet[i].ctrY[0]+=m_lineSet[i].customVar;
		if (m_lineSet[i].customVar<14.0f*16.0f&&m_pStage->m_curFrame%2==1)
			m_lineSet[i].customVar+=1;
		if (m_lineSet[i].halfLen[0]>96.0f*16.0f)
			m_lineSet[i].halfLen[0]-=2;
		if (m_bgTypeBCFrame%512>=256)
			m_lineSet[i].ctrX[0]-=delta;
		else
			m_lineSet[i].ctrX[0]+=delta;
		m_lineSet[i].angle[0]+=delta/2.0f;
		delta=-delta;
	}
	m_bgTypeBCFrame++;
	if (m_lineSet[0].ctrY[0]<80.0f*16.0f)
	{
		delta=80.0f*16.0f-m_lineSet[0].ctrY[0];
		for (int i=0;i<2;i++)
			for (int j=18;j>=0;j--)
				m_lineSet[i].ctrY[j]+=delta;
	}
	else
		if (m_lineSet[0].ctrY[0]>304.0f*16.0f)
		{
			delta=m_lineSet[0].ctrY[0]-304.0f*16.0f;
			for (int i=0;i<2;i++)
				for (int j=18;j>=0;j--)
					m_lineSet[i].ctrY[j]-=delta;
		}
	StepBGParticles();
}

void CBossStage6::StepBGTypeD()
{
	if (m_bgTypeDTimer==0)
	{
		for (int i=0;i<64;i++)
		{
			m_bgParticle[i].origY=240.0f*16.0f;
			m_bgParticle[i].velY=-1.0f*16.0f;
		}
		m_bgParticleColor[0]=m_pStage->m_pStageRes->m_palette[6*3]/255.0f;
		m_bgParticleColor[1]=m_pStage->m_pStageRes->m_palette[6*3+1]/255.0f;
		m_bgParticleColor[2]=m_pStage->m_pStageRes->m_palette[6*3+2]/255.0f;
		m_bgTypeDTimer++;
	}
	StepBGParticles();
	for (int i=0;i<64;i++)
	{
		int img=3-(int)(m_bgParticle[i].curY/(64.0f*16.0f));
		if (img<0)
			img=0;
		m_bgParticle[i].imgIdx=img+172;
	}
}

void CBossStage6::StepBackground()
{
	if (m_curPhase<2)
		return;

	switch(m_curPhase)
	{
	case 2:
	case 3:
		StepBGTypeA();
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		StepBGTypeB();
		break;
	case 8:
	case 9:
	case 10:
	case 11:
		StepBGTypeC();
		break;
	case 12:
	default:
		StepBGTypeD();
		break;
	}
}

void CBossStage6::DrawBGParticles()
{
	for (int i=0;i<64;i++)
	{
		if (m_bgParticle[i].bFlag==false)
			continue;
		int imgIdx=m_bgParticle[i].imgIdx;
		if (imgIdx==0)
		{
			imgIdx=m_bgParticle[i].age/8+172;
			if (imgIdx>175)
				imgIdx=175;
		}
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
		m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(pImg,m_bgParticle[i].curX,m_bgParticle[i].curY,
												  m_bgParticleColor[0],m_bgParticleColor[1],m_bgParticleColor[2]);
	}
}

void CBossStage6::DrawLineSetMember(int setIdx,int memberIdx,unsigned char colorIdx)
{
	float x[2],y[2];
	x[0]=m_lineSet[setIdx].ctrX[memberIdx]+(float)cos(m_lineSet[setIdx].angle[memberIdx]/256.0f*2*PI)*m_lineSet[setIdx].halfLen[memberIdx];
	y[0]=m_lineSet[setIdx].ctrY[memberIdx]+(float)sin(m_lineSet[setIdx].angle[memberIdx]/256.0f*2*PI)*m_lineSet[setIdx].halfLen[memberIdx];
	x[1]=m_lineSet[setIdx].ctrX[memberIdx]-(float)cos(m_lineSet[setIdx].angle[memberIdx]/256.0f*2*PI)*m_lineSet[setIdx].halfLen[memberIdx];
	y[1]=m_lineSet[setIdx].ctrY[memberIdx]-(float)sin(m_lineSet[setIdx].angle[memberIdx]/256.0f*2*PI)*m_lineSet[setIdx].halfLen[memberIdx];
	float c[]={m_pStage->m_pStageRes->m_palette[colorIdx*3]/255.0f,m_pStage->m_pStageRes->m_palette[colorIdx*3+1]/255.0f,
			   m_pStage->m_pStageRes->m_palette[colorIdx*3+2]/255.0f};
	m_pStage->m_pStageRes->DrawLine(x[0],y[0],x[1],y[1],c[0],c[1],c[2],1);
}

void CBossStage6::DrawBGTypeA()
{
	DrawBGParticles();
	DrawLineSetMember(0,18,8);
	DrawLineSetMember(0,12,8);
	DrawLineSetMember(1,18,8);
	DrawLineSetMember(1,12,8);
	DrawLineSetMember(0,6,9);
	DrawLineSetMember(1,6,9);
	DrawLineSetMember(0,0,15);
	DrawLineSetMember(1,0,15);
}

void CBossStage6::DrawBGTypeB()
{
	DrawBGParticles();
	DrawLineSetMember(0,18,8);
	DrawLineSetMember(0,12,8);
	DrawLineSetMember(1,18,8);
	DrawLineSetMember(1,12,8);
	DrawLineSetMember(0,6,9);
	DrawLineSetMember(1,6,9);
	DrawLineSetMember(0,0,15);
	DrawLineSetMember(1,0,15);
}

void CBossStage6::DrawBGTypeC()
{
	DrawBGParticles();
	DrawLineSetMember(0,18,8);
	DrawLineSetMember(0,12,8);
	DrawLineSetMember(1,18,8);
	DrawLineSetMember(1,12,8);
	DrawLineSetMember(0,6,9);
	DrawLineSetMember(1,6,9);
	DrawLineSetMember(0,0,15);
	DrawLineSetMember(1,0,15);
}

void CBossStage6::DrawBGTypeD()
{
	m_pStage->m_pStageRes->ClearPlayArea(96.0f/255.0f,0,0);
	C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(17);
	pImage->Draw((float)32+m_pStage->m_playAreaUpperLeftX-32,(float)256+m_pStage->m_playAreaUpperLeftY-16);
	DrawBGParticles();
}

void CBossStage6::DrawBackground()
{
	if (m_curPhase==0)
	{
		m_pStage->m_pBackground->Draw();
		return;
	}
	if (m_curPhase==1)
	{
		if (m_curPhaseFrame/4<8)
			m_pStage->m_pBackground->Draw();
		else
			m_pStage->m_pStageRes->ClearPlayArea(0,0,0);
		m_pStage->m_pStageRes->DrawPlayAreaMask(m_pStage->m_pStageRes->m_pStageBB->GetMaskPtr(m_curPhaseFrame/4),1,1,1);
		return;
	}
	m_pStage->m_pStageRes->ClearPlayArea(m_bgColor[0]/255.0f,m_bgColor[1]/255.0f,m_bgColor[2]/255.0f);
	switch(m_curPhase)
	{
	case 2:
	case 3:
		DrawBGTypeA();
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		DrawBGTypeB();
		break;
	case 8:
	case 9:
	case 10:
	case 11:
		DrawBGTypeC();
	    break;
	case 12:
	default:
		DrawBGTypeD();
	    break;
	}
}

void CBossStage6::DrawBoss()
{
	if (m_curPhase==0xfe)
	{
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
	else
	{
		if (m_curImage>=184)
		{
			int imgIdx=m_curImage;
			if (m_bHitThisFrame)
				imgIdx+=4;
			for (int i=0;i<4;i++)
			{
				C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx+i);
				m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX-96.0f*16.0f+(float)i*64.0f*16.0f,m_curY);
			}
		}
		else
		{
			C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
			if (m_bHitThisFrame)
				m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImg,m_curX,m_curY);
			else
				m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
		}
	}
}

void CBossStage6::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(22800);
}

void CBossStage6::DrawSpecialBullet()
{
	if (m_bDrawSpecialBullet)
	{
		m_shinkiBullet.Draw();
		m_curvingBullet.Draw();
	}
}

}






























