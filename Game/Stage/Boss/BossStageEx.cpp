#include ".\bossstageex.h"
#include "../Stage.h"
#include "../../Game.h"

namespace th5w{

CBossStageEx::CBossStageEx(void)
{
	m_bDrawSpecialBullet=false;
	m_invulnerableTimeLeft=0;
	m_bgTimer=0;
	m_bFireWave[0]=m_bFireWave[1]=false;
	m_bPhase6Type2AngleInc=false;
	m_phase16ShootOrigX=192.0f*16.0f;
	m_phase17ColorChangeMode=0;
}

CBossStageEx::~CBossStageEx(void)
{
}

void CBossStageEx::Initialize()
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

	m_curvingBullet.m_pStage=m_pStage;
	m_curvingBullet.Initialize();
}

int CBossStageEx::GetDmgVerStageEx()
{
	if (m_pStage->m_bInCharaBomb)
		m_invulnerableTimeLeft=39;
	if (m_invulnerableTimeLeft==0)
		return GetDmgOnAndCrashChara();
	return 0;
}

void CBossStageEx::EndPhaseVerStageEx(int nextPhaseEndHP,int explodeEffectIdx)
{
	if (explodeEffectIdx!=0xffff)
	{
		AddExplodeEffect(explodeEffectIdx,m_curX,m_curY);
		if (m_bTimeoutPhaseEnd==false)
		{
			m_pStage->m_pEnemyBullet->m_bDropPointItemOnBonus=true;
			m_pStage->m_pEnemyBullet->m_noEnemyBullet=true;
			DropItem();
		}
	}
	m_bTimeoutPhaseEnd=true;
	m_curPhase++;
	m_curPhaseFrame=0;
	m_curMode=0;
	m_nModeChange=0;
	m_curHP=m_curPhaseEndHP;
	m_curPhaseEndHP=nextPhaseEndHP;
}

void CBossStageEx::Phase0()
{
	if (m_curPhaseFrame==1)
	{
		m_curHP=26500;
		m_curPhaseEndHP=23800;
		m_curImage=180;
		m_imgGoLeft=186;
		m_imgGoRight=184;
		m_imgStay=180;
		m_invulnerableTimeLeft=0;
		m_xLowerBound=64.0f*16.0f;
		m_xUpperBound=320.0f*16.0f;
		m_yLowerBound=48.0f*16.0f;
		m_yUpperBound=96.0f*16.0f;
	}
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=192)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_pStage->m_bPlaySound[13]=true;
		m_bgColor[0]=m_bgColor[1]=m_bgColor[2]=0;
		m_curOuterImg=196;
	}
}

void CBossStageEx::Phase1()
{
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=64)
	{
		m_curPhase++;
		m_curMode=1;
		m_nModeChange=0;
		m_curPhaseFrame=0;
		m_bDrawSpecialBullet=true;
	}
}

int CBossStageEx::CommonShootOuter()
{
	m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,7,6,m_curPhaseFrame-48,2048.0f);
	if (m_curPhaseFrame==48)
	{
		m_curImage=181;
		m_pStage->m_bPlaySound[8]=true;
	}
	if (m_curPhaseFrame<64)
		return 0;

	int res=1;
	switch(m_curPhase)
	{
	case 2:
		if (m_nModeChange%2==0)
			res=Phase2ShootType1();
		else
			res=Phase2ShootType2();
		break;
	case 4:
		res=Phase4Shoot();
		break;
	case 6:
		if (m_nModeChange%2==0)
			res=Phase6ShootType1();
		else
			res=Phase6ShootType2();
		break;
	case 8:
		res=Phase8Shoot();
		break;
	case 10:
		if (m_nModeChange%2==0)
			res=Phase10ShootType1();
		else
			res=Phase10ShootType2();
		break;
	case 12:
		res=Phase12Shoot();
		break;
	case 14:
		if (m_nModeChange%2==0)
			res=Phase14ShootType1();
		else
			res=Phase14ShootType2();
		break;
	case 16:
		res=Phase16Shoot();
		break;
	}
	if (res==1)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		return 1;
	}
	return 0;
}

int CBossStageEx::Phase2ShootType1()
{
	ENEMY_SHOOT_INFO si;
	si.shootOrigX=m_curX;
	si.shootOrigY=m_curY;
	si.bulletBornType=16;
	si.bulletSpeed=(float)(40+rand()%32);
	si.shootAngle=(float)(rand()%16*8-64);
	si.shootType=EST03_NWAY_TO_CHARA;
	si.bulletImg=84;
	si.nWay=5;
	si.nWayUnitAngle=12;
	m_pStage->m_pEnemyBullet->Shoot(&si);
	if (m_curPhaseFrame%4==0)
		m_pStage->m_bPlaySound[3]=true;

	return m_curPhaseFrame==128?1:0;
}

int CBossStageEx::Phase2ShootType2()
{
	if (m_curPhaseFrame==64)
		m_phase2Type2Angle=(float)(rand()%256);
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=19;
		si.bulletSpeed=64;
		si.shootAngle=m_phase2Type2Angle;
		if (m_nModeChange/2%2==0)
			m_phase2Type2Angle+=2;
		else
			m_phase2Type2Angle-=2;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.bulletMoveType=EBMT01_SLOW_DOWN_THEN_TURN_TO_CHARA;
		si.specialMoveVar=1;
		si.nWay=12;
		si.nWayUnitAngle=10;
		si.bulletImg=48;
		if (m_curPhaseFrame%8==0)
			m_pStage->m_pEnemyBullet->Shoot(&si);
		else
		{
			si.nWay=8;
			si.nMulti=10;
			si.bulletImg=84;
			m_pStage->m_pEnemyBullet->Shoot(&si,true);
		}
		m_pStage->m_bPlaySound[3]=true;
	}
	RandomWalk(m_curPhaseFrame-100);

	return m_curPhaseFrame==128?1:0;
}

int CBossStageEx::Phase6ShootType1()
{
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=16;
		si.bulletSpeed=(float)(12+rand()%32);
		si.shootAngle=(float)(rand()%128+128);
		si.shootType=EST02_NWAY;
		si.bulletMoveType=EBMT09_SPEED_UP_VERTICAL;
		si.specialMoveVar=1;
		si.bulletImg=46;
		si.nWay=3;
		si.nWayUnitAngle=12;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);
		if (m_curPhaseFrame%4==0)
		{
			m_pStage->m_bPlaySound[3]=true;
			si.bulletBornType=18;
			si.shootOrigX=m_curX+(float)(rand()%(96*16)-48*16);
			si.shootOrigY=m_curY+(float)(rand()%(64*16)-32*16);
			si.bulletImg=0;
			si.shootType=EST07_MULTI_TO_CHARA;
			si.bulletSpeed=48;
			si.nMulti=4;
			si.deltaSpeed=6;
			si.shootAngle=0;
			m_pStage->m_pEnemyBullet->Shoot(&si);
		}
	}
	return m_curPhaseFrame==128?1:0;
}

int CBossStageEx::Phase6ShootType2()
{
	if (m_curPhaseFrame==64)
	{
		m_bPhase6Type2AngleInc=!m_bPhase6Type2AngleInc;
		m_phase6Type2Angle=(float)(rand()%256);
	}
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletBornType=19;
		si.bulletSpeed=88;
		if (m_bPhase6Type2AngleInc)
			m_phase6Type2Angle+=3;
		else
			m_phase6Type2Angle-=3;
		si.shootAngle=m_phase6Type2Angle;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.nWay=22;
		si.bulletImg=52;
		m_pStage->m_pEnemyBullet->Shoot(&si);
		m_pStage->m_bPlaySound[3]=true;
	}

	return m_curPhaseFrame==128?1:0;
}

int CBossStageEx::Phase10ShootType1()
{
	if (m_curPhaseFrame<144)
	{
		float c[]={m_pStage->m_pStageRes->m_palette[3*3]/255.0f,m_pStage->m_pStageRes->m_palette[3*3+1]/255.0f,
				   m_pStage->m_pStageRes->m_palette[3*3+2]/255.0f};
		float angle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
		if (m_curPhaseFrame%16==0)
		{
			m_pStage->m_pLaser->AddLaserFixed((m_curPhaseFrame-64)/8,m_curX,m_curY,c,8,angle+48,47,80);
			ENEMY_SHOOT_INFO si;
			si.shootOrigX=m_curX;
			si.shootOrigY=m_curY;
			si.bulletBornType=16;
			si.bulletImg=68;
			si.shootType=EST04_NWAY_ALLAROUND;
			si.shootAngle=(float)(rand()%256);
			si.bulletSpeed=64;
			si.nWay=32;
			m_pStage->m_pEnemyBullet->Shoot(&si);
		}
		if (m_curPhaseFrame%16==8)
			m_pStage->m_pLaser->AddLaserFixed((m_curPhaseFrame-64)/8,m_curX,m_curY,c,8,angle-48,47,80);
	}
	for (int i=0;i<10;i++)
		if (m_pStage->m_pLaser->GetMode(i)==LASER_MODE_WAIT_TO_GROW)
			m_pStage->m_pLaser->LaserAngle(i)+=(float)(i%2*2-1);

	return m_curPhaseFrame==208?1:0;
}

int CBossStageEx::Phase10ShootType2()
{
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX+(float)(rand()%(48*16)-24*16);
		si.shootOrigY=m_curY;
		si.bulletBornType=18;
		si.shootType=EST03_NWAY_TO_CHARA;
		si.bulletMoveType=EBMT03_SPEED_UP;
		si.specialMoveVar=2;
		si.bulletSpeed=64;
		si.nWay=7;
		si.nWayUnitAngle=9;
		si.bulletImg=68;
		si.shootAngle=0;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);
		m_pStage->m_bPlaySound[3]=true;
	}

	RandomWalk(m_curPhaseFrame-100);

	return m_curPhaseFrame==128?1:0;
}

int CBossStageEx::Phase14ShootType1()
{
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletSpeed=56;
		si.bulletBornType=18;
		si.shootType=EST03_NWAY_TO_CHARA;
		si.bulletImg=47;
		si.shootAngle=0;
		si.nWay=9;
		si.nWayUnitAngle=15;
		m_pStage->m_pEnemyBullet->Shoot(&si);
		if (m_curPhaseFrame%64==0)
		{
			float angle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
			if (m_curPhaseFrame%128==0)
				angle+=32;
			else
				angle-=32;
			m_curvingBullet.AddBullet(m_curX,m_curY,angle,48,11);
			m_pStage->m_bPlaySound[15]=true;
		}
	}
	return m_curPhaseFrame==256?1:0;
}

int CBossStageEx::Phase14ShootType2()
{
	if (m_curPhaseFrame%8==0)
	{
		float angle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
		if (m_curPhaseFrame%16==0)
			angle+=64;
		else
			angle-=64;
		m_curvingBullet.AddBullet(m_curX,m_curY,angle,64,11);
		m_pStage->m_bPlaySound[15]=true;
	}
	RandomWalk(m_curPhaseFrame%32);
	return m_curPhaseFrame==128?1:0;
}

int CBossStageEx::Phase4Shoot()
{
	if (m_curPhaseFrame==64)
	{
		m_phase4Angle=64;
		m_bPhase4AngleInc=false;
		m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[0]=m_pStage->m_pStageRes->m_palette[7*3]/255.0f;
		m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[1]=m_pStage->m_pStageRes->m_palette[7*3+1]/255.0f;
		m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[2]=m_pStage->m_pStageRes->m_palette[7*3+2]/255.0f;
	}
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO si;
		if (m_curPhaseFrame%4==0)
		{
			si.shootOrigX=m_curX;
			si.shootOrigY=m_curY;
			si.bulletBornType=16;
			si.bulletImg=0;
			si.shootType=EST03_NWAY_TO_CHARA;
			si.nWay=5;
			si.nWayUnitAngle=6;
			si.bulletSpeed=80;
			si.shootAngle=m_phase4Angle;
			m_pStage->m_pEnemyBullet->Shoot(&si);
			si.shootAngle=-si.shootAngle;
			m_pStage->m_pEnemyBullet->Shoot(&si);
			if (m_bPhase4AngleInc)
			{
				m_phase4Angle+=3;
				if (m_phase4Angle>=63.999f)
					m_bPhase4AngleInc=false;
			}
			else
			{
				m_phase4Angle-=3;
				if (m_phase4Angle<=20.001f)
					m_bPhase4AngleInc=true;
			}
			if (m_curPhaseFrame%16==0)
			{
				m_phase4AimX=m_curX+(float)(rand()%(96*16)-48*16);
				m_phase4AimY=m_curY+(float)(rand()%(64*16)-32*16);
				m_phase4AimAngle=m_pStage->m_pChara->AngleToChara(m_phase4AimX,m_phase4AimY);
			}
		}
		si.shootOrigX=m_phase4AimX;
		si.shootOrigY=m_phase4AimY;
		si.bulletBornType=18;
		si.shootAngle=m_phase4AimAngle;
		si.shootType=EST00_SINGLE;
		si.bulletSpeed=96;
		si.bulletImg=84;
		m_pStage->m_pEnemyBullet->Shoot(&si);
	}
	if (m_curPhaseFrame%128==0)
	{
		if (m_curPhaseFrame%256==0)
		{
			if (m_bFireWave[0]==false)
			{
				m_bFireWave[0]=true;
				m_fireWaveY[0]=0;
			}
		}
		else
		{
			if (m_bFireWave[1]==false)
			{
				m_bFireWave[1]=true;
				m_fireWaveY[1]=0;
			}
		}
		m_pStage->m_bPlaySound[13]=true;
	}
	if (m_curPhaseFrame>=256)
		RandomWalk(m_curPhaseFrame%128-96);

	return 0;
}

int CBossStageEx::Phase8Shoot()
{
	if (m_curPhaseFrame==64)
		m_phase8Angle=(float)(rand()%256);
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO si;
		si.shootOrigX=m_curX;
		si.shootOrigY=m_curY;
		si.bulletImg=52;
		si.bulletBornType=19;
		si.bulletSpeed=32;
		si.shootType=EST04_NWAY_ALLAROUND;
		si.bulletMoveType=EBMT08_BOUNCE_X_YTOP;
		si.nWay=6+m_curPhaseFrame/128;
		if (si.nWay>14)
			si.nWay=14;
		si.specialMoveVar=1;
		m_phase8Angle+=2;
		si.shootAngle=m_phase8Angle;
		m_pStage->m_pEnemyBullet->Shoot(&si,true);
		m_pStage->m_bPlaySound[3]=true;
	}
	return 0;
}

int CBossStageEx::Phase12Shoot()
{
	if (m_curPhaseFrame==64)
	{
		m_phase12LaserAngle=0;
		m_bPhase12LaserAngleInc=true;
	}
	if (m_curPhaseFrame%4==0)
	{
		if (m_bPhase12LaserAngleInc)
		{
			m_phase12LaserAngle+=8;
			if (m_phase12LaserAngle>127.999f)
				m_bPhase12LaserAngleInc=false;
		}
		else
		{
			m_phase12LaserAngle-=8;
			if (m_phase12LaserAngle<0.001f)
				m_bPhase12LaserAngleInc=true;
		}
		float c[]={m_pStage->m_pStageRes->m_palette[2*3]/255.0f,m_pStage->m_pStageRes->m_palette[2*3+1]/255.0f,
				   m_pStage->m_pStageRes->m_palette[2*3+2]/255.0f};
		m_pStage->m_pLaser->AddLaserShootOut(m_curX,m_curY,c,m_phase12LaserAngle,30,88);
	}
	if (m_curPhaseFrame>=256)
	{
		RandomWalk(m_curPhaseFrame%64-32);
		if (m_curPhaseFrame>=512&&m_curPhaseFrame%16==0)
		{
			ENEMY_SHOOT_INFO si;
			si.shootOrigX=m_curX;
			si.shootOrigY=m_curY;
			si.bulletBornType=16;
			si.shootType=EST05_NWAY_ALLAROUND_TO_CHARA;
			si.shootAngle=0;
			si.bulletSpeed=64;
			si.nWay=32;
			si.bulletImg=68;
			m_pStage->m_pEnemyBullet->Shoot(&si);
		}
	}
	return 0;
}

int CBossStageEx::Phase16Shoot()
{
	ENEMY_SHOOT_INFO si;
	si.shootOrigX=m_phase16ShootOrigX;
	si.shootOrigY=m_curY+104.0f*16.0f;
	si.bulletSpeed=48;
	si.bulletBornType=18;
	si.shootType=EST02_NWAY;
	si.nWay=3;
	si.nWayUnitAngle=8;
	si.bulletImg=47;
	if (m_curPhaseFrame==64)
	{
		m_phase16ShootAngle=32;
		m_phase16Var3=0;
		m_phase16Var4=0;
		m_phase16Var5=0;
		m_pStage->m_bPlaySound[8]=true;
	}
	if (m_curPhaseFrame<128)
	{
		if (m_curPhaseFrame%8==0)
		{
			float deltaAngle=(float)((m_curPhaseFrame%16==0)?2:-2);
			float c[]={m_pStage->m_pStageRes->m_palette[14*3]/255.0f,m_pStage->m_pStageRes->m_palette[14*3+1]/255.0f,
					   m_pStage->m_pStageRes->m_palette[14*3+2]/255.0f};
			m_pStage->m_pGatherEffect->AddEffect(si.shootOrigX,si.shootOrigY,4096.0f,deltaAngle,c[0],c[1],c[2],8);
		}
		return 0;
	}
	if (m_curPhaseFrame%2==0)
	{
		if (m_curPhaseFrame%16==0)
		{
			float deltaAngle=(float)((m_curPhaseFrame%32==0)?2:-2);
			float c[]={m_pStage->m_pStageRes->m_palette[14*3]/255.0f,m_pStage->m_pStageRes->m_palette[14*3+1]/255.0f,
					   m_pStage->m_pStageRes->m_palette[14*3+2]/255.0f};
			m_pStage->m_pGatherEffect->AddEffect(si.shootOrigX,si.shootOrigY,2048.0f,deltaAngle,c[0],c[1],c[2],8);

		}
		si.shootAngle=128.0f-m_phase16ShootAngle;
		m_pStage->m_pEnemyBullet->Shoot(&si,false,false);
		si.shootAngle=m_phase16ShootAngle;
		m_pStage->m_pEnemyBullet->Shoot(&si,false,false);
		m_pStage->m_bPlaySound[3]=true;
		m_phase16ShootAngle-=(float)m_phase16Var5;
		if (m_curPhaseFrame%32==0)
		{
			m_phase16Var5+=m_phase16Var4;
			if (m_phase16Var3==0)
			{
				m_phase16Var4++;
				if (m_phase16Var4>=16)
					m_phase16Var3++;
			}
			else
			{
				m_phase16Var4--;
				if (m_phase16Var4==0)
				{
					m_phase16ShootOrigX=m_pStage->m_pChara->GetCurX();
					return 1;
				}
			}
		}
	}
	return 0;
}

void CBossStageEx::Phase2()
{
	bool bEndPhase=false;
	if (m_curMode==0)
	{
		if (RandomWalk(m_curPhaseFrame-32)!=0)
		{
			m_curPhaseFrame=0;
			m_nModeChange++;
			m_curMode++;
			if (m_nModeChange>=32)
				bEndPhase=true;
		}
	}
	else
		CommonShootOuter();
	if (bEndPhase==false)
	{
		if (GetDmgVerStageEx()!=0)
		{
			AddDefeatScoreAndPopupNumbers(20);
			bEndPhase=true;
		}
	}
	if (bEndPhase)
	{
		switch(m_curPhase)
		{
		case 2:
			EndPhaseVerStageEx(21000,0);
			break;
		case 6:
			EndPhaseVerStageEx(15100,1);
			break;
		case 10:
			EndPhaseVerStageEx(9600,2);
		    break;
		case 14:
			EndPhaseVerStageEx(3400,3);
		    break;
		}
	}
}

void CBossStageEx::Phase3()
{
	if (m_bgColor[0]<96)
		m_bgColor[0]+=2;
	Phase7_11_15();
}

void CBossStageEx::Phase4()
{
	if (m_bgColor[0]<96)
		m_bgColor[0]+=2;
	Phase8_12();
}

void CBossStageEx::Phase5_6()
{
	if (m_bgColor[2]<96)
	{
		m_bgColor[2]+=2;
		m_bgColor[0]-=2;
	}
	if (m_curPhase==5)
		Phase7_11_15();
	else
		Phase2();
}

void CBossStageEx::Phase7_11_15()
{
	if (MoveTo(192.0f*16.0f,64.0f*16.0f)!=0)
	{
		m_curPhaseFrame=0;
		m_curPhase++;
		m_curMode=1;
	}
	GetDmgVerStageEx();
}

void CBossStageEx::Phase8_12()
{
	CommonShootOuter();
	if (m_curPhaseFrame<=4000)
		if (GetDmgVerStageEx()!=0)
			AddDefeatScoreAndPopupNumbers(20);
		else
			return;
	switch(m_curPhase)
	{
	case 4:
		EndPhaseVerStageEx(18100,1);
		break;
	case 8:
		EndPhaseVerStageEx(12600,2);
		break;
	case 12:
		EndPhaseVerStageEx(6800,3);
		break;
	}
	m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[0]=m_pStage->m_pStageRes->m_palette[9*3]/255.0f;
	m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[1]=m_pStage->m_pStageRes->m_palette[9*3+1]/255.0f;
	m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[2]=m_pStage->m_pStageRes->m_palette[9*3+2]/255.0f;
	m_curOuterImg=200;
}

void CBossStageEx::Phase9_10()
{
	if (m_bgColor[0]<48)
	{
		m_bgColor[0]++;
		m_bgColor[2]--;
	}
	if (m_curPhase==9)
		Phase7_11_15();
	else
		Phase2();
}

void CBossStageEx::Phase13_14()
{
	if (m_bgColor[1]<64)
	{
		if (m_bgColor[0]>0)
			m_bgColor[0]--;
		m_bgColor[1]++;
		if (m_bgColor[2]>0)
			m_bgColor[2]--;
	}
	if (m_curPhase==13)
		Phase7_11_15();
	else
		Phase2();
}

void CBossStageEx::Phase16()
{
	if (CommonShootOuter()!=0)
	{
		m_nModeChange++;
		if (m_nModeChange>8)
		{
			EndPhaseVerStageEx(0,4);
			m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[0]=m_pStage->m_pStageRes->m_palette[9*3]/255.0f;
			m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[1]=m_pStage->m_pStageRes->m_palette[9*3+1]/255.0f;
			m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[2]=m_pStage->m_pStageRes->m_palette[9*3+2]/255.0f;
			m_curOuterImg=200;
			return;
		}
	}
	if (GetDmgVerStageEx()!=0)
	{
		AddDefeatScoreAndPopupNumbers(20);
		EndPhaseVerStageEx(0,4);
		m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[0]=m_pStage->m_pStageRes->m_palette[9*3]/255.0f;
		m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[1]=m_pStage->m_pStageRes->m_palette[9*3+1]/255.0f;
		m_pStage->m_pEnemyBullet->m_dotBulletShadowColor[2]=m_pStage->m_pStageRes->m_palette[9*3+2]/255.0f;
		m_curOuterImg=200;
	}
}

void CBossStageEx::Phase17Shoot()
{
	m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,7,6,m_curPhaseFrame-64);
	if (m_curPhaseFrame==64)
	{
		m_curImage=181;
		m_pStage->m_bPlaySound[8]=true;
		m_phase17ShootAngle=(float)(rand()%256);
	}
	if (m_curPhaseFrame>=128)
	{
		if (m_curPhaseFrame%2==0)
		{
			ENEMY_SHOOT_INFO si;
			si.shootOrigX=m_curX;
			si.shootOrigY=m_curY;
			si.bulletImg=116;
			si.bulletBornType=18;
			si.shootType=EST02_NWAY;
			si.nWay=5;
			si.nWayUnitAngle=21;
			si.bulletSpeed=40.0f+64.0f*(float)(3400-m_curHP)/3400.0f;
			si.shootAngle=m_phase17ShootAngle;
			m_pStage->m_pEnemyBullet->Shoot(&si);
			si.shootAngle+=128.0f;
			m_pStage->m_pEnemyBullet->Shoot(&si);
			m_phase17ShootAngle+=7.0f;
			m_pStage->m_bPlaySound[9]=true;
		}
		float angle=(float)((m_curPhaseFrame-128)*2);
		m_curX=192.0f*16.0f+(float)sin(angle/256.0f*2*PI)*64.0f*16.0f*(float)(3400-m_curHP)/3400.0f;
	}
}

void CBossStageEx::Phase17()
{
	if (m_phase17ColorChangeMode==0)
	{
		if (m_bgColor[1]>0)
			m_bgColor[1]--;
		else
			m_phase17ColorChangeMode=1;
	}
	else
	{
		if (m_phase17ColorChangeMode==1)
		{
			m_bgColor[0]+=2;
			m_bgColor[1]+=1;
			m_bgColor[2]+=2;
			if (m_bgColor[0]>=128)
				m_phase17ColorChangeMode=2;
		}
		else
		{
			m_bgColor[0]-=2;
			m_bgColor[1]-=1;
			m_bgColor[2]-=2;
			if (m_bgColor[0]==0)
				m_phase17ColorChangeMode=1;
		}
	}
	Phase17Shoot();
	if (m_curPhaseFrame<=5000)
	{
		if (GetDmgVerStageEx()!=0)
			m_bSuccessDefeat=true;
		else
			return;
	}
	else
		m_bSuccessDefeat=false;
	AddExplodeEffect(4,m_curX,m_curY);
	m_curPhaseFrame=0;
	m_curPhase=0xfd;
	m_bDrawSpecialBullet=false;
	m_pStage->m_pEnemyBullet->m_bDropPointItemOnBonus=false;
}

void CBossStageEx::Step()
{
	if (m_invulnerableTimeLeft>0)
		m_invulnerableTimeLeft--;
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
	case 6:
		Phase5_6();
		break;
	case 7:
	case 11:
	case 15:
		Phase7_11_15();
		break;
	case 8:
	case 12:
		Phase8_12();
		break;
	case 9:
	case 10:
		Phase9_10();
		break;
	case 13:
	case 14:
		Phase13_14();
		break;
	case 16:
		Phase16();
		break;
	case 17:
		Phase17();
		break;
	default:
		DeathSequence(200);
		break;
	}
	m_curvingBullet.Step();
	StepFireWave();
	StepHPGauge(m_curHP,26500);
	if (m_curPhase>=2)
		StepBackground();
}

void CBossStageEx::StepBackground()
{
	if (m_bgTimer==0)
	{
		for (int i=0;i<19;i++)
		{
			m_lineSeq.halfLen[i]=1.0f*16.0f;
			m_lineSeq.angle[i]=0;
		}
		m_bgTimer=1;
	}
	for (int i=18;i>0;i--)
	{
		m_lineSeq.halfLen[i]=m_lineSeq.halfLen[i-1];
		m_lineSeq.angle[i]=m_lineSeq.angle[i-1];
	}
	m_lineSeq.halfLen[0]+=5.0f*16.0f;
	if (m_lineSeq.halfLen[0]>=320.0f*16.0f)
	{
		m_lineSeq.halfLen[0]=1.0f*16.0f;
		m_bgTimer=3-m_bgTimer;
	}
	if (m_bgTimer==1)
		m_lineSeq.angle[0]+=1;
	else
		m_lineSeq.angle[0]-=1;
}

void CBossStageEx::StepFireWave()
{
	for (int i=0;i<2;i++)
		if (m_bFireWave[i])
		{
			m_fireWaveY[i]+=4;
			if (m_fireWaveY[i]>=604)
				m_bFireWave[i]=false;
		}
	for (int i=0;i<2;i++)
	{
		if (m_bFireWave[i]==false)
			continue;
		for (int cur=m_fireWaveY[i]/16*16+8;cur>=m_fireWaveY[i]-256;cur-=16)
		{
			if (cur>m_fireWaveY[i])
				continue;
			float len=(float)sin((m_fireWaveY[i]-cur)/512.0*2*PI)*128.0f*16.0f-16.0f*16.0f;
			if (len<0) continue;
			float yL=(float)(cur-8.0f)*16.0f;
			float yH=(float)(cur+8.0f)*16.0f;
			float cX=m_pStage->m_pChara->GetCurX();
			float cY=m_pStage->m_pChara->GetCurY();
			if (cY>=yL&&cY<=yH)
			{
				if (i==0)
				{
					if (cX<len)
						m_pStage->m_pChara->m_bMiss=true;
				}
				else
				{
					if (cX>384.0f*16.0f-len)
						m_pStage->m_pChara->m_bMiss=true;
				}
			}
		}
	}
}

void CBossStageEx::DrawFireWave()
{
	for (int i=0;i<2;i++)
	{
		if (m_bFireWave[i]==false)
			continue;
		for (int cur=m_fireWaveY[i]/16*16+8;cur>=m_fireWaveY[i]-256;cur-=16)
		{
			if (cur>m_fireWaveY[i])
				continue;
			float len=(float)sin((m_fireWaveY[i]-cur)/512.0*2*PI)*128.0f*16.0f-16.0f*16.0f;
			if (len<0) continue;
			float yL=(float)(cur-8.0f)*16.0f;
			float yH=(float)(cur+8.0f)*16.0f;
			if (len>=8.0f*16.0f)
				if (i==0)
					m_pStage->m_pStageRes->DrawRectangle(0,yL,len-8.0f*16.0f,16.0f*16.0f,1,1,1,true);
				else
					m_pStage->m_pStageRes->DrawRectangle(384.0f*16.0f-(len-8.0f*16.0f),yL,len-8.0f*16.0f,16.0f*16.0f,1,1,1,true);
			if (i==0)
			{
				C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(168);
				m_pStage->m_pStageRes->DrawStageObjectImage(pImg,len,yL+8.0f*16.0f);
			}
			else
			{
				C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(169);
				m_pStage->m_pStageRes->DrawStageObjectImage(pImg,384.0f*16.0f-len,yL+8.0f*16.0f);
			}
		}
	}
}

void CBossStageEx::DrawSet(float radius,float angle,unsigned char colorIdx)
{
	float c[]={m_pStage->m_pStageRes->m_palette[colorIdx*3]/255.0f,m_pStage->m_pStageRes->m_palette[colorIdx*3+1]/255.0f,
			   m_pStage->m_pStageRes->m_palette[colorIdx*3+2]/255.0f};
	m_pStage->m_pStageRes->DrawCircle(192.0f*16.0f,184.0f*16.0f,radius,c[0],c[1],c[2],false);
	float x[6],y[6];
	for (int i=0;i<6;i++)
	{
		x[i]=192.0f*16.0f+(float)cos((angle+i*256.0f/6.0f)/256.0f*2*PI)*radius;
		y[i]=184.0f*16.0f+(float)sin((angle+i*256.0f/6.0f)/256.0f*2*PI)*radius;
	}
	m_pStage->m_pStageRes->DrawLine(x[0],y[0],x[2],y[2],c[0],c[1],c[2],1);
	m_pStage->m_pStageRes->DrawLine(x[2],y[2],x[4],y[4],c[0],c[1],c[2],1);
	m_pStage->m_pStageRes->DrawLine(x[4],y[4],x[0],y[0],c[0],c[1],c[2],1);
	m_pStage->m_pStageRes->DrawLine(x[1],y[1],x[3],y[3],c[0],c[1],c[2],1);
	m_pStage->m_pStageRes->DrawLine(x[3],y[3],x[5],y[5],c[0],c[1],c[2],1);
	m_pStage->m_pStageRes->DrawLine(x[5],y[5],x[1],y[1],c[0],c[1],c[2],1);
}

void CBossStageEx::DrawBackground()
{
	if (m_curPhase==0||m_curPhase>=0xfe)	//as from code of PC98 ver, should be m_curPhase>=0xff
	{										//but from replay videos, it behaves as if m_curPhase>=0xfe, don't know why
		m_pStage->m_pBackground->Draw();
		return;
	}
	if (m_curPhase==1)
	{
		//PC98 ver does not draw stage background in the first 16 frames, this seems to be bug
		//but does not matter much. Hence we do the same here.
		m_pStage->m_pStageRes->ClearPlayArea(m_bgColor[0]/255.0f,m_bgColor[1]/255.0f,m_bgColor[2]/255.0f);
		m_pStage->m_pStageRes->DrawPlayAreaMask(m_pStage->m_pStageRes->m_pStageBB->GetMaskPtr(m_curPhaseFrame/4),1,1,1);
		return;
	}

	m_pStage->m_pStageRes->ClearPlayArea(m_bgColor[0]/255.0f,m_bgColor[1]/255.0f,m_bgColor[2]/255.0f);
	DrawSet(m_lineSeq.halfLen[18],m_lineSeq.angle[18],8);
	DrawSet(m_lineSeq.halfLen[9],m_lineSeq.angle[9],9);
	if (m_curPhase<9||m_curPhase>12)
		DrawSet(m_lineSeq.halfLen[0],m_lineSeq.angle[0],15);
	else
		DrawSet(m_lineSeq.halfLen[0],m_lineSeq.angle[0],9);
}

void CBossStageEx::DrawBoss()
{
	if (m_curPhase==0xfe)
	{
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
	else
	{
		int imgIdx=m_curImage;
		if (m_invulnerableTimeLeft%2!=0)
			imgIdx+=8;
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
		if (m_bHitThisFrame)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImg,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);

		if (m_curImage<=181&&m_curPhase>=2)
		{
			pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curOuterImg+m_pStage->m_curFrame%16/4);
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
		}
	}
}

void CBossStageEx::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(26500);
}

void CBossStageEx::DrawSpecialBullet()
{
	if (m_bDrawSpecialBullet)
	{
		DrawFireWave();
		m_curvingBullet.Draw();
	}
}

}




















