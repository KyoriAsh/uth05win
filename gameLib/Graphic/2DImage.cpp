#include "../../Game/Game.h"
#include ".\2dimage.h"

namespace th5w{

C2DImage::C2DImage(void)
{
	m_texID=0;
}

C2DImage::~C2DImage(void)
{
}

unsigned char* C2DImage::GetDataPointer()
{
	return m_data;
}

bool C2DImage::UploadToTexture()
{
	if(m_texID!=0)
		return false;

	m_texWidth=m_width;
	m_texHeight=m_height;

	//for better compatibility with graphic cards, force texture size to power of 2
	if (((m_width&(m_width-1))!=0)||((m_height&(m_height-1))!=0))
	{
		for (m_texWidth=1;m_texWidth<m_width;m_texWidth<<=1);
		for (m_texHeight=1;m_texHeight<m_height;m_texHeight<<=1);
		unsigned char *tempdata=new unsigned char[m_texWidth*m_texHeight*4];
		if (tempdata==NULL)
			return false;
		for (int i=0;i<m_height;i++)
			memcpy(tempdata+m_texWidth*i*4,m_data+m_width*i*4,m_width*4);
		delete m_data;
		m_data=tempdata;
	}

	glGenTextures(1,&m_texID);
	glBindTexture(GL_TEXTURE_2D,m_texID);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_texWidth,m_texHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,m_data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	delete m_data;
	m_data=NULL;

	return true;
}

void C2DImage::Draw(float drawX,float drawY,float fadeInScale,float drawScale,
					bool bUseAlpha,bool coordIsUpperLeft,bool bDepthTest,float drawDepth)
{
	if (m_texID==0)
		return;

	if (coordIsUpperLeft)
		drawY=CGame::s_pCurGame->m_gameWindow.m_height-1-drawY-(m_height-1);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
			0,CGame::s_pCurGame->m_windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,m_texID);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glColor4f(1,1,1,fadeInScale);

	if (bDepthTest==true)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if (bUseAlpha)
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}
	else
		glDisable(GL_BLEND);

	glBegin(GL_QUADS);
		glTexCoord2d(0,0);
		glVertex3f(drawX,drawY,drawDepth);
		glTexCoord2d(m_width/(double)m_texWidth,0);
		glVertex3f(drawX+m_width*drawScale,drawY,drawDepth);
		glTexCoord2d(m_width/(double)m_texWidth,m_height/(double)m_texHeight);
		glVertex3f(drawX+m_width*drawScale,drawY+m_height*drawScale,drawDepth);
		glTexCoord2d(0,m_height/(double)m_texHeight);
		glVertex3f(drawX,drawY+m_height*drawScale,drawDepth);
	glEnd();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void C2DImage::DrawColorScaled(float drawX,float drawY,float colorScaleR,float colorScaleG,
							   float colorScaleB,bool coordIsUpperLeft)
{
	if (m_texID==0)
		return;

	if (coordIsUpperLeft)
		drawY=CGame::s_pCurGame->m_gameWindow.m_height-1-drawY-(m_height-1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,m_texID);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
			   0,CGame::s_pCurGame->m_windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glColor4f(colorScaleR,colorScaleG,colorScaleB,1.0f);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
    glBegin(GL_QUADS);
		glTexCoord2d(0,0);
		glVertex3f(drawX,drawY,0);
		glTexCoord2d(m_width/(double)m_texWidth,0);
		glVertex3f(drawX+m_width,drawY,0);
		glTexCoord2d(m_width/(double)m_texWidth,m_height/(double)m_texHeight);
		glVertex3f(drawX+m_width,drawY+m_height,0);
		glTexCoord2d(0,m_height/(double)m_texHeight);
		glVertex3f(drawX,drawY+m_height,0);
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void C2DImage::DrawWhiteMask(float drawX,float drawY,bool coordIsUpperLeft)
{
	if (m_texID==0)
		return;

	if (coordIsUpperLeft)
		drawY=CGame::s_pCurGame->m_gameWindow.m_height-1-drawY-(m_height-1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,m_texID);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
			   0,CGame::s_pCurGame->m_windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE);
    glBegin(GL_QUADS);
		glTexCoord2d(0,0);
		glVertex3f(drawX,drawY,0);
		glTexCoord2d(m_width/(double)m_texWidth,0);
		glVertex3f(drawX+m_width,drawY,0);
		glTexCoord2d(m_width/(double)m_texWidth,m_height/(double)m_texHeight);
		glVertex3f(drawX+m_width,drawY+m_height,0);
		glTexCoord2d(0,m_height/(double)m_texHeight);
		glVertex3f(drawX,drawY+m_height,0);
	glEnd();

	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glColor3f(1,1,1);
	glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
		glVertex3f(drawX,drawY,0);
		glVertex3f(drawX+m_width,drawY,0);
		glVertex3f(drawX+m_width,drawY+m_height,0);
		glVertex3f(drawX,drawY+m_height,0);
	glEnd();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

C2DImage* C2DImage::Create2DImage(int width,int height)
{
	unsigned char *data=new unsigned char[width*height*4];
	if (data==NULL)
		return NULL;

	C2DImage *pRet=new C2DImage;
	if (pRet==NULL)
	{
		delete data;
		return NULL;
	}

	pRet->m_width=width;
	pRet->m_height=height;
	pRet->m_data=data;
	pRet->m_texID=0;

	return pRet;
}

void C2DImage::Destroy()
{
	if (m_texID!=0)
		glDeleteTextures(1,&m_texID);
	if (m_data)
		delete m_data;
	delete this;
}

}




























