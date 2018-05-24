#pragma once
#include "../scheme.h"
#include "../../gameLib/Graphic/2DImageArray.h"

#define SELECTCHARASCREEN_END_SELECTED_CHARA 1
#define SELECTCHARASCREEN_END_BACK 2

namespace th5w{

class CSelectCharaScreen : public CScheme
{
protected:
	bool m_bCharaSelectable[4];
	int m_charaDisplayImg[4];
	bool m_bCharaClearFlag[4];

	unsigned char m_palette[48];

protected:
	int m_curScrFade;
	int m_cursorPos;
protected:
	C2DImageArray m_spriteArray;
	C2DImage *m_pBGImage;

protected:
	bool m_bQuit;
	int m_quitCode;

	unsigned short m_lastKeyState;

public:
	void Initialize();
	virtual int Step();
	void Draw();

protected:
	void ParseKeyEvent();

public:
	CSelectCharaScreen(void);
	virtual ~CSelectCharaScreen(void);
};

}