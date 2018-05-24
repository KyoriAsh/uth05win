#pragma once
#include <windows.h>
#include "../../gameLib/Graphic/2DImage.h"

namespace th5w{

class CTitleRollingTama
{
protected:
	int m_curScrFade;
	int m_curFrame;
	C2DImage *m_rollingImg[8];
	unsigned char m_palette[48];

public:
	void Initialize();
	int Step();
	void Draw();

public:
	CTitleRollingTama(void);
	~CTitleRollingTama(void);
};

}
