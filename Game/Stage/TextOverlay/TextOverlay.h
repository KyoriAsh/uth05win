#pragma once
#include <stdio.h>

namespace th5w{

#define TEXT_OVERLAY_NONE 0
#define TEXT_OVERLAY_STAGE_FADE_IN 1
#define TEXT_OVERLAY_DRAW_STAGE_NAME_BGM_NAME 2
#define TEXT_OVERLAY_STAGE_FADE_OUT 3

class CStage;

class CTextOverlay
{
friend class CStage;

protected:
	CStage *m_pStage;
	int m_curMode;

	int m_stageFadeTimer;

	int m_stageNameBGMNameTimer;
	char m_stageName[100];
	char m_bgmName[100];

protected:
	void StepStageFadeIn();
	void DrawStageFadeIn();
	void StepStageNameBGMName();
	void DrawStageNameBGMName();
	void StepStageFadeOut();
	void DrawStageFadeOut();

public:
	void Initialize();
	void Step();
	void Draw();
	void SetMode(int mode,char *stageName=NULL,char *bgmName=NULL);
public:
	CTextOverlay(CStage *pStage);
	~CTextOverlay(void);
};

}
