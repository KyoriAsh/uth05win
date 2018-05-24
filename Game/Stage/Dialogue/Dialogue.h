#pragma once

namespace th5w{

class CStage;

class CDialogue
{
friend class CStage;

protected:
	CStage *m_pStage;
	bool m_bTranScript;
	unsigned char *m_pTranScript;

protected:
	unsigned char *m_pScript;
	bool m_bRunning;
	int m_dialogBGFadeTimer;
	int m_activeChara;			//0 for chara, 1 for boss
	int m_face[2];
	unsigned char *m_words[2];
	int m_wordNChar[2];
	int m_curWordTotalChar;

	int m_nBoss;
	int m_bossImg[2];
	int m_bossX[2];
	int m_bossY[2];

	bool m_bFading;
	bool m_bFadeToWhite;
	bool m_bInstantFade;

protected:
	int StepScript();
public:
	void Initialize();
	int Step();
	void Draw();
	void Run();
	__forceinline void SetDialogueScript(unsigned char *pScript) {m_pScript=pScript;}
	__forceinline bool IsRunning() {return m_bRunning;}

public:
	CDialogue(CStage *pStage);
	~CDialogue(void);
};

}