#include ".\titlerollingtama.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "../Game.h"

namespace th5w{

CTitleRollingTama::CTitleRollingTama(void)
{
	for (int i=0;i<8;i++)
		m_rollingImg[i]=NULL;
}

CTitleRollingTama::~CTitleRollingTama(void)
{
	for (int i=0;i<8;i++)
		if (m_rollingImg[i])
		{
			m_rollingImg[i]->Destroy();
			m_rollingImg[i]=NULL;
		}
}

void CTitleRollingTama::Initialize()
{
	m_curScrFade=100;
	m_curFrame=0;

	CCommonFunctionGraphic::LoadPIFromDat(&m_rollingImg[0],m_palette,&CGame::s_pCurGame->m_th5Dat1,"OP2A.PI");
	CCommonFunctionGraphic::LoadPIFromDat(&m_rollingImg[1],NULL,&CGame::s_pCurGame->m_th5Dat1,"OP2B.PI");
	CCommonFunctionGraphic::LoadPIFromDat(&m_rollingImg[2],NULL,&CGame::s_pCurGame->m_th5Dat1,"OP2C.PI");
	CCommonFunctionGraphic::LoadPIFromDat(&m_rollingImg[3],NULL,&CGame::s_pCurGame->m_th5Dat1,"OP2D.PI");
	CCommonFunctionGraphic::LoadPIFromDat(&m_rollingImg[4],NULL,&CGame::s_pCurGame->m_th5Dat1,"OP2E.PI");
	CCommonFunctionGraphic::LoadPIFromDat(&m_rollingImg[5],NULL,&CGame::s_pCurGame->m_th5Dat1,"OP2F.PI");
	CCommonFunctionGraphic::LoadPIFromDat(&m_rollingImg[6],NULL,&CGame::s_pCurGame->m_th5Dat1,"OP2G.PI");
	CCommonFunctionGraphic::LoadPIFromDat(&m_rollingImg[7],NULL,&CGame::s_pCurGame->m_th5Dat1,"OP2H.PI");
}

int CTitleRollingTama::Step()
{
	m_curScrFade=m_curFrame+64;
	{
		if (m_curScrFade>100)
			m_curScrFade=100;
	}
	if (m_curFrame==64)
		return 1;
	return 0;
}

void CTitleRollingTama::Draw()
{
	glClearColor(m_palette[3]/255.0f,m_palette[4]/255.0f,m_palette[5]/255.0f,1);
	glClear(GL_COLOR_BUFFER_BIT);

	int drawImgIdx=m_curFrame/8;
	if (drawImgIdx>=8)
		drawImgIdx=7;
	m_rollingImg[drawImgIdx]->Draw(0,278+40);
	CCommonFunctionGraphic::ScreenFade((float)m_curScrFade);

	m_curFrame++;
}

}



















