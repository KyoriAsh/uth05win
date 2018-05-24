#include ".\boss.h"
#include "../Stage.h"
#include "../../Game.h"
#include "../../../gameLib/Graphic/Th5ExtFont.h"

namespace th5w{

CBoss::CBoss(void)
{
	m_curPhase=0;
	m_curMode=0;
	m_nModeChange=0;
	m_curPhaseFrame=0;
	m_velX=0;
	m_velY=0;
	m_bHitThisFrame=false;
	m_bTimeoutPhaseEnd=true;
	m_lastDrawnHP=0;
	m_forceGoUpOrDown=0;		//PC98 ver does not initialize this var, seems bug
	m_xLowerBound=32.0f*16.0f;
	m_xUpperBound=352.0f*16.0f;
	m_yLowerBound=24.0f*16.0f;
	m_yUpperBound=128.0f*16.0f;

	m_bDrawBoss=true;

	memset(m_explodeEffect,0,sizeof(m_explodeEffect));
}

CBoss::~CBoss(void)
{
}

void CBoss::Initialize()
{
}

void CBoss::StepHPGauge(int curHP,int fullHP)
{
	if (curHP>m_lastDrawnHP)
	{
		m_lastDrawnHP+=fullHP/128;
		if (m_lastDrawnHP>curHP)
			m_lastDrawnHP=curHP;
	}
	else
		m_lastDrawnHP=curHP;
}

void CBoss::DrawHPGaugeAndExtStrEnemy(int fullHP)
{
	const float gaugeColor[][3]={{1,0,0},{1,0,1},{0,1,1},{1,1,0},{1,1,1}};
	if (m_lastDrawnHP>0)
	{
		int colorIdx=m_lastDrawnHP*4/fullHP;
		m_pStage->m_pStageRes->DrawGauge(m_lastDrawnHP,fullHP,0x38*8,0x9*16+40,
							gaugeColor[colorIdx][0],gaugeColor[colorIdx][1],gaugeColor[colorIdx][2]);
		unsigned char extEnemy[]={0xea,0xeb,0xec,0};
		CTh5ExtFont::DrawExtString(extEnemy,100,0x3d*8,0x8*16+40,1,1,0);
	}
}

int CBoss::GetDmgAndTestCrash(float dmgAreaHalfWidth,float dmgAreaHalfHeight,int soundIdx)
{
	int dmg=m_pStage->GetDamageFromChara(m_curX,m_curY,dmgAreaHalfWidth,dmgAreaHalfHeight,true);
	if (dmg!=0)
		m_pStage->m_bPlaySound[soundIdx]=true;

	if (m_pStage->m_pChara->TestCrash(m_curX,m_curY,16.0f*16.0f,16.0f*16.0f))
		m_pStage->m_pChara->m_bMiss=true;

	return dmg;
}

void CBoss::GetDmgOffAndCrashChara()
{
	GetDmgAndTestCrash(m_bossDmgAreaHalfWidth,m_bossDmgAreaHalfHeight,10);
	m_bHitThisFrame=false;
	m_HPLostThisFrame=0;
}

int CBoss::GetDmgOnAndCrashChara()
{
	int dmg=GetDmgAndTestCrash(m_bossDmgAreaHalfWidth,m_bossDmgAreaHalfHeight,4);
	if (dmg!=0)
		m_bHitThisFrame=true;
	else
		m_bHitThisFrame=false;
	m_curHP-=dmg;
	m_HPLostThisFrame=dmg;
	if (m_curHP<=m_curPhaseEndHP)
		return 1;

	return 0;
}

void CBoss::AddDefeatScoreAndPopupNumbers(int n1000)
{
	m_pStage->AddScore(n1000*1000);
	for (int i=0;i<n1000;i++)
	{
		float drawX=m_curX-64.0f*16.0f+(float)(rand()%2048);
		float drawY=m_curY-64.0f*16.0f+(float)(rand()%2048);
		m_pStage->m_pPopupNumber->AddNumber(drawX,drawY,1000*10,
											m_pStage->m_pStageRes->m_palette[11*3]/255.0f,
											m_pStage->m_pStageRes->m_palette[11*3+1]/255.0f,
											m_pStage->m_pStageRes->m_palette[11*3+2]/255.0f);
	}
	m_bTimeoutPhaseEnd=false;				//it is not a good way to set this flag here, however, PC98 ver does so
}

int CBoss::RandomWalk(int frameIdx)
{
	if (frameIdx<0)
		return 0;

	if (frameIdx==0)
	{
		if (m_curX<144.0f*16.0f)
			m_randomWalkMovingAngle=(float)(rand()%96-48);
		else
			if (m_curX>240.0f*16.0f)
				m_randomWalkMovingAngle=(float)(rand()%96+80);	//PC98 ver chooses randomly from 48/80/112/144, seems bug
			else
				m_randomWalkMovingAngle=(float)(rand()%256);
		while (m_randomWalkMovingAngle>=256.0f)
			m_randomWalkMovingAngle-=256.0f;
		while (m_randomWalkMovingAngle<0.0f)
			m_randomWalkMovingAngle+=256.0f;
		if (m_forceGoUpOrDown==1&&m_randomWalkMovingAngle<128.0f)
			m_randomWalkMovingAngle=256.0f-m_randomWalkMovingAngle;
		if (m_forceGoUpOrDown==2&&m_randomWalkMovingAngle>128.0f)
			m_randomWalkMovingAngle=256.0f-m_randomWalkMovingAngle;
		m_forceGoUpOrDown=0;
	}
	m_velX=(float)cos(m_randomWalkMovingAngle/256.0f*2*PI)*(64-frameIdx*2);
	m_velY=(float)sin(m_randomWalkMovingAngle/256.0f*2*PI)*(64-frameIdx*2);
	m_curX+=m_velX;
	m_curY+=m_velY;
	if (m_velX<0)
		m_curImage=m_imgGoLeft;
	else
		m_curImage=m_imgGoRight;
	if (m_curY<m_yLowerBound)
	{
		m_curY=m_yLowerBound;
		m_forceGoUpOrDown=2;
	}
	if (m_curY>m_yUpperBound)
	{
		m_curY=m_yUpperBound;
		m_forceGoUpOrDown=1;
	}
	if (m_curX<m_xLowerBound)
		m_curX=m_xLowerBound;
	if (m_curX>m_xUpperBound)
		m_curX=m_xUpperBound;
	if (frameIdx<28)
		return 0;
	m_curImage=m_imgStay;
	return 1;
}

int CBoss::MoveTo(float tarX,float tarY)
{
	if (m_curX!=tarX||m_curY!=tarY)
	{
		float deltaX=(tarX-m_curX);
		if (fabs(deltaX)>=16.0f)
			m_curX+=deltaX/16.0f;
		else
			if (fabs(deltaX)>=4.0f)
				m_curX+=deltaX/4.0f;
			else
				m_curX=tarX;
		float deltaY=(tarY-m_curY);
		if (fabs(deltaY)>=16.0f)
			m_curY+=deltaY/16.0f;
		else
			if (fabs(deltaY)>=4.0f)
				m_curY+=deltaY/4.0f;
			else
				m_curY=tarY;
		if (deltaX>=0)
			m_curImage=m_imgGoRight;
		else
			m_curImage=m_imgGoLeft;
		return 0;
	}
	else
	{
		m_curImage=m_imgStay;
		return 1;
	}
	return 0;
}

void CBoss::DropItem()
{
	int setIdx=0;
	if (m_pStage->m_pChara->GetPower()>=128)
		setIdx=1;
	int itemSet[2][5]={{ITEM_POWER,ITEM_POWER,ITEM_BIGPOWER,ITEM_POWER,ITEM_POWER},
					   {ITEM_POINT,ITEM_POINT,ITEM_POINT,ITEM_POINT,ITEM_POINT}};
	for (int i=0;i<5;i++)
		m_pStage->m_pItem->AddItem(m_curX-64.0f*16.0f+(float)(rand()%2048),m_curY-64.0f*16.0f+(float)(rand()%2048),
								   itemSet[setIdx][i],0,-48);
}

void CBoss::AddExplodeEffect(int explodeEffectIdx,float x,float y)
{
	for (int i=0;i<EXPLODE_EFFECT_N_RECORD;i++)
		if (m_explodeEffect[i].flag==0)
		{
			m_explodeEffect[i].flag=1;
			m_explodeEffect[i].age=0;
			m_explodeEffect[i].ctrX=x;
			m_explodeEffect[i].ctrY=y;
			m_explodeEffect[i].xLen=8;
			m_explodeEffect[i].yLen=8;
			m_explodeEffect[i].deltaXLen=11.0f*16.0f;
			m_explodeEffect[i].deltaYLen=11.0f*16.0f;
			m_explodeEffect[i].angleOffset=0;
			switch(explodeEffectIdx)
			{
			case 1:
				m_explodeEffect[i].angleOffset=32;
				break;
			case 2:
				m_explodeEffect[i].angleOffset=-32;
				break;
			case 3:
				m_explodeEffect[i].deltaXLen=13.0f*16.0f;
				m_explodeEffect[i].deltaYLen=7.0f*16.0f;
			    break;
			case 4:
				m_explodeEffect[i].deltaXLen=7.0f*16.0f;
				m_explodeEffect[i].deltaYLen=13.0f*16.0f;
			    break;
			}
			break;
		}
	m_pStage->m_bPlaySound[15]=true;
}

void CBoss::StepExplodeEffect()
{
	for (int i=0;i<EXPLODE_EFFECT_N_RECORD;i++)
		if (m_explodeEffect[i].flag!=0)
		{
			m_explodeEffect[i].xLen+=m_explodeEffect[i].deltaXLen;
			m_explodeEffect[i].yLen+=m_explodeEffect[i].deltaYLen;
			m_explodeEffect[i].age++;
			if (m_explodeEffect[i].age>=32)
				m_explodeEffect[i].flag=0;
		}
}

void CBoss::DrawExplodeEffect()
{
	for (int i=0;i<EXPLODE_EFFECT_N_RECORD;i++)
		if (m_explodeEffect[i].flag!=0)
		{
			for (int j=0;j<64;j++)
			{
				float angle=(float)j*4;
				float drawX=m_explodeEffect[i].ctrX+(float)cos(angle/256.0f*2*PI)*m_explodeEffect[i].xLen;
				float drawY=m_explodeEffect[i].ctrY
							+(float)sin((angle+m_explodeEffect[i].angleOffset)/256.0f*2*PI)*m_explodeEffect[i].yLen;
				C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(164);
				m_pStage->m_pStageRes->DrawStageObjectImage(pImg,drawX,drawY);
			}
		}
}

void CBoss::EndPhase(int nextPhaseEndHP,int explodeEffectIdx)
{
	if (explodeEffectIdx!=0xffff)
	{
		AddExplodeEffect(explodeEffectIdx,m_curX,m_curY);
		if (m_bTimeoutPhaseEnd==false)
		{
			m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
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

void CBoss::Draw()
{
	if (m_bDrawBoss)
	{
		DrawBoss();
		DrawExplodeEffect();
		if (m_curPhase==0xfe)
		{
			if (m_curPhaseFrame<64)
			{
				for (int i=0;i<16;i++)
				{
					float drawX=m_deathExplodeCenterX+(float)cos(i*16/256.0f*2*PI)*(8+m_curPhaseFrame/2*192);
					float drawY=m_deathExplodeCenterY+(float)sin(i*16/256.0f*2*PI)*(8+m_curPhaseFrame/2*192);
					C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(3);
					m_pStage->m_pStageRes->DrawStageObjectImage(pImg,drawX,drawY);
				}
				if (m_curPhaseFrame/2<8)
					m_pStage->m_curScrFade=m_curPhaseFrame/2%2*50+100;
				else
					if (m_curPhaseFrame/2<16)
						m_pStage->m_curScrFade=196-m_curPhaseFrame/2*6;
					else
						m_pStage->m_curScrFade=100;
			}
		}
	}
}

void CBoss::DrawSpecialBullet()
{
}

void CBoss::DeathSequence(int n1000)
{
	if (m_curPhase==0xfd)
	{
		if (m_curPhaseFrame==1)
		{
			m_bHitThisFrame=false;
			AddExplodeEffect(0,m_curX,m_curY);
			m_pStage->m_bPlaySound[13]=true;
		}
		if (m_curPhaseFrame==16)
			AddExplodeEffect(4,m_curX,m_curY);
		if (m_curPhaseFrame==32)
		{
			m_deathExplodeCenterX=m_curX;
			m_deathExplodeCenterY=m_curY;
			m_curPhase++;
			m_pStage->m_pEnemyBullet->m_noEnemyBullet=m_bSuccessDefeat;
			if (m_bSuccessDefeat)
				AddDefeatScoreAndPopupNumbers(n1000);
			m_curImage=4;
			m_curPhaseFrame=0;
			m_pStage->m_pChara->m_invulnerableTimeLeft=255;
		}
		m_pStage->m_bHomingEnemyExist=false;
		return;
	}
	if (m_curPhase==0xfe)
	{
		if (m_curPhaseFrame%16==0)
		{
			m_curImage++;
			if (m_curImage>=12)
			{
				m_curPhase++;
				m_curPhaseFrame=0;
				m_pStage->m_pChara->m_bBombDisabled=true;
				m_bDrawBoss=false;
			}
		}
		m_pStage->m_bHomingEnemyExist=false;
		return;
	}
	if (m_curPhase==0xff)
	{
		m_pStage->m_bHomingEnemyExist=false;
		if (m_curPhaseFrame==1)
		{
			if (CGame::GVar().m_playStage!=5)
				m_pStage->m_pDialogue->Run();
			return;
		}
		if (m_curPhaseFrame==2)
			m_pStage->m_pSummary->Run();
		if (m_curPhaseFrame==416)				//stuck at frame 416 until all score has been added
			if (m_pStage->m_drawScore<m_pStage->m_curScore)
				m_curPhaseFrame--;
			else
			{
				m_pStage->m_pTextOverlay->SetMode(TEXT_OVERLAY_STAGE_FADE_OUT);
				m_pStage->m_pSummary->Stop();
			}
		if (m_curPhaseFrame==488)
			m_pStage->SetQuit(STAGE_END_CLEAR);
	}
}

}



























