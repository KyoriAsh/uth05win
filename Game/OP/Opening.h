#pragma once
#include <windows.h>
#include "../scheme.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "OpParticle.h"

namespace th5w{

class COpening : public CScheme
{
protected:
	int m_curFrame;

	th5w::C2DImageArray m_spriteArray;
	th5w::C2DImageArray m_patternArray;
	th5w::C2DImage *m_pBGImage;
	unsigned char m_globalPalette[48];

	int m_curScrFade;

	COpParticle m_particle;

	bool m_bKeyPressed;
	bool m_playSound15;

public:
	void Initialize();
	virtual int Step();
	virtual void Draw();
public:
	COpening(void);
	virtual ~COpening(void);
};

}
