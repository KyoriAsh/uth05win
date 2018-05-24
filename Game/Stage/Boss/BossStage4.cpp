#include ".\bossstage4.h"
#include "../Stage.h"
#include "../../Game.h"
#include "BossStage4Mai.h"
#include "BossStage4Yuki.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

namespace th5w{

CBossStage4::CBossStage4(void)
{
}

CBossStage4::~CBossStage4(void)
{
}

void CBossStage4::Initialize()
{
	m_curX=96.0f*16.0f;
	m_curY=64.0f*16.0f;
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_velX=0;
	m_velY=0;
	m_curImage=180;
	m_bossDmgAreaHalfWidth=24.0f*16.0f;
	m_bossDmgAreaHalfHeight=24.0f*16.0f;

	m_2curX=288.0f*16.0f;
	m_2curY=64.0f*16.0f;
	m_2lastX=m_2curX;
	m_2lastY=m_2curY;
	m_2curImage=180;

	m_bDrawCurvingBullet=false;

	m_curvingBullet.m_pStage=m_pStage;
	m_curvingBullet.Initialize();
}

int CBossStage4::GetDmgAndTestCrash2(float dmgAreaHalfWidth,float dmgAreaHalfHeight,int soundIdx)
{
	int dmg=m_pStage->GetDamageFromChara(m_2curX,m_2curY,dmgAreaHalfWidth,dmgAreaHalfHeight,true);
	if (dmg!=0)
		m_pStage->m_bPlaySound[soundIdx]=true;

	if (m_pStage->m_pChara->TestCrash(m_curX,m_curY,16.0f*16.0f,16.0f*16.0f))
		m_pStage->m_pChara->m_bMiss=true;

	return dmg;
}

void CBossStage4::GetDmgOffAndCrashChara2()
{
	GetDmgAndTestCrash2(m_bossDmgAreaHalfWidth,m_bossDmgAreaHalfHeight,10);
}

int CBossStage4::GetDmgOnAndCrashChara2()
{
	int dmg=GetDmgAndTestCrash2(m_bossDmgAreaHalfWidth,m_bossDmgAreaHalfHeight,4);
	if (dmg!=0)
		m_b2HitThisFrame=true;
	else
		m_b2HitThisFrame=false;
	m_2curHP-=dmg;
	if (m_2curHP<=m_2curPhaseEndHP)
		return 1;

	return 0;
}

int CBossStage4::RandomWalkVerStage4(int frameIdx,float &angle,int &forceMove,
									 float &x,float &y,float &vx,float &vy,int &img)
{
	if (frameIdx<0)
		return 0;

	if (frameIdx==0)
	{
		if (x<144.0f*16.0f)
			angle=(float)(rand()%96-48);
		else
			if (x>240.0f*16.0f)
				angle=(float)(rand()%96+80);	//PC98 ver chooses randomly from 48/80/112/144, seems bug
			else
				angle=(float)(rand()%256);
		while (angle>=256.0f)
			angle-=256.0f;
		while (angle<0.0f)
			angle+=256.0f;
		if (forceMove==1&&angle<128.0f)
			angle=256.0f-angle;
		if (forceMove==2&&angle>128.0f)
			angle=256.0f-angle;
		forceMove=0;
	}
	vx=(float)cos(angle/256.0f*2*PI)*(100-frameIdx*2);
	vy=(float)sin(angle/256.0f*2*PI)*(100-frameIdx*2);
	x+=vx;
	y+=vy;
	if (vx<0)
		img=182;
	else
		img=181;
	if (y<24.0f*16.0f)
	{
		y=24.0f*16.0f;
		forceMove=2;
	}
	if (y>128.0f*16.0f)
	{
		y=128.0f*16.0f;
		forceMove=1;
	}
	if (x<32.0f*16.0f)
		x=32.0f*16.0f;
	if (x>352.0f*16.0f)
		x=352.0f*16.0f;
	if (frameIdx<48)
		return 0;
	img=180;
	return 1;
}

void CBossStage4::DeathSequenceVerStage4()
{
	if (m_curPhase==0xfd)
	{
		if (m_curPhaseFrame==16)
			if (m_whichDie==0)
				AddExplodeEffect(4,m_curX,m_curY);
			else
				AddExplodeEffect(4,m_2curX,m_2curY);
		if (m_curPhaseFrame==32)
		{
			if (m_whichDie==0)
			{
				m_deathExplodeCenterX=m_curX;
				m_deathExplodeCenterY=m_curY;
				m_curImage=4;
				m_2curImage=180;
			}
			else
			{
				m_deathExplodeCenterX=m_2curX;
				m_deathExplodeCenterY=m_2curY;
				m_curImage=180;
				m_2curImage=4;
			}
			m_curPhase++;
			m_pStage->m_pEnemyBullet->m_noEnemyBullet=m_bSuccessDefeat;
			m_curPhaseFrame=0;
			m_pStage->m_bPlaySound[12]=true;
			m_pStage->m_pChara->m_invulnerableTimeLeft=255;
		}
		return;
	}
	if (m_curPhase==0xfe)
	{
		if (m_curPhaseFrame%16==0)
		{
			if (m_whichDie==0)
				m_curImage++;
			else
				m_2curImage++;
			if (m_curPhaseFrame>=128)
			{
				m_curPhase++;
				m_curPhaseFrame=0;
			}
		}
		return;
	}
	if (m_curPhase==0xff)
	{
		if (m_curPhaseFrame==1)
		{
			char tx2File[]="_DM08.TX2";
			if (m_whichDie==0)
				tx2File[4]=48+9;
			Cth5DatFile *pPackFile=CGame::s_pCurGame->FindResource(tx2File);
			int idx=pPackFile->GetChildFileIndex(tx2File);
			unsigned char *pContent=pPackFile->GetChildFileContentPointer(idx);
			int offset=*(unsigned short *)(pContent+CGame::GVar().m_playChara*2);
			m_pStage->m_pDialogue->SetDialogueScript(pContent+offset);
			m_pStage->m_pDialogue->Run();
			return;
		}
		CBoss *pPart2;
		if (m_whichDie!=0)
		{
			CBossStage4Mai *p=new CBossStage4Mai;
			p->SetX(m_curX);
			p->SetY(m_curY);
			p->SetStage(m_pStage);
			pPart2=p;
			m_pStage->m_pTextOverlay->SetMode(TEXT_OVERLAY_DRAW_STAGE_NAME_BGM_NAME,NULL,m_pStage->m_pStageRes->m_stage4BossPart2BGMNames[0]);
		}
		else
		{
			CBossStage4Yuki *p=new CBossStage4Yuki;
			p->SetX(m_2curX);
			p->SetY(m_2curY);
			p->SetStage(m_pStage);
			pPart2=p;
			m_pStage->m_pTextOverlay->SetMode(TEXT_OVERLAY_DRAW_STAGE_NAME_BGM_NAME,NULL,m_pStage->m_pStageRes->m_stage4BossPart2BGMNames[1]);
		}
		pPart2->Initialize();
		pPart2->Step();
		m_pStage->m_pBoss=pPart2;
		delete this;
	}
}

int CBossStage4::Move(int frameIdx)
{
	RandomWalkVerStage4(frameIdx,m_randomWalkMovingAngle,m_forceGoUpOrDown,m_curX,m_curY,m_velX,m_velY,m_curImage);
	return RandomWalkVerStage4(frameIdx,m_2randomWalkMovingAngle,m_2forceGoUpOrDown,
							   m_2curX,m_2curY,m_2velX,m_2velY,m_2curImage);
}

void CBossStage4::Phase0()
{
	if (m_curPhaseFrame==1)
	{
		m_curHP=4500;
//		m_curHP=50;			//TODO: comment this line
		m_curPhaseEndHP=0;
		m_2curHP=4500;
//		m_2curHP=50;		//TODO: comment this line
		m_2curPhaseEndHP=0;
	}
	GetDmgOffAndCrashChara();
	GetDmgOffAndCrashChara2();
	if (m_curPhaseFrame>=128)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_pStage->m_bPlaySound[13]=true;
	}
}

void CBossStage4::Phase1()
{
	GetDmgOffAndCrashChara();
	GetDmgOffAndCrashChara2();
	if (m_curPhaseFrame>=64)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=1;
		m_nModeChange=0;
		m_bDrawCurvingBullet=true;
		m_curMode=1;
		m_2curMode=1;
	}
}

int CBossStage4::MaiShootType0_1()
{
	if (m_curPhaseFrame%6==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST07_MULTI_TO_CHARA;
		shootInfo.shootAngle=(float)(rand()%64-32);
		shootInfo.bulletImg=47;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;
		shootInfo.nMulti=2;
		shootInfo.deltaSpeed=6;
		shootInfo.bulletSpeed=(float)(16+rand()%32);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame!=128)
		return 0;
	return 1;
}

int CBossStage4::MaiShootType0_2__2_2()
{
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.shootAngle=(float)(240+(m_curPhaseFrame-48)/4*5);
		shootInfo.bulletImg=0;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;
		shootInfo.nWay=5;
		shootInfo.nWayUnitAngle=10;
		shootInfo.bulletSpeed=50;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame!=160)
		return 0;
	return 1;
}

int CBossStage4::MaiShootType0_3()
{
	if (m_curPhaseFrame==48)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST03_NWAY_TO_CHARA;
		shootInfo.bulletMoveType=EBMT01_SLOW_DOWN_THEN_TURN_TO_CHARA;
		shootInfo.shootAngle=0;
		shootInfo.bulletImg=116;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;
		shootInfo.nWay=16;
		shootInfo.nWayUnitAngle=8;
		shootInfo.bulletSpeed=60;
		shootInfo.specialMoveVar=1;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true,false);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame!=96)
		return 0;
	return 1;
}

int CBossStage4::MaiShootType0_4()
{
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=18;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.shootAngle=64;
		shootInfo.bulletImg=46;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;
		shootInfo.nWay=8;
		shootInfo.nWayUnitAngle=12;
		shootInfo.bulletSpeed=64;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
	if (m_curPhaseFrame!=128)
		return 0;
	return 1;
}

int CBossStage4::MaiShootType1()
{
	if (m_curPhaseFrame==48)
	{
		float color[]={m_pStage->m_pStageRes->m_palette[8*3]/255.0f,m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f,
					   m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f};
		float angle=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
		m_pStage->m_pLaser->AddLaserFixed(0,m_curX,m_curY,color,8,angle);
	}
	if (m_curPhaseFrame==80)
		m_pStage->m_pLaser->StartGrow(0);
	if (m_curPhaseFrame%4==0)
		if (!(m_curPhaseFrame<80&&m_curPhaseFrame>=144&&m_curPhaseFrame%8!=0))
		{
			int deltaAngle=(int)(m_pStage->m_pLaser->LaserAngle(0)-m_pStage->m_pChara->AngleToChara(m_curX,m_curY));
			while(deltaAngle<0)
				deltaAngle+=256;
			while(deltaAngle>=256)
				deltaAngle-=256;
			if (deltaAngle!=0)
			{
				if (deltaAngle>=128)
					m_pStage->m_pLaser->LaserAngle(0)+=1;
				else
					m_pStage->m_pLaser->LaserAngle(0)-=1;
			}
		}
	if (m_curPhaseFrame==160)
	{
		m_pStage->m_pLaser->RemoveLaser(0);
		return 1;
	}
	return 0;
}

int CBossStage4::MaiShootType2_1()
{
	if (m_curPhaseFrame==48||m_curPhaseFrame==96)
	{
		float angle=m_curX<m_2curX?64.0f:-64.0f;
		angle+=m_pStage->m_pChara->AngleToChara(m_curX,m_curY);
		m_curvingBullet.AddBullet(m_curX,m_curY,angle,32,9);
		m_pStage->m_bPlaySound[15]=true;
	}
	if (m_curPhaseFrame==128)
		return 1;
	return 0;
}

int CBossStage4::MaiShootType2_3__2_4()
{
	if (m_curPhaseFrame==48)
	{
		m_phase2MaiShootType2_3__2_4_baseAngle=32;
		m_phase2MaiShootType2_3__2_4_deltaAngle=-4;
	}
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=16;
		shootInfo.shootType=EST04_NWAY_ALLAROUND;
		shootInfo.shootAngle=m_phase2MaiShootType2_3__2_4_baseAngle;
		m_phase2MaiShootType2_3__2_4_baseAngle+=m_phase2MaiShootType2_3__2_4_deltaAngle;
		shootInfo.bulletImg=46;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY-8.0f*16.0f;
		shootInfo.nWay=4;
		shootInfo.bulletSpeed=38;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
		if (m_curPhaseFrame%16==0)
		{
			m_phase2MaiShootType2_3__2_4_baseAngle-=(float)(16+rand()%8);
			m_phase2MaiShootType2_3__2_4_deltaAngle=-m_phase2MaiShootType2_3__2_4_deltaAngle;
		}
	}
	if (m_curPhaseFrame!=256)
		return 0;
	return 1;
}

void CBossStage4::Phase2ShootMai()
{
	if (m_curPhaseFrame<48)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,8,9,m_curPhaseFrame-24);
		m_curImage=183;
		if (m_curPhaseFrame==32)
			m_pStage->m_bPlaySound[8]=true;
		return;
	}
	int res;
	if (m_nModeChange<10)
	{
		switch(m_curMode)
		{
		case 1:
			res=MaiShootType0_1();
			break;
		case 2:
			res=MaiShootType0_2__2_2();
		    break;
		case 3:
			res=MaiShootType0_3();
		    break;
		case 4:
			res=MaiShootType0_4();
			break;
		}
	}
	else
		if (m_nModeChange<15)
		{
			res=MaiShootType1();
		}
		else
		{
			switch(m_curMode)
			{
			case 1:
				res=MaiShootType2_1();
				break;
			case 2:
				res=MaiShootType0_2__2_2();
				break;
			case 3:
			case 4:
				res=MaiShootType2_3__2_4();
				break;
			}
		}
	if (res!=0)
	{
		m_curPhaseFrame=0;
		m_curMode=0;
		m_curImage=180;
		m_2curImage=180;
	}
	else
		m_curImage=184;
}

int CBossStage4::YukiShootType0_1__1_4()
{
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=16;
		shootInfo.shootType=EST03_NWAY_TO_CHARA;
		shootInfo.shootAngle=(float)(rand()%64-32);
		shootInfo.bulletImg=50;
		shootInfo.shootOrigX=m_2curX;
		shootInfo.shootOrigY=m_2curY-8.0f*16.0f;
		shootInfo.nWay=7;
		shootInfo.nWayUnitAngle=5;
		shootInfo.bulletMoveType=EBMT10_STRAIGHT;
		shootInfo.bulletSpeed=(float)(16+rand()%32);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true);
	}
	return 0;
}

int CBossStage4::YukiShootType0_2__2_4()
{
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=16;
		shootInfo.shootType=EST07_MULTI_TO_CHARA;
		shootInfo.shootAngle=0;
		shootInfo.bulletImg=50;
		shootInfo.shootOrigX=m_2curX;
		shootInfo.shootOrigY=m_2curY-8.0f*16.0f;
		shootInfo.nMulti=8;
		shootInfo.deltaSpeed=4;
		shootInfo.bulletSpeed=32;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
	return 0;
}

int CBossStage4::YukiShootType0_3__1_1()
{
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=16;
		shootInfo.shootType=EST04_NWAY_ALLAROUND;
		shootInfo.shootAngle=(float)(rand()%256);
		shootInfo.bulletImg=50;
		shootInfo.shootOrigX=m_2curX;
		shootInfo.shootOrigY=m_2curY-8.0f*16.0f;
		shootInfo.nWay=32;
		shootInfo.bulletSpeed=40;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
	return 0;
}

int CBossStage4::YukiShootType0_4()
{
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=16;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.shootAngle=64;
		shootInfo.bulletImg=50;
		shootInfo.shootOrigX=m_2curX;
		shootInfo.shootOrigY=m_2curY-8.0f*16.0f;
		shootInfo.nWay=8;
		shootInfo.nWayUnitAngle=12;
		shootInfo.bulletSpeed=64;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
	return 0;
}

int CBossStage4::YukiShootType1_2()
{
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=16;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.shootAngle=(float)(rand()%256);
		shootInfo.bulletImg=0;
		shootInfo.shootOrigX=m_2curX;
		shootInfo.shootOrigY=m_2curY-8.0f*16.0f;
		shootInfo.nWay=2;
		shootInfo.nWayUnitAngle=2;
		shootInfo.bulletSpeed=(float)(24+rand()%32);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
	return 0;
}

int CBossStage4::YukiShootType1_3()
{
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=16;
		shootInfo.shootType=EST07_MULTI_TO_CHARA;
		shootInfo.shootAngle=0;
		shootInfo.bulletImg=48;
		shootInfo.shootOrigX=m_2curX;
		shootInfo.shootOrigY=m_2curY-8.0f*16.0f;
		shootInfo.nMulti=8;
		shootInfo.deltaSpeed=6;
		shootInfo.bulletSpeed=32;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
	return 0;
}

int CBossStage4::YukiShootType2_1()
{
	if (m_curPhaseFrame==48||m_curPhaseFrame==96)
	{
		float angle=m_2curX<m_curX?64.0f:-64.0f;
		angle+=m_pStage->m_pChara->AngleToChara(m_2curX,m_2curY);
		m_curvingBullet.AddBullet(m_2curX,m_2curY,angle,32,11);
		m_pStage->m_bPlaySound[15]=true;
	}
	return 0;
}

int CBossStage4::YukiShootType2_2__2_3()
{
	if (m_curPhaseFrame==48)
	{
		m_phase2YukiShootType2_2__2_3_baseAngle=0;
		m_phase2YukiShootType2_2__2_3_deltaAngle=4;
	}
	if (m_curPhaseFrame%4==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.bulletBornType=16;
		shootInfo.shootType=EST04_NWAY_ALLAROUND;
		shootInfo.shootAngle=m_phase2YukiShootType2_2__2_3_baseAngle;
		m_phase2YukiShootType2_2__2_3_baseAngle+=m_phase2YukiShootType2_2__2_3_deltaAngle;
		shootInfo.bulletImg=50;
		shootInfo.shootOrigX=m_2curX;
		shootInfo.shootOrigY=m_2curY-8.0f*16.0f;
		shootInfo.nWay=4;
		shootInfo.bulletSpeed=38;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
		if (m_curPhaseFrame%16==0)
		{
			m_phase2YukiShootType2_2__2_3_baseAngle+=(float)(16+rand()%8);
			m_phase2YukiShootType2_2__2_3_deltaAngle=-m_phase2YukiShootType2_2__2_3_deltaAngle;
		}
	}
		return 0;
}

void CBossStage4::Phase2ShootYuki()
{
	if (m_curPhaseFrame<48)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_2curX,m_2curY,8,9,m_curPhaseFrame-24);
		m_curImage=183;
		return;
	}
	int res;
	if (m_nModeChange<10)
	{
		switch(m_curMode)
		{
		case 1:
			res=YukiShootType0_1__1_4();
			break;
		case 2:
			res=YukiShootType0_2__2_4();
		    break;
		case 3:
			res=YukiShootType0_3__1_1();
		    break;
		case 4:
			res=YukiShootType0_4();
			break;
		}
	}
	else
		if (m_nModeChange<15)
		{
			switch(m_curMode)
			{
			case 1:
				res=YukiShootType0_3__1_1();
				break;
			case 2:
				res=YukiShootType1_2();
				break;
			case 3:
				res=YukiShootType1_3();
				break;
			case 4:
				res=YukiShootType0_1__1_4();
				break;
			}
		}
		else
		{
			switch(m_curMode)
			{
			case 1:
				res=YukiShootType2_1();
				break;
			case 2:
			case 3:
				res=YukiShootType2_2__2_3();
				break;
			case 4:
				res=YukiShootType0_2__2_4();
				break;
			}
		}
	if (res==0)
		m_2curImage=184;
}

void CBossStage4::Phase2()
{
	bool bEndPhase=false;
	if (m_curMode==0)
	{
		int res;
		if (m_nModeChange==9||m_nModeChange==14)
		{
			if (m_curPhaseFrame==16)
			{
				m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
				m_pStage->m_bPlaySound[15]=true;
				AddExplodeEffect(1,m_curX,m_curY);
				AddExplodeEffect(2,m_2curX,m_2curY);
			}
			res=Move(m_curPhaseFrame-64);
		}
		else
			res=Move(m_curPhaseFrame-16);
		if (res!=0)
		{
			m_curPhaseFrame=0;
			m_nModeChange++;
			if (m_nModeChange>=36)
			{
				bEndPhase=true;
				m_whichDie=0;
				m_bSuccessDefeat=false;
			}
			else
			{
				m_curMode=m_nModeChange%4+1;
				m_2curMode=m_nModeChange%4+1;
			}
		}
	}
	else
	{
		Phase2ShootMai();
		Phase2ShootYuki();
	}
	if (bEndPhase==false)
	{
		if (m_curHP+m_2curHP<=5500)
		{
			if (m_nModeChange<9)
			{
				m_curMode=0;
				m_curPhaseFrame=0;
				m_nModeChange=9;
			}
			else
				if (m_curHP+m_2curHP<=2250)
					if (m_nModeChange<14)
					{
						m_curMode=0;
						m_curPhaseFrame=0;
						m_nModeChange=14;
					}
		}
		int res=0;
		if (GetDmgOnAndCrashChara())
			res=1;
		else
			if (GetDmgOnAndCrashChara2())
				res=2;
		if (res!=0)
		{
			bEndPhase=true;
			m_whichDie=res-1;
			m_bSuccessDefeat=true;
		}
	}
	if (bEndPhase)
	{
		m_bDrawCurvingBullet=false;
		m_curPhase=0xfd;
		m_curPhaseFrame=0;
		m_pStage->m_pLaser->RemoveLaser(0);
	}
}

void CBossStage4::Step()
{
	m_curPhaseFrame++;
	m_bHitThisFrame=false;
	m_b2HitThisFrame=false;
	m_pStage->m_bHomingEnemyExist=true;
	m_pStage->m_homingEnemyX=m_curX;
	m_pStage->m_homingEnemyY=m_curY;
	if (m_2curY>m_curY)
	{
		m_pStage->m_homingEnemyX=m_2curX;
		m_pStage->m_homingEnemyY=m_2curY;
	}
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
	default:
		DeathSequenceVerStage4();
		return;
	}
	m_curvingBullet.Step();
	StepHPGauge(m_curHP+m_2curHP,9000);
}

void CBossStage4::DrawBossImage(int imgIdx,float x,float y,bool bHit)
{
	if (imgIdx==184||imgIdx==200)
		imgIdx+=m_pStage->m_curFrame%8/2;
	C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
	if (bHit)
		m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImg,x,y);
	else
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,x,y);
}

void CBossStage4::DrawBoss()
{
	if (m_curPhase==0xfe)
	{
		if (m_whichDie==0)
		{
			C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
			DrawBossImage(m_2curImage+16,m_2curX,m_2curY,m_b2HitThisFrame);
		}
		else
		{
			C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_2curImage);
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_2curX,m_2curY);
			DrawBossImage(m_curImage,m_curX,m_curY,m_bHitThisFrame);
		}
	}
	else
	{
		if(m_curPhase<=0xfd||m_whichDie==1)
			DrawBossImage(m_curImage,m_curX,m_curY,m_bHitThisFrame);
		if(m_curPhase<=0xfd||m_whichDie==0)
			DrawBossImage(m_2curImage+16,m_2curX,m_2curY,m_b2HitThisFrame);
	}
}

void CBossStage4::DrawBackground()
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
			pImage->Draw((float)96+m_pStage->m_playAreaUpperLeftX-32,(float)72+m_pStage->m_playAreaUpperLeftY-16);
		}
		m_pStage->m_pStageRes->DrawPlayAreaMask(m_pStage->m_pStageRes->m_pStageBB->GetMaskPtr(m_curPhaseFrame/4),0.6f,0.6f,1);
		return;
	}
	m_pStage->m_pStageRes->ClearPlayArea(0,0,0);
	C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
	pImage->Draw((float)96+m_pStage->m_playAreaUpperLeftX-32,(float)72+m_pStage->m_playAreaUpperLeftY-16);
}

void CBossStage4::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(9000);
}

void CBossStage4::DrawSpecialBullet()
{
	if (m_bDrawCurvingBullet)
		m_curvingBullet.Draw();
}

}

































