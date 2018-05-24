#include ".\enemybullet.h"
#include "../Stage.h"
#include "../Chara/Chara.h"
#include "../FloatingText/FloatingText.h"
#include "../GatherEffect/GatherEffect.h"
#include "../SparkEffect/SparkEffect.h"
#include "../PopupNumber/PopupNumber.h"
#include "../Item/Item.h"
#include <math.h>
#include "../../Game.h"
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

CEnemyBullet::CEnemyBullet(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CEnemyBullet::~CEnemyBullet(void)
{
}

void CEnemyBullet::Initialize()
{
	memset(m_bullets,0,sizeof(m_bullets));
	m_noEnemyBullet=false;
	m_clearEnemyBulletTimeLeft=0;

	m_clipXLow=-128;
	m_clipXHigh=6272;
	m_clipYLow=-128;
	m_clipYHigh=6016;
	m_bounceXLow=0;
	m_bounceXHigh=6144;
	m_bounceYLow=0;
	m_bounceYHigh=5888;
	m_dotBulletShadowColor[0]=m_pStage->m_pStageRes->m_palette[9*3]/255.0f;
	m_dotBulletShadowColor[1]=m_pStage->m_pStageRes->m_palette[9*3+1]/255.0f;
	m_dotBulletShadowColor[2]=m_pStage->m_pStageRes->m_palette[9*3+2]/255.0f;

	m_bDropPointItemOnBonus=false;
}

void CEnemyBullet::TuneEnemyShootInfo(ENEMY_SHOOT_INFO *pInfo)
{
	if (pInfo->shootType==EST06_MULTI||pInfo->shootType==EST07_MULTI_TO_CHARA)
	{
		if (m_pStage->m_playerPerformance>=38)
			pInfo->nMulti++;
		if (m_pStage->m_playerPerformance<=24&&pInfo->nMulti>=2)
			pInfo->nMulti--;
	}
	switch(CGame::GVar().m_playDifficulty)
	{
	case 0:								//easy
		switch(pInfo->shootType)
		{
		case EST02_NWAY:
		case EST03_NWAY_TO_CHARA:
		case EST08_NWAY_MULTI:
		case EST09_NWAY_MULTI_TO_CHARA:
			if (pInfo->nWay>=3)
				pInfo->nWay-=2;
			break;
		case EST04_NWAY_ALLAROUND:
		case EST05_NWAY_ALLAROUND_TO_CHARA:
		case EST10_NWAY_ALLAROUND_MULTI:
		case EST11_NWAY_ALLAROUND_MULTI_TO_CHARA:
		case EST12_RANDOM:
		case EST13_RANDOM_BASE_SPEED_32:
			pInfo->nWay/=2;
			break;
		case EST06_MULTI:
		case EST07_MULTI_TO_CHARA:
			pInfo->deltaSpeed-=pInfo->deltaSpeed/4;
			if (pInfo->nMulti>=2)	//PC98 ver uses "if pInfo->nWayUnitAngle", which doen't make sense. should be bug
				pInfo->nMulti--;
		    break;
		}
		break;
	case 2:								//hard
		switch(pInfo->shootType)
		{
		case EST00_SINGLE:
		case EST01_SINGLE_TO_CHARA:
			pInfo->shootType+=6;					//becomes EST06_MULTI or EST07_MULTI_TO_CHARA
			pInfo->nMulti=2;
			pInfo->deltaSpeed=6;
			break;
		case EST04_NWAY_ALLAROUND:
		case EST05_NWAY_ALLAROUND_TO_CHARA:
		case EST10_NWAY_ALLAROUND_MULTI:
		case EST11_NWAY_ALLAROUND_MULTI_TO_CHARA:
			pInfo->nWay+=(pInfo->nWay/4>6)?6:(pInfo->nWay/4);
			break;
		case EST06_MULTI:
		case EST07_MULTI_TO_CHARA:
		case EST08_NWAY_MULTI:
		case EST09_NWAY_MULTI_TO_CHARA:
			pInfo->deltaSpeed+=pInfo->deltaSpeed/2;
		    break;
		case EST12_RANDOM:
		case EST13_RANDOM_BASE_SPEED_32:
			pInfo->nWay+=pInfo->nWay/2;
		    break;
		}
		break;
	case 3:								//lunatic
		switch(pInfo->shootType)
		{
		case EST00_SINGLE:
		case EST01_SINGLE_TO_CHARA:
			pInfo->shootType+=2;					//becomes EST02_NWAY or EST03_NWAY_TO_CHARA
			pInfo->nWay=3;
			pInfo->nWayUnitAngle=6;
			break;
		case EST02_NWAY:
		case EST03_NWAY_TO_CHARA:
		case EST08_NWAY_MULTI:
		case EST09_NWAY_MULTI_TO_CHARA:
			pInfo->nWay+=2;
			break;
		case EST04_NWAY_ALLAROUND:
		case EST05_NWAY_ALLAROUND_TO_CHARA:
		case EST10_NWAY_ALLAROUND_MULTI:
		case EST11_NWAY_ALLAROUND_MULTI_TO_CHARA:
			pInfo->nWay+=(pInfo->nWay/3>12)?12:(pInfo->nWay/3);
			break;
		case EST06_MULTI:
		case EST07_MULTI_TO_CHARA:
			pInfo->deltaSpeed+=pInfo->deltaSpeed/2;
			pInfo->nMulti++;
		    break;
		case EST12_RANDOM:
		case EST13_RANDOM_BASE_SPEED_32:
			pInfo->nWay*=2;
		    break;
		}
	    break;
	}
}

int CEnemyBullet::GetNRecordNeeded(ENEMY_SHOOT_INFO *shootInfo)
{
	switch(shootInfo->shootType)
	{
	case EST00_SINGLE:
	case EST01_SINGLE_TO_CHARA:
	case EST14_DEFAULT_SINGLE:
	case EST15_DEFAULT_SINGLE_TO_CHARA:
		return 1;
		break;
	case EST02_NWAY:
	case EST03_NWAY_TO_CHARA:
	case EST04_NWAY_ALLAROUND:
	case EST05_NWAY_ALLAROUND_TO_CHARA:
	case EST12_RANDOM:
	case EST13_RANDOM_BASE_SPEED_32:
		return shootInfo->nWay;
		break;
	case EST06_MULTI:
	case EST07_MULTI_TO_CHARA:
		return shootInfo->nMulti;
	    break;
	case EST08_NWAY_MULTI:
	case EST09_NWAY_MULTI_TO_CHARA:
	case EST10_NWAY_ALLAROUND_MULTI:
	case EST11_NWAY_ALLAROUND_MULTI_TO_CHARA:
		return shootInfo->nWay*shootInfo->nMulti;
	    break;
	}

	return 0;
}

int CEnemyBullet::FindFreeRecord(int *retIdxArray,int startIdx,int nIdx,int nNeeded)
{
	int nFound=0;
	for (int i=startIdx;i<startIdx+nIdx;i++)
		if (m_bullets[i].flag==0)
		{
			retIdxArray[nFound++]=i;
			if (nFound==nNeeded)
				return nNeeded;
		}

	retIdxArray[nFound]=-1;

	return nFound;
}

void CEnemyBullet::FillRecordSpeedAndAngle(int *freeRecIdx,ENEMY_SHOOT_INFO *shootInfo)
{
	float angleToChara=m_pStage->m_pChara->AngleToChara(shootInfo->shootOrigX,shootInfo->shootOrigY);

	bool bRandSpeed=false;

	float baseAngle=0.0f;
	float deltaAngle=0.0f;
	int nWay=1;
	int nMulti=1;

	switch(shootInfo->shootType)
	{
	case EST01_SINGLE_TO_CHARA:
	case EST15_DEFAULT_SINGLE_TO_CHARA:
		baseAngle=angleToChara;
	case EST00_SINGLE:
	case EST14_DEFAULT_SINGLE:
		break;
	case EST03_NWAY_TO_CHARA:
		baseAngle=angleToChara;
	case EST02_NWAY:
		baseAngle-=(shootInfo->nWay-1)/2.0f*shootInfo->nWayUnitAngle;
		deltaAngle=shootInfo->nWayUnitAngle;
		nWay=shootInfo->nWay;
		break;
	case EST05_NWAY_ALLAROUND_TO_CHARA:
		baseAngle=angleToChara;
	case EST04_NWAY_ALLAROUND:
		deltaAngle=256.0f/shootInfo->nWay;
		nWay=shootInfo->nWay;
		break;
	case EST07_MULTI_TO_CHARA:
		baseAngle=angleToChara;
	case EST06_MULTI:
		nMulti=shootInfo->nMulti;
		break;
	case EST09_NWAY_MULTI_TO_CHARA:
		baseAngle=angleToChara;
	case EST08_NWAY_MULTI:
		baseAngle-=(shootInfo->nWay-1)/2.0f*shootInfo->nWayUnitAngle;
		deltaAngle=shootInfo->nWayUnitAngle;
		nWay=shootInfo->nWay;
		nMulti=shootInfo->nMulti;
		break;
	case EST11_NWAY_ALLAROUND_MULTI_TO_CHARA:
		baseAngle=angleToChara;
	case EST10_NWAY_ALLAROUND_MULTI:
		deltaAngle=256.0f/shootInfo->nWay;
		nWay=shootInfo->nWay;
		nMulti=shootInfo->nMulti;
		break;
	case EST13_RANDOM_BASE_SPEED_32:
		bRandSpeed=true;
	case EST12_RANDOM:
		for (int i=0;i<shootInfo->nWay;i++)
		{
			if (freeRecIdx[i]==-1)
				return;
			m_bullets[freeRecIdx[i]].curAngle=(float)(rand()%256);
			m_bullets[freeRecIdx[i]].curSpeed=shootInfo->bulletSpeed+(bRandSpeed?(float)(rand()%32):0);
		}
		return;
	}

	baseAngle+=shootInfo->shootAngle;
	int curIdx=0;
	for (int i=0;i<nWay;i++)
		for (int j=0;j<nMulti;j++)
		{
			if (freeRecIdx[curIdx]==-1)
				return;
			m_bullets[freeRecIdx[curIdx]].curAngle=baseAngle+deltaAngle*i;
			m_bullets[freeRecIdx[curIdx]].curSpeed=shootInfo->bulletSpeed+shootInfo->deltaSpeed*j;
			curIdx++;
		}
}

unsigned char CEnemyBullet::AdjustBulletImageBasedOnAngle(unsigned char curImgIdx, float angle)
{
	if (curImgIdx<52)
		return curImgIdx;

	unsigned char baseIdx;
	if (curImgIdx<68)
		baseIdx=52;
	else
		if (curImgIdx<84)
			baseIdx=68;
		else
			if (curImgIdx<116)
				baseIdx=84;
			else
				baseIdx=116;
	if (curImgIdx>=180)
		baseIdx=curImgIdx;
    int discreteAngle=(((int)(angle+4-0.001))%256+256)%256;		//PC98 ver uses (angle+3)
																//as we use float here, 4-inf is preferred
																//4 or 4+inf will cause a different result with PC98 ver
	if (baseIdx<84)
		discreteAngle%=128;
	return baseIdx+discreteAngle/8;
}

void CEnemyBullet::Shoot(ENEMY_SHOOT_INFO *shootInfo,bool bSpecialBullet,bool bAdjustSpeed)
{
	if (m_noEnemyBullet==true)
		return;

	if (shootInfo->bulletBornType%16==1)
	{
		unsigned char bulletBornType=shootInfo->bulletBornType;
		shootInfo->bulletBornType--;
		if (bSpecialBullet)
			shootInfo->bulletBornType=0xfe;		//special value for special bullet type
		m_pStage->m_pGatherEffect->AddEffect(shootInfo->shootOrigX,shootInfo->shootOrigY,1024,2,
											 m_pStage->m_pStageRes->m_palette[9*3]/255.0f,
											 m_pStage->m_pStageRes->m_palette[9*3+1]/255.0f,
											 m_pStage->m_pStageRes->m_palette[9*3+2]/255.0f,
											 8,shootInfo);
		shootInfo->bulletBornType=bulletBornType;
		return;
	}

	//backup incoming bullet speed to restore its value when return
	//the original code in PC98 ver also backup shootAngle but as we pass these value as
	//function parameters when adding single bullets, hence only bulletSpeed will be changed when adjusting speed
	//during precheck
	float bulletSpeedBackup=shootInfo->bulletSpeed;

	//pre shoot check
	if (m_clearEnemyBulletTimeLeft<17&&m_clearEnemyBulletTimeLeft!=0)
		return;
	if (shootInfo->shootOrigX<=m_clipXLow||shootInfo->shootOrigX>=m_clipXHigh||
		shootInfo->shootOrigY<=m_clipYLow||shootInfo->shootOrigY>=m_clipYHigh)
		return;
	if (m_pStage->m_pChara->TestCrash(shootInfo->shootOrigX,shootInfo->shootOrigY,4.0f*16.0f,4.0f*16.0f))
	{
		m_pStage->m_pChara->m_bMiss=true;
	}
	//adjust speed
	if (bAdjustSpeed)
		shootInfo->bulletSpeed=m_pStage->AdjustSpeedBasedOnPlayerPerformance(shootInfo->bulletSpeed);

	//dot bullets and imaged bullets are put in different ranges
	int startRec,nRec;
	if (shootInfo->bulletImg!=0)
	{
		startRec=ENEMY_BULLET_N_RECORD_DOT;
		nRec=ENEMY_BULLET_N_RECORD_IMAGED;
	}
	else
	{
		startRec=0;
		nRec=ENEMY_BULLET_N_RECORD_DOT;
	}

	unsigned char bornStage=0;
	if (shootInfo->bulletBornType%16==3)
		bornStage=3;
	if (shootInfo->bulletBornType%16==2)
		bornStage=4;
	unsigned char speedFadeIn=2;
	if (bSpecialBullet==false)
		if (shootInfo->bulletSpeed<64||m_clearEnemyBulletTimeLeft!=0)
			if (shootInfo->bulletBornType/16==0)
				speedFadeIn=0;

	int nRecordNeeded=GetNRecordNeeded(shootInfo);
	int freeRecIdx[ENEMY_BULLET_N_RECORD];
	FindFreeRecord(freeRecIdx,startRec,nRec,nRecordNeeded);

	for (int i=0;i<nRecordNeeded;i++)
	{
		if (freeRecIdx[i]==-1)
			break;
		m_bullets[freeRecIdx[i]].flag=1;
		m_bullets[freeRecIdx[i]].bornStageAndGrazeFlag=bornStage;
		m_bullets[freeRecIdx[i]].curX=shootInfo->shootOrigX;
		m_bullets[freeRecIdx[i]].curY=shootInfo->shootOrigY;
		if (bSpecialBullet)
		{
			m_bullets[freeRecIdx[i]].origX=shootInfo->shootOrigX;
			m_bullets[freeRecIdx[i]].origY=shootInfo->shootOrigY;
			m_bullets[freeRecIdx[i]].dieStageAndMoveType=1;
			m_bullets[freeRecIdx[i]].totalDistanceTravelled=0;
			m_bullets[freeRecIdx[i]].accumulator=0;
			m_bullets[freeRecIdx[i]].delta=shootInfo->specialBulletDelta;
			m_bullets[freeRecIdx[i]].specialMoveType=shootInfo->bulletMoveType;
			m_bullets[freeRecIdx[i]].specialMoveVar=shootInfo->specialMoveVar;
		}
		else
		{
			m_bullets[freeRecIdx[i]].dieStageAndMoveType=speedFadeIn;
			m_bullets[freeRecIdx[i]].accumulator=32;
			m_bullets[freeRecIdx[i]].delta=72.0f-shootInfo->bulletSpeed;
		}
		m_bullets[freeRecIdx[i]].age=0;
		m_bullets[freeRecIdx[i]].bulletType=shootInfo->shootType;
	}

	//calculate speed and angle of all bullets that are shot
	FillRecordSpeedAndAngle(freeRecIdx,shootInfo);

	//update velX,velY and image based on speed and angle
	for (int i=0;i<nRecordNeeded;i++)
	{
		if (freeRecIdx[i]==-1)
			break;
		m_bullets[freeRecIdx[i]].origSpeed=m_bullets[freeRecIdx[i]].curSpeed;
		UpdateVelXYFromSpeedAngle(freeRecIdx[i]);
		m_bullets[freeRecIdx[i]].imgIdx=shootInfo->bulletImg;
		if (m_bullets[freeRecIdx[i]].imgIdx>=52)
			m_bullets[freeRecIdx[i]].imgIdx=AdjustBulletImageBasedOnAngle(m_bullets[freeRecIdx[i]].imgIdx,m_bullets[freeRecIdx[i]].curAngle);
	}

	shootInfo->bulletSpeed=bulletSpeedBackup;
}

void CEnemyBullet::SpecialMove(int recordIdx)
{
	ENEMY_BULLET &pBul=m_bullets[recordIdx];

	bool bTurnToChara=false;
	bool bBounceXLow=false,bBounceYLow=false,bBounceXHigh=false,bBounceYHigh=false;
	switch(pBul.specialMoveType)
	{
	case EBMT01_SLOW_DOWN_THEN_TURN_TO_CHARA:
		bTurnToChara=true;
	case EBMT02_SLOW_DOWN_THEN_TURN:
		if (pBul.curSpeed<=0)
		{
			pBul.accumulator++;
			if (bTurnToChara)
				pBul.curAngle=m_pStage->m_pChara->AngleToChara(pBul.curX,pBul.curY);
			else
				pBul.curAngle+=pBul.delta;
			pBul.imgIdx=AdjustBulletImageBasedOnAngle(pBul.imgIdx,pBul.curAngle);
			pBul.curSpeed=pBul.origSpeed;
			if (pBul.accumulator>=pBul.specialMoveVar)
				pBul.dieStageAndMoveType=2;
			UpdateVelXYFromSpeedAngle(recordIdx);
		}
		else
		{
			UpdateVelXYFromSpeedAngle(recordIdx);
			pBul.curSpeed-=1;
		}
		return;
	case EBMT03_SPEED_UP:
		pBul.curSpeed+=pBul.specialMoveVar;
		UpdateVelXYFromSpeedAngle(recordIdx);
		return;
	case EBMT04_SLOW_DOWN_AND_TURN:
		if (pBul.curSpeed<=0)
		{
			pBul.curAngle=pBul.delta;
			pBul.curSpeed=pBul.origSpeed;
			pBul.dieStageAndMoveType=2;
			UpdateVelXYFromSpeedAngle(recordIdx);
			pBul.imgIdx=AdjustBulletImageBasedOnAngle(pBul.imgIdx,pBul.curAngle);
		}
		else
		{
			UpdateVelXYFromSpeedAngle(recordIdx);
			pBul.imgIdx=AdjustBulletImageBasedOnAngle(pBul.imgIdx,pBul.curAngle);
			pBul.curSpeed-=2;
			if (pBul.curSpeed<0) pBul.curSpeed=0;
			if (pBul.curSpeed<32)
			{
				float deltaAngle=pBul.delta-pBul.curAngle;
				while(deltaAngle>128) deltaAngle-=256;
				while(deltaAngle<-128) deltaAngle+=256;
				pBul.curAngle+=deltaAngle/4;
			}
		}
		return;
	case EBMT05_BOUNCE_X:
		bBounceXLow=bBounceXHigh=true;
	    break;
	case EBMT06_BOUNCE_Y:
		bBounceYLow=bBounceYHigh=true;
		break;
	case EBMT07_BOUNCE_X_Y:
		bBounceXLow=bBounceXHigh=bBounceYLow=bBounceYHigh=true;
		break;
	case EBMT08_BOUNCE_X_YTOP:
		bBounceXLow=bBounceXHigh=bBounceYLow=true;
	    break;
	case EBMT09_SPEED_UP_VERTICAL:
		if (m_pStage->m_curFrame%2==0)
		{
			pBul.velY+=pBul.specialMoveVar;
			pBul.curAngle=(float)(atan2(pBul.velY,pBul.velX)/2.0/PI*256);
			pBul.imgIdx=AdjustBulletImageBasedOnAngle(pBul.imgIdx,pBul.curAngle);
		}
	    return;
	case EBMT10_STRAIGHT:		//simply move in straight line. existed in PC98 ver just because
	    return;									//it used integer coordinates and speed, which is not precise
	}
	if ((pBul.curX<=m_bounceXLow&&bBounceXLow==true)||(pBul.curX>=m_bounceXHigh&&bBounceXHigh==true))
	{
		pBul.curAngle=128.0f-pBul.curAngle;
		pBul.imgIdx=AdjustBulletImageBasedOnAngle(pBul.imgIdx,pBul.curAngle);
		UpdateVelXYFromSpeedAngle(recordIdx);
		pBul.accumulator++;
		if (pBul.accumulator>=pBul.specialMoveVar)
			pBul.dieStageAndMoveType=2;
	}
	if ((pBul.curY<=m_bounceYLow&&bBounceYLow==true)||(pBul.curY>=m_bounceYHigh&&bBounceYHigh==true))
	{
		pBul.curAngle=-pBul.curAngle;
		pBul.imgIdx=AdjustBulletImageBasedOnAngle(pBul.imgIdx,pBul.curAngle);
		UpdateVelXYFromSpeedAngle(recordIdx);
		pBul.accumulator++;
		if (pBul.accumulator>=pBul.specialMoveVar)
			pBul.dieStageAndMoveType=2;
	}
}

void CEnemyBullet::Step()
{
	int nAvailBullet=0;

	if (m_noEnemyBullet==false)
	{
		for (int i=0;i<ENEMY_BULLET_N_RECORD;i++)
		{
			if (m_bullets[i].flag==0||m_bullets[i].flag==2)
			{
				m_bullets[i].flag=0;
				continue;
			}
			nAvailBullet++;
			if (m_clearEnemyBulletTimeLeft!=0)						//is clearing all bullets
			{
				if (m_bullets[i].dieStageAndMoveType>=4)			//bullet in die stage
				{
					m_bullets[i].dieStageAndMoveType++;
					if (m_bullets[i].dieStageAndMoveType>=20)		//when reach 20 ,remove this bullet
					{
						m_bullets[i].lastX=m_bullets[i].curX;
						m_bullets[i].lastY=m_bullets[i].curY;
						m_bullets[i].curX+=m_bullets[i].velX;
						m_bullets[i].curY+=m_bullets[i].velY;
						m_bullets[i].flag=2;
						continue;
					}
					if (m_bullets[i].dieStageAndMoveType%4==0)		//imageIdx increase 1 every 4 frames
						m_bullets[i].imgIdx++;
				}
				else
				{
					m_bullets[i].dieStageAndMoveType=4;				//enter die stage
					if (i<ENEMY_BULLET_N_RECORD_DOT)				//change img based on bullet image type
						m_bullets[i].imgIdx=156;					//dot bullet death image
					else
						m_bullets[i].imgIdx=160;					//image bullet death image
					int removeBulletScore;
					if (m_bullets[i].age==0)						//remove bullet image also differ based on bullet age
						removeBulletScore=10;
					else
						removeBulletScore=100;
					m_pStage->AddScore(removeBulletScore);
				}
			}
			if (m_bullets[i].bornStageAndGrazeFlag==2)	//set grazed flag to false for bullets just ended born stage
				m_bullets[i].bornStageAndGrazeFlag=0;
			if (m_bullets[i].bornStageAndGrazeFlag>2)	//bullets still in born stage
			{
				if (m_bullets[i].bornStageAndGrazeFlag==3)		//first move backward and then forward
				{
					m_bullets[i].lastX=m_bullets[i].curX;		//move backward with 8x speed
					m_bullets[i].lastY=m_bullets[i].curY;
					m_bullets[i].curX-=m_bullets[i].velX*8;
					m_bullets[i].curY-=m_bullets[i].velY*8;
					m_bullets[i].bornStageAndGrazeFlag=5;		//change to born stage 5
				}
				else
					if (m_bullets[i].bornStageAndGrazeFlag==4)		//move forward
					{
						m_bullets[i].lastX=m_bullets[i].curX;		//move forward with 1x speed
						m_bullets[i].lastY=m_bullets[i].curY;
						m_bullets[i].curX+=m_bullets[i].velX;
						m_bullets[i].curY+=m_bullets[i].velY;
						m_bullets[i].bornStageAndGrazeFlag=5;		//change to born stage 5
					}
					else		//bornStageAndGrazeFlag between 5 and 20
					{
						m_bullets[i].lastX=m_bullets[i].curX;		//move forward with 1/3x speed
						m_bullets[i].lastY=m_bullets[i].curY;
						m_bullets[i].curX+=m_bullets[i].velX/3;
						m_bullets[i].curY+=m_bullets[i].velY/3;
						m_bullets[i].bornStageAndGrazeFlag++;
					}
				if (m_bullets[i].bornStageAndGrazeFlag>=20)			//born stage end, change to normal mode
				{
					if (m_bullets[i].curX<=m_clipXLow||m_bullets[i].curX>=m_clipXHigh||
						m_bullets[i].curY<=m_clipYLow||m_bullets[i].curY>=m_clipYHigh)
						m_bullets[i].flag=2;						//outside play area, remove
					else
						m_bullets[i].bornStageAndGrazeFlag=2;		//2 for just ended born stage
				}
			}
			else
			{
				if (m_bullets[i].dieStageAndMoveType==1)			//bullet with special move type
					SpecialMove(i);
				else
				{
					if (m_bullets[i].dieStageAndMoveType==0)		//bullet that has speed fade in
					{
						m_bullets[i].accumulator--;
						//speed transit from 72 to origSpeed linearly within 32 frames
						m_bullets[i].curSpeed=m_bullets[i].origSpeed+m_bullets[i].delta*m_bullets[i].accumulator/32.0f;
						if (m_bullets[i].accumulator==0)			//end of fade in frames
						{
							m_bullets[i].curSpeed=m_bullets[i].origSpeed;
							m_bullets[i].dieStageAndMoveType=2;		//change to normal move type
						}
						//update velX, velY from curSpeed, curAngle
						UpdateVelXYFromSpeedAngle(i);
					}
				}
				m_bullets[i].lastX=m_bullets[i].curX;
				m_bullets[i].lastY=m_bullets[i].curY;
				m_bullets[i].curX+=m_bullets[i].velX;
				m_bullets[i].curY+=m_bullets[i].velY;
				if (m_bullets[i].curX<=m_clipXLow||m_bullets[i].curX>=m_clipXHigh||
					m_bullets[i].curY<=m_clipYLow||m_bullets[i].curY>=m_clipYHigh)
				{
					m_bullets[i].flag=2;						//outside play area, remove
					continue;
				}
				if (m_clearEnemyBulletTimeLeft>0)
					continue;
				float charaX=m_pStage->m_pChara->GetCurX();
				float charaY=m_pStage->m_pChara->GetCurY();
				if (m_bullets[i].bornStageAndGrazeFlag==0)		//test graze first
				{
					//the graze area inf PC98 ver is very strange: left 16, right 20, up/down 22
					//we rotate it 90 degrees clockwise here: up 16, down 20, left/right 22
					if (m_bullets[i].curX>=charaX-22.0f*16.0f&&m_bullets[i].curX<=charaX+22.0f*16.0f)
						if (m_bullets[i].curY>=charaY-16.0f*16.0f&&m_bullets[i].curY<=charaY+20.0f*16.0f)
						{
							m_pStage->m_pSparkEffect->AddEffect(m_bullets[i].curX,m_bullets[i].curY,32,2,false);
							m_bullets[i].bornStageAndGrazeFlag=1;
							if (m_pStage->m_curNGraze<999)
							{
								m_pStage->m_curNGraze++;
								m_pStage->AddScore(m_pStage->m_grazeBonus);
							}
						}
				}
				else
				{
					//test crash with chara, the PC98 ver assumes only grazed bullets can crash chara
					if (m_pStage->m_pChara->TestCrash(m_bullets[i].curX,m_bullets[i].curY,4.0f*16.0f,4.0f*16.0f))
					{
						m_bullets[i].flag=2;
						m_pStage->m_pChara->m_bMiss=true;
					}

				}
			}
		}
	}
	else
	{
		int totalBonus=0;
		int nAvailBullet=0;
		int bonusLimit[]={960,1280,1280,1280,1600};
		int maxSingleBonus=bonusLimit[CGame::GVar().m_playDifficulty];
		for (int i=0;i<ENEMY_BULLET_N_RECORD;i++)
		{
			if (m_bullets[i].flag==1)
			{
				m_bullets[i].velX=m_bullets[i].velY=0;
				m_bullets[i].lastX=m_bullets[i].curX;
				m_bullets[i].lastY=m_bullets[i].curY;
				int curBonus=(3*nAvailBullet*nAvailBullet-nAvailBullet)/2+1;
				if (curBonus>maxSingleBonus) curBonus=maxSingleBonus;
				m_pStage->m_pPopupNumber->AddNumber(m_bullets[i].curX,m_bullets[i].curY,curBonus*10,
													m_pStage->m_pStageRes->m_palette[15*3]/255.0f,
													m_pStage->m_pStageRes->m_palette[15*3+1]/255.0f,
													m_pStage->m_pStageRes->m_palette[15*3+2]/255.0f);
				totalBonus+=curBonus;
				m_bullets[i].flag=2;
				if (nAvailBullet%4==0&&m_bDropPointItemOnBonus)
					m_pStage->m_pItem->AddItem(m_bullets[i].curX,m_bullets[i].curY,ITEM_POINT,0,-48);
				nAvailBullet++;
			}
		}
		if (totalBonus>0)
			m_pStage->m_pFloatingText->SetTextType(FLOATING_TEXT_BONUS,totalBonus*10);
		m_pStage->AddScore(totalBonus);
		m_noEnemyBullet=false;
	}

	if (m_clearEnemyBulletTimeLeft>0)
		m_clearEnemyBulletTimeLeft--;
}

void CEnemyBullet::Draw()
{
	//draw imaged bullets
	for (int i=ENEMY_BULLET_N_RECORD_DOT;i<ENEMY_BULLET_N_RECORD;i++)
	{
		if (m_bullets[i].flag!=1)
			continue;
		//PC98 ver classify born stage and normal stage by if bornStageAndGrazeFlag>3 or not
		//if >3, it draws corresponding image from born sequence, otherwise use imgIdx of the bullet
		//This seems to be a bug:
		//1. if bornStageAndGrazeFlag==3, bullet is obviously in born stage. (see Step())
		//2. actually, bullets with bornStageAndGrazeFlag 3 should not be drawn.
		//   because this value only exists for just-added bullets, which may have a position jump
		//   after the first Step() call (i.e. pos-=vel*8, see Step() for detail)
		if (m_bullets[i].bornStageAndGrazeFlag==3)
			continue;
		if (m_bullets[i].bornStageAndGrazeFlag>3)
		{
			unsigned char imgIdx;
			if (m_bullets[i].imgIdx<48)
				imgIdx=11;
			else
				if (m_bullets[i].imgIdx<52)
					imgIdx=15;
				else
					if (m_bullets[i].imgIdx<68)
						imgIdx=11;
					else
						if (m_bullets[i].imgIdx<116)
							imgIdx=15;
						else
							if (m_bullets[i].imgIdx<152)
								imgIdx=11;
							else
								imgIdx=15;
			imgIdx+=m_bullets[i].bornStageAndGrazeFlag/4;
			m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx),
										   m_bullets[i].curX,m_bullets[i].curY);
		}
		else
		{
			m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_bullets[i].imgIdx),
										   m_bullets[i].curX,m_bullets[i].curY);
		}
	}

	if (m_noEnemyBullet||m_clearEnemyBulletTimeLeft!=0)
	{
		for (int i=0;i<ENEMY_BULLET_N_RECORD_DOT;i++)
			if (m_bullets[i].flag==1)
				if (m_bullets[i].dieStageAndMoveType>=4)
					m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_bullets[i].imgIdx),
												   m_bullets[i].curX,m_bullets[i].curY);

	}
	else
	{
		for (int i=0;i<ENEMY_BULLET_N_RECORD_DOT;i++)
		{
			if (m_bullets[i].flag!=1)
				continue;
			if (m_bullets[i].bornStageAndGrazeFlag<=2)
			{
				m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_pImgDotBullet,m_bullets[i].curX,m_bullets[i].curY);
				m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(m_pStage->m_pStageRes->m_pImgDotBulletShadow,m_bullets[i].curX,
														  m_bullets[i].curY,m_dotBulletShadowColor[0],
														  m_dotBulletShadowColor[1],m_dotBulletShadowColor[2]);
			}
			else
				if (m_bullets[i].bornStageAndGrazeFlag>=4)
				{
					unsigned char imgIdx=147+m_bullets[i].bornStageAndGrazeFlag/4;
					m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx),
												   m_bullets[i].curX,m_bullets[i].curY);
				}
		}
	}
}

}





















