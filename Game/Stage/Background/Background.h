#pragma once

namespace th5w{

class CStage;

class CBackground
{
friend class CStage;

protected:
	CStage *m_pStage;

protected:
	unsigned char *m_mapTileIdxArray;
	int m_curPos;
	int m_curSpeed;
	int m_curDeltaPixel;
	bool m_bDraw;
	bool m_bForceDraw;
public:
	__forceinline int GetLastScrollDeltaPixel() {return m_curDeltaPixel;}
	__forceinline int GetCurScrollSpeed() {return m_curSpeed;}
	__forceinline void SetDraw(bool enable) {m_bDraw=enable;}
	__forceinline void ForceDrawFirstScreen(bool enable) {m_bForceDraw=enable;}
public:
	void Initialize();
	void Step();
	void Draw();
public:
	CBackground(CStage *pStage);
	~CBackground(void);
};

}