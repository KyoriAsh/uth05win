#include ".\commonfunctionsystem.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <tchar.h>
#include "../Misc/md5.h"
#include "../Misc/sha1.h"

namespace th5w{

CCommonFunctionSystem::CCommonFunctionSystem(void)
{
}

CCommonFunctionSystem::~CCommonFunctionSystem(void)
{
}

void CCommonFunctionSystem::CreateConsoleWindow()
{
	int  hConHandle;
	long lStdHandle;
	FILE   *fp;
	// allocate a console for this appc
	AllocConsole();
	// set the screen buffer to be big enough to let us scroll text
	//    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),	&coninfo);
	//  coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	// How many lines do you want to have in the console buffer
	//    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),	coninfo.dwSize);
	// redirect unbuffered STDOUT to the console
	//    g_hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	lStdHandle = (long)(__int64)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 ); 
	// redirect unbuffered STDIN to the console
	lStdHandle = (long)(__int64)GetStdHandle(STD_INPUT_HANDLE); 
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" ); 
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	// redirect unbuffered STDERR to the console
	lStdHandle = (long)(__int64)GetStdHandle(STD_ERROR_HANDLE); 
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	SetConsoleTitle(_T("Console")); 
	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog m_lastPoint to console as well
	// Uncomment the next line if you are using c++ cio or comment if you don't
}

bool CCommonFunctionSystem::GetFileMD5(unsigned char *retMD5,const char *fileName)
{
	MD5_CTX ctxMD5;
	MD5Init(&ctxMD5);
	FILE *fp=fopen(fileName,"rb");
	if (fp==NULL)
		return false;
	int blockSize=1024*128;
	unsigned char *buffer=new unsigned char[blockSize];
	if (buffer==NULL)
	{
		fclose(fp);
		return false;
	}
	int nRead;
	while((nRead=(int)fread(buffer,1,blockSize,fp))>0)
		MD5Update(&ctxMD5,buffer,nRead);
	delete buffer;
	MD5Final(retMD5,&ctxMD5);

	return true;
}

}































