#include ".\enemy.h"
#include "../Stage.h"
#include "../../Game.h"
#include "../Item/Item.h"
#include "../SparkEffect/SparkEffect.h"
#include "../Chara/Chara.h"
#include "../Background/Background.h"
#include <string.h>
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

//table obtained from PC98 ver data segment of main.exe
const unsigned char CEnemy::s_enemyDropTable[]={0,1,0,0,1,1,0,1,
												0,1,1,1,0,0,0,1,
												0,1,1,0,0,1,0,1,
												0,1,1,1,1,0,1,1,
												0,0,1,1,0,1,0,1,
												1,1,0,0,1,1,0,1,
												1,0,0,1,0,1,1,0,
												0,1,1,1,0,1,0,3};

CEnemy::CEnemy(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CEnemy::~CEnemy(void)
{
}

void CEnemy::Initialize()
{
	memset(m_enemy,0,sizeof(m_enemy));
	m_pEnemyTableReadPtr=m_pStage->m_pStageRes->m_pStageStd->m_pEnemyTable;
	m_enemyDropTableIdx=rand()%16;
	m_noAddEnemy=false;
}

void CEnemy::AddEnemy(unsigned char enemyType,unsigned char subType,float enemyX,float enemyY,unsigned char dropItemType)
{
	for (int i=0;i<ENEMY_N_RECORD;i++)
		if (m_enemy[i].flag==0)
		{
//			printf("add enemy\n");
			m_enemy[i].flag=3;
			m_enemy[i].age=0;
			m_enemy[i].accumulator=0;
			m_enemy[i].loopCounter=0;
			m_enemy[i].pScript=m_pStage->m_pStageRes->m_pStageStd->m_enemyScript[enemyType];
			m_enemy[i].curX=enemyX;
			m_enemy[i].curY=enemyY;
			if (m_enemy[i].curX>=192.0f*16.0f)
				m_enemy[i].atLeftHalfOfScreen=false;
			else
				m_enemy[i].atLeftHalfOfScreen=true;
			m_enemy[i].dropItemType=dropItemType;
			m_enemy[i].subType=subType;
			m_enemy[i].hitThisFrame=false;
			if (CGame::GVar().m_playDifficulty==3)
				m_enemy[i].bAutoShoot=true;
			else
				m_enemy[i].bAutoShoot=false;
			m_enemy[i].canLeave=0;
			m_enemy[i].nImage=1;
			m_enemy[i].imgChangeInterval=4;
			//m_enemy[i].idx30=0;
			m_enemy[i].bTakeDamage=false;
			m_enemy[i].bCrashChara=false;
			m_enemy[i].autoShootTimer=rand()%256;
			m_enemy[i].autoShootInterval=128;
			m_enemy[i].shootInfo.shootType=EST15_DEFAULT_SINGLE_TO_CHARA;
			m_enemy[i].shootInfo.bulletBornType=0;
			m_enemy[i].shootInfo.bulletSpeed=42.0f;
			m_enemy[i].shootInfo.shootOrigX=0.0f;
			m_enemy[i].shootInfo.shootOrigY=0.0f;
			m_enemy[i].shootInfo.bulletImg=0;
			break;
		}
}

void CEnemy::EnemyDie(ENEMY *pEnemy)
{
	pEnemy->flag=128;
	pEnemy->nImage=1;
	pEnemy->bTakeDamage=false;
	pEnemy->bCrashChara=false;
	pEnemy->velX=0;
	pEnemy->velY=0;
	if (pEnemy->dropItemType!=0xfe)				//0xfe for no drop
	{
		unsigned char dropItemType=pEnemy->dropItemType;
		m_enemyDropTableIdx++;
		if (pEnemy->dropItemType==0xff)
		{
			if (m_enemyDropTableIdx%2==0)
			{
				dropItemType=s_enemyDropTable[m_enemyDropTableIdx/2%64];
				m_pStage->m_pItem->AddItem(pEnemy->curX,pEnemy->curY,dropItemType,0,-48);
			}
		}
		else
			m_pStage->m_pItem->AddItem(pEnemy->curX,pEnemy->curY,dropItemType,0,-48);
	}
	m_pStage->m_bPlaySound[3]=true;
	m_pStage->AddScore(pEnemy->score);
	m_pStage->m_pSparkEffect->AddEffect(pEnemy->curX,pEnemy->curY,64,7,false);
}

void CEnemy::StepEnemyTable()
{
	int nextAppearFrame=m_pEnemyTableReadPtr[0]+m_pEnemyTableReadPtr[1]*256;
	if (nextAppearFrame==0)
		return;
//	printf("%d %d\n",nextAppearFrame,m_pStage->m_curFrame);
	if (nextAppearFrame==m_pStage->m_curFrame)
	{
		int nEnemy=m_pEnemyTableReadPtr[2];
		m_pEnemyTableReadPtr+=3;
		for (int i=0;i<nEnemy;i++)
		{
			unsigned char enemyType=m_pEnemyTableReadPtr[0];
			int xInt=*(short *)(m_pEnemyTableReadPtr+1);
			int yInt=*(short *)(m_pEnemyTableReadPtr+3);
//			printf("%d %d %d\n",enemyType,xInt,yInt);
			float x=(float)xInt;
			float y=(float)yInt;
			if (xInt==999*16) x=(float)(rand()%(384*16));
			if (yInt==999*16) y=(float)(rand()%(368*16));
			unsigned char dropItemType=m_pEnemyTableReadPtr[5];
			unsigned char enemySubType=m_pEnemyTableReadPtr[6];
			if (!m_noAddEnemy)
				AddEnemy(enemyType,enemySubType,x,y,dropItemType);
			m_pEnemyTableReadPtr+=8;
		}
	}
}

void CEnemy::StepEnemyScript(ENEMY *pEnemy)
{
#define VANISH_AND_RETURN {pEnemy->flag=2;return;}
#define TEST_LOOP(nFrame,curCmdLen)\
	{\
		if (pEnemy->accumulator>=nFrame)\
		{\
			ptr+=curCmdLen;\
			pEnemy->accumulator=0;\
		}\
		else\
			pEnemy->accumulator++;\
	}
	unsigned char *&ptr=pEnemy->pScript;
	while(1)
	{
//		if (pEnemy==&m_enemy[0])
//			printf("%d ggg %d\n",m_pStage->m_curFrame,*ptr);
		switch(*ptr)
		{
		case 0:
			VANISH_AND_RETURN;
		case 1:
			pEnemy->flag=1;
			pEnemy->imgIdx=ptr[1];
			pEnemy->life=ptr[2]+ptr[3]*256;
			pEnemy->score=ptr[4]+ptr[5]*256;
			pEnemy->bTakeDamage=true;
			pEnemy->bCrashChara=true;
			//printf("%d %d %d\n",pEnemy->imgIdx,pEnemy->life,pEnemy->score);
			ptr+=6;
			break;
		case 2:
			if (pEnemy->accumulator==0)
			{
				pEnemy->curAngle=ptr[1];
				pEnemy->curSpeed=ptr[2];
				UpdateVelXYFromSpeedAngle(pEnemy);
			}
			if (MoveAndTestLeave(pEnemy))
				VANISH_AND_RETURN;
			TEST_LOOP(ptr[3],4);
			return;
		case 3:
			if (pEnemy->accumulator==0)
				UpdateVelXYFromSpeedAngle(pEnemy);
			if (MoveAndTestLeave(pEnemy))
				VANISH_AND_RETURN;
			TEST_LOOP(ptr[1],2);
			return;
		case 4:
			if (pEnemy->accumulator==0)
			{
				pEnemy->curSpeed=ptr[1];
				UpdateVelXYFromSpeedAngle(pEnemy);
			}
			if (MoveAndTestLeave(pEnemy))
				VANISH_AND_RETURN;
			TEST_LOOP(ptr[2],3);
			return;
		case 5:
		case 6:
			if (pEnemy->accumulator==0)
			{
				pEnemy->curAngle=ptr[1];
				pEnemy->curSpeed=ptr[2];
				pEnemy->deltaAngle=ptr[3];
			}
			UpdateVelXYFromSpeedAngle(pEnemy);
			if (ptr[0]==6)
			{
				pEnemy->velX+=*(char *)(ptr+4);
				pEnemy->velY+=*(char *)(ptr+5);
			}
			if (MoveAndTestLeave(pEnemy))
				VANISH_AND_RETURN;
			pEnemy->curAngle+=pEnemy->deltaAngle;
			if (ptr[0]==5)
				TEST_LOOP(ptr[4],5)
			else
				TEST_LOOP(ptr[6],7)
			return;
		case 7:
		case 8:
			UpdateVelXYFromSpeedAngle(pEnemy);
			if (ptr[0]==8)
			{
				pEnemy->velX+=*(char *)(ptr+1);
				pEnemy->velY+=*(char *)(ptr+2);
			}
			if (MoveAndTestLeave(pEnemy))
				VANISH_AND_RETURN;
			pEnemy->curAngle+=pEnemy->deltaAngle;
			if (ptr[0]==7)
				TEST_LOOP(ptr[1],2)
			else
				TEST_LOOP(ptr[3],4)
			return;
		case 9:
			if (pEnemy->accumulator==0)
			{
				pEnemy->lastX=pEnemy->curX;
				pEnemy->lastY=pEnemy->curY;
			}
			TEST_LOOP(ptr[1],2);
			return;
		case 10:
			if (pEnemy->accumulator==0)
				pEnemy->velX=0;
			pEnemy->velY=(float)m_pStage->m_pBackground->GetLastScrollDeltaPixel()*16.0f;
			if (MoveAndTestLeave(pEnemy))
				VANISH_AND_RETURN;
			TEST_LOOP(ptr[1],2);
			return;
		case 11:
		case 12:
			if (pEnemy->accumulator==0)
			{
				pEnemy->curAngle=0;
				pEnemy->deltaAngle=ptr[2];
			}
			if (ptr[0]==11)
			{
				pEnemy->velX=(float)cos(pEnemy->curAngle/256.0f*2*PI)*ptr[1];
				pEnemy->velY=*(char *)(ptr+3);
			}
			else
			{
				pEnemy->velX=*(char *)(ptr+3);
				pEnemy->velY=(float)cos(pEnemy->curAngle/256.0f*2*PI)*ptr[1];
			}
			if (MoveAndTestLeave(pEnemy))
				VANISH_AND_RETURN;
			pEnemy->curAngle+=pEnemy->deltaAngle;
			TEST_LOOP(ptr[4],5);
			return;
		case 13:
			pEnemy->curSpeed=ptr[2];
			pEnemy->curAngle=m_pStage->m_pChara->AngleToChara(pEnemy->curX,pEnemy->curY)+ptr[1];
			UpdateVelXYFromSpeedAngle(pEnemy);
			ptr+=3;
			break;
		case 14:
			pEnemy->curAngle+=ptr[1];
			UpdateVelXYFromSpeedAngle(pEnemy);
			ptr+=2;
			break;
		case 15:
			pEnemy->curSpeed+=*(char *)(ptr+1);
			UpdateVelXYFromSpeedAngle(pEnemy);
			ptr+=2;
			break;
		case 16:
			pEnemy->curAngle=(float)(rand()%256);
			UpdateVelXYFromSpeedAngle(pEnemy);
			ptr+=1;
			break;
		case 17:
		case 57:
		case 55:
		{
			ENEMY_SHOOT_INFO shootInfo;
			memcpy(&shootInfo,&pEnemy->shootInfo,sizeof(ENEMY_SHOOT_INFO));
			shootInfo.shootOrigX+=pEnemy->curX;
			shootInfo.shootOrigY+=pEnemy->curY;
			m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
//			printf("%f %f\n",pEnemy->shootInfo.shootAngle,shootInfo.shootAngle);
			if (ptr[0]==17)
				m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
			else
				if (ptr[0]==57)
					m_pStage->m_pEnemyBullet->Shoot(&shootInfo,false,false);
				else
					m_pStage->m_pEnemyBullet->Shoot(&shootInfo,true,true);
			ptr+=1;
			break;
		}
		case 18:
			pEnemy->bAutoShoot=false;
			pEnemy->shootInfo.bulletBornType=ptr[1];
			pEnemy->shootInfo.shootType=ptr[2];
			pEnemy->shootInfo.shootAngle=ptr[3];
			pEnemy->shootInfo.bulletSpeed=ptr[4];
			pEnemy->shootInfo.bulletImg=ptr[5];
			ptr+=6;
			break;
		case 19:
			pEnemy->shootInfo.bulletBornType=ptr[1];
			ptr+=2;
			break;
		case 20:
			pEnemy->shootInfo.shootOrigX=*(short *)(ptr+1);
			pEnemy->shootInfo.shootOrigY=*(short *)(ptr+3);
			ptr+=5;
			break;
		case 21:
			pEnemy->shootInfo.shootAngle=ptr[1];
			ptr+=2;
			break;
		case 22:
		case 31:
			pEnemy->shootInfo.shootAngle+=ptr[1];
			ptr+=2;
			break;
		case 23:
			pEnemy->shootInfo.shootAngle=(float)(rand()%256);
			ptr++;
			break;
		case 24:
			pEnemy->shootInfo.bulletImg=ptr[1];
			ptr+=2;
			break;
		case 25:
			pEnemy->shootInfo.bulletSpeed=ptr[1];
			ptr+=2;
			break;
		case 26:
			pEnemy->shootInfo.bulletSpeed+=*(char *)(ptr+1);
			ptr+=2;
			break;
		case 27:
			pEnemy->shootInfo.shootType+=*(char *)(ptr+1);
			ptr+=2;
			break;
		case 28:
			{
				int newInterval=ptr[1]*(32-m_pStage->m_playerPerformance)/64+ptr[1];
				if (newInterval<16) newInterval=16;
				if (newInterval>255) newInterval=255;
				pEnemy->autoShootInterval=newInterval;
				ptr+=2;
				break;
			}
		case 29:
			pEnemy->bAutoShoot=true;
			ptr++;
			break;
		case 30:
			pEnemy->bAutoShoot=false;
			ptr++;
			break;
		case 32:
			pEnemy->shootInfo.nWay=ptr[1];
			pEnemy->shootInfo.nWayUnitAngle=ptr[2];
			ptr+=3;
			break;
		case 33:
			pEnemy->shootInfo.nMulti=ptr[1];
			pEnemy->shootInfo.deltaSpeed=ptr[2];
			ptr+=3;
			break;
		case 34:
			pEnemy->canLeave|=1;
			ptr++;
			break;
		case 35:
			pEnemy->canLeave|=0x10;
			ptr++;
			break;
		case 36:
			pEnemy->canLeave|=0x11;
			ptr++;
			break;
		case 37:
			pEnemy->nImage=ptr[1];
			pEnemy->imgChangeInterval=ptr[2];
			ptr+=3;
			break;
		case 38:
			m_pStage->m_bPlaySound[ptr[1]]=true;
			ptr+=2;
			break;
		case 39:
			pEnemy->imgIdx=ptr[1];
			ptr+=2;
			break;
		case 40:
			pEnemy->bTakeDamage=false;
			pEnemy->bAutoShoot=false;
			ptr++;
			break;
		case 41:
			pEnemy->bTakeDamage=true;
			pEnemy->bAutoShoot=CGame::GVar().m_playDifficulty==3?true:false;
			ptr++;
			break;
		case 42:
			pEnemy->bCrashChara=false;
			ptr++;
			break;
		case 43:
			pEnemy->bCrashChara=true;
			ptr++;
			break;
		case 44:
			pEnemy->lastX=pEnemy->curX;
			pEnemy->lastY=pEnemy->curY;
			pEnemy->curX=*(short *)(ptr+1);
			pEnemy->curY=*(short *)(ptr+3);
			TEST_LOOP(0,5);
			return;
		case 45:
			pEnemy->lastX=pEnemy->curX;
			pEnemy->lastY=pEnemy->curY;
			pEnemy->curX+=*(short *)(ptr+1);
			pEnemy->curY+=*(short *)(ptr+3);
			TEST_LOOP(0,5);
			return;
		case 46:
			pEnemy->imgIdx+=*(char *)(ptr+1);
			ptr+=2;
			break;
		case 47:
			pEnemy->curAngle=ptr[1];
			pEnemy->curSpeed=ptr[2];
			UpdateVelXYFromSpeedAngle(pEnemy);
			ptr+=3;
			break;
		case 48:
			pEnemy->curAngle=ptr[1];
			pEnemy->curSpeed=ptr[2];
			if (pEnemy->atLeftHalfOfScreen)
				pEnemy->curAngle=128.0f-pEnemy->curAngle;
			UpdateVelXYFromSpeedAngle(pEnemy);
			ptr+=3;
			break;
		case 49:
			pEnemy->curSpeed=ptr[1];
			UpdateVelXYFromSpeedAngle(pEnemy);
			ptr+=2;
			break;
		case 50:
		case 51:
			if (pEnemy->loopCounter<ptr[2])
			{
				pEnemy->loopCounter++;
				ptr-=ptr[1];
			}
			else
			{
				pEnemy->loopCounter=0;
				ptr+=3;
			}
			break;
		case 52:
			if (pEnemy->subType==ptr[1])
				ptr+=3;
			else
				ptr+=3+ptr[2];
			break;
		case 53:
			ptr-=ptr[1];
			break;
		case 54:
			if (MoveAndTestLeave(pEnemy))
				VANISH_AND_RETURN;
			TEST_LOOP(ptr[1],2);
			return;
		case 56:
			pEnemy->shootInfo.bulletMoveType=ptr[1];
			ptr+=2;
			break;
		case 58:
			pEnemy->shootInfo.shootAngle=m_pStage->m_pChara->AngleToChara(pEnemy->curX+pEnemy->shootInfo.shootOrigX,
																		  pEnemy->curY+pEnemy->shootInfo.shootOrigY);
			pEnemy->shootInfo.shootAngle+=*(char *)(ptr+1);
			ptr+=2;
			break;
		}
	}
#undef TEST_LOOP
#undef VANISH_AND_RETURN
}

void CEnemy::Step()
{
	m_pStage->m_bHomingEnemyExist=false;
	int nAliveEnemy=0;
//	printf("%d\n",m_pStage->m_curFrame);
	for (int i=0;i<ENEMY_N_RECORD;i++)
	{
		m_enemy[i].hitThisFrame=false;
		if (m_enemy[i].flag==0||m_enemy[i].flag==2)
		{
			m_enemy[i].flag=0;
			continue;
		}
		if (m_enemy[i].flag>=128)
		{
			m_enemy[i].lastX=m_enemy[i].curX;
			m_enemy[i].lastY=m_enemy[i].curY;
			m_enemy[i].curX+=m_enemy[i].velX;
			m_enemy[i].curY+=m_enemy[i].velY;
			m_enemy[i].flag++;
			m_enemy[i].imgIdx=(m_enemy[i].flag-128)/4+4;
			if (m_enemy[i].imgIdx>=12)
				m_enemy[i].flag=2;
			continue;
		}
		StepEnemyScript(&m_enemy[i]);
		//printf("\t%d %f %f %f %f\n",i,m_enemy[i].curX,m_enemy[i].curY,m_enemy[i].velX,m_enemy[i].velY);
		if (m_enemy[i].bCrashChara)
			if (m_pStage->m_pChara->TestCrash(m_enemy[i].curX,m_enemy[i].curY,12.0f*16.0f,12.0f*16.0f))
			{
				m_pStage->m_pChara->m_bMiss=true;
				EnemyDie(&m_enemy[i]);
				continue;
			}
		if (m_enemy[i].bTakeDamage)
			if (m_enemy[i].life!=0xffff)			//life of 0xffff for not targetable and invulnerable
				if (m_enemy[i].curX>=-16.0f*16.0f&&m_enemy[i].curX<=400.0f*16.0f)
					if (m_enemy[i].curY>=-16.0f*16.0f&&m_enemy[i].curY<=384.0f*16.0f)
					{
						nAliveEnemy++;
						if (m_pStage->m_bHomingEnemyExist==false||m_pStage->m_homingEnemyY<m_enemy[i].curY)
						{
							m_pStage->m_bHomingEnemyExist=true;
							m_pStage->m_homingEnemyX=m_enemy[i].curX;
							m_pStage->m_homingEnemyY=m_enemy[i].curY;
						}
						int dmg=m_pStage->GetDamageFromChara(m_enemy[i].curX,m_enemy[i].curY,16.0f*16.0f,12.0f*16.0f,false);
						if (dmg>0)
						{
							if (m_enemy[i].life==0xfffe)
								m_pStage->m_bPlaySound[10]=true;
							else
							{
								if (dmg>=m_enemy[i].life)
								{
									EnemyDie(&m_enemy[i]);
									continue;
								}
								else
								{
									m_enemy[i].life-=dmg;
									m_enemy[i].hitThisFrame=true;
								}
							}
						}
					}
		if (m_enemy[i].bAutoShoot)
		{
			m_enemy[i].autoShootTimer++;
			if (m_enemy[i].autoShootTimer>=m_enemy[i].autoShootInterval)
				if (m_enemy[i].curY<304.f*16.0f)
					if (m_pStage->m_pChara->TestCrash(m_enemy[i].curX,m_enemy[i].curY,48.0f*16.0f,48.0f*16.0f)==false)
					{
						m_enemy[i].autoShootTimer=0;
						ENEMY_SHOOT_INFO shootInfo;
						memcpy(&shootInfo,&m_enemy[i].shootInfo,sizeof(ENEMY_SHOOT_INFO));
						shootInfo.shootOrigX+=m_enemy[i].curX;
						shootInfo.shootOrigY+=m_enemy[i].curY;
						m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
						m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
					}
		}
		m_enemy[i].age++;
	}

	//printf("%d\n",nAliveEnemy);
	if (nAliveEnemy>0)
	{
		int frameInterval;
		if (nAliveEnemy<8)
			frameInterval=144-nAliveEnemy*8;
		else
			frameInterval=80;
		frameInterval-=CGame::GVar().m_playStage*16;
		if (frameInterval<4) frameInterval=4;
		if (m_pStage->m_curFrame%frameInterval==0)
			if (m_pStage->GetDream()<128)
				m_pStage->SetDream(m_pStage->GetDream()+1);
	}
}

void CEnemy::Draw()
{
	for (int i=0;i<ENEMY_N_RECORD;i++)
	{
		if (m_enemy[i].flag!=1&&m_enemy[i].flag<128)
			continue;
		if (m_enemy[i].lastY<=-16.0f*16.0f||m_enemy[i].lastY>=384.0f*16.0f)		//PC98 ver uses lastY, don't know why
			continue;
		if (m_enemy[i].curX<=-16.0f*16.0f||m_enemy[i].curX>=400.0f*16.0f)
			continue;
		if (m_enemy[i].curY<=-16.0f*16.0f||m_enemy[i].curY>=384.0f*16.0f)
			continue;

		unsigned char img=(m_enemy[i].age/m_enemy[i].imgChangeInterval);
		if (m_enemy[i].nImage==0)
			img=m_enemy[i].imgIdx;
		else
			img=m_enemy[i].imgIdx+img%m_enemy[i].nImage;
		if (m_enemy[i].hitThisFrame==false)
			m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(img),m_enemy[i].curX,m_enemy[i].curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(img),m_enemy[i].curX,m_enemy[i].curY);
	}
}

}



















