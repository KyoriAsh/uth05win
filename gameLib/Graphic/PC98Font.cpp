#include ".\pc98font.h"
#include <tchar.h>
#include "../../Game/Game.h"
#include <atlimage.h>

namespace th5w{

bool CPC98Font::s_bFontLoaded=false;
GLuint CPC98Font::s_fontTex=0;

bool CPC98Font::s_bCacheInited=false;
int CPC98Font::s_curCacheWriteIdx;
int CPC98Font::s_charIdxInCache[65536];
int CPC98Font::s_cacheList[200];
GLuint CPC98Font::s_displayListBase;

GLuint CPC98Font::s_CommonCharTex=0;

CPC98Font::CPC98Font(void)
{
}

CPC98Font::~CPC98Font(void)
{
}

bool CPC98Font::LoadAnex86BMP(const char *fileName)
{
	CImage fontImg;
	TCHAR fileNameBuf[1000];
	wsprintf(fileNameBuf,_T("%S"),fileName);
	if (fontImg.Load(fileNameBuf)!=0)
		return false;
	if (fontImg.GetBPP()!=1)
		return false;
	if (fontImg.GetWidth()!=2048)
		return false;
	if (fontImg.GetHeight()!=2048)
		return false;

	unsigned char *pData=(unsigned char *)fontImg.GetBits();
	pData+=fontImg.GetPitch()*(fontImg.GetHeight()-1);			//move to last line

	unsigned char *texImage=new unsigned char[2048*2048*2];
	if (texImage==NULL)
		return false;

	unsigned char *pWrite=texImage;
	for (int i=0;i<fontImg.GetHeight();i++)
	{
		for (int j=0;j<fontImg.GetWidth();j++,pWrite+=2)
			(*pWrite)=(*(pWrite+1))=((pData[j/8]&(1<<(7-j%8)))==0)?255:0;
		pData-=fontImg.GetPitch();
	}

	glGenTextures(1,&s_fontTex);
	glBindTexture(GL_TEXTURE_2D,s_fontTex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE4_ALPHA4,fontImg.GetWidth(),fontImg.GetHeight(),0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,texImage);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	delete texImage;

	s_bFontLoaded=true;

	return true;
}

bool CPC98Font::CreateTextureForCommonChar()
{
	if (s_CommonCharTex!=0)
		return false;

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i=0;i<16;i++)
		for (int j=0;j<16;j++)
		{
			char str[3];
			str[0]=i*16+j;
			str[1]=0;
			str[2]=0;
			DrawString(str,100,j*16,i*16,1,1,1,1.0,false);
		}

	glGenTextures(1,&s_CommonCharTex);
	glBindTexture(GL_TEXTURE_2D,s_CommonCharTex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glCopyTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE4_ALPHA4,0,0,256,256,0);

	return true;
}

void CPC98Font::DestroyTextureForCommonChar()
{
	if (s_CommonCharTex!=0)
	{
		glDeleteTextures(1,&s_CommonCharTex);
		s_CommonCharTex=0;
	}
}

int CPC98Font::ConvertCharToTexXY(float *outX,float *outY,unsigned char *str)
{
	unsigned char twoBytes[2];
	twoBytes[0]=str[0];
	twoBytes[1]=str[1];
	if (twoBytes[0]<128)
	{
		*outX=8*twoBytes[0]/2048.0f;
		*outY=(2032)/2048.0f;			//1-byte ASCII chars are located on top row of bmp
		return 1;
	}

	if (!((twoBytes[0]>=0x81&&twoBytes[0]<=0x9f)||(twoBytes[0]>=0xe0&&twoBytes[0]<=0xfc)))
	{
		printf("char code error 0x%x%x\n",twoBytes[0],twoBytes[1]);
		return 0;
	}

	//second byte of Shift-JIS must be in range 40-fc and can not be 7f
	if (twoBytes[1]<0x40||twoBytes[1]>0xfc||twoBytes[1]==0x7f)
	{
		printf("char code error 0x%x%x\n",twoBytes[0],twoBytes[1]);
		return 0;
	}

	//convert character code to coordinate in anex86.bmp
	if (twoBytes[1]>0x7f)
		twoBytes[1]--;					//no space between 0x7e and 0x80 in bmp, hence later chars moved 1 forward
	if (twoBytes[0]>=0xe0)
		twoBytes[0]-=0xe0-0xa0;			//no space between 0x90 and 0xe0 in bmp, hence later chars moved leftward
	int charX=16+(twoBytes[0]-0x81)*32;	//two 16-pixel columns for each group of codes with the same first byte
	if (twoBytes[1]>=0x9e)				//chars in the second column, although the second column start with 0x9f,
	{									//we use 0x9e here because we have decremented it by 1 above.
		charX+=16;
		twoBytes[1]-=0x9e-0x40;			//also convert the y to be the same for the first column
	}
	int charY=528+(twoBytes[1]-0x40)*16;

	charY=2047-charY-15;				//from bmp top-down Y to GL texture bottom-up Y, also become
										//coordinate of lower-left corner
	*outX=(charX)/2048.0f;
	*outY=(charY)/2048.0f;

	return 2;
}

GLuint CPC98Font::GetDisplayListForChar(int charIdx)
{
	if (!s_bCacheInited)
	{
		memset(s_charIdxInCache,0xff,sizeof(s_charIdxInCache));
		memset(s_cacheList,0xff,sizeof(s_cacheList));
		s_curCacheWriteIdx=0;
		s_displayListBase=glGenLists(sizeof(s_cacheList)/sizeof(int));
		s_bCacheInited=true;
	}

	if (s_charIdxInCache[charIdx]==-1)
	{
		//printf("new char %d\n",charIdx);
		wglUseFontBitmaps(wglGetCurrentDC(),charIdx,1,s_displayListBase+s_curCacheWriteIdx);
		if (s_cacheList[s_curCacheWriteIdx]!=-1)
		{
			//printf("refresh cache %d -> %d\n",s_cacheList[s_curCacheWriteIdx],charIdx);
			s_charIdxInCache[s_cacheList[s_curCacheWriteIdx]]=-1;
		}
		s_charIdxInCache[charIdx]=s_curCacheWriteIdx;
		s_cacheList[s_curCacheWriteIdx]=charIdx;
		int retIdx=s_curCacheWriteIdx;
		s_curCacheWriteIdx=(s_curCacheWriteIdx+1)%(sizeof(s_cacheList)/sizeof(int));
		return s_displayListBase+retIdx;
	}
	return s_displayListBase+s_charIdxInCache[charIdx];
}

void CPC98Font::FinalizeCache()
{
	if (s_bCacheInited)
	{
		glDeleteLists(s_displayListBase,sizeof(s_cacheList)/sizeof(int));
		s_bCacheInited=false;
	}
}

bool CPC98Font::DrawString(char *str,int nChar,int drawX,int drawY,float colorR,float colorG,
						   float colorB,float fadeInScale,bool coordIsUpperLeft)
{
	if (CGame::GVar().m_bUseSystemFont==false&&s_bFontLoaded==false)
		return false;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,s_fontTex);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
			   0,CGame::s_pCurGame->m_windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor4f(colorR,colorG,colorB,fadeInScale);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	if (coordIsUpperLeft)
		drawY=CGame::s_pCurGame->m_gameWindow.m_height-1-drawY-(16-1);

	if (CGame::GVar().m_bUseSystemFont)
	{
		if (s_CommonCharTex!=0)
			glBindTexture(GL_TEXTURE_2D,s_CommonCharTex);
		for (int i=0;i<nChar;i++)
		{
			unsigned char *twoBytes=(unsigned char *)str;
			if (twoBytes[0]==0)
				break;
			int charLen;
			if (IsDBCSLeadByteEx(CGame::GVar().m_textCodePage,twoBytes[0]))
				charLen=2;
			else
				charLen=1;

			if (charLen==1&&s_CommonCharTex!=0)
			{
				glEnable(GL_TEXTURE_2D);
				float texXLow=(float)(twoBytes[0]%16*16)/256.0f;
				float texYLow=(float)(twoBytes[0]/16*16)/256.0f;
				float texXHigh=texXLow+8/256.0f;
				float texYHigh=texYLow+16/256.0f;
				glBegin(GL_QUADS);
					glTexCoord2f(texXLow,texYLow);
					glVertex2f((float)drawX,(float)drawY);
					glTexCoord2f(texXHigh,texYLow);
					glVertex2f((float)drawX+charLen*8,(float)drawY);
					glTexCoord2f(texXHigh,texYHigh);
					glVertex2f((float)drawX+charLen*8,(float)drawY+16);
					glTexCoord2f(texXLow,texYHigh);
					glVertex2f((float)drawX,(float)drawY+16);
				glEnd();
			}
			else
			{
				TCHAR tstr[5];
				memset(tstr,0,sizeof(tstr));
				MultiByteToWideChar(CGame::GVar().m_textCodePage,0,(char*)twoBytes,charLen,tstr,5);
				unsigned char *s=(unsigned char *)tstr;

				GLuint dispList=GetDisplayListForChar(s[0]+s[1]*256);
				//wglUseFontBitmaps(wglGetCurrentDC(),s[0]+s[1]*256,1,dispList);

				glDisable(GL_TEXTURE_2D);
				glRasterPos2f((float)drawX,(float)drawY+2);
				glCallList(dispList);
			}

			str+=charLen;
			drawX+=charLen*8;
		}
		//printf("\n");
		//glDeleteLists(dispList,1);
	}
	else
	{
		for (int i=0;i<nChar;i++)
		{
			unsigned char *twoBytes=(unsigned char *)str;
			if (twoBytes[0]==0)
				break;

			float texXLow;
			float texYLow;
			int charLen=ConvertCharToTexXY(&texXLow,&texYLow,twoBytes);
			if (charLen==0)
				break;
			float texXHigh=texXLow+(charLen*8)/2048.0f;
			float texYHigh=texYLow+(16)/2048.0f;

			glBegin(GL_QUADS);
				glTexCoord2f(texXLow,texYLow);
				glVertex2f((float)drawX,(float)drawY);
				glTexCoord2f(texXHigh,texYLow);
				glVertex2f((float)drawX+charLen*8,(float)drawY);
				glTexCoord2f(texXHigh,texYHigh);
				glVertex2f((float)drawX+charLen*8,(float)drawY+16);
				glTexCoord2f(texXLow,texYHigh);
				glVertex2f((float)drawX,(float)drawY+16);
			glEnd();

			drawX+=charLen*8;
			str+=charLen;
		}
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	return true;
}

}




















