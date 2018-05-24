#include ".\floatingtext.h"
#include "../../../gameLib/Graphic/Th5ExtFont.h"
#include "../Stage.h"
#include <string.h>
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

const unsigned char c_floatingText[][15]={{0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0},
										  {0x48,0x49,0x4a,0x4b,0x4c,0},
										  {0x58,0x59,0x5a,0x5b,0},
										  {0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0},
										  {0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0}};
const int c_floatingTextLen[]={8,5,4,8,10};

CFloatingText::CFloatingText(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CFloatingText::~CFloatingText(void)
{
}

void CFloatingText::Initialize()
{
	m_textType=FLOATING_TEXT_NONE;
	m_textAge=0;
}

void CFloatingText::SetTextType(int textType,int number)
{
	if (m_textType==FLOATING_TEXT_NONE||(m_textType!=textType&&m_textAge>=64))
	{
		m_textType=textType;
		m_textAge=0;
		m_textLen=c_floatingTextLen[textType];
		m_textCurX=52-m_textLen*2;
		if (textType==FLOATING_TEXT_BONUS)
		{
			m_textDestX=16;
			m_textNumber=number;
		}
		else
			m_textDestX=28-m_textLen;
	}
}

void CFloatingText::Step()
{
	if (m_textAge>=128)
	{
		m_textAge=0;
		m_textType=FLOATING_TEXT_NONE;
		return;
	}
	if (m_textAge>=m_textLen-1)
		if (m_textCurX>m_textDestX)
		{
			m_textCurX-=2;
			m_floating=true;
		}
		else
			m_floating=false;
	m_textAge++;
}

void CFloatingText::Draw()
{
	m_textAge--;
	if (m_textAge>=m_textLen-1)
	{
		if (m_floating==true)
		{
			CTh5ExtFont::DrawExtString(c_floatingText[m_textType],100,
									   (m_textCurX+2)*8-32+m_pStage->m_playAreaUpperLeftX,
									   2*16-16+m_pStage->m_playAreaUpperLeftY,1,1,1);
		}
		else
		{
			CTh5ExtFont::DrawExtString(c_floatingText[m_textType],100,
									   m_textDestX*8-32+m_pStage->m_playAreaUpperLeftX,
									   2*16-16+m_pStage->m_playAreaUpperLeftY,1,1,1);
			if (m_textType==FLOATING_TEXT_BONUS)
				m_pStage->m_pStageRes->DrawNumber(0x1c*8-32+m_pStage->m_playAreaUpperLeftX,
									 2*16-16+m_pStage->m_playAreaUpperLeftY,
									 m_textNumber,7,false,true,1,1,1);
		}
	}
	else
	{
		CTh5ExtFont::DrawExtString(c_floatingText[m_textType],m_textAge,
								   (m_textCurX+(m_textLen-1-m_textAge+1)*2)*8-32+m_pStage->m_playAreaUpperLeftX,
								   2*16-16+m_pStage->m_playAreaUpperLeftY,1,1,1);
	}
	m_textAge++;
}

}












