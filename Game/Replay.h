#pragma once
#include <vector>
#include <string>
#include <string.h>
using namespace std;

namespace th5w{

enum InfoResult{
	InfoRes_FileNotExist,
	InfoRes_InvalidReplay,
	InfoRes_GameVersionMismatch,
	InfoRes_ModFileMisMatch,
	InfoRes_ValidReplay
};
typedef struct strREPLAY_BRIEF_INFO{
	char playerName[8];
	unsigned int saveTime;
	int playChara;
	int playDifficulty;
	unsigned char stageFlag[7];
	int stageScore[7];
	InfoResult infoRes;
}REPLAY_BRIEF_INFO;

class CReplay
{
public:
	char m_playerName[8];
	unsigned int m_saveTime;
public:
	int m_nInitLife;
	int m_nInitBomb;
	int m_playDifficulty;
	int m_playChara;
	bool m_bPracticeMode;

public:
	unsigned char m_stageFlag[7];
	int m_nCurLife[7];
	int m_nCurBomb[7];
	int m_curPower[7];
	int m_powerOverflowLevel[7];
	int m_curDream[7];
	int m_curScore[7];
	int m_totalNPointItem[7];
	int m_playerPerformance[7];
	int m_stageScore[7];
	int m_randSeed[7];
	vector<unsigned short> m_keyState[7];

protected:
	int m_opIdx;
	int m_opStage;
	bool m_bOpRead;

public:
	static bool GetRepFileInfo(REPLAY_BRIEF_INFO *pOut, const char *fileName);
public:
	bool Reset();
	bool OpenStage(int stageIdx,bool bRead);
	bool CloseStage(bool bCleared=false);		//no need to pass bCleared for read mode
	bool GetNextData(unsigned short *pOut);
	bool PutNextdata(unsigned short data);
public:
	bool LoadFile(const char *fileName);
	bool SaveFile(const char *fileName);

public:
	CReplay(void);
	~CReplay(void);
};

}
