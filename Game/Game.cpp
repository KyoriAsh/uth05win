#include ".\game.h"
#include "../gameLib/Graphic/PC98Font.h"
#include "../gameLib/Graphic/Th5ExtFont.h"
#include "../gameLib/CommonFunction/CommonFunctionSystem.h"
#include <time.h>

namespace th5w{

CGame* CGame::s_pCurGame=NULL;

CGame::CGame(void)
{
	m_windowWidth=640;
	m_windowHeight=480;
	m_playArea.left=32;
	m_playArea.top=16+40;
	m_playArea.right=m_playArea.left+384;
	m_playArea.bottom=m_playArea.top+368;

	m_pCurScheme=NULL;

	memset(m_fpsTimeArray,0,sizeof(m_fpsTimeArray));
	m_fpsTimeIdx=0;
	m_fps=0;
}

CGame::~CGame(void)
{
}

void CGame::UpdateFPS()
{
	if (m_fpsTimeIdx<20||m_fpsTimeIdx%20!=0)
	{
		QueryPerformanceCounter(&m_fpsTimeArray[m_fpsTimeIdx%20]);
		m_fpsTimeIdx++;
	}
	else
	{
		LARGE_INTEGER lastTime=m_fpsTimeArray[m_fpsTimeIdx%20];
		QueryPerformanceCounter(&m_fpsTimeArray[m_fpsTimeIdx%20]);
		m_fps=(double)m_performanceFreq.QuadPart/(m_fpsTimeArray[m_fpsTimeIdx%20].QuadPart-lastTime.QuadPart)*20;
		m_fpsTimeIdx++;
	}
}

void CGame::SetVSYNC(bool bOn)
{
	if (bOn)
		wglSwapIntervalEXT(1);
	else
		wglSwapIntervalEXT(0);
}

Cth5DatFile* CGame::FindResource(const char *fileName)
{
	if (m_modDat.GetChildFileIndex(fileName)!=-1)
		return &m_modDat;
	if (m_th5Dat1.GetChildFileIndex(fileName)!=-1)
		return &m_th5Dat1;
	if (m_th5Dat2.GetChildFileIndex(fileName)!=-1)
		return &m_th5Dat2;

	return NULL;
}

void CGame::LoadModConfig()
{
	int fileIdx=m_modDat.GetChildFileIndex("mod.ini");
	if (fileIdx==-1)
		return;
	m_modDat.Childfseek(fileIdx,0,SEEK_SET);
	char curLine[1000];
	while(m_modDat.Childfgets(curLine,1000,fileIdx)!=NULL)
	{
		if (strlen(curLine)>0&&curLine[strlen(curLine)-1]=='\n')		//replace trailing '\n' with 0
			curLine[strlen(curLine)-1]=0;

		char *ptr=curLine;

		while(ptr[0]==' ') ptr++;										//remove leading spaces

		if (strlen(ptr)<2)
			continue;

		if (curLine[0]=='/'&&curLine[1]=='/')							//commented line
			continue;

		if (strchr(ptr,'=')==NULL)										//'=' not found
			continue;

		char *value=strchr(ptr,'=');
		value[0]=0;														//cut the string to 2 parts

		value++;
		while(value[0]==' ') value++;									//remove leading spaces

		char variable[100];
		sscanf(ptr,"%s",variable);										//sscanf to remove trailing spaces

		if (strcmp(variable,"TEXT_CODEPAGE")==0)
		{
			sscanf(value,"%d",&GVar().m_textCodePage);
			continue;
		}
		if (strcmp(variable,"FONT_CHARSET")==0)
		{
			sscanf(value,"%d",&GVar().m_fontCharset);
			continue;
		}
		if (strcmp(variable,"USE_SYSTEM_FONT")==0)
		{
			int v;
			sscanf(value,"%d",&v);
			GVar().m_bUseSystemFont=(v==0?false:true);
			continue;
		}
	}
}

bool CGame::Initialize()
{
	m_globalVar.Initialize();

	if (m_gameWindow.Create(false,m_windowWidth,m_windowHeight)==false)
		return false;

	ShowWindow(m_gameWindow.m_hWnd,SW_HIDE);
	glewInit();
	wglSwapIntervalEXT(1);			//turn on VSYNC

	if (m_input.Initialize()==false)
	{
		MessageBox(m_gameWindow.m_hWnd,_T("DirectInput cannot be initialized."),NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
		return false;
	}

	if (th5w::CPMDPlayer::Initialize(m_gameWindow.m_hWnd)==false)
	{
		MessageBox(m_gameWindow.m_hWnd,_T("PMDPlayer cannot not be initialized."),NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
		return false;
	}
	if (m_soundEffect.Initialize(m_gameWindow.m_hWnd)==false)
	{
		MessageBox(m_gameWindow.m_hWnd,_T("Sound effect files cannot not be loaded and initialized."),NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
		return false;
	}

	//md5 of kaiki1.dat and kaiki2.dat, used to verify these files
	unsigned char dat1MD5[]={203,110,35,68,170,147,49,44,60,244,11,241,111,58,20,242};
	unsigned char dat2MD5[]={202,32,227,240,57,70,193,115,160,42,98,243,123,172,175,209};
	unsigned char ZUNCOMMD5[]={0,5,230,87,18,82,190,211,141,65,103,153,192,5,60,11};

	unsigned char md5[16];

	//load kaiki1.dat
	CCommonFunctionSystem::GetFileMD5(md5,"kaiki1.dat");
	for (int i=0;i<16;i++)
		if (md5[i]!=dat1MD5[i])
		{
			MessageBox(m_gameWindow.m_hWnd,_T("kaiki1.dat missing or corrupted."),NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
			return false;
		}
	if (m_th5Dat1.LoadFile("kaiki1.dat")==false)
	{
		MessageBox(m_gameWindow.m_hWnd,_T("kaiki1.dat missing or corrupted."),NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
		return false;
	}

	//load kaiki2.dat
	CCommonFunctionSystem::GetFileMD5(md5,"kaiki2.dat");
	for (int i=0;i<16;i++)
		if (md5[i]!=dat2MD5[i])
		{
			MessageBox(m_gameWindow.m_hWnd,_T("kaiki2.dat missing or corrupted."),NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
			return false;
		}
	if (m_th5Dat2.LoadFile("kaiki2.dat")==false)
	{
		MessageBox(m_gameWindow.m_hWnd,_T("kaiki2.dat missing or corrupted."),NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
		return false;
	}

	//load zun.com
	CCommonFunctionSystem::GetFileMD5(md5,"zun.com");
	for (int i=0;i<16;i++)
		if (md5[i]!=ZUNCOMMD5[i])
		{
			MessageBox(m_gameWindow.m_hWnd,_T("zun.com missing or corrupted."),NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
			return false;
		}
	if (th5w::CTh5ExtFont::LoadZUNCOM("zun.com")==false)
	{
		MessageBox(m_gameWindow.m_hWnd,_T("zun.com missing or corrupted."),NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
		return false;
	}


	//load mod file
	if (GVar().m_bUseMod)
		if (m_modDat.LoadFile(GVar().m_modFileName)==false)
		{
			//printf("%s\n",GVar().m_modFileName);
			TCHAR msgText[1000];
			wsprintf(msgText,_T("mod file %S missing or corrupted."),GVar().m_modFileName);
			MessageBox(m_gameWindow.m_hWnd,msgText,NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
			return false;
		}
		else
		{
			LoadModConfig();
		}

	//load anex86 font
	if (GVar().m_bUseSystemFont==false&&th5w::CPC98Font::LoadAnex86BMP("anex86.bmp")==false)
	{
		MessageBox(m_gameWindow.m_hWnd,_T("anex86.bmp not found. Using system font instead."),NULL,MB_OK|MB_APPLMODAL|MB_ICONWARNING);
		GVar().m_bUseSystemFont=true;
	}

	//create and select system font
	if (GVar().m_bUseSystemFont)
	{
		HFONT hFont,hOldFont;
		HDC hDC=wglGetCurrentDC();
		glDisable(GL_TEXTURE_2D);
		hFont = CreateFont(	-16,							// Height Of Font
							0,								// Width Of Font
							0,								// Angle Of Escapement
							0,								// Orientation Angle
							0,								// Font Weight
							FALSE,							// Italic
							FALSE,							// Underline
							FALSE,							// Strikeout
							CGame::GVar().m_fontCharset,	// Character Set Identifier
							OUT_DEFAULT_PRECIS,				// Output Precision
							CLIP_DEFAULT_PRECIS,			// Clipping Precision
							DEFAULT_QUALITY,				// Output Quality
							FF_DONTCARE|FIXED_PITCH,		// Family And Pitch
							NULL);							// Font Name
		if (hFont==NULL)
		{
			TCHAR msgText[1000];
			wsprintf(msgText,
					 _T("Could not find a font in your system that supports the charset required: %d."),
					 CGame::GVar().m_fontCharset);
			MessageBox(m_gameWindow.m_hWnd,msgText,NULL,MB_OK|MB_APPLMODAL|MB_ICONSTOP);
			return false;
		}
		hOldFont=(HFONT)SelectObject(hDC,hFont);
		CPC98Font::CreateTextureForCommonChar();
	}

	//disable screen saver
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,FALSE,NULL,0);

	//force our program to run on the first CPU
	//this is to avoid potential bugs in the BIOS/HAL that cause wrong results of QueryPerformanceCounter
	HANDLE hThread=GetCurrentThread();
	SetThreadAffinityMask(hThread,1);
	//SetThreadPriority(hThread,THREAD_PRIORITY_ABOVE_NORMAL);
	m_fpsLimit=60;
	QueryPerformanceCounter(&m_lastFrameTime);
	QueryPerformanceFrequency(&m_performanceFreq);

	//set random seed
	srand((unsigned)time(NULL));

	ShowWindow(m_gameWindow.m_hWnd,SW_SHOW);

	return true;
}

void CGame::Run()
{
	s_pCurGame=this;
	if (Initialize()==false)
	{
		m_gameWindow.Destroy();
		return;
	}

	CSchemeSwitcher::GetFirstScheme(&m_pCurScheme,&m_curSchemeID);

	ACCEL accel[]={FALT|FVIRTKEY, VK_RETURN, 0};
	HACCEL HAccel=CreateAcceleratorTable(accel, sizeof(accel)/sizeof(ACCEL));

	MSG	msg;										// Windows Message Structure
	bool done=false;
	m_thisFrameLostTime=0.0f;
	int nMsg=0;
	while(!done)									// Loop That Runs While done=FALSE
	{
		if (m_gameWindow.m_bActive==false)
		{
			if (GetMessage(&msg,NULL,0,0)==0)
			{
				done=true;
				continue;
			}
			if (!TranslateAccelerator(m_gameWindow.m_hWnd, HAccel, &msg))
			{
				TranslateMessage(&msg);			// Translate The Message
				DispatchMessage(&msg);			// Dispatch The Message
			}
			continue;
		}
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			nMsg++;
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=true;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				if (!TranslateAccelerator(m_gameWindow.m_hWnd, HAccel, &msg))
				{
					TranslateMessage(&msg);			// Translate The Message
					DispatchMessage(&msg);			// Dispatch The Message
				}
			}
		}
		else										// If There Are No Messages
		{
			if (m_gameWindow.m_bActive==false)
				continue;

			//LARGE_INTEGER a;
			//QueryPerformanceCounter(&a);
			int stepRes=m_pCurScheme->Step();
			//LARGE_INTEGER b;
			//QueryPerformanceCounter(&b);
			//printf("%lf\n",(double)m_performanceFreq.QuadPart/(b.QuadPart-a.QuadPart));

			m_gameWindow.m_pGLContext->MakeCurrent();
			m_pCurScheme->Draw();
			glFinish();

			CPMDPlayer::FillSoftwareBuffer(1000);

			{
				LARGE_INTEGER thisFrameTime;
				QueryPerformanceCounter(&thisFrameTime);
				double frameQuota=1000/m_fpsLimit;
				//printf("%lf ",frameQuota-(thisFrameTime.QuadPart-m_lastFrameTime.QuadPart)
				//			   /(double)m_performanceFreq.QuadPart*1000);
			}
			//fps limit
			int fillcount=0;
			while(1)
			{
				LARGE_INTEGER thisFrameTime;
				QueryPerformanceCounter(&thisFrameTime);
				if ((double)m_performanceFreq.QuadPart/(thisFrameTime.QuadPart-m_lastFrameTime.QuadPart)<=m_fpsLimit)
				{
					//CPMDPlayer::SetPlayThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
					m_thisFrameDuration=(float)((thisFrameTime.QuadPart-m_lastFrameTime.QuadPart)
											   /(double)m_performanceFreq.QuadPart);
					m_thisFrameDuration-=m_thisFrameLostTime;
					m_lastFrameTime=thisFrameTime;
					m_thisFrameLostTime=0.0f;
					//printf("%d %d\n",fillcount,nMsg);
					nMsg=0;
					break;
				}
				else
				{
					double frameQuota=1000/m_fpsLimit;
					if (frameQuota-(thisFrameTime.QuadPart-m_lastFrameTime.QuadPart)
						/(double)m_performanceFreq.QuadPart*1000>4)
						if (CPMDPlayer::FillSoftwareBuffer(1000))
							fillcount++;
				}
			}

			m_gameWindow.m_pGLContext->SwapBuffers();
			UpdateFPS();

			if (stepRes!=0)
			{
				CScheme *pNextScheme;
				int nextSchemeID;
				if (CSchemeSwitcher::SwitchScheme(&pNextScheme,&nextSchemeID,m_curSchemeID,stepRes)==true)
				{
					delete m_pCurScheme;
					m_pCurScheme=pNextScheme;
					m_curSchemeID=nextSchemeID;
					if (pNextScheme==NULL)
						break;
				}
			}
		}
	}
	OnGameEnd();
}

void CGame::OnDeactivate()
{
	CPMDPlayer::OnDeactivate();
	QueryPerformanceCounter(&m_lastDeactivateTime);

	//enable screen saver
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,TRUE,NULL,0);

}

void CGame::OnActivate()
{
	//disable screen saver again
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,FALSE,NULL,0);

	CPMDPlayer::OnActivate();
	LARGE_INTEGER curTime;
	QueryPerformanceCounter(&curTime);
	m_thisFrameLostTime+=(float)((curTime.QuadPart-m_lastFrameTime.QuadPart)
								/(double)m_performanceFreq.QuadPart);

	//In some cases(e.g. press the win key to inactive the window and win key again to activate),
	//the WM_ACTIVATE message is not received via the GetMessage/PeekMessage mechanism (don't know why).
	//Therefore, it is possible that at this point, the window is activated (m_bAcitve==true) but the
	//main thread is still stuck in GetMessage, so we have to send a message to wake it up.
	//Any message would make sense.
	PostMessage(m_gameWindow.m_hWnd,WM_PAINT,0,0);
}

void CGame::OnGameEnd()
{
	m_globalVar.SaveConfig();
	CPC98Font::FinalizeCache();
	CPC98Font::DestroyTextureForCommonChar();
	//enable screen saver
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,TRUE,NULL,0);
	m_gameWindow.Destroy();
}

void CGame::OnPaintInactivate()
{
	m_gameWindow.m_pGLContext->MakeCurrent();
	if (m_pCurScheme)
		m_pCurScheme->Draw();
	m_gameWindow.m_pGLContext->SwapBuffers();
}

}
