#include ".\popupnumber.h"
#include <string.h>
#include "../Stage.h"

namespace th5w{

CPopupNumber::CPopupNumber(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CPopupNumber::~CPopupNumber(void)
{
}

void CPopupNumber::Initialize()
{
	memset(m_numbers,0,sizeof(m_numbers));
}

void CPopupNumber::AddNumber(float origX,float origY,int number,float colorR,float colorG,float colorB)
{
	for (int i=0;i<POPUP_NUMBER_N_RECORD;i++)
		if (m_numbers[i].flag==0)
		{
			m_numbers[i].flag=1;
			m_numbers[i].age=0;
			m_numbers[i].curX=origX;
			m_numbers[i].curY=origY;
			m_numbers[i].nDigit=1;
			for (int j=0;j<POPUP_NUMBER_MAX_DIGIT;j++)
			{
				m_numbers[i].digits[j]=number%10;
				number/=10;
				if (m_numbers[i].digits[j]!=0)
					m_numbers[i].nDigit=j+1;
			}
			if (number!=0)
			{
				for (int j=0;j<POPUP_NUMBER_MAX_DIGIT;j++)
					m_numbers[i].digits[j]=9;
				m_numbers[i].nDigit=POPUP_NUMBER_MAX_DIGIT;
			}
			m_numbers[i].color[0]=colorR;
			m_numbers[i].color[1]=colorG;
			m_numbers[i].color[2]=colorB;
			break;
		}
}

void CPopupNumber::Step()
{
	for (int i=0;i<POPUP_NUMBER_N_RECORD;i++)
	{
		if (m_numbers[i].flag==0||m_numbers[i].flag==2)
		{
			m_numbers[i].flag=0;
			continue;
		}
		m_numbers[i].lastY=m_numbers[i].curY;
		if (m_numbers[i].age>=24)
			m_numbers[i].curY-=8;
		m_numbers[i].age++;
		if (m_numbers[i].age>36)
			m_numbers[i].flag=2;
	}
}

void CPopupNumber::Draw()
{
	for (int i=0;i<POPUP_NUMBER_N_RECORD;i++)
	{
		if (m_numbers[i].flag==0)		//yes, the PC98 ver draws numbers with flag==2 as well
			continue;
		for (int j=0;j<m_numbers[i].nDigit;j++)
			m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(m_pStage->m_pStageRes->m_pImgNumber[m_numbers[i].digits[j]],
													  m_numbers[i].curX+(m_numbers[i].nDigit*4-j*8)*16,m_numbers[i].curY,
													  m_numbers[i].color[0], m_numbers[i].color[1], m_numbers[i].color[2]);
	}
}

}



















