#pragma once
#include <stdio.h>
#include "include/glew.h"
#include <gl/GL.h>

namespace th5w{

class C2DImage
{
public:
	int m_width,m_height;
protected:
	int m_texWidth,m_texHeight;
protected:
	unsigned char *m_data;
	GLuint m_texID;
public:
	__forceinline GLuint GetTexID(){return m_texID;}

public:
	static C2DImage* Create2DImage(int width,int height);
	void Destroy();
	unsigned char* GetDataPointer();
	bool UploadToTexture();
	void Draw(float drawX,float drawY,float fadeInScale=1.0,float drawScale=1.0,bool bUseAlpha=true,
			  bool coordIsUpperLeft=true, bool bDepthTest=false,float drawDepth=0.0);
	void DrawColorScaled(float drawX,float drawY,float colorScaleR,float colorScaleG,
						 float colorScaleB,bool coordIsUpperLeft=true);
	void DrawWhiteMask(float drawX,float drawY,bool coordIsUpperLeft=true);

protected:
	C2DImage(void);
	~C2DImage(void);
};

}
