#pragma once

namespace th5w{

class CStage;

class CSummary
{
friend class CStage;

protected:
	CStage *m_pStage;
	bool m_bRunning;
	int m_curFrame;

public:
	__forceinline void Run() {m_bRunning=true;}
	__forceinline void Stop() {m_bRunning=false;}
	void Initialize();
	void Step();
	void Draw();

protected:
	void StepNormalSummary();
	void StepEndGameSummary();
	void DrawNormalSummary();
	void DrawEndGameSummary();
	void DrawItem(int textIdx,int row,bool useRed);

public:
	CSummary(CStage *pStage);
	~CSummary(void);
};

}
