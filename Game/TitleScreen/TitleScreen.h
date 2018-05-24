#pragma once
#include "..\scheme.h"
#include "../../gameLib/Graphic/2DImageArray.h"
#include "TitleRollingTama.h"

#define TITLESCREEN_END_QUIT_GAME 1
#define TITLESCREEN_END_START_GAME 2
#define TITLESCREEN_END_START_EXTRA 3
#define TITLESCREEN_END_CHOOSE_REPLAY 4
#define TITLESCREEN_END_VIEW_HIGHSCORE 5

namespace th5w{

class CTitleScreen : public CScheme
{
protected:
	int m_curFrame;

	C2DImage *m_pMenuImgPractice;
	C2DImage *m_pMenuImgReplay;
	th5w::C2DImageArray m_spriteArray;
	th5w::C2DImageArray m_patternArray;
	unsigned char m_globalPalette[48];

	int m_curScrFade;

	int m_flagRollingTama;		//0 for not played, 1 for playing, 2 for played
	CTitleRollingTama m_tama;

	bool m_bInitWithRollTama;

	C2DImage *m_pLastTamaWithAlpha;
	th5w::C2DImage *m_pBGImage;
	int m_bgFade;

	//menu vars
	int m_mainMenuNItem;
	int m_mainMenuCursorPosition;
	bool m_bMainMenuItemEnabled[6];

	int m_optionMenuNItem;
	int m_optionMenuCursorPosition;
	bool m_bOptionMenuItemEnabled[6];

	int m_curMenu;			//1 for main menu, 1 for option menu
    bool m_bMenuOn;

	unsigned short m_lastKeyState;

protected:
	bool m_bQuit;
	int m_quitCode;

public:
	void Initialize(bool bRollTama,int cursorInitialPos=0, bool bSwitchMusic=true);
	virtual int Step();
	virtual void Draw();

protected:
	void DrawBackground();
	void DrawMainMenuItems();
	void DrawOptionMenuItems();
	void DrawMainMenuCursor();
	void DrawOptionMenuCursor();
	void OnRollTamaEnd();
	void ParseKeyEvent();
	void DrawDescString(char *descStr);

protected:
	__forceinline void ChangeValue(int *value, int deltaValue, int minValue,int maxValue)
	{
		(*value)+=deltaValue;
		if ((*value)<minValue) (*value)+=maxValue-minValue+1;
		if ((*value)>maxValue) (*value)-=maxValue-minValue+1;
	}

protected:
	void MoveCursorInMenu(int *cursorPos,int deltaPos,int nItem,bool *bItemMask);

public:
	CTitleScreen(void);
	virtual ~CTitleScreen(void);
};

}
