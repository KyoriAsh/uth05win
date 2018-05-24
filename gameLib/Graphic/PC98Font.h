/*
	This class is NOT thread-safe. Make sure not to call is simultaneously from multiple threads
*/

#pragma once
#include "../Graphic/include/glew.h"
#include <gl/gl.h>

namespace th5w{

class CPC98Font
{
protected:
	static bool s_bFontLoaded;
	static GLuint s_fontTex;

protected:
	static bool s_bCacheInited;
	static int s_curCacheWriteIdx;
	static int s_charIdxInCache[65536];
	static int s_cacheList[200];
	static GLuint s_displayListBase;

protected:
	static GLuint s_CommonCharTex;

public:
	static bool CreateTextureForCommonChar();
	static void DestroyTextureForCommonChar();


protected:
	static int ConvertCharToTexXY(float *outX,float *outY,unsigned char *str);
	static GLuint GetDisplayListForChar(int charIdx);

public:
	static bool LoadAnex86BMP(const char *fileName);
	static bool DrawString(char *str,int nChar,int drawX,int drawY,float colorR,float colorG,float colorB,
						   float fadeInScale=1.0,bool coordIsUpperLeft=true);	//the string must be in Shift-JIS code
	static void FinalizeCache();
public:
	CPC98Font(void);
	~CPC98Font(void);
};

}
