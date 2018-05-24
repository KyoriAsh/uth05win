#include ".\pmdplayer.h"
#include "..\..\include\PMDWin\PMDWinImort.h"

#pragma comment(lib,"lib/PMDWin.lib")

namespace th5w{

LPDIRECTSOUND8 CPMDPlayer::s_pDSound8=NULL;
LPDIRECTSOUNDBUFFER8 CPMDPlayer::s_pDSoundBuffer8=NULL;
HANDLE CPMDPlayer::s_hEvent[s_bufNSeg+1];
bool CPMDPlayer::s_bMusicDataExist=false;
DWORD CPMDPlayer::s_playThreadID=0;
HANDLE CPMDPlayer::s_hPlayThread=NULL;
bool CPMDPlayer::s_bPlayingWhenLastDeactivate=false;
bool CPMDPlayer::s_bInitialized=false;
int CPMDPlayer::s_localBufferReadHead;
int CPMDPlayer::s_localBufferWriteHead;
unsigned char CPMDPlayer::s_localBuffer[s_localBufferLength];
CRITICAL_SECTION CPMDPlayer::s_csLocalBuf;


#define BUFFER_SEG_LENGTH 44100

CPMDPlayer::CPMDPlayer(void)
{
}

CPMDPlayer::~CPMDPlayer(void)
{
}

bool CPMDPlayer::InitPMDWin()
{
	bool initres=pmdwininit(NULL);
	if (initres==false)
		return false;

	setpcmrate(SOUND_44K);

	return true;
}
LPDIRECTSOUNDBUFFER8 CPMDPlayer::CreateBasicBuffer() 
{ 
	WAVEFORMATEX waveFormatEx; 
	waveFormatEx.wFormatTag=WAVE_FORMAT_PCM;
	waveFormatEx.nChannels=2;
	waveFormatEx.nSamplesPerSec=44100;
	waveFormatEx.nBlockAlign=4;
	waveFormatEx.nAvgBytesPerSec=176400;
	waveFormatEx.wBitsPerSample=16;
	waveFormatEx.cbSize=0;

	// Set up DSBUFFERDESC structure.
	DSBUFFERDESC dsBufDesc; 
	dsBufDesc.dwSize=sizeof(DSBUFFERDESC);
	dsBufDesc.dwFlags=DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|
					  DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPOSITIONNOTIFY|DSBCAPS_LOCSOFTWARE;
/*
	explanation of why use DSBCAPS_LOCSOFTWARE here:
	Due to a well know bug in Realtek sound drivers, a sound buffer receives notification for every
	sound buffer on the system. That is, if there's another running program using directsound, your
	sound buffer get noticed even for those buffers. What's more, there's no way to know that the
	notification is for another buffer except call GetCurrentPosition to check the play cursor of
	the current sound buffer. This is not a good way to do so because when GetCurrentPosition is
	called, the play cursor has already moves a bit as sound is being played at background. Besides
	that, there's possibility that such wrong notifications just occurs at the "right" position, in
	which case the above method would not help. Hence, the best way to avoid this is to force use
	a software buffer.
	For a bit more information, see:
	http://www.eggheadcafe.com/forumarchives/win32programmerdirectxaudio/Jun2005/post23436521.asp
*/

	dsBufDesc.dwBufferBytes=BUFFER_SEG_LENGTH*s_bufNSeg;
	dsBufDesc.lpwfxFormat=&waveFormatEx;
	dsBufDesc.dwReserved=0;
	dsBufDesc.guid3DAlgorithm=GUID_NULL;

	LPDIRECTSOUNDBUFFER pDSoundBuf;
	if (s_pDSound8->CreateSoundBuffer(&dsBufDesc, &pDSoundBuf, NULL)!=DS_OK)
		return NULL;

	LPDIRECTSOUNDBUFFER8 ret;
	if (pDSoundBuf->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&ret)!=S_OK)
	{
		pDSoundBuf->Release();
		return NULL;
	}
	pDSoundBuf->Release();

	return ret;
}

bool CPMDPlayer::Initialize(HWND hWnd)
{
	if (s_pDSound8!=NULL)
		return false;

	if (DirectSoundCreate8(NULL, &s_pDSound8, NULL)!=DS_OK)
		return false;
	if (s_pDSound8->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)!=DS_OK)
	{
		s_pDSound8->Release();
		s_pDSound8=NULL;
		return false;
	}

	s_pDSoundBuffer8=CreateBasicBuffer();
	if (s_pDSoundBuffer8==NULL)
		return false;

	LPDIRECTSOUNDNOTIFY8 pDSoundNotify8;
	if (s_pDSoundBuffer8->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&pDSoundNotify8)!=S_OK)
	{
		s_pDSoundBuffer8->Release();
		return false;
	}
	DSBPOSITIONNOTIFY notify[s_bufNSeg];
	for (int i=0;i<s_bufNSeg;i++)
	{
		notify[i].dwOffset=i*BUFFER_SEG_LENGTH;
		s_hEvent[i]=CreateEvent(NULL,FALSE,FALSE,NULL);
		notify[i].hEventNotify=s_hEvent[i];
	}
	if (pDSoundNotify8->SetNotificationPositions(s_bufNSeg,notify)!=DS_OK)
		return false;

	if (InitPMDWin()==false)
		return false;

	s_hEvent[s_bufNSeg]=CreateEvent(NULL,FALSE,FALSE,NULL);

	InitializeCriticalSection(&s_csLocalBuf);
	s_localBufferReadHead=0;
	s_localBufferWriteHead=0;

	s_bInitialized=true;
	return true;
}

void CPMDPlayer::Finalize()
{
	if (s_pDSoundBuffer8!=NULL)
		s_pDSoundBuffer8->Release();
	s_pDSoundBuffer8=NULL;
	if (s_pDSound8!=NULL)
		s_pDSound8->Release();
	s_pDSound8=NULL;
}

bool CPMDPlayer::LoadPMDData(unsigned char *musicData,int size)
{
	if (s_bMusicDataExist)
	{
		Pause();
		if (UnloadPMDData()==false)
			return false;
		s_bMusicDataExist=false;
	}

	if (music_load2(musicData,size)!=PMDWIN_OK)
		return false;

	music_start();
	s_localBufferReadHead=0;
	s_localBufferWriteHead=0;

	void *pWriteBuf;
	DWORD writeBufSize;
	if (s_pDSoundBuffer8->Lock(0,0,&pWriteBuf,&writeBufSize,NULL,NULL,DSBLOCK_ENTIREBUFFER)!=DS_OK)
		return false;

	getpcmdata((short *)pWriteBuf,(BUFFER_SEG_LENGTH/4)*s_bufNSeg);
	s_pDSoundBuffer8->Unlock(pWriteBuf,writeBufSize,NULL,0);

	s_hPlayThread=CreateThread(NULL,0,PlayThread,NULL,0,&s_playThreadID);
	s_bMusicDataExist=true;
	return true;
}

bool CPMDPlayer::UnloadPMDData()
{
	if (s_bMusicDataExist==false)
		return false;

	Pause();
	SetEvent(s_hEvent[s_bufNSeg]);
	WaitForSingleObject(s_hPlayThread,INFINITE);
	s_hPlayThread=NULL;
	s_playThreadID=0;

	music_stop();
	s_bMusicDataExist=false;

	return true;
}

void CPMDPlayer::Play()
{
	s_pDSoundBuffer8->SetCurrentPosition(0);
	if (s_pDSoundBuffer8->Play(0,0,DSBPLAY_LOOPING)==DSERR_BUFFERLOST)
	{
		s_pDSoundBuffer8->Restore();
		s_pDSoundBuffer8->Play(0,0,DSBPLAY_LOOPING);
	}
}

void CPMDPlayer::Pause()
{
	s_pDSoundBuffer8->Stop();
}

void CPMDPlayer::Resume()
{
	if (s_pDSoundBuffer8->Play(0,0,DSBPLAY_LOOPING)==DSERR_BUFFERLOST)
	{
		s_pDSoundBuffer8->Restore();
		s_pDSoundBuffer8->Play(0,0,DSBPLAY_LOOPING);
	}
}

void CPMDPlayer::SetPlayThreadPriority(int nPriority)
{
	if (s_hPlayThread!=NULL)
		SetThreadPriority(s_hPlayThread,nPriority);
}

bool CPMDPlayer::FillSoftwareBuffer(int nSample)
{
	if (s_bMusicDataExist==false)
		return false;
	EnterCriticalSection(&s_csLocalBuf);
	int testHead=s_localBufferReadHead;
	if (testHead<=s_localBufferWriteHead)
		testHead+=s_localBufferLength;
	if (nSample*4>=testHead-s_localBufferWriteHead-4)
		nSample=(testHead-s_localBufferWriteHead-4)/4;
	if (nSample==0)
	{
		LeaveCriticalSection(&s_csLocalBuf);
		return false;
	}

	if (nSample*4>s_localBufferLength-s_localBufferWriteHead)
	{
		int part0Sample=(s_localBufferLength-s_localBufferWriteHead)/4;
		int part1Sample=nSample-part0Sample;
		getpcmdata((short*)(s_localBuffer+s_localBufferWriteHead),part0Sample);
		getpcmdata((short*)(s_localBuffer),part1Sample);
		s_localBufferWriteHead=part1Sample*4;
	}
	else
	{
		getpcmdata((short*)(s_localBuffer+s_localBufferWriteHead),nSample);
		s_localBufferWriteHead+=nSample*4;
		s_localBufferWriteHead%=s_localBufferLength;
	}
	LeaveCriticalSection(&s_csLocalBuf);
	return true;
}

bool CPMDPlayer::CopyFromSoftwareBuffer(unsigned char *outBuf,int copyLen)
{
	EnterCriticalSection(&s_csLocalBuf);
	int testHead=s_localBufferWriteHead;
	if (testHead<s_localBufferReadHead)
		testHead+=s_localBufferLength;
	if (copyLen>=testHead-s_localBufferReadHead)
		copyLen=testHead-s_localBufferReadHead;
	if (copyLen==0)
	{
		LeaveCriticalSection(&s_csLocalBuf);
		return false;
	}

	if (copyLen>s_localBufferLength-s_localBufferReadHead)
	{
		int part0Len=s_localBufferLength-s_localBufferReadHead;
		int part1Len=copyLen-part0Len;
		memcpy(outBuf,s_localBuffer+s_localBufferReadHead,part0Len);
		memcpy(outBuf+part0Len,s_localBuffer,part1Len);
		s_localBufferReadHead=part1Len;
	}
	else
	{
		memcpy(outBuf,s_localBuffer+s_localBufferReadHead,copyLen);
		s_localBufferReadHead+=copyLen;
		s_localBufferReadHead%=s_localBufferLength;
	}
	LeaveCriticalSection(&s_csLocalBuf);
	return true;
}

DWORD CPMDPlayer::PlayThread(void *pParam)
{
	bool firstTime=true;
	while(1)
	{
		DWORD eventIdx=WaitForMultipleObjects(s_bufNSeg+1,s_hEvent,FALSE,20)-WAIT_OBJECT_0;
		if (eventIdx==WAIT_TIMEOUT)
		{
			FillSoftwareBuffer(1000);
			continue;
		}
		if (eventIdx==s_bufNSeg)
			break;
		if (eventIdx==0&&firstTime)
		{
			firstTime=false;
			continue;
		}
		if (eventIdx<s_bufNSeg)
		{
			void *pWriteBuf[2];
			DWORD writeBufSize[2];
			HRESULT hr=s_pDSoundBuffer8->Lock(((eventIdx-1+s_bufNSeg)%s_bufNSeg)*BUFFER_SEG_LENGTH,BUFFER_SEG_LENGTH,
											  &pWriteBuf[0],&writeBufSize[0],
											  &pWriteBuf[1],&writeBufSize[1],
											  0);
			if (hr!=DS_OK)
			{
				printf("err\n");
				if (hr==DSERR_BUFFERLOST)
				{
					s_pDSoundBuffer8->Restore();
					printf("restore\n");
					hr=s_pDSoundBuffer8->Lock(((eventIdx-1+s_bufNSeg)%s_bufNSeg)*BUFFER_SEG_LENGTH,BUFFER_SEG_LENGTH,
																&pWriteBuf[0],&writeBufSize[0],
																&pWriteBuf[1],&writeBufSize[1],
																0);
					if (hr!=DS_OK)
						continue;
				}
				else
					continue;
			}
			CopyFromSoftwareBuffer((unsigned char *)pWriteBuf[0],BUFFER_SEG_LENGTH);
			//getpcmdata((short *)pWriteBuf[0],BUFFER_SEG_LENGTH/4);
			s_pDSoundBuffer8->Unlock(pWriteBuf[0],writeBufSize[0],pWriteBuf[1],writeBufSize[1]);
		}
	}

	return 0;
}

void CPMDPlayer::SetFrequency(int newFreq)
{
	s_pDSoundBuffer8->SetFrequency(newFreq);
}

void CPMDPlayer::SetVolume(float volume)
{
	if (volume>100.0f)
		volume=100.0f;
	if (volume<0.0f)
		volume=0.0f;
	int dsVolume=(int)((volume-100.0f)*20.0f);
	s_pDSoundBuffer8->SetVolume(dsVolume);
}

void CPMDPlayer::OnDeactivate()
{
	s_bPlayingWhenLastDeactivate=false;

	if (!s_bInitialized)
		return;
	DWORD dwStatus;
	if (s_pDSoundBuffer8->GetStatus(&dwStatus)!=DS_OK)
		return;
	if (dwStatus&DSBSTATUS_PLAYING)
	{
		s_bPlayingWhenLastDeactivate=true;
		Pause();
	}
}

void CPMDPlayer::OnActivate()
{
	if (s_bPlayingWhenLastDeactivate==true)
		Resume();
}

}





















