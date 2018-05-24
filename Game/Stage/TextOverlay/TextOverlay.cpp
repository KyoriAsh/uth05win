#include ".\textoverlay.h"
#include "../Stage.h"
#include "../../../gameLib/Graphic/Th5ExtFont.h"
#include "../../../gameLib/Graphic/PC98Font.h"
#include "../../Game.h"

namespace th5w{

CTextOverlay::CTextOverlay(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CTextOverlay::~CTextOverlay(void)
{
}

void CTextOverlay::Initialize()
{
	m_curMode=TEXT_OVERLAY_NONE;
	m_stageFadeTimer=0;
	m_stageNameBGMNameTimer=0;
	SetMode(TEXT_OVERLAY_STAGE_FADE_IN);
}

void CTextOverlay::StepStageFadeIn()
{
	if (m_stageFadeTimer<72)
		m_stageFadeTimer++;
	else
		SetMode(TEXT_OVERLAY_DRAW_STAGE_NAME_BGM_NAME,
				m_pStage->m_pStageRes->m_pStageStd->m_stageName,m_pStage->m_pStageRes->m_pStageStd->m_stageBGMName);
}

void CTextOverlay::DrawStageFadeIn()
{
	m_stageFadeTimer--;
	unsigned char fontIdx;
	if (m_stageFadeTimer>=8)
		fontIdx=0x40-m_stageFadeTimer/8;
	else
		fontIdx=0x94;

	unsigned char s[25];
	for (int i=0;i<24;i++)
		s[i]=fontIdx;
	s[24]=0;
	for (int i=0;i<23;i++)
		CTh5ExtFont::DrawExtString(s,24,m_pStage->m_playAreaUpperLeftX,i*16+m_pStage->m_playAreaUpperLeftY,
								   0,0,0);
	m_stageFadeTimer++;
}

void CTextOverlay::StepStageFadeOut()
{
	m_stageFadeTimer++;
}

void CTextOverlay::DrawStageFadeOut()
{
	m_stageFadeTimer--;
	unsigned char fontIdx;
	if (m_stageFadeTimer>=8)
		fontIdx=0x37+m_stageFadeTimer/8;
	else
		fontIdx=0x02;
	if (fontIdx>=0x40)
		fontIdx=0x94;

	unsigned char s[25];
	for (int i=0;i<24;i++)
		s[i]=fontIdx;
	s[24]=0;
	for (int i=0;i<23;i++)
		CTh5ExtFont::DrawExtString(s,24,m_pStage->m_playAreaUpperLeftX,i*16+m_pStage->m_playAreaUpperLeftY,
								   0,0,0);
	m_stageFadeTimer++;
}

void CTextOverlay::StepStageNameBGMName()
{
	m_stageNameBGMNameTimer++;
	if (m_stageNameBGMNameTimer==224)
	{
		m_stageNameBGMNameTimer=0;
		SetMode(TEXT_OVERLAY_NONE);
		return;
	}
}

void CTextOverlay::DrawStageNameBGMName()
{
	unsigned char imgIdx;
	C2DImage *pImg=NULL;
	if (m_stageNameBGMNameTimer<32)
	{
		imgIdx=m_stageNameBGMNameTimer/4+16;
		pImg=m_pStage->m_pStageRes->m_txtBBArray.GetImagePtr(imgIdx);
	}
	if (m_stageNameBGMNameTimer>=192)
	{
		imgIdx=(m_stageNameBGMNameTimer-192)/2;
		pImg=m_pStage->m_pStageRes->m_txtBBArray.GetImagePtr(imgIdx);
	}
	float colory[]={m_pStage->m_pStageRes->m_palette[11*3]/255.0f,m_pStage->m_pStageRes->m_palette[11*3+1]/255.0f,
					m_pStage->m_pStageRes->m_palette[11*3+2]/255.0f};
	float colorw[]={m_pStage->m_pStageRes->m_palette[15*3]/255.0f,m_pStage->m_pStageRes->m_palette[15*3+1]/255.0f,
					m_pStage->m_pStageRes->m_palette[15*3+2]/255.0f};
	if (m_stageName[0]!=0)
	{
		{
			int drawX,drawY,len;
			if (CGame::GVar().m_playStage<5)
			{
				drawX=21;
				len=7;
			}
			else
			{
				drawX=17;
				len=11;
			}
			drawY=11;
			if (pImg!=NULL)
				for (int i=0;i<len;i++)
					pImg->DrawColorScaled((float)drawX*8-8-32+m_pStage->m_playAreaUpperLeftX+i*16,
										  (float)drawY*16-8-16+m_pStage->m_playAreaUpperLeftY,
										  colory[0],colory[1],colory[2]);
			if (m_stageNameBGMNameTimer>=24&&m_stageNameBGMNameTimer<192)
			{
				unsigned char stageTitle[]={0xbc,0xbd,0xaa,0xb0,0xae,0x02,0xa1,0};
				unsigned char finalStage[]={0xaf,0xb2,0xb7,0xaa,0xb5,0x02,0xbc,0xbd,0xaa,0xb0,0xae,0};
				unsigned char extraStage[]={0xae,0xc1,0xbd,0xbb,0xaa,0x02,0xbc,0xbd,0xaa,0xb0,0xae,0};
				unsigned char *title;
				if (CGame::GVar().m_playStage<5)
				{
					stageTitle[6]=0xa1+CGame::GVar().m_playStage;
					title=stageTitle;
				}
				if (CGame::GVar().m_playStage==5)
					title=finalStage;
				if (CGame::GVar().m_playStage==6)
					title=extraStage;
				CTh5ExtFont::DrawExtString(title,100,drawX*8-32+m_pStage->m_playAreaUpperLeftX,
										   drawY*16-16+m_pStage->m_playAreaUpperLeftY,
										   colory[0],colory[1],colory[2]);
			}
		}
		{
			int drawX=28-(int)strlen(m_stageName)/2;
			int drawY=13;
			if (pImg!=NULL)
				for (int i=0;i<(int)strlen(m_stageName)/2;i++)
					pImg->DrawColorScaled((float)drawX*8-8-32+m_pStage->m_playAreaUpperLeftX+i*16,
										  (float)drawY*16-8-16+m_pStage->m_playAreaUpperLeftY,
										  colorw[0],colorw[1],colorw[2]);
			if (m_stageNameBGMNameTimer>=24&&m_stageNameBGMNameTimer<192)
				CPC98Font::DrawString(m_stageName,100,drawX*8-32+m_pStage->m_playAreaUpperLeftX,
									  drawY*16-16+m_pStage->m_playAreaUpperLeftY,
									  colorw[0],colorw[1],colorw[2]);
		}
	}
	if (m_bgmName[0]!=0)
	{
		int drawX,drawY,len;
		drawX=48-(int)strlen(m_bgmName);
		drawY=23;
		if (pImg!=NULL)
			pImg->DrawColorScaled((float)drawX*8-8-32+m_pStage->m_playAreaUpperLeftX,
								  (float)drawY*16-8-16+m_pStage->m_playAreaUpperLeftY,
								  colory[0],colory[1],colory[2]);
		if (m_stageNameBGMNameTimer>=24&&m_stageNameBGMNameTimer<192)
			CTh5ExtFont::DrawExtChar(0x03,drawX*8-32+m_pStage->m_playAreaUpperLeftX,
									 drawY*16-16+m_pStage->m_playAreaUpperLeftY,
									 colory[0],colory[1],colory[2]);
		drawX=51-(int)strlen(m_bgmName);
		len=(int)strlen(m_bgmName)/2+(int)strlen(m_bgmName)%2;
		if (pImg!=NULL)
			for (int i=0;i<len;i++)
				pImg->DrawColorScaled((float)drawX*8-8-32+m_pStage->m_playAreaUpperLeftX+i*16,
									  (float)drawY*16-8-16+m_pStage->m_playAreaUpperLeftY,
									  colorw[0],colorw[1],colorw[2]);
		if (m_stageNameBGMNameTimer>=24&&m_stageNameBGMNameTimer<192)
			CPC98Font::DrawString(m_bgmName,100,drawX*8-32+m_pStage->m_playAreaUpperLeftX,
								  drawY*16-16+m_pStage->m_playAreaUpperLeftY,
								  colorw[0],colorw[1],colorw[2]);
	}
}

void CTextOverlay::Step()
{
	switch(m_curMode)
	{
	case TEXT_OVERLAY_NONE:
		return;
	case TEXT_OVERLAY_STAGE_FADE_IN:
		StepStageFadeIn();
		return;
	case TEXT_OVERLAY_DRAW_STAGE_NAME_BGM_NAME:
		StepStageNameBGMName();
		return;
	case TEXT_OVERLAY_STAGE_FADE_OUT:
		StepStageFadeOut();
		return;
	}
}

void CTextOverlay::Draw()
{
	switch(m_curMode)
	{
	case TEXT_OVERLAY_NONE:
		return;
	case TEXT_OVERLAY_STAGE_FADE_IN:
		DrawStageFadeIn();
		return;
	case TEXT_OVERLAY_DRAW_STAGE_NAME_BGM_NAME:
		DrawStageNameBGMName();
		return;
	case TEXT_OVERLAY_STAGE_FADE_OUT:
		DrawStageFadeOut();
		return;
	}
}

void CTextOverlay::SetMode(int mode,char *stageName,char *bgmName)
{
	m_curMode=mode;
	m_stageFadeTimer=0;
	if (mode==TEXT_OVERLAY_DRAW_STAGE_NAME_BGM_NAME)
	{
		if (stageName!=NULL)
			strcpy(m_stageName,stageName);
		else
			m_stageName[0]=0;
		if (bgmName!=NULL)
			strcpy(m_bgmName,bgmName);
		else
			m_bgmName[0]=0;
	}
}

}
























