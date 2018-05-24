#pragma once
#include "../scheme.h"
#include "../../gameLib/Graphic/2DImageArray.h"
#include "../Replay.h"

#define SELECTREPLAYSCREEN_END_SELECTED_REPLAY 1
#define SELECTREPLAYSCREEN_END_BACK 2
#define SELECTREPLAYSCREEN_END_SAVE_REPLAY_END 3
#define SELECTREPLAYSCREEN_END_SAVE_REPLAY_SKIPPED 4

namespace th5w{

class CSelectReplayScreen : public CScheme
{
protected:
	bool m_bLoadMode;
	int m_curFrame;
	unsigned short m_lastKeyState,m_curKeyState;

protected:
	int m_nFilePerPage;
	int m_nPage;
	int m_curPage;
	int m_curCursorPos;
	int m_curCursorColPos;
	int m_listUpperLeftX;
	int m_listUpperLeftY;
	bool m_bCurPageRepFileExist[50];
	REPLAY_BRIEF_INFO m_repInfo[50];

	th5w::C2DImageArray m_shineArray;
	C2DImage *m_pBGImage;

	bool m_bEnterNameMode;
	int m_curCharX;
	int m_curCharY;
	int m_nameCursorPos;
	int m_selectedSlot;
	int m_curRowY;
	char m_curEnterName[10];

	int m_curScrFade;
	bool m_bQuit;
	int m_quitCode;

protected:
	void SwitchPage(int pageIdx);
	void StepEnterNameMode();
	void DrawEnterNameMode();

public:
	void Initialize(bool bLoadMode,bool bSwitchMusic);
	int Step();
	void Draw();

public:
	CSelectReplayScreen(void);
	~CSelectReplayScreen(void);
};

}
