#include ".\th5extfont.h"
#include <stdio.h>
#include "../../Game/Game.h"

namespace th5w{

bool CTh5ExtFont::s_bFontLoaded=false;
GLuint CTh5ExtFont::s_fontTex=0;

CTh5ExtFont::CTh5ExtFont(void)
{
}

CTh5ExtFont::~CTh5ExtFont(void)
{
}

bool CTh5ExtFont::LoadZUNCOM(const char *fileName)
{
	unsigned char *texImage=new unsigned char[256*256*2];
	if (texImage==NULL)
		return false;

	FILE *fp=fopen(fileName,"rb");
	if (fp==NULL)
	{
		delete texImage;
		return false;
	}

	fseek(fp,0x2541,SEEK_SET);		//seek to gaiji area

	for (int i=0;i<256;i++)
	{
		unsigned char buf[32];
		fread(buf,1,32,fp);
		int cx,cy;
		cx=(i%16)*16;
		cy=(i/16)*16;
		for (int j=0;j<32;j++)
		{
			int ax,ay;
			ax=cx+(j%2)*8;
			ay=cy+j/2;
			for (int k=0;k<8;k++)
				texImage[(255-ay)*256*2+(ax+k)*2]=texImage[(255-ay)*256*2+(ax+k)*2+1]=((buf[j]&(1<<(7-k)))==0?0:255);
		}
	}
	fclose(fp);

	glGenTextures(1,&s_fontTex);
	glBindTexture(GL_TEXTURE_2D,s_fontTex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE4_ALPHA4,256,256,0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,texImage);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	delete texImage;

	s_bFontLoaded=true;

	return true;
}

void CTh5ExtFont::DrawExtChar(unsigned char extChar,int drawX,int drawY,
							  float colorR,float colorG,float colorB, float fadeInScale,bool coordIsUpperLeft)
{
	if (s_bFontLoaded==false)
		return;

	float texXLow=(float)(extChar%16*16)/256.0f;
	float texYLow=(float)(255-extChar/16*16-15)/256.0f;
	float texXHigh=texXLow+16.0f/256.0f;
	float texYHigh=texYLow+16.0f/256.0f;

	if (coordIsUpperLeft)
		drawY=CGame::s_pCurGame->m_gameWindow.m_height-1-drawY-(16-1);

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

	glBegin(GL_QUADS);
		glTexCoord2f(texXLow,texYLow);
		glVertex2f((float)drawX,(float)drawY);
		glTexCoord2f(texXHigh,texYLow);
		glVertex2f((float)drawX+16,(float)drawY);
		glTexCoord2f(texXHigh,texYHigh);
		glVertex2f((float)drawX+16,(float)drawY+16);
		glTexCoord2f(texXLow,texYHigh);
		glVertex2f((float)drawX,(float)drawY+16);
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}


void CTh5ExtFont::DrawExtString(const unsigned char *extStr,int nChar,int drawX,int drawY, float colorR,float colorG,float colorB, float fadeInScale,bool coordIsUpperLeft,bool bReverse)
{
	if (s_bFontLoaded==false)
		return;

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

	for (int i=0;i<nChar;i++)
	{
		if (extStr[i]==0)
			break;
		float texXLow=(float)(extStr[i]%16*16)/256.0f;
		float texYLow=(float)(255-extStr[i]/16*16-15)/256.0f;
		float texXHigh=texXLow+16.0f/256.0f;
		float texYHigh=texYLow+16.0f/256.0f;

		if (bReverse)
		{
			glDisable(GL_TEXTURE_2D);
			glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE);
			glDisable(GL_BLEND);
			glColor4f(0,0,0,1);
			glBegin(GL_QUADS);
				glVertex2f((float)drawX,(float)drawY);
				glVertex2f((float)drawX+16,(float)drawY);
				glVertex2f((float)drawX+16,(float)drawY+16);
				glVertex2f((float)drawX,(float)drawY+16);
			glEnd();

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor4f(1,1,1,1);
			glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_ALPHA);
			glBegin(GL_QUADS);
				glTexCoord2f(texXLow,texYLow);
				glVertex2f((float)drawX,(float)drawY);
				glTexCoord2f(texXHigh,texYLow);
				glVertex2f((float)drawX+16,(float)drawY);
				glTexCoord2f(texXHigh,texYHigh);
				glVertex2f((float)drawX+16,(float)drawY+16);
				glTexCoord2f(texXLow,texYHigh);
				glVertex2f((float)drawX,(float)drawY+16);
			glEnd();
			glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
			glColor4f(colorR,colorG,colorB,fadeInScale);
			glBlendFunc(GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA);
			glDisable(GL_TEXTURE_2D);
		}

		glBegin(GL_QUADS);
			glTexCoord2f(texXLow,texYLow);
			glVertex2f((float)drawX,(float)drawY);
			glTexCoord2f(texXHigh,texYLow);
			glVertex2f((float)drawX+16,(float)drawY);
			glTexCoord2f(texXHigh,texYHigh);
			glVertex2f((float)drawX+16,(float)drawY+16);
			glTexCoord2f(texXLow,texYHigh);
			glVertex2f((float)drawX,(float)drawY+16);
		glEnd();

		drawX+=16;
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}
}


























