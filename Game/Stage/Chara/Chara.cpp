#include ".\chara.h"
#include <stdio.h>
#include "../Stage.h"
#include "../EnemyBullet/EnemyBullet.h"
#include "../Item/Item.h"
#include "../FloatingText/FloatingText.h"
#include "../PopupNumber/PopupNumber.h"
#include "../CharaBullet/CharaBullet.h"
#include "../../../gameLib/CommonFunction/CommonFunctionInput.h"
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

CChara::CChara(void)
{
	m_pStage=NULL;
	m_deltaX=0;
	m_deltaY=0;
	m_bDraw=true;

	m_collisionAreaAlpha=0;
}

CChara::~CChara(void)
{
}

C2DImage* CChara::GetStageSprite(int spriteIdx)
{
	return m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(spriteIdx);
}

C2DImage* CChara::GetStagePattern(int patternIdx)
{
	return m_pStage->m_pStageRes->m_patternArray.GetImagePtr(patternIdx);
}

void CChara::MoveChara(unsigned short keyState)
{
	m_deltaX=m_deltaY=0;

	if (m_bCharaCanMove==false)
		return;

	char dirX=0,dirY=0;
	if (CCommonFunctionInput::UpKey(keyState)) dirY=-1;
	if (CCommonFunctionInput::DownKey(keyState)) dirY=1;
	if (CCommonFunctionInput::LeftKey(keyState)) dirX=-1;
	if (CCommonFunctionInput::RightKey(keyState)) dirX=1;

	float deltaX,deltaY;

	if (dirX*dirY!=0)		//diagonal move
	{
		deltaX=dirX*m_moveSpeedDiagonal;
		deltaY=dirY*m_moveSpeedDiagonal;
	}
	else
	{
		deltaX=dirX*m_moveSpeed;
		deltaY=dirY*m_moveSpeed;
	}
	if (CCommonFunctionInput::LShiftKey(keyState))
	{
		deltaX*=0.5;
		deltaY*=0.5;
		if (m_collisionAreaAlpha<100)
			m_collisionAreaAlpha+=15;
		if (m_collisionAreaAlpha>100)
			m_collisionAreaAlpha=100;
	}
	else
	{
		if (m_collisionAreaAlpha>0)
			m_collisionAreaAlpha-=15;
		if (m_collisionAreaAlpha<0)
			m_collisionAreaAlpha=0;
	}

	m_deltaX=deltaX;
	m_deltaY=deltaY;

	m_lastX=m_curX;
	m_lastY=m_curY;
	m_curX+=deltaX;
	m_curY+=deltaY;
	if (m_curX<m_moveAreaXLow) m_curX=m_moveAreaXLow;
	if (m_curX>m_moveAreaXHigh) m_curX=m_moveAreaXHigh;
	if (m_curY<m_moveAreaYLow) m_curY=m_moveAreaYLow;
	if (m_curY>m_moveAreaYHigh) m_curY=m_moveAreaYHigh;
}

void CChara::DropBomb()
{
	if (m_pStage->m_bInCharaBomb)
		return;
	if (m_pStage->m_nCurBomb==0)
		return;
	if (m_bBombDisabled)
		return;
	if (m_nextLifeComeTimeLeft!=0)
		if (m_nextLifeComeTimeLeft<=32)
			return;
		else
		{										//bomb on death
			m_nextLifeComeTimeLeft=0;
			m_cannotControlTimeLeft=0;			//let player control chara again, PC98 ver does not do so
			m_bMiss=false;
		}
	m_pStage->m_nCurBomb--;
	m_pStage->m_bInCharaBomb=true;
	m_bombTimer=0;
	m_invulnerableTimeLeft=m_bombInvulnerableTime;
	m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(m_bombClearEnemyBulletTime);
	m_pStage->m_bPlaySound[13]=true;
	m_pStage->m_bInCharaBombForItem=true;
	m_pStage->m_nBomb++;
	m_pStage->ChangePlayerPerformance(-1);
}

void CChara::DecreaseDreamInBomb()
{
	if (m_bombTimer%2==0||(m_pStage->m_bInBossBattle&&m_bombTimer%4==0))
	{
		if (m_pStage->GetDream()>1)
			m_pStage->SetDream(m_pStage->GetDream()-1);
	}
}

void CChara::OnMissDropItem()
{
	int bigPowerIdx=rand()%5;
	int curPosArea;
	if (m_curX<128.0f*16.0f)
		curPosArea=0;
	else
		if (m_curX<=256.0f*16.0f)
			curPosArea=1;
		else
			curPosArea=2;
	float velX[3][5]={{0,12,24,36,48},{-24,-12,0,12,24},{0,-12,-24,-36,-48}};
	float velY[3][5]={{-48,-56,-64,-72,-80},{-48,-56,-64,-56,-48},{-48,-56,-64,-72,-80}};
	for (int i=0;i<5;i++)
	{
		unsigned char itemType;
		if (i==bigPowerIdx)
			itemType=ITEM_BIGPOWER;
		else
			itemType=rand()%2;		//power or point
		if (m_pStage->m_nCurLife==1)
			itemType=ITEM_FULLPOWER;
		m_pStage->m_pItem->AddItem(m_curX,m_curY,itemType,velX[curPosArea][i],velY[curPosArea][i],false,false);
	}
}

void CChara::CaughtItem(float itemX,float itemY,unsigned char itemType)
{
	const int powerOverflowBonus[]={1,1,2,3,4,5,6,7,8,9,
									10,20,30,40,50,60,70,80,90,100,
									150,200,250,300,350,400,450,500,550,600,
									650,700,750,800,850,900,950,1000,1050,1100,
									1200,1250,1280};
	int popupNumberColor=0;
	int score;
	switch(itemType)
	{
	case ITEM_POWER:
		if (m_power<128)
		{
			SetPower(m_power+1);
			score=1;
		}
		else
		{
			m_pStage->m_powerOverflowLevel++;
			if (m_pStage->m_powerOverflowLevel>=42)
			{
				m_pStage->m_powerOverflowLevel=42;
				popupNumberColor=1;
				if (m_pStage->m_bInCharaBombForItem==0)
					if (m_pStage->GetDream()<128)
						m_pStage->SetDream(m_pStage->GetDream()+1);
			}
			score=powerOverflowBonus[m_pStage->m_powerOverflowLevel];
		}
		break;
	case ITEM_POINT:
		{
			int pointScorePenalty;
			int maxScore;
			if (m_pStage->GetDream()>=128)
			{
				pointScorePenalty=0;
				maxScore=m_pStage->m_pointItemMaxScore;
			}
			else
			{
				pointScorePenalty=((int)itemY)-m_pStage->GetDream()*24-896;
				maxScore=5120;
			}
			if (pointScorePenalty<=0)
			{
				m_getPointItemPerformance++;
				if (m_pStage->m_bInCharaBombForItem==false)
				{
					int deltaDream=6-((int)itemY)/1024;
					m_pStage->SetDream(m_pStage->GetDream()+deltaDream);
				}
				popupNumberColor=1;
			}
			else
				maxScore=2800-pointScorePenalty/2;
			score=maxScore;
			m_getPointItemPerformance++;
			m_pStage->m_totalNPointItem++;
			if (m_pStage->m_curNPointItem<999)
				m_pStage->m_curNPointItem++;
			
			//check extend
			if (m_pStage->m_totalNPointItem%100==0)
			{
				m_pStage->ChangePlayerPerformance(4);
				if (m_pStage->m_nCurLife<99)
				{
					m_pStage->m_nCurLife++;
					m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
					m_pStage->m_pFloatingText->SetTextType(FLOATING_TEXT_EXTEND);
					m_pStage->m_bPlaySound[7]=1;
				}
			}
			break;
		}
	case ITEM_DREAM:
		if (m_pStage->GetDream()<128)
		{
			score=1;
			m_pStage->SetDream(128);
		}
		else
		{
			score=12800;
			popupNumberColor=1;
		}
		m_getPointItemPerformance+=2;
		if (m_pStage->m_bInCharaBombForItem==false)
			m_getPointItemPerformance+=2;
	    break;
	case ITEM_BIGPOWER:
		if (m_power<128)
		{
			SetPower(m_power+10);
			score=1;
		}
		else
		{
			m_pStage->m_powerOverflowLevel+=5;
			if (m_pStage->m_powerOverflowLevel>=42)
			{
				m_pStage->m_powerOverflowLevel=42;
				popupNumberColor=1;
				score=2560;
			}
			else
				score=powerOverflowBonus[m_pStage->m_powerOverflowLevel];
		}
	    break;
	case ITEM_BOMB:
		m_pStage->m_nCurBomb++;
		score=100;
		break;
	case ITEM_1UP:													//no clear enemy bullet here, same as PC98 ver
		m_pStage->ChangePlayerPerformance(3);
		m_pStage->m_nCurLife++;										//no clamp to 99 here, which is the same as PC98 ver
		m_pStage->m_bPlaySound[7]=1;
		m_pStage->m_pFloatingText->SetTextType(FLOATING_TEXT_EXTEND);
		score=100;
		break;
	case ITEM_FULLPOWER:
		m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
		m_pStage->m_pFloatingText->SetTextType(FLOATING_TEXT_FULLPOWER);
		m_power=128;									//assign twice to avoid floating text "FullPower"
		SetPower(128);
		score=100;
		break;
	}
	m_pStage->AddScore(score);
	float color[3];
	color[0]=m_pStage->m_pStageRes->m_palette[(15-popupNumberColor*4)*3]/255.0f;		//0 for white, 1 for yellow
	color[1]=m_pStage->m_pStageRes->m_palette[(15-popupNumberColor*4)*3+1]/255.0f;
	color[2]=m_pStage->m_pStageRes->m_palette[(15-popupNumberColor*4)*3+2]/255.0f;
	m_pStage->m_pPopupNumber->AddNumber(itemX,itemY,score*10,color[0],color[1],color[2]);
	if (m_getPointItemPerformance>=32)
	{
		m_getPointItemPerformance-=32;
		m_pStage->ChangePlayerPerformance(1);
	}
}

void CChara::Step()
{
	if (m_invulnerableTimeLeft>0)
	{
		m_invulnerableTimeLeft--;
		m_bMiss=false;
	}
	else
	{
		if (m_bMiss)
		{
			m_nextLifeComeTimeLeft=40;
			m_bMiss=false;
			m_invulnerableTimeLeft=192;
			m_cannotControlTimeLeft=72;
			m_deltaX=0;
			m_deltaY=0;
		}
	}
	if (m_cannotControlTimeLeft!=0)
	{
		m_lastX=m_curX;m_lastY=m_curY;
		m_curX+=m_deltaX;m_curY+=m_deltaY;
		m_cannotControlTimeLeft--;
	}
	else
	{
		MoveChara(m_pStage->m_curGameKeyState);
		if (CCommonFunctionInput::ZKey(m_pStage->m_curGameKeyState))
		{
			if (m_charaShootTimer==0)
				m_charaShootTimer=18;
		}
		if (m_charaShootTimer>18)
			m_charaShootTimer=0;
		if (m_bCharaCanShoot&&m_charaShootTimer!=0)
		{
			if (m_charaShootTimer%6==0)
				m_pStage->m_bPlaySound[1]=true;
			ShootBullet();
			m_charaShootTimer--;
		}
	}
	if (CCommonFunctionInput::XPressed(m_pStage->m_curGameKeyState,m_pStage->m_lastGameKeyState))
		DropBomb();
	if (m_nextLifeComeTimeLeft!=0)
	{
		m_nextLifeComeTimeLeft--;
		if (m_nextLifeComeTimeLeft<=32)
		{
			m_collisionAreaAlpha=0;
			if (m_nextLifeComeTimeLeft==32)
			{
				m_deltaX=0;m_deltaY=0;
				m_pStage->m_powerOverflowLevel=0;
				m_missExplodeRadius=0.0f;
				m_missExplodeBaseAngle=(float)(rand()%256);
				OnMissDropItem();
				int powerDec=m_power/4;
				if (powerDec>16) powerDec=16;
				SetPower(m_power-powerDec);
				m_pStage->m_bPlaySound[2]=true;
				if (m_pStage->m_playerPerformance>38)
					m_pStage->m_playerPerformance=38;
				m_pStage->ChangePlayerPerformance(-4);
				m_pStage->m_nMiss++;
			}
			if (m_pStage->GetDream()<=2)
				m_pStage->SetDream(1);
			else
				if (m_pStage->m_bInBossBattle||m_pStage->m_curFrame%2!=0)
					m_pStage->SetDream(m_pStage->GetDream()-2);
			m_missExplodeRadius+=7.0f*16.0f;
			m_missExplodeBaseAngle+=8.0f;
			if (m_nextLifeComeTimeLeft<4)
				m_pStage->m_curScrFade=100+m_nextLifeComeTimeLeft%2*50;
			if (m_nextLifeComeTimeLeft==0)
			{
				m_curX=192.0f*16.0f;
				m_curY=368.0f*16.0f;
				m_lastX=m_curX;
				m_lastY=m_curY;
				m_deltaX=0;
				m_deltaY=-2.0f*16.0f;
				if (m_pStage->m_nCurLife<=1)
				{
					m_pStage->EnableContinueScreen();
					DisableDraw();
				}
				else
				{
					m_pStage->m_nCurLife--;
					m_pStage->m_nCurBomb=m_pStage->m_nBombOnMiss;
					m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(32);
				}
			}
		}
	}
}

void CChara::Draw()
{
	if (m_bDraw==false)
		return;

	if (m_nextLifeComeTimeLeft==0||m_nextLifeComeTimeLeft>32)
	{
		int imgIdx=0;
		if (m_deltaX>0)
			imgIdx=2;
		if (m_deltaX<0)
			imgIdx=1;

		if (m_invulnerableTimeLeft==0||m_pStage->m_curFrame%4!=0)
			m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx),m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx),m_curX,m_curY);
		if (m_powerLevel>=2)
		{
			m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(26),m_lastX-24.0f*16.0f,m_lastY);
			m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(26),m_lastX+24.0f*16.0f,m_lastY);
		}
	}
	else
	{
		if (m_nextLifeComeTimeLeft>1)
		{
			for (int i=0;i<4;i++)
			{
				float drawX,drawY;
				drawX=(float)cos((m_missExplodeBaseAngle+i*64.0f)/256.0f*2*PI)*m_missExplodeRadius+m_curX;
				drawY=(float)sin((m_missExplodeBaseAngle+i*64.0f)/256.0f*2*PI)*m_missExplodeRadius+m_curY;
				m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(3),drawX,drawY);
				drawX=(float)cos(-(m_missExplodeBaseAngle+i*64.0f)/256.0f*2*PI)*m_missExplodeRadius/2+m_curX;
				drawY=(float)sin(-(m_missExplodeBaseAngle+i*64.0f)/256.0f*2*PI)*m_missExplodeRadius/2+m_curY;
				m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(3),drawX,drawY);
			}
		}
	}
}

void CChara::DrawCollisionArea()
{
	if (m_bDraw==false)
		return;

	//unlike PC98 ver, we draw the collision area
	if (m_nextLifeComeTimeLeft==0||m_nextLifeComeTimeLeft>32)
	{
		float drawX=floor(m_curX/16.0f)*16.0f;
		float drawY=floor(m_curY/16.0f)*16.0f;
		m_pStage->m_pStageRes->DrawCircle(drawX,drawY,6.0f*16.0f,0.5f,0.25f,0.25f,true,(float)m_collisionAreaAlpha/100.0f*0.75f*0.3f);
		m_pStage->m_pStageRes->DrawCircle(drawX,drawY,5.0f*16.0f,0.5f,0.25f,0.25f,true,(float)m_collisionAreaAlpha/100.0f*0.75f*0.6f);
		m_pStage->m_pStageRes->DrawCircle(drawX,drawY,4.0f*16.0f,0.5f,0.25f,0.25f,true,(float)m_collisionAreaAlpha/100.0f*0.75f*0.9f);
		m_pStage->m_pStageRes->DrawCircle(drawX,drawY,3.0f*16.0f,1.0f,1.0f,1.0f,true,(float)m_collisionAreaAlpha/100.0f);
	}
}

bool CChara::TestCrash(float testX,float testY, float testHalfWidth,float testHalfHeight)
{
	if (testX>=m_curX-testHalfWidth&&testX<=m_curX+testHalfWidth)
		if (testY>=m_curY-testHalfHeight&&testY<=m_curY+testHalfHeight)
			return true;

	return false;
}

void CChara::ShootBullet()
{
}

void CChara::StepBomb()
{
}

void CChara::DrawBomb()
{
}

int CChara::GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss)
{
	return 0;
}

CCharaBullet* CChara::GetCharaBullet()
{
	return m_pStage->m_pCharaBullet;
}

void CChara::SetPower(int newPower)
{
	if (newPower>128) newPower=128;
	if (newPower<1) newPower=1;
	if (newPower==128&&m_power<=127)
	{
		m_pStage->m_pEnemyBullet->SetClearBulletTimeLeft(20);
		m_pStage->m_pFloatingText->SetTextType(FLOATING_TEXT_FULLPOWER);
	}
	m_power=newPower;
	int powerLevel[]={1,6,12,16,24,32,48,72,96,128};
	for (int i=0;i<10;i++)
		if (m_power>=powerLevel[i])
			m_powerLevel=i;
}


void CChara::ShootPowerLevel0Common()
{
	if (m_charaShootTimer%6==0)
		GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,10);
}

void CChara::ShootPowerLevel1Common()
{
	if (m_charaShootTimer%6==0)
		GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,188+(float)(rand()%8),10);
}

}









