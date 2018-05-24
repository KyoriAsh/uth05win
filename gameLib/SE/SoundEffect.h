#pragma once
#include <windows.h>
#include <vector>
#include "../../include/DSUtil/dsutil.h"
using namespace std;

#pragma comment(lib,"Winmm.lib")
//#pragma comment(lib,"dxerr9.lib")

namespace th5w{

class CSoundEffect
{
protected:
	CSoundManager m_manager;
	vector<CSound*> m_vpSound;
public:
	bool Initialize(HWND hWnd);
	void PlaySound(int soundIdx);
public:
	CSoundEffect(void);
	~CSoundEffect(void);
};

}