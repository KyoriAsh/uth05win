#pragma once
#include <vector>
#include "2DImage.h"

namespace th5w{

class C2DImageArray
{
protected:
	std::vector<C2DImage *> m_vp2DImage;
public:
	__forceinline int NImage()
	{
		return (int)m_vp2DImage.size();
	}
	void UnloadImage(int imgIdx);			//unload one image
	void UnloadRange(int rangeStart);		//unload images from rangeStart to last image
	void AddImage(C2DImage *pImage);
	void DrawMulti(int idxStart,int nImage,float drawX,float drawY,float fadeInScale=1.0,float drawScale=1.0,
				   bool bUseAlpha=true,bool coordIsUpperLeft=true, bool bDepthTest=false,float drawDepth=0.0);

	C2DImage* GetImagePtr(int imgIdx);
public:
	C2DImageArray(void);
	~C2DImageArray(void);
};

}
