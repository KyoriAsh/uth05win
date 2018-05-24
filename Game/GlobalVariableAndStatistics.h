#pragma once
#include "Replay.h"
#include <tchar.h>

namespace th5w{

class CGlobalVariableAndStatistics
{
public:
//program macro
	int m_gameVersion;

//program config
	unsigned int m_textCodePage;
	bool m_bUseSystemFont;
	unsigned int m_fontCharset;

//mod
	bool m_bUseMod;
	char m_modFileName[200];
	unsigned char m_modMD5[16];

//these variables should be initialized before OnBeginGame

	//loaded from config
	bool m_bClear[4][5];
	bool m_bExtraPlayable[4];
	unsigned char m_defaultHighScoreName[10];
	char m_defaultReplayName[10];

	//loaded from config and can be change in main menu
	int m_nInitLife;
	int m_nInitBomb;
	int m_initDifficulty;

	void LoadConfig();
	void LoadProgramConfig();
	void SaveConfig();
	void OnClear();

	//loaded from score file
	unsigned char m_highScoreName[4][5][5][10];
	int m_highScore[4][5][5];
	unsigned char m_highScoreFlag[4][5][5];
	void LoadHighScore();
	void SaveHighScore();

public:
/* --------------variables above this line should not be accessed during game-------------- */

	//determined by user choice in main menu
	//make sure to set these values before call OnBeginGame
//this one must be set
	int m_playStage;
//either initialize this set of variables
	bool m_bReplayMode;
	CReplay m_replay;
//or this set
	bool m_bPracticeMode;
	int m_playDifficulty;
	int m_playChara;

//these variables are initialized by OnBeginGame

	//changeable during game
	int m_nCurLife;
	int m_nCurBomb;
	int m_curPower;
	int m_powerOverflowLevel;
	int m_curDream;
	int m_curScore;
	int m_totalNPointItem;
	int m_curNGraze;
	int m_playerPerformance;
	int m_curHighScore;
	int m_nContinueUsed;

	//constant during game
	int m_nContinueLimit;
	int m_pointItemMaxScore;
	int m_grazeBonus;
	int m_playerPerformanceLowerBound;
	int m_playerPerformanceUpperBound;
	int m_nLifeOnContinue;
	int m_nBombOnMiss;
	int m_randomSeed[7];

	bool m_bCanSaveReplay;

//environment
	char m_workingPath[1000];
	char m_replaySubDir[100];
public:
	void Initialize();
	void OnBeginGame();
public:
	CGlobalVariableAndStatistics(void);
	~CGlobalVariableAndStatistics(void);
};

}
