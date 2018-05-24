#include ".\midboss.h"
#include "../Stage.h"
#include "../../../gameLib/Graphic/Th5ExtFont.h"

namespace th5w{

CMidBoss::CMidBoss(void)
{
	m_curPhase=0;
	m_curPhaseFrame=0;
	m_lastDrawnHP=0;
	m_explodeAngle=(float)(rand()%256);	//PC98 does does not initialize this var, but it doens't matter much
}

CMidBoss::~CMidBoss(void)
{
}

void CMidBoss::Initialize()
{
}

int CMidBoss::GetDamageAndPlaySoundWhenHit(float dmgAreaHalfWidth,float dmgAreaHalfHeight,int soundIdx)
{
	int dmg=m_pStage->GetDamageFromChara(m_curX,m_curY,dmgAreaHalfWidth,dmgAreaHalfHeight,true);
	if (dmg!=0)
		m_pStage->m_bPlaySound[soundIdx]=true;

	return dmg;
}

void CMidBoss::AddDefeatScoreAndPopupNumbers(int n1280)
{
	m_pStage->AddScore(n1280*1280);
	for (int i=0;i<n1280;i++)
	{
		float drawX=m_curX-64.0f*16.0f+(float)(rand()%2048);
		float drawY=m_curY-64.0f*16.0f+(float)(rand()%2048);
		m_pStage->m_pPopupNumber->AddNumber(drawX,drawY,1280*10,
											m_pStage->m_pStageRes->m_palette[11*3]/255.0f,
											m_pStage->m_pStageRes->m_palette[11*3+1]/255.0f,
											m_pStage->m_pStageRes->m_palette[11*3+2]/255.0f);
	}
}

void CMidBoss::StepHPGauge(int curHP,int fullHP)
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

void CMidBoss::DrawHPGaugeAndExtStrEnemy(int fullHP)
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

int CMidBoss::DeathSequence()
{
	if (m_curPhase!=0xfe)
		return 1;

	if (m_curPhaseFrame==1)
		m_pStage->m_pEnemy->m_noAddEnemy=false;
	if (m_curPhaseFrame%16==0)
	{
		m_curImage++;
		if (m_curImage>=12)
			m_curPhase=0xff;
	}

	return 0;
}

void CMidBoss::DrawExplode()
{
	float radius=(float)(m_curPhaseFrame*16);
	float angle=m_explodeAngle;
	if (radius>48.0f*16.0f)
	{
		radius=48.0f*16.0f;
		angle=m_explodeAngle+(float)(m_curPhaseFrame-48+1);
	}
	for (int i=0;i<16;i++)
	{
		float drawX=m_curX+(float)cos((angle+i*16.0f)/256.0f*2*PI)*radius;
		float drawY=m_curY+(float)sin((angle+i*16.0f)/256.0f*2*PI)*radius;
		C2DImage *pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_curImage);
		m_pStage->m_pStageRes->DrawStageObjectImage(pImage,drawX,drawY);
	}
}

}
















