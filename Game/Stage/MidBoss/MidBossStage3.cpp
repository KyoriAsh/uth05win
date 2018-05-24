#include ".\midbossstage3.h"
#include <stdlib.h>
#include "../Stage.h"
#include "../../game.h"

namespace th5w{

CMidBossStage3::CMidBossStage3(void)
{
}

CMidBossStage3::~CMidBossStage3(void)
{
}

void CMidBossStage3::Initialize()
{
	m_curX=192.0f*16.0f;
	m_curY=-32.0f*16.0f;
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_velX=0;
	m_velY=8.0f;
	m_curHP=1400;
	m_curImage=208;
}

void CMidBossStage3::ChargeAnimation()
{
	m_pStage->m_pGatherEffect->AddEffectTrailed(m_curX,m_curY,10,11,m_curPhaseFrame-1);
	if (m_curPhaseFrame==1)
	{
		m_pStage->m_bPlaySound[8]=true;
		m_curImage=210;
		return;
	}
	if (m_curPhaseFrame==16)
		m_curImage=211;
}

void CMidBossStage3::Phase0()
{
	m_lastX=m_curX;
	m_lastY=m_curY;
	m_curX+=m_velX;
	m_curY+=m_velY;
	GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,10);
	if (m_curPhaseFrame>=192)
	{
		m_curPhase++;
		m_curPhaseFrame=0;
		m_phase1CurMode=1;
		m_bPhase1FadingIn=false;
		m_phase1NModeChange=1;
	}
}

void CMidBossStage3::FadeOutAndIn()
{
	if (m_curPhaseFrame%4!=0)
		return;
	if (m_bPhase1FadingIn)
	{
		m_curImage--;
		if (m_curImage<212)
		{
			m_curImage=208;
			m_bPhase1FadingIn=false;
			m_curPhaseFrame=0;
			m_phase1CurMode=0;
		}
	}
	else
	{
		m_curImage++;
		if (m_curImage>=220)
		{
			m_curImage=219;
			m_curY+=12.0f*16.0f;
			m_curX=m_pStage->m_pChara->GetCurX();
			if (m_curX<64.0f*16.0f) m_curX=64.0f*16.0f;
			if (m_curX>320.0f*16.0f) m_curX=320.0f*16.0f;
			m_bPhase1FadingIn=true;
		}
	}
}

void CMidBossStage3::Phase1Mode1()
{
	if (m_curPhaseFrame<32)
	{
		ChargeAnimation();
		return;
	}
	if (m_curPhaseFrame>=32&&m_curPhaseFrame<=64&&m_curPhaseFrame%16==0)
	{
		int idx=(m_curPhaseFrame-32)/16;
		const unsigned char bulletImg[]={44,48,44};
		const unsigned char nWay[]={3,4,7};
		const float unitAngle[]={16,5,10};
		const float bulletSpeed[]={24,48,32};
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=2;
		shootInfo.bulletImg=bulletImg[idx];
		shootInfo.bulletSpeed=bulletSpeed[idx];
		shootInfo.shootType=EST08_NWAY_MULTI;
		shootInfo.nWay=nWay[idx];
		shootInfo.nWayUnitAngle=unitAngle[idx];
		shootInfo.nMulti=4;
		shootInfo.deltaSpeed=8;
		shootInfo.shootAngle=64;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[15]=true;
		m_curY-=4.0f*16.0f;
		return;
	}
	if (m_curPhaseFrame>=96)
		FadeOutAndIn();
}

void CMidBossStage3::Phase1Mode2()
{
	if (m_curPhaseFrame<32)
	{
		ChargeAnimation();
		return;
	}
	if (m_curPhaseFrame>=32&&m_curPhaseFrame<=64&&m_curPhaseFrame%16==0)
	{
		int idx=(m_curPhaseFrame-32)/16;
		const unsigned char bulletImg[]={44,48,44};
		const unsigned char nWay[][4]={{1,1,1,1},{1,2,3,4},{1,3,4,5}};
		const float unitAngle[][4]={{1,1,1,1},{1,4,3,4},{1,5,5,4}};
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.bulletImg=bulletImg[idx];
		shootInfo.bulletSpeed=24;
		shootInfo.shootType=EST09_NWAY_MULTI_TO_CHARA;
		shootInfo.nWay=nWay[idx][CGame::GVar().m_playDifficulty];
		shootInfo.nWayUnitAngle=unitAngle[idx][CGame::GVar().m_playDifficulty];
		shootInfo.nMulti=8;
		shootInfo.deltaSpeed=8;
		shootInfo.shootAngle=0;
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[15]=true;
		m_curY-=4.0f*16.0f;
	}
	if (m_curPhaseFrame>=96)
		FadeOutAndIn();
}

void CMidBossStage3::Phase1Mode3()
{
	if (m_curPhaseFrame<32)
	{
		ChargeAnimation();
		return;
	}
	if (m_curPhaseFrame>=32&&m_curPhaseFrame<=64&&m_curPhaseFrame%16==0)
	{
		//You may observe that in this shoot some bullets are not pointing to the direction it is going.
		//Although this can be seen all over the game because bullet images are not drawn by rotation
		//but by using one image from a pre-generated image set. The set (loaded from PC98ver dat file)
		//was generated such that angle between each adjacent pair of images is 8/256*2*PI. This quite
		//limited precision would definitely causes artifacts.
		//However, the matter here is much more obvious here. This is due to a bug of the MIKO16.BFT
		//file of PC98 ver:
		//	(all angles are in units of 1/256*2*PI)
		//	image index    :	...	58	59	60	61	62	63	64	...
		//	expected angle :	...	48	56	64	72	80	88	96	...
		//	current angle  :	...	48	56	64	80	88	88	96	...
		//As you can see, the 61st and 62nd images are not correct, causing a large gap of 16/256*2*PI
		//between the 60th and the 61st image.
		//This can be fixed by replacing 61st and 62nd image with horizontally-flipped version of the
		//59th and 58th. However, I'm too lazy to do that.
		int idx=(m_curPhaseFrame-32)/16;
		const unsigned char nWay[]={16,21,16};
		const float unitAngle[]={8,4,8};
		ENEMY_SHOOT_INFO shootInfo;
		shootInfo.shootOrigX=m_curX;
		shootInfo.shootOrigY=m_curY;
		shootInfo.bulletBornType=18;
		shootInfo.bulletImg=52;
		shootInfo.bulletSpeed=48;
		shootInfo.shootType=EST02_NWAY;
		shootInfo.nWay=nWay[idx];
		shootInfo.nWayUnitAngle=unitAngle[idx];
		shootInfo.shootAngle=64;
		m_pStage->m_pEnemyBullet->TuneEnemyShootInfo(&shootInfo);
		m_pStage->m_pEnemyBullet->Shoot(&shootInfo);
		m_pStage->m_bPlaySound[15]=true;
		m_curY-=4.0f*16.0f;
		return;
	}
	if (m_curPhaseFrame>=96)
		FadeOutAndIn();
}

void CMidBossStage3::Phase1()
{
	m_lastX=m_curX;
	m_lastY=m_curY;
	bool bPhaseEnd=false;

	switch(m_phase1CurMode)
	{
	case 0:
		if (m_curPhaseFrame>=32)
		{
			m_phase1CurMode=m_phase1NModeChange%3+1;
			m_phase1NModeChange++;
			m_curPhaseFrame=0;
			if (m_phase1NModeChange>=13)
				bPhaseEnd=true;
		}
		break;
	case 1:
		Phase1Mode1();
		break;
	case 2:
		Phase1Mode2();
	    break;
	case 3:
		Phase1Mode3();
	    break;
	default:
		return;
	}
	if (bPhaseEnd==false)
	{
		if (m_curImage<212)
		{
			int dmg=GetDamageAndPlaySoundWhenHit(24.0f*16.0f,24.0f*16.0f,4);
			if (dmg!=0)
				m_bHitThisFrame=true;
			m_curHP-=dmg;
		}
		if (m_curHP<=0)
		{
			m_pStage->m_pEnemyBullet->m_noEnemyBullet=true;
			AddDefeatScoreAndPopupNumbers(15);
			m_pStage->m_pItem->AddItem(m_curX,m_curY,ITEM_1UP,0,-48);
			bPhaseEnd=true;
		}
	}
	if (bPhaseEnd)
	{
		m_curPhase=0xfe;
		m_curImage=4;
		m_curPhaseFrame=0;
		m_pStage->m_pSparkEffect->AddEffect(m_curX,m_curY,128,48,true);
		m_pStage->m_bPlaySound[12]=true;
	}
}

int CMidBossStage3::Step()
{
	m_curPhaseFrame++;
	m_bHitThisFrame=false;

	switch(m_curPhase)
	{
	case 0:
		Phase0();
		break;
	case 1:
		Phase1();
		break;
	default:
		int ret=DeathSequence();
		StepHPGauge(m_curHP,1400);
		return ret;
	}
	StepHPGauge(m_curHP,1400);
	m_pStage->m_bHomingEnemyExist=true;
	m_pStage->m_homingEnemyX=m_curX;
	m_pStage->m_homingEnemyY=m_curY;

	return 0;
}

void CMidBossStage3::Draw()
{
	if (m_curPhase<0xfe)
	{
		int imgIdx;
		if (m_curImage!=208)
			imgIdx=m_curImage;
		else
			imgIdx=208+m_pStage->m_curFrame%16/8;
		C2DImage *pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
		if (m_bHitThisFrame)
			m_pStage->m_pStageRes->DrawStageObjectImageWhiteMask(pImage,m_curX,m_curY);
		else
			m_pStage->m_pStageRes->DrawStageObjectImage(pImage,m_curX,m_curY);
	}
	else
		if (m_curPhase==0xfe)
			DrawExplode();
}

void CMidBossStage3::DrawStatistics()
{
	DrawHPGaugeAndExtStrEnemy(1400);
}

}
