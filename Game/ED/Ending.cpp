#include ".\ending.h"
#include "../../gameLib/PMDPlayer/PMDPlayer.h"
#include "../Game.h"
#include "../../gameLib/CommonFunction/CommonFunctionMusicSE.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include "../../gameLib/Graphic/PC98Font.h"
#include <math.h>

namespace th5w{

CEnding::CEnding(void)
{
	m_pBGImage=NULL;
	m_pCenterImage=NULL;
	m_pCenterImageCandi=NULL;
	m_lastKey=0;
	m_curKey=0;
	m_curMusicFade=100;
	m_bMusicFadeOut=false;
	m_bWaitNFrame=false;
}

CEnding::~CEnding(void)
{
	if (m_pBGImage!=NULL)
		m_pBGImage->Destroy();
	m_pBGImage=NULL;

	if (m_pCenterImage!=NULL)
		m_pCenterImage->Destroy();
	m_pCenterImage=NULL;

	if (m_pCenterImageCandi!=NULL)
		m_pCenterImageCandi->Destroy();
	m_pCenterImageCandi=NULL;
}

void CEnding::Initialize(int edType)
{
	m_curFrame=0;
	memset(m_palette,0,sizeof(m_palette));
	m_lastTextColorIdx=0;
	m_curTextColorIdx=0;
	m_textFade=100;
	m_bFading=false;
	m_bWaitKey=false;
	m_curCenterImgIdx=-1;

	m_bCtrImgFading=false;
	m_ctrImgCurFade=0;
	m_ctrImgTotalFade=4;

	edType=edType*10+CGame::GVar().m_playChara;
	char edFileName[20];
	sprintf(edFileName,"_ED%02d.TXT",edType);
	Cth5DatFile *pPackFile=CGame::s_pCurGame->FindResource(edFileName);
	int idx=pPackFile->GetChildFileIndex(edFileName);
	m_pScript=pPackFile->GetChildFileContentPointer(idx);
	m_pScriptBase=m_pScript;
}

int CEnding::ReadNumber(int defValue)
{
	if (m_pScript[0]<'0'||m_pScript[1]>'9')
		return defValue;
	int res=0;
	while(m_pScript[0]>='0'&&m_pScript[0]<='9')
	{
		res=res*10+m_pScript[0]-48;
		m_pScript++;
	}
	return res;
}

int CEnding::ReadNumberComma(int defValue)
{
	if (m_pScript[0]==',')
	{
		m_pScript++;
		return ReadNumber(defValue);
	}
	return defValue;
}

int CEnding::Step()
{
	if (m_curFrame==0)
		CPMDPlayer::Pause();
	m_curFrame++;
	m_lastKey=m_curKey;
	m_curKey=CCommonFunctionInput::GetAllKeyState();

	if (m_bMusicFadeOut)
	{
		m_curMusicFade-=0.5f;
		if (m_curMusicFade<=0)
		{
			m_curMusicFade=0;
			m_bMusicFadeOut=false;
		}
		CPMDPlayer::SetVolume(m_curMusicFade);
	}
	if (m_bWaitNFrame)
	{
		m_nWaitFrameLeft--;
		if (m_nWaitFrameLeft==0)
			m_bWaitNFrame=false;
		return 0;
	}
	if (m_bFading)
	{
		float nextScale=m_curScrScale+m_deltaScrScale;
		if ((nextScale-m_tarScrScale)*(m_curScrScale-m_tarScrScale)<=0.0f)
		{
			m_curScrScale=m_tarScrScale;
			m_bFading=false;
		}
		else
			m_curScrScale=nextScale;
		return 0;
	}
	if (m_textFade<100)
	{
		m_textFade+=5;
		if (m_textFade>=100)
		{
			m_textFade=100;
			m_bWaitKey=true;
		}
		return 0;
	}
	if (m_bWaitKey)
	{
		if (CCommonFunctionInput::ZPressed(m_curKey,m_lastKey))
			m_bWaitKey=false;
		return 0;
	}

	if (m_pScript-m_pScriptBase==938)
	{
		int i=1;
	}

	while(1)
	{
		unsigned char curChar=m_pScript[0];
		if (curChar<=32||curChar==127)
		{
			m_pScript++;
			continue;
		}
		if (curChar=='\\'&&m_pScript[1]!='s'&&m_pScript[1]!='S'&&m_pScript[1]!='k'&&m_pScript[1]!='K')
		{
			switch(m_pScript[1])
			{
			case 'c':
			case 'C':
				if (m_pScript[2]=='=')
				{
					vector <unsigned char> a;
					a.push_back(m_pScript[3]);
					a.push_back(m_pScript[4]);
					m_pScript+=6;
					unsigned char res=(unsigned char)ReadNumber(15);
					m_colorTable.push_back(make_pair(a,res));
				}
				else
				{
					m_pScript+=2;
					m_curTextColorIdx=(unsigned char)ReadNumber(15);
				}
				break;
			case 't':
			case 'T':
				m_pScript+=2;
				m_curScrScale=(float)ReadNumber(100);
				break;
			case 'p':
			case 'P':
				switch(m_pScript[2])
				{
				case ',':
					{
						char s[100];
						sscanf((char *)m_pScript+3,"%s",s);
						m_pScript+=3+(int)strlen(s);
						strlwr(s);
						int idx=CGame::s_pCurGame->m_th5Dat1.GetChildFileIndex(s);
						C2DImage *pImg;
						CCommonFunctionGraphic::LoadPIFromDat(&pImg,m_palette,&CGame::s_pCurGame->m_th5Dat1,s);
						C2DImage **ppTar;
						if (strstr(s,"bk")!=NULL||strcmp(s,"white.pi")==0)			//hack here
							ppTar=&m_pBGImage;
						else
						{
							ppTar=&m_pCenterImageCandi;
							//m_curCenterImgIdx=0;
						}
						if (*ppTar!=NULL)
							(*ppTar)->Destroy();
						*ppTar=pImg;
					}
					break;
				case '=':
					m_pScript+=3;
					m_curText.clear();
					break;
				}
				break;
			case 'f':
			case 'F':
				switch(m_pScript[2])
				{
				case 'i':
					m_bFading=true;
					m_tarScrScale=100;
					m_curScrScale=0;
					m_pScript+=3;
					m_deltaScrScale=5.0f/(float)ReadNumber(1);
					return 0;
				case 'o':
					m_bFading=true;
					m_tarScrScale=0;
					m_curScrScale=100;
					m_pScript+=3;
					m_deltaScrScale=-5.0f/(float)ReadNumber(1);
					return 0;
				case 'm':
					m_pScript+=3;
					ReadNumber(1);
					m_bMusicFadeOut=true;
				    break;
				}
				break;
			case 'w':
			case 'W':
				switch(m_pScript[2])
				{
				case 'i':
					m_bFading=true;
					m_tarScrScale=100;
					m_curScrScale=200;
					m_pScript+=3;
					m_deltaScrScale=-5.0f/(float)ReadNumber(1);
					return 0;
				case 'o':
					m_bFading=true;
					m_tarScrScale=200;
					m_curScrScale=100;
					m_pScript+=3;
					m_deltaScrScale=5.0f/(float)ReadNumber(1);
					return 0;
				case 'k':
				case 'K':
					m_pScript++;
				default:
					m_pScript+=2;
					int res=ReadNumber(32);
					m_bWaitNFrame=true;
					m_nWaitFrameLeft=res;
					//printf("\\w %d\n",res);
					return 0;
				}
				break;
			case '=':
				if (m_pScript[2]=='=')
				{
					m_pScript+=3;
					int a=ReadNumber(4);
					int b=ReadNumberComma(1);
					m_ctrImgCurFade=0;
					m_ctrImgTotalFade=b*4;
					m_curCenterImgIdx=a;
				}
				else
				{
					m_pScript+=2;
					int a=ReadNumber(4);
					m_curCenterImgIdx=a;
				}
				if (m_pCenterImageCandi!=NULL)
				{
					if (m_pCenterImage!=NULL)
						m_pCenterImage->Destroy();
					m_pCenterImage=m_pCenterImageCandi;
					m_pCenterImageCandi=NULL;
				}
				break;
			case 'm':
			case 'M':
				switch(m_pScript[2])
				{
				case ',':
					{
						char s[100];
						sscanf((char *)m_pScript+3,"%s",s);
						m_pScript+=3+(int)strlen(s);
						strlwr(s);
						strcat(s,".m2");
						CCommonFunctionMusicSE::LoadMusicToPMDFromDat(&CGame::s_pCurGame->m_th5Dat1,s);
						CPMDPlayer::FillSoftwareBuffer(10000);
						CPMDPlayer::Play();
						m_curMusicFade=100;
						break;
					}
				case '$':
					{
						m_pScript+=3;
						CPMDPlayer::Pause();
						break;
					}
				case '*':
					{
						m_pScript+=3;
						CPMDPlayer::Play();
						m_curMusicFade=100;
						break;
					}
				}
				break;
			case '$':
				CPMDPlayer::Pause();
				CPMDPlayer::SetVolume(100.0f);
				return 1;
			case 'v':
				m_pScript+=2;
				ReadNumber(2);
				break;
			default:
				printf("err %c %d\n",m_pScript[1],m_pScript[1]);
				while(1);
				break;
			}
		}
		else
		{
			m_lastText=m_curText;
			m_lastTextColorIdx=m_curTextColorIdx;
			m_curTextColorIdx=15;
			m_curText.clear();
			while(1)
			{
				if (m_pScript[0]<=32||m_pScript[0]==127)
				{
					m_pScript++;
					continue;
				}
				unsigned char a=m_pScript[0];
				unsigned char b=m_pScript[1];
				m_pScript+=2;
				if (a=='\\'&&((b=='s')||(b=='S')||(b=='k')||(b=='K')))
				{
					if ((b=='s'||b=='S')&&(m_pScript[0]=='-'))
						m_pScript++;
					break;
				}
				if (a=='\\'&&(b=='='||b=='w'||b=='W'))
				{
					m_pScript-=2;
					break;
				}
				m_curText.push_back(a);
				m_curText.push_back(b);
				if (a=='@')
				{
					if(b=='!')
					{
						if (m_pScript[0]=='!'||m_pScript[0]=='?')
						{
							m_curText.push_back(m_pScript[0]);
							m_pScript++;
						}
					}
					else
						if (b>='0'&&b<='9')
						{
							while (m_pScript[0]>='0'&&m_pScript[0]<='9')
							{
								m_curText.push_back(m_pScript[0]);
								m_pScript++;
							}
						}
				}
				else
				{
					if (a=='\\'&&b=='c')
						while (m_pScript[0]>='0'&&m_pScript[0]<='9')
						{
							m_curText.push_back(m_pScript[0]);
							m_pScript++;
						}
				}
			}
			if (m_curText.size()>0)
			{
				for (int i=0;i<(int)m_colorTable.size();i++)
					if (m_curText[0]==m_colorTable[i].first[0]&&m_curText[1]==m_colorTable[i].first[1])
						m_curTextColorIdx=m_colorTable[i].second;
				m_textFade=0;
				break;
			}
		}
	}

	return 0;
}

void CEnding::DrawText(vector<unsigned char> &text,unsigned char colorIdx, int scale)
{
	float c[]={m_palette[colorIdx*3]/255.0f,m_palette[colorIdx*3+1]/255.0f,m_palette[colorIdx*3+2]/255.0f};
	int x=80;
	int y=320+40;
	for (int i=0;i<(int)text.size();)
	{
		if (text[i]=='\\'&&text[i+1]=='n')
		{
			x=80;
			y+=16;
			i+=2;
			continue;
		}
		if (text[i]=='\\'&&text[i+1]=='c')
		{
			int idx=0;
			i+=2;
			if (text[i]<'0'||text[i]>'9')
				idx=15;
			else
				while(text[i]>='0'&&text[i]<='9')
					idx=idx*10+text[i++]-48;
			c[0]=m_palette[idx*3]/255.0f;
			c[1]=m_palette[idx*3+1]/255.0f;
			c[2]=m_palette[idx*3+2]/255.0f;
			continue;
		}
		if (text[i]=='@')
		{
			unsigned char idx;
			switch(text[i+1])
			{
			case 't':			//{09_sweat}
			case 'T':
				idx=9;
				i+=2;
				break;
			case 'h':
			case 'H':
				idx=6;
				i+=2;
				break;
			case '?':			//{08_?}
				idx=8;
				i+=2;
			    break;
			case '!':
				if (text[i+2]=='!')			//{10_!!}
				{
					idx=10;
					i+=3;
				}
				else
					if (text[i+2]=='?')		//{11_!?}
					{
						idx=11;
						i+=3;
					}
					else					//{07_!}
					{
						idx=7;
						i+=2;
					}
			    break;
			default:
				i++;
				idx=0;
				while (text[i]>='0'&&text[i]<='9')
				{
					idx=idx*10+text[i]-48;
					i++;
				}
				break;
			}
			CTh5ExtFont::DrawExtChar(idx,x,y,c[0],c[1],c[2],scale/100.0f);
		}
		else
		{
			CPC98Font::DrawString((char *)(&text[i]),1,x,y,c[0],c[1],c[2],scale/100.f);
			i+=2;
		}
		x+=16;
		if (x>=560)
		{
			x=144;
			y+=16;
		}
	}
	if (text.size()>0&&m_bWaitKey)
	{
		CTh5ExtFont::DrawExtChar(0x1c+(m_curFrame%32)/8,576,y,1,1,1,scale/100.0f);
	}
}

void CEnding::Draw()
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	if (m_pBGImage!=NULL)
		m_pBGImage->Draw(0,40);

	if (m_curCenterImgIdx>=0&&m_curCenterImgIdx<=3)
	{
		GLuint texID=m_pCenterImage->GetTexID();

		float drawX=160;
		float drawY=64+40;
		drawY=CGame::s_pCurGame->m_gameWindow.m_height-1-drawY-(200-1);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texID);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
				   0,CGame::s_pCurGame->m_windowHeight);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor4f(1,1,1,1);
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

		glDisable(GL_DEPTH_TEST);

		glDisable(GL_BLEND);
		float w=320/1024.0f;
		float h=200/512.0f;
		float t1=(float)(m_curCenterImgIdx%2)*w;
		float t2=(float)(1-m_curCenterImgIdx/2)*h;
		glBegin(GL_QUADS);
			glTexCoord2d(t1,t2);
			glVertex2f(drawX,drawY);
			glTexCoord2d(t1+w,t2);
			glVertex2f(drawX+320,drawY);
			glTexCoord2d(t1+w,t2+h);
			glVertex2f(drawX+320,drawY+200);
			glTexCoord2d(t1,t2+h);
			glVertex2f(drawX,drawY+200);
		glEnd();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	if (m_textFade<100)
		DrawText(m_lastText,m_lastTextColorIdx,100-m_textFade);
	DrawText(m_curText,m_curTextColorIdx,m_textFade);

	if (m_curScrScale<0) m_curScrScale=0;
	if (m_curScrScale>200) m_curScrScale=200;
	float overlayColor;
	if (m_curScrScale<100)
		overlayColor=0;
	else
		overlayColor=1;
	float overlayAlpha=fabs(m_curScrScale-100)/100;
	CCommonFunctionGraphic::DrawRectangle(0,40,640,440,
										  overlayColor,overlayColor,overlayColor,overlayAlpha);
}

}




















