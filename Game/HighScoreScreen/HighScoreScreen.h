#pragma once
#include "..\scheme.h"
#include "../../gameLib/Graphic/2DImageArray.h"

#define HIGHSCORESCREEN_END_VIEW_END 1
#define HIGHSCORESCREEN_END_ENTER_END 2
#define HIGHSCORESCREEN_END_ENTER_SKIPPED 3

namespace th5w{

class CHighScoreScreen : public CScheme
{
protected:
	unsigned char m_charTable[3][17];

protected:
	bool m_bViewMode;
	int m_curFrame;
	unsigned short m_lastKeyState,m_curKeyState;

	int m_curPage;
	int m_opChara;
	int m_opRank;
	unsigned char m_curEnterName[10];
	int m_curCursorPos;
	int m_curCharX,m_curCharY;

	unsigned char m_palette[48];
	C2DImage *m_pBGImage;

	C2DImageArray m_patternArray;
	C2DImageArray m_numberArray;

protected:
	int m_curScrFade;
	bool m_bQuit;
	int m_quitCode;

protected:
	void DrawItem(int x,int y,unsigned char name[10],int score,unsigned char flag,bool bHighLight);

public:
	void Initialize(bool bViewMode,bool clearFlag);
	int Step();
	void Draw();

public:
	CHighScoreScreen(void);
	virtual ~CHighScoreScreen(void);
};

}
