#include ".\bossstage1.h"
#include "../Stage.h"
#include "../../Game.h"

namespace th5w{

CBossStage1::CBossStage1(void)
{
}

CBossStage1::~CBossStage1(void)
{
}

void CBossStage1::Initialize()
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
	m_imgGoLeft=186;
	m_imgGoRight=184;
}

void CBossStage1::Phase0()
{
	if (m_curPhaseFrame==1)
	{
		m_curHP=4650;
		m_curPhaseEndHP=2550;
	}
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=224&&m_curPhaseFrame%8==0)
	{
		int idx=9;
		if (m_curPhaseFrame!=224)
			idx=8;
		float color[]={m_pStage->m_pStageRes->m_palette[idx*3]/255.0f,m_pStage->m_pStageRes->m_palette[idx*3+1]/255.0f,
					   m_pStage->m_pStageRes->m_palette[idx*3+2]/255.0f};
		m_pStage->m_pGatherEffect->AddEffect(m_curX,m_curY,5120,3,color[0],color[1],color[2],32);
	}
	if (m_curPhaseFrame>=256)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_pStage->m_bPlaySound[13]=true;
	}
}

void CBossStage1::Phase1()
{
	GetDmgOffAndCrashChara();
	if (m_curPhaseFrame>=32)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_curMode=0;
		m_nModeChange=0;
		m_lastMode=0xff;
		m_phase2NFrameBeforeRandomWalk=64;
	}
}

void CBossStage1::Phase2ShootType1()
{
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=2;
		shootInfo.bulletImg=44;
		shootInfo.shootType=EST00_SINGLE;
		shootInfo.shootAngle=(float)128-(m_curPhaseFrame-16)/2*10;
		shootInfo.bulletSpeed=24;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
}

void CBossStage1::Phase2ShootType2()
{
	if (m_curPhaseFrame%2==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=2;
		shootInfo.bulletImg=44;
		shootInfo.shootType=EST00_SINGLE;
		shootInfo.shootAngle=(float)(m_curPhaseFrame-16)/2*10;
		shootInfo.bulletSpeed=24;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
}

void CBossStage1::Phase2ShootType3()
{
	if (m_curPhaseFrame==32)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.bulletImg=48;
		shootInfo.shootType=EST09_NWAY_MULTI_TO_CHARA;
		shootInfo.shootAngle=0;
		shootInfo.nWay=5;
		shootInfo.nWayUnitAngle=16;
		shootInfo.nMulti=5;
		shootInfo.deltaSpeed=7;
		shootInfo.bulletSpeed=16;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[15]=1;
	}
}

void CBossStage1::Phase2ShootType4()
{
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.bulletImg=48;
		shootInfo.shootType=EST06_MULTI;
		shootInfo.shootAngle=m_randomWalkMovingAngle+128+8-(float)(m_curPhaseFrame-16)/8*8;
		shootInfo.nMulti=8;
		shootInfo.deltaSpeed=6;
		shootInfo.bulletSpeed=16;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[15]=1;
	}
}


void CBossStage1::Phase2Shoot()
{
	if (m_curPhaseFrame<16)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,8,9,m_curPhaseFrame-1);
		if (m_curPhaseFrame==1)
			m_pStage->m_bPlaySound[8]=true;
	}
	else
	{
		if (RandomWalk(m_curPhaseFrame-16)!=0)
		{
			m_curPhaseFrame=0;
			m_curMode=0;
		}
		switch(m_curMode)		//behavior here is a bit different than PC98 ver, because copying that will make
		{						//the code somewhat ugly
		case 1:
			Phase2ShootType1();
			break;
		case 2:
			Phase2ShootType2();
			break;
		case 3:
			Phase2ShootType3();
		    break;
		case 4:
			Phase2ShootType4();
		    break;
		}
	}
}

void CBossStage1::Phase2()
{
	if (m_curMode==0)
	{
		if (RandomWalk(m_curPhaseFrame-m_phase2NFrameBeforeRandomWalk)!=0)
		{
			m_curPhaseFrame=0;
			m_nModeChange++;
			if (m_nModeChange>=32)
			{
				EndPhase(450,1);
				m_phase3ShootNFrame=80;
				return;
			}
			m_curMode=rand()%4+1;
			while(m_curMode==m_lastMode)
				m_curMode=rand()%4+1;
			m_lastMode=m_curMode;
		}
	}
	else
	{
		Phase2Shoot();
		if (m_curPhaseFrame==0)
			if (m_phase2NFrameBeforeRandomWalk>12)
				m_phase2NFrameBeforeRandomWalk-=12;
	}
	if (GetDmgOnAndCrashChara()!=0)
	{
		AddDefeatScoreAndPopupNumbers(5);
		EndPhase(450,1);
		m_phase3ShootNFrame=80;
		return;
	}
}

void CBossStage1::Phase3ShootType1()
{
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=16;
		shootInfo.bulletImg=0;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.shootAngle=(float)(64+rand()%(m_curPhaseFrame*2)-m_curPhaseFrame);
		shootInfo.nWay=3;
		shootInfo.nWayUnitAngle=3;
		shootInfo.bulletMoveType=EBMT10_STRAIGHT;
		shootInfo.bulletSpeed=(float)(16+rand()%32);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true,true);

		shootInfo.bulletBornType=3;
		shootInfo.bulletImg=44;
		shootInfo.shootType=EST00_SINGLE;
		shootInfo.shootAngle=(float)(64+rand()%(m_curPhaseFrame*2)-m_curPhaseFrame);
		shootInfo.bulletSpeed=(float)(24+rand()%32);
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
}

void CBossStage1::Phase3ShootType2()
{
	if (m_curPhaseFrame%16==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=2;
		shootInfo.bulletImg=48;
		shootInfo.shootType=EST04_NWAY_ALLAROUND;
		shootInfo.shootAngle=(float)(rand()%256);
		shootInfo.bulletSpeed=32;
		shootInfo.nWay=12;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
}

void CBossStage1::Phase3ShootType3()
{
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=0;
		shootInfo.shootType=EST04_NWAY_ALLAROUND;
		shootInfo.nWay=3;
		shootInfo.bulletImg=44;
		shootInfo.shootAngle=(float)(56+(m_curPhaseFrame-32)/8*6);
		shootInfo.bulletSpeed=(float)(24+(m_curPhaseFrame-32)/8*4);
		if (m_curPhaseFrame%16==0)
			shootInfo.bulletSpeed/=2.0f;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
}

void CBossStage1::Phase3ShootType4()
{
	if (m_curPhaseFrame%8==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=0;
		shootInfo.shootType=EST04_NWAY_ALLAROUND;
		shootInfo.nWay=3;
		shootInfo.bulletImg=44;
		shootInfo.shootAngle=(float)(72-(m_curPhaseFrame-32)/8*6);
		shootInfo.bulletSpeed=(float)(24+(m_curPhaseFrame-32)/8*4);
		if (m_curPhaseFrame%16==0)
			shootInfo.bulletSpeed/=2.0f;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[3]=true;
	}
}

void CBossStage1::Phase3Shoot()
{
	if (m_curPhaseFrame<32)
	{
		m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,8,9,m_curPhaseFrame-16);
		if (m_curPhaseFrame==16)
		{
			m_pStage->m_bPlaySound[8]=true;
			m_curImage=188;
		}
	}
	else
	{
		if (m_curPhaseFrame<64)
			m_curImage++;
		else
			if (m_curPhaseFrame<96)
				m_curImage+=(m_curPhaseFrame%2==0?1:0);
			else
				if (m_curPhaseFrame<128)
					m_curImage+=(m_curPhaseFrame%4==0?1:0);
				else
					m_curImage+=(m_curPhaseFrame%8==0?1:0);
		if (m_curImage>=192)
			m_curImage=188;
		switch(m_curMode)
		{
		case 1:
			Phase3ShootType1();
			break;
		case 2:
			Phase3ShootType2();
			break;
		case 3:
			Phase3ShootType3();
		    break;
		case 4:
			Phase3ShootType4();
		    break;
		}
		if (m_curPhaseFrame>=m_phase3ShootNFrame)
		{
			m_curPhaseFrame=0;
			m_curMode=0;
			m_curImage=180;
		}
	}
}

void CBossStage1::Phase3()
{
	if (m_curMode==0)
	{
		if (RandomWalk(m_curPhaseFrame-16)!=0)
		{
			m_curPhaseFrame=0;
			m_nModeChange++;
			if (m_nModeChange>=24)
			{
				EndPhase(0,2);
				return;
			}
			m_curMode=rand()%4+1;
			while(m_curMode==m_lastMode)
				m_curMode=rand()%4+1;
			m_lastMode=m_curMode;
		}
	}
	else
	{
		Phase3Shoot();
		if (m_curPhaseFrame==0)
			if (m_phase3ShootNFrame<180)
				m_phase3ShootNFrame+=24;
	}
	if (GetDmgOnAndCrashChara()!=0)
	{
		AddDefeatScoreAndPopupNumbers(5);
		EndPhase(0,2);
		return;
	}
}

void CBossStage1::Phase4Shoot()
{
	if (m_curPhaseFrame==1)
		m_curImage=188;
	if (m_curPhaseFrame==32)
	{
		float color[]={m_pStage->m_pStageRes->m_palette[8*3]/255.0f,m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f,
					   m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f};
		m_pStage->m_pLaser->AddLaserFixed(0,m_curX,m_curY,color,8,224);
		m_pStage->m_pLaser->AddLaserFixed(1,m_curX,m_curY,color,8,160);
		return;
	}
	if (m_curPhaseFrame<32)
		return;
	if (m_curPhaseFrame==64)
	{
		m_pStage->m_pLaser->StartGrow(0);
		m_pStage->m_pLaser->StartGrow(1);
	}
	bool bChange=false;
	if (m_pStage->m_pLaser->LaserAngle(1)>=128)
		bChange=true;
	if (m_pStage->m_pLaser->LaserAngle(1)<128&&m_pStage->m_pLaser->LaserAngle(1)>=96&&m_curPhaseFrame%2==0)
		bChange=true;
	if (m_pStage->m_pLaser->LaserAngle(1)<96&&m_pStage->m_pLaser->LaserAngle(1)>=88&&m_curPhaseFrame%3==0)
		bChange=true;
	if (m_pStage->m_pLaser->LaserAngle(1)<88&&m_pStage->m_pLaser->LaserAngle(1)>=80&&m_curPhaseFrame%4==0)
		bChange=true;
	if (m_pStage->m_pLaser->LaserAngle(1)<80&&m_curPhaseFrame%5==0)
		bChange=true;
	if (bChange&&m_pStage->m_pLaser->LaserAngle(1)>74)
	{
		m_pStage->m_pLaser->LaserAngle(1)-=1;
		m_pStage->m_pLaser->LaserAngle(0)+=1;
	}
	if (m_pStage->m_curFrame%16==0)
	{
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=16;
		shootInfo.shootAngle=(float)(m_curPhaseFrame-48)/16*14;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.bulletMoveType=EBMT10_STRAIGHT;
		const int nWay[]={5,6,7,8};
		shootInfo.nWay=nWay[CGame::GVar().m_playDifficulty];
		shootInfo.nWayUnitAngle=1;
		shootInfo.bulletSpeed=32;
		shootInfo.bulletImg=0;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true);
		shootInfo.shootType=EST13_RANDOM_BASE_SPEED_32;
		shootInfo.bulletSpeed=16;
		shootInfo.nWay=1+(m_curPhaseFrame-1)/64;
		if (shootInfo.nWay>8)
			shootInfo.nWay=8;
		shootInfo.bulletImg=44;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
	}
}

void CBossStage1::Phase4()
{
	Phase4Shoot();
	if (m_curPhaseFrame>=1300)
		m_bSuccessDefeat=false;
	else
	{
		if (GetDmgOnAndCrashChara()!=0)
			m_bSuccessDefeat=true;
		else
			return;
	}
	m_pStage->m_pLaser->RemoveLaser(0);
	m_pStage->m_pLaser->RemoveLaser(1);
	m_curPhase=0xfd;
	m_curPhaseFrame=0;
}

void CBossStage1::Step()
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
	default:
		DeathSequence(10);
	    break;
	}
	StepHPGauge(m_curHP,4650);
}

void CBossStage1::DrawBoss()
{
	if (m_curPhase==0xfe)
	{
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
	else
	{
		int imgIdx;
		if (m_curImage==184||m_curImage==186)
			imgIdx=m_curImage+m_pStage->m_curFrame%8/4;
		else
			if (m_curImage==180)
				imgIdx=m_curImage+m_pStage->m_curFrame%16/4;
			else
				imgIdx=m_curImage;
		C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
		if (m_bHitThisFrame)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImg,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImg,m_curX,m_curY);
	}
}

void CBossStage1::DrawBackground()
{
	if (m_curPhase==0||m_curPhase>=0xfe)	//as from code of PC98 ver, should be m_curPhase>=0xff
	{										//but from replay videos, it behaves as if m_curPhase>=0xfe, don't know why
		m_pStage->m_pBackground->Draw();
		return;
	}
	if (m_curPhase==1)
	{
		m_pStage->m_pBackground->Draw();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
				   0,CGame::s_pCurGame->m_windowHeight);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE);
		glColor4f(0,0,0,1);

		unsigned char *pMask=m_pStage->m_pStageRes->m_pStageBB->GetMaskPtr(m_curPhaseFrame/2);
		for (int i=0;i<23;i++)
			for (int j=0;j<24;j++)
			{
				if ((pMask[i*4+j/8]&(1<<(7-j%8)))!=0)
					glColor4f(0,0,0,1);
				else
					glColor4f(0,0,0,0);
				float x,y;
				x=(float)(m_pStage->m_playAreaUpperLeftX+j*16);
				y=CGame::s_pCurGame->m_windowHeight-1-(float)(m_pStage->m_playAreaUpperLeftY+i*16);
				glBegin(GL_QUADS);
					glVertex2f(x,y-16);
					glVertex2f(x+16,y-16);
					glVertex2f(x+16,y);
					glVertex2f(x,y);
				glEnd();
			}

		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA);
		glColor3f(80/255.0f,0,80/255.0f);
		float xL=(float)m_pStage->m_playAreaUpperLeftX;
		float xH=(float)m_pStage->m_playAreaUpperLeftX+384;
		float yH=(float)CGame::s_pCurGame->m_windowHeight-1-m_pStage->m_playAreaUpperLeftY+1;
		float yL=(float)yH-368;
		glBegin(GL_QUADS);
			glVertex2f(xL,yL);
			glVertex2f(xH,yL);
			glVertex2f(xH,yH);
			glVertex2f(xL,yH);
		glEnd();

		C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,pImage->GetTexID());
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
		xL=(float)m_pStage->m_playAreaUpperLeftX+64-32;
		xH=xL+320;
		yH=(float)CGame::s_pCurGame->m_windowHeight-1-m_pStage->m_playAreaUpperLeftY+1;
		yL=(float)yH-192+1;
		glBegin(GL_QUADS);
			glTexCoord2d(0,0);
			glVertex3f(xL,yL,0);
			glTexCoord2d(320/512.0f,0);
			glVertex3f(xH,yL,0);
			glTexCoord2d(320/512.0f,192/256.0f);
			glVertex3f(xH,yH,0);
			glTexCoord2d(0,192/256.0f);
			glVertex3f(xL,yH,0);
		glEnd();
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		return;
	}
	C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(16);
	m_pStage->m_pStageRes->ClearPlayArea(85/255.0f,0,85/255.0f);
	pImage->Draw((float)64-32+m_pStage->m_playAreaUpperLeftX,(float)16-16+m_pStage->m_playAreaUpperLeftY);
}

void CBossStage1::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(4650);
}

}


















