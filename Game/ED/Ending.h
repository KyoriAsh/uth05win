#pragma once
#include <windows.h>
#include "../scheme.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include <vector>
using namespace std;

namespace th5w{

class CEnding : public CScheme
{
protected:
	unsigned short m_lastKey;
	unsigned short m_curKey;

	int m_curFrame;
	unsigned char *m_pScript;
	unsigned char *m_pScriptBase;

	C2DImage *m_pBGImage;
	C2DImage *m_pCenterImage;
	C2DImage *m_pCenterImageCandi;
	int m_curCenterImgIdx;

	unsigned char m_palette[48];
	unsigned char m_lastTextColorIdx;
	unsigned char m_curTextColorIdx;
	vector<unsigned char> m_lastText;
	vector<unsigned char> m_curText;
	vector<pair<vector<unsigned char>, unsigned char> > m_colorTable;
	int m_textFade;

	bool m_bWaitKey;
	bool m_bFading;
	float m_curScrScale;
	float m_tarScrScale;
	float m_deltaScrScale;
	bool m_bCtrImgFading;
	int m_ctrImgCurFade;
	int m_ctrImgTotalFade;
	float m_curMusicFade;
	bool m_bMusicFadeOut;

	bool m_bWaitNFrame;
	int m_nWaitFrameLeft;

protected:
	int ReadNumber(int defValue);
	int ReadNumberComma(int defValue);
	void DrawText(vector<unsigned char> &text,unsigned char colorIdx, int scale);

public:
	void Initialize(int edType);
	virtual int Step();
	virtual void Draw();
public:
	CEnding(void);
	virtual ~CEnding(void);
};

}
