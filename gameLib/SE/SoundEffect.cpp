#include ".\soundeffect.h"
#include <stdio.h>
#include <tchar.h>

namespace th5w{

CSoundEffect::CSoundEffect(void)
{
}

CSoundEffect::~CSoundEffect(void)
{
}

bool CSoundEffect::Initialize(HWND hWnd)
{
	if (m_manager.Initialize(hWnd,DSSCL_PRIORITY)!=S_OK)
		return false;

	m_vpSound.clear();
	m_vpSound.push_back(NULL);
	CSound *pSound;
	for (int i=1;i<16;i++)
	{
		TCHAR fileName[1000];
		wsprintf(fileName,_T("SE/s%02d.wav"),i);
		if (m_manager.Create(&pSound,fileName,DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|
											  DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPOSITIONNOTIFY|DSBCAPS_LOCSOFTWARE)!=S_OK)
			return false;
		m_vpSound.push_back(pSound);
	}

	return true;
}

void CSoundEffect::PlaySound(int soundIdx)
{
	if (soundIdx<0||soundIdx>=(int)m_vpSound.size())
		return;
	if (m_vpSound[soundIdx]==NULL)
		return;

	m_vpSound[soundIdx]->Stop();
	m_vpSound[soundIdx]->Reset();
	m_vpSound[soundIdx]->Play();
}

}