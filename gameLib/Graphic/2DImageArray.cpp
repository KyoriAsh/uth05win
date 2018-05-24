#include <windows.h>
#include ".\2dimagearray.h"

namespace th5w{

C2DImageArray::C2DImageArray(void)
{
}

C2DImageArray::~C2DImageArray(void)
{
	UnloadRange(0);
}

void C2DImageArray::UnloadImage(int imgIdx)
{
	if (imgIdx<0||imgIdx>=NImage())
		return;
	if (m_vp2DImage[imgIdx]!=NULL)
	{
		m_vp2DImage[imgIdx]->Destroy();
		m_vp2DImage[imgIdx]=NULL;
	}
	if (imgIdx==NImage()-1)
		m_vp2DImage.pop_back();
}

void C2DImageArray::UnloadRange(int rangeStart)
{
	while(NImage()>rangeStart)
		UnloadImage(NImage()-1);
}

void C2DImageArray::AddImage(C2DImage *pImage)
{
	m_vp2DImage.push_back(pImage);
}

C2DImage* C2DImageArray::GetImagePtr(int imgIdx)
{
	if (imgIdx<0||imgIdx>=NImage())
		return NULL;
	return m_vp2DImage[imgIdx];
}

void C2DImageArray::DrawMulti(int idxStart,int nImage,float drawX,float drawY,float fadeInScale,float drawScale,
							  bool bUseAlpha,bool coordIsUpperLeft, bool bDepthTest,float drawDepth)
{
	for (int i=idxStart;i<idxStart+nImage;i++)
	{
		if (i>=NImage())
			return;
		m_vp2DImage[i]->Draw(drawX,drawY,fadeInScale,drawScale,bUseAlpha,coordIsUpperLeft,bDepthTest,drawDepth);
		drawX+=m_vp2DImage[i]->m_width*drawScale;
	}
}

}























