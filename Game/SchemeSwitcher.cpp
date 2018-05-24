#include ".\schemeswitcher.h"
#include "OP/Opening.h"
#include "TitleScreen/TitleScreen.h"
#include "SelectCharaScreen/SelectCharaScreen.h"
#include "Stage/Stage.h"
#include "Game.h"
#include "ed/Ending.h"
#include "SelectReplayScreen/SelectReplayScreen.h"
#include "HighScoreScreen/HighScoreScreen.h"


#define SCHEME_DUMMY 0
#define SCHEME_OPENING 1
#define SCHEME_TITLESCREEN 2
#define SCHEME_SELECTCHARASCREEN 3
#define SCHEME_STAGE 4
#define SCHEME_ENDING 5
#define SCHEME_SELECTREPLAYSCREEN 6
#define SCHEME_HIGHSCORESCREEN 7

namespace th5w{

CSchemeSwitcher::CSchemeSwitcher(void)
{
}

CSchemeSwitcher::~CSchemeSwitcher(void)
{
}

bool CSchemeSwitcher::SwitchScheme(CScheme** ppOutNextScheme,int *pOutNextSchemeID, int curSchemeID, int curSchemeExitValue)
{
	if (curSchemeExitValue==1&&curSchemeID==SCHEME_OPENING)
	{
		CTitleScreen *pScheme=new CTitleScreen;
		pScheme->Initialize(true);
		*ppOutNextScheme=(CScheme*)pScheme;
		*pOutNextSchemeID=SCHEME_TITLESCREEN;
		return true;
	}

	if (curSchemeID==SCHEME_TITLESCREEN)
	{
		if (curSchemeExitValue==TITLESCREEN_END_QUIT_GAME)
		{
			*ppOutNextScheme=NULL;
			return true;
		}
		if (curSchemeExitValue==TITLESCREEN_END_START_GAME||curSchemeExitValue==TITLESCREEN_END_START_EXTRA)
		{
			CSelectCharaScreen *pScheme=new CSelectCharaScreen;
			pScheme->Initialize();
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_SELECTCHARASCREEN;
			return true;
		}
		if (curSchemeExitValue==TITLESCREEN_END_CHOOSE_REPLAY)
		{
			CSelectReplayScreen *pScheme=new CSelectReplayScreen;
			pScheme->Initialize(true,false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_SELECTREPLAYSCREEN;
			return true;
		}
		if (curSchemeExitValue==TITLESCREEN_END_VIEW_HIGHSCORE)
		{
			CHighScoreScreen *pScheme=new CHighScoreScreen;
			pScheme->Initialize(true,false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_HIGHSCORESCREEN;
			return true;
		}
	}

	if (curSchemeID==SCHEME_SELECTCHARASCREEN)
	{
		if (curSchemeExitValue==SELECTCHARASCREEN_END_SELECTED_CHARA)
		{
			CStage *pScheme=new CStage;
			pScheme->Initialize();
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_STAGE;
			return true;
		}
		if (curSchemeExitValue==SELECTCHARASCREEN_END_BACK)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(false,CGame::GVar().m_playDifficulty==4?1:0,false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
	}
	if (curSchemeID==SCHEME_SELECTREPLAYSCREEN)
	{
		if (curSchemeExitValue==SELECTREPLAYSCREEN_END_SELECTED_REPLAY)
		{
			CStage *pScheme=new CStage;
			pScheme->Initialize();
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_STAGE;
			return true;
		}
		if (curSchemeExitValue==SELECTREPLAYSCREEN_END_BACK)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(false,2,false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
		if (curSchemeExitValue==SELECTREPLAYSCREEN_END_SAVE_REPLAY_END||curSchemeExitValue==SELECTREPLAYSCREEN_END_SAVE_REPLAY_SKIPPED)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(false,0,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
	}

	if (curSchemeID==SCHEME_STAGE)
	{
		if (curSchemeExitValue==STAGE_END_CLEAR)
		{
			if (CGame::GVar().m_playStage<5)
			{
				CGame::GVar().m_playStage++;
				CStage *pScheme=new CStage;
				pScheme->Initialize();
				*ppOutNextScheme=(CScheme*)pScheme;
				*pOutNextSchemeID=SCHEME_STAGE;
				return true;
			}
			else
			{
				CEnding *pScheme=new CEnding;
				if (CGame::GVar().m_playStage==5)
					pScheme->Initialize(CGame::GVar().m_nContinueUsed==0?1:0);
				else
					pScheme->Initialize(2);
				CGame::GVar().OnClear();
				CGame::GVar().SaveConfig();
				*ppOutNextScheme=(CScheme*)pScheme;
				*pOutNextSchemeID=SCHEME_ENDING;
				return true;
			}
		}
		if (curSchemeExitValue==STAGE_END_ALL_MISSED)
		{
			CHighScoreScreen *pScheme=new CHighScoreScreen;
			pScheme->Initialize(false,false);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_HIGHSCORESCREEN;
			return true;
		}
		if (curSchemeExitValue==STAGE_END_PLAYER_QUIT)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(true,0,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
		if (curSchemeExitValue==STAGE_END_REPLAY_END)
		{
			CSelectReplayScreen *pScheme=new CSelectReplayScreen;
			pScheme->Initialize(true,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_SELECTREPLAYSCREEN;
			return true;
		}
	}

	if (curSchemeID==SCHEME_ENDING)
	{
		CHighScoreScreen *pScheme=new CHighScoreScreen;
		pScheme->Initialize(false,true);
		*ppOutNextScheme=(CScheme*)pScheme;
		*pOutNextSchemeID=SCHEME_HIGHSCORESCREEN;
		return true;
	}
	if (curSchemeID==SCHEME_HIGHSCORESCREEN)
	{
		if (curSchemeExitValue==HIGHSCORESCREEN_END_ENTER_END||curSchemeExitValue==HIGHSCORESCREEN_END_ENTER_SKIPPED)
		{
			CSelectReplayScreen *pScheme=new CSelectReplayScreen;
			if (curSchemeExitValue==HIGHSCORESCREEN_END_ENTER_END)
				pScheme->Initialize(false,false);
			else
				pScheme->Initialize(false,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_SELECTREPLAYSCREEN;
			return true;
		}
		if (curSchemeExitValue==HIGHSCORESCREEN_END_VIEW_END)
		{
			CTitleScreen *pScheme=new CTitleScreen;
			pScheme->Initialize(false,3,true);
			*ppOutNextScheme=(CScheme*)pScheme;
			*pOutNextSchemeID=SCHEME_TITLESCREEN;
			return true;
		}
	}

	return false;
}

bool CSchemeSwitcher::GetFirstScheme(CScheme** ppOutNextScheme,int *pOutNextSchemeID)
{
	COpening *pOP=new COpening;
	pOP->Initialize();
	*ppOutNextScheme=pOP;
	*pOutNextSchemeID=SCHEME_OPENING;

	return true;
}

}
