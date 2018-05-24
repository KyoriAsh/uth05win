/*
	As the PMDWin.dll is NOT thread-safe, this class has all members static and constructor private.
	construction/destruction is done via static functions Initialize/Finalize
	This class itself is also not thread safe. Be sure not to make simultaneous calls from multiple
	threads.
*/
#pragma once
#include <windows.h>
#include <dsound.h>
#include <vector>
using namespace std;
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")

namespace th5w{

class CPMDPlayer
{
protected:
	static LPDIRECTSOUND8 s_pDSound8;
	static LPDIRECTSOUNDBUFFER8 s_pDSoundBuffer8;
	const static int s_bufNSeg=5;
	static HANDLE s_hEvent[s_bufNSeg+1];
	static bool s_bMusicDataExist;
	static DWORD s_playThreadID;
	static HANDLE s_hPlayThread;
	static bool s_bInitialized;

	static CRITICAL_SECTION s_csLocalBuf;
	const static int s_localBufferLength=882000;		//keep this at least twice as large as BUFFER_SEG_LENGTH
														//which is defined in cpp
														//this value must also be multiple of 4
	static int s_localBufferReadHead;
	static int s_localBufferWriteHead;
	static unsigned char s_localBuffer[s_localBufferLength];

protected:
	static bool InitPMDWin();
public:
	static bool Initialize(HWND hWnd);
	static bool LoadPMDData(unsigned char *musicData,int size);
	static bool UnloadPMDData();
	static void Play();
	static void Pause();
	static void Resume();
	static void Finalize();
	static void SetFrequency(int newFreq);
	static void SetVolume(float volume);		//0 silence, 100 normal, no amplification

	static void OnDeactivate();
	static void OnActivate();

	static void SetPlayThreadPriority(int nPriority);
protected:
	static bool s_bPlayingWhenLastDeactivate;

public:
	static bool FillSoftwareBuffer(int nSample);		//1 sample occupies 4 bytes, return false if software buffer
														//is full
protected:
	static bool CopyFromSoftwareBuffer(unsigned char *outBuf,int copyLen);	//return false if software buffer in empty
	static DWORD WINAPI PlayThread(void *pParam);
	static LPDIRECTSOUNDBUFFER8 CreateBasicBuffer();
private:
	CPMDPlayer(void);
	~CPMDPlayer(void);
};

}
