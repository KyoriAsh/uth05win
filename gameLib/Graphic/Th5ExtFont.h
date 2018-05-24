#pragma once
#include "../Graphic/include/glew.h"
#include <gl/gl.h>

namespace th5w{

class CTh5ExtFont
{
protected:
	static bool s_bFontLoaded;
	static GLuint s_fontTex;
public:
	static bool LoadZUNCOM(const char *fileName);
	static void DrawExtChar(unsigned char extChar,int drawX,int drawY,float colorR,float colorG,float colorB,
							float fadeInScale=1.0,bool coordIsUpperLeft=true);
	static void DrawExtString(const unsigned char *extStr,int nChar,int drawX,int drawY,
							  float colorR,float colorG,float colorB,
							  float fadeInScale=1.0,bool coordIsUpperLeft=true,bool bReverse=false);

public:
	CTh5ExtFont(void);
	~CTh5ExtFont(void);
};

}