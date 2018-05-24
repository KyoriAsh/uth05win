#include ".\stage2bg.h"
#include "../Stage.h"
#include "../Boss/Boss.h"
#include "../Background/Background.h"
#include "../../../gameLib/Graphic/Th5ExtFont.h"
#include <string.h>

namespace th5w{

CStage2BG::CStage2BG(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CStage2BG::~CStage2BG(void)
{
}

void CStage2BG::Initialize()
{
	m_scrScale=0;
	m_nSprite=0;
	memset(m_bgSprite,0,sizeof(m_bgSprite));

	m_spriteColorScale=0;
	m_spriteColorDeltaScale=1;
}

void CStage2BG::RegenerateSprite(int recordIdx)
{
	int idx=m_pStage->m_curFrame%4096;
	if (idx<1000)
	{
		m_bgSprite[recordIdx].angle=(float)(rand()%32+48);
		m_bgSprite[recordIdx].curX=(80-m_bgSprite[recordIdx].angle)*128+1024;
	}
	else
		if (idx<1128)
		{
			m_bgSprite[recordIdx].angle=(float)((idx-1000)/8+48+rand()%32);
			m_bgSprite[recordIdx].curX=(float)((80-m_bgSprite[recordIdx].angle)*128+1024+(idx-1000)*24);
		}
		else
			if (idx<2000)
			{
				m_bgSprite[recordIdx].angle=(float)(rand()%32+64);
				m_bgSprite[recordIdx].curX=(96-m_bgSprite[recordIdx].angle)*128+2048;
			}
			else
				if (idx<2128)
				{
					m_bgSprite[recordIdx].angle=(float)(64-(idx-2000)/8+rand()%32);
					m_bgSprite[recordIdx].curX=(float)((96-m_bgSprite[recordIdx].angle)*128+2048-(idx-2000)*24);
				}
				else
					if (idx<3000)
					{
						m_bgSprite[recordIdx].angle=(float)(rand()%32+48);
						m_bgSprite[recordIdx].curX=(80-m_bgSprite[recordIdx].angle)*128+1024;
					}
					else
						if (idx<3128)
						{
							m_bgSprite[recordIdx].angle=(float)(48-(idx-3000)/8+rand()%32);
							m_bgSprite[recordIdx].curX=(float)((80-m_bgSprite[recordIdx].angle)*128+1024-(idx-3000)*24);
						}
						else
							if (idx<3968)
							{
								m_bgSprite[recordIdx].angle=(float)(rand()%32+32);
								m_bgSprite[recordIdx].curX=(64-m_bgSprite[recordIdx].angle)*128;
							}
							else
							{
								m_bgSprite[recordIdx].angle=(float)((idx-3968)/8+32+rand()%32);
								m_bgSprite[recordIdx].curX=(float)((64-m_bgSprite[recordIdx].angle)*128+(idx-3968)*24);
							}
	m_bgSprite[recordIdx].curX+=rand()%8*16;
	m_bgSprite[recordIdx].age=8;
	m_bgSprite[recordIdx].curY=0;
	m_bgSprite[recordIdx].velX=(float)cos(m_bgSprite[recordIdx].angle/256.0f*2*PI)*128.0f;
	m_bgSprite[recordIdx].velY=(float)sin(m_bgSprite[recordIdx].angle/256.0f*2*PI)*128.0f;
}

void CStage2BG::StepSprites()
{
	if (m_nSprite<64&&m_pStage->m_curFrame%2==0)
	{
		m_bgSprite[m_nSprite].flag=1;
		m_bgSprite[m_nSprite].angle=(float)(rand()%32+48);
		m_bgSprite[m_nSprite].curX=(80-m_bgSprite[m_nSprite].angle)*128+1024;
		m_bgSprite[m_nSprite].curY=0;
		m_bgSprite[m_nSprite].velX=(float)cos(m_bgSprite[m_nSprite].angle/256.0f*2*PI)*128.0f;
		m_bgSprite[m_nSprite].velY=(float)sin(m_bgSprite[m_nSprite].angle/256.0f*2*PI)*128.0f;
		m_bgSprite[m_nSprite].age=8;			//PC98 ver does not initialize age here, seems bug
		m_nSprite++;
	}
	for (int i=0;i<64;i++)
		if (m_bgSprite[i].flag!=0)
		{
			m_bgSprite[i].lastX=m_bgSprite[i].curX;
			m_bgSprite[i].lastY=m_bgSprite[i].curY;
			m_bgSprite[i].curX+=m_bgSprite[i].velX;
			m_bgSprite[i].curY+=m_bgSprite[i].velY;
			if (m_bgSprite[i].curX<=-8.0f*16.0f||m_bgSprite[i].curX>=392.0f*16.0f
				||m_bgSprite[i].curY<=-8.0f*16.0f||m_bgSprite[i].curY>=376.0f*16.0f)
				RegenerateSprite(i);
			else
				if (i%4!=0)
					m_bgSprite[i].age++;
		}
	if (m_pStage->m_curFrame%4==0)
	{
		m_spriteColorScale+=m_spriteColorDeltaScale;
		if (m_spriteColorScale>=63)
			m_spriteColorDeltaScale=-1;
		if (m_spriteColorScale<=32)
			m_spriteColorDeltaScale=1;
		m_pStage->m_pStageRes->m_palette[0]=m_spriteColorScale*2;
		m_pStage->m_pStageRes->m_palette[1]=m_spriteColorScale*2;
		m_pStage->m_pStageRes->m_palette[2]=m_spriteColorScale*4;
	}
}

void CStage2BG::Step()
{
	if (m_pStage->m_pBoss)
	{
		if (m_pStage->m_pBoss->GetCurPhase()>1&&m_pStage->m_pBoss->GetCurPhase()<0xfe)
			return;
		if (m_pStage->m_pBoss->GetCurPhase()==1&&m_pStage->m_pBoss->GetCurPhaseFrame()>=32)
			return;
	}
	if (m_pStage->m_curFrame<304)
	{
		m_pStage->m_pBackground->ForceDrawFirstScreen(true);
		if (m_pStage->m_curFrame<256)
			return;
		m_pStage->m_pStageRes->m_palette[0]=303-m_pStage->m_curFrame;
		m_pStage->m_pStageRes->m_palette[1]=303-m_pStage->m_curFrame;
		m_pStage->m_pStageRes->m_palette[2]=(303-m_pStage->m_curFrame)*5;
		m_pStage->m_curScrFade=100+m_scrScale;
		m_scrScale+=2;
		return;
	}
	if (m_pStage->m_curFrame==304)
		m_pStage->m_pBackground->SetDraw(false);
	if (m_pStage->m_curFrame>=306)
	{
		if (m_scrScale!=0)
		{
			if (m_scrScale<=4)
				m_scrScale=0;
			else
				m_scrScale-=4;
			m_pStage->m_curScrFade=100+m_scrScale;
			return;
		}
		StepSprites();
	}
	if (m_pStage->m_curFrame==4000)
	{
		m_pStage->m_pBackground->ForceDrawFirstScreen(false);
		m_pStage->m_pBackground->SetDraw(true);
	}
}

void CStage2BG::Draw()
{
	if (m_pStage->m_pBoss)
	{
		if (m_pStage->m_pBoss->GetCurPhase()>1&&m_pStage->m_pBoss->GetCurPhase()<0xfe)
			return;
		if (m_pStage->m_pBoss->GetCurPhase()==1&&m_pStage->m_pBoss->GetCurPhaseFrame()>=32)
			return;
	}

	//PC98 ver hacks the background tile index buffer here, we choose to use font for exactly the same effect
	if (m_pStage->m_curFrame<304)
	{
		if (m_pStage->m_curFrame<256)
			return;

		int start=11-(m_pStage->m_curFrame-256)/4;
		int len=((m_pStage->m_curFrame-256)/4+1)*2;
		unsigned char extStr[30];
		for (int i=0;i<len;i++)
			extStr[i]=0x94;
		extStr[len]=0;
		for (int i=0;i<23;i++)
			CTh5ExtFont::DrawExtString(extStr,24,m_pStage->m_playAreaUpperLeftX+start*16,
									   i*16+m_pStage->m_playAreaUpperLeftY,
									   m_pStage->m_pStageRes->m_palette[0]/255.0f,m_pStage->m_pStageRes->m_palette[1]/255.0f,
									   m_pStage->m_pStageRes->m_palette[2]/255.0f);
		return;
	}
	for (int i=0;i<64;i++)
		if (m_bgSprite[i].flag!=0)
		{
			int imgIdx=m_bgSprite[i].age/16;
			if (imgIdx>=3)
				imgIdx=3;
			imgIdx+=172;
			C2DImage *pImg=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(imgIdx);
			m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(pImg,m_bgSprite[i].curX,m_bgSprite[i].curY,
													  m_pStage->m_pStageRes->m_palette[0]/255.0f,
													  m_pStage->m_pStageRes->m_palette[1]/255.0f,
													  m_pStage->m_pStageRes->m_palette[2]/255.0f);
		}

}

}





















