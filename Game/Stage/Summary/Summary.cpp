#include ".\summary.h"
#include "../../../gameLib/Graphic/Th5ExtFont.h"
#include "../../../gameLib/Graphic/PC98Font.h"
#include "../Stage.h"
#include "../Boss/Boss.h"
#include "../../Game.h"
#include <string.h>
#include <math.h>

namespace th5w{

	char rateText[][100]={"悪霊ボス退治失敗！！　　　　　　　×　０．０",
						  "プレイヤー数ペナルティ（初期６人）×　０．３",
						  "プレイヤー数ペナルティ（初期５人）×　０．５",
						  "プレイヤー数ペナルティ（初期４人）×　０．７",
						  "コンティニューペナルティ（１回）　×　０．８",
						  "コンティニューペナルティ（２回）　×　０．６",
						  "コンティニューペナルティ（３回）　×　０．４",
						  "難易度ボーナス（Ｅａｓｙ）　　　　×　０．５",
						  "難易度ボーナス（Ｎｏｒｍａｌ）　　×　１．０",
						  "難易度ボーナス（Ｈａｒｄ）　　　　×　１．２",
						  "難易度ボーナス（Ｌｕｎａｔｉｃ）　×　１．４",
						  "難易度ボーナス（Ｅｘｔｒａ）　　　×　２．０"};	//PC98 ver does not have a bonus string
																			//for extra stage. Added here.
	char strStageBonus[100]="ＳＴＡＧＥ　Ｂｏｎｕｓ";
	char strDreamBonus[100]="ＤＲＥＡＭ　Ｂｏｎｕｓ";
	char strGraze_x50[100]="カスリ弾数　×　　５０";
	char strPointItem[100]="　×　得点アイテム数　";
	char strPlusNoMissBonus[100]="　＋　ノーミスボーナス";
	char strPlusNoBombBonus[100]="　＋　ノーボムボーナス";
	char strTotal[100]="　　　ＴＯＴＡＬ";

	char strAllClear[100]="ＡＬＬ　Ｃｌｅａｒ　　";
	char strRemainingLifeX10000[100]="残り人数　×１００００";
	char strTotalPointItemBonus[100]="総得点アイテムボーナス";

CSummary::CSummary(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CSummary::~CSummary(void)
{
}

void CSummary::Initialize()
{
	if (CGame::GVar().m_bUseMod)
	{
		int fileIdx=CGame::s_pCurGame->m_modDat.GetChildFileIndex("summary.txt");
		if (fileIdx!=-1)
		{
			CGame::s_pCurGame->m_modDat.Childfseek(fileIdx,0,SEEK_SET);
			for (int i=0;i<12;i++)
			{
				CGame::s_pCurGame->m_modDat.Childfgets(rateText[i],100,fileIdx);
				if (rateText[i][strlen(rateText[i])-1]=='\n')
					rateText[i][strlen(rateText[i])-1]=0;
			}
			char *textArray[]={strStageBonus,strDreamBonus,strGraze_x50,strPointItem,strPlusNoMissBonus,
							   strPlusNoBombBonus,strTotal,strAllClear,strRemainingLifeX10000,strTotalPointItemBonus};
			for (int i=0;i<sizeof(textArray)/sizeof(char *);i++)
			{
				CGame::s_pCurGame->m_modDat.Childfgets(textArray[i],100,fileIdx);
				if (textArray[i][strlen(textArray[i])-1]=='\n')
					textArray[i][strlen(textArray[i])-1]=0;
			}
		}
	}

	m_bRunning=false;
	m_curFrame=0;
}

void CSummary::StepNormalSummary()
{
	if (m_curFrame==0)
	{
		int totalScore=0;
		totalScore+=(CGame::GVar().m_playStage+1)*100;
		totalScore+=m_pStage->GetDream()*10;
		totalScore+=m_pStage->m_curNGraze*5;
		totalScore*=m_pStage->m_curNPointItem;
		bool bNoMiss=(m_pStage->m_nMiss==0);
		bool bNoBomb=(m_pStage->m_nBomb==0&&bNoMiss);
		if (bNoMiss)
			totalScore+=CGame::GVar().m_playStage*5000+10000;
		if (bNoBomb)
			totalScore+=CGame::GVar().m_playStage*5000+10000;
		if (totalScore>=1200000)
			m_pStage->ChangePlayerPerformance(4);
		else
			if (totalScore>=800000)
				m_pStage->ChangePlayerPerformance(2);
			else
				if (totalScore<=100000)
					m_pStage->ChangePlayerPerformance(-2);
				else
					if (totalScore<=200000)
						m_pStage->ChangePlayerPerformance(-1);
		if (m_pStage->m_pBoss->m_bSuccessDefeat==false)
			totalScore=0;
		else
		{
			if (m_pStage->m_nLifeOnContinue>=4)
				totalScore=totalScore*(15-m_pStage->m_nLifeOnContinue*2)/10;
			if (m_pStage->m_nContinueUsed>0)
				totalScore=totalScore*(10-m_pStage->m_nContinueUsed*2)/10;
			int scale[]={5,10,12,14};
			totalScore=totalScore*scale[CGame::GVar().m_playDifficulty]/10;
		}
		m_pStage->AddScore(totalScore);
	}
}

void CSummary::StepEndGameSummary()
{
	if (m_curFrame==0)
	{
		int totalScore=1000;
		totalScore+=m_pStage->GetDream()*10;
		totalScore+=m_pStage->m_curNGraze*5;
		totalScore+=(m_pStage->m_nCurLife-1)*1000;
		totalScore*=m_pStage->m_curNPointItem;

		bool bNoMiss=(m_pStage->m_nMiss==0);
		bool bNoBomb=(m_pStage->m_nBomb==0&&bNoMiss);
		if (bNoMiss)
			totalScore+=50000;
		if (bNoBomb)
			totalScore+=50000;
		totalScore+=m_pStage->m_totalNPointItem*250;

		if (totalScore>=1200000)
			m_pStage->ChangePlayerPerformance(4);
		else
			if (totalScore>=800000)
				m_pStage->ChangePlayerPerformance(2);
			else
				if (totalScore<=100000)
					m_pStage->ChangePlayerPerformance(-2);
				else
					if (totalScore<=200000)
						m_pStage->ChangePlayerPerformance(-1);
		if (m_pStage->m_pBoss->m_bSuccessDefeat==false)
			totalScore=0;
		else
		{
			if (m_pStage->m_nLifeOnContinue>=4)
				totalScore=totalScore*(15-m_pStage->m_nLifeOnContinue*2)/10;
			if (m_pStage->m_nContinueUsed>0)
				totalScore=totalScore*(10-m_pStage->m_nContinueUsed*2)/10;
			int scale[]={5,10,12,14,20};
			totalScore=totalScore*scale[CGame::GVar().m_playDifficulty]/10;
		}
		m_pStage->AddScore(totalScore);
	}
}

void CSummary::Step()
{
	if (m_bRunning==false)
		return;

	if (CGame::GVar().m_playStage<5)
		StepNormalSummary();
	else
		StepEndGameSummary();
	m_curFrame++;
}

void CSummary::DrawItem(int textIdx,int row,bool useRed)
{
	int offX=m_pStage->m_playAreaUpperLeftX-32;
	int offY=m_pStage->m_playAreaUpperLeftY-16;
	CPC98Font::DrawString(rateText[textIdx],100,0x6*8+offX,row*16+offY,useRed?1.0f:0.0f,useRed?0.0f:1.0f,0.0f);
}

void CSummary::DrawNormalSummary()
{
	bool bNoMiss=(m_pStage->m_nMiss==0);
	bool bNoBomb=(m_pStage->m_nBomb==0&&bNoMiss);
	m_pStage->m_curScrFade=60;
	int offX=m_pStage->m_playAreaUpperLeftX-32;
	int offY=m_pStage->m_playAreaUpperLeftY-16;

	const unsigned char extClearBonus[]={0x4d,0x4e,0x4f,0x02,0x58,0x59,0x5a,0x5b,0};
	CTh5ExtFont::DrawExtString(extClearBonus,100,0x14*8+offX,0x04*16+offY,1,1,1);
	CPC98Font::DrawString(strStageBonus,100,0x6*8+offX,0x8*16+offY,1,1,1);
	CPC98Font::DrawString(strDreamBonus,100,0x6*8+offX,0x0a*16+offY,1,1,1);
	CPC98Font::DrawString(strGraze_x50,100,0x6*8+offX,0x0c*16+offY,1,1,1);
	CPC98Font::DrawString(strPointItem,100,0x6*8+offX,0x0e*16+offY,1,1,1);

	if (bNoMiss)
	{
		CPC98Font::DrawString(strPlusNoMissBonus,100,0x6*8+offX,0x10*16+offY,0,1,1);
	}
	if (bNoBomb)
	{
		CPC98Font::DrawString(strPlusNoBombBonus,100,0x6*8+offX,0x11*16+offY,0,1,1);
	}
	CPC98Font::DrawString(strTotal,100,0x6*8+offX,0x15*16+offY,1,1,1);

	int totalScore=0;
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x08*16+offY,(CGame::GVar().m_playStage+1)*100*10,8,
						 false,true,1,1,1);
	totalScore+=(CGame::GVar().m_playStage+1)*100;
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x0a*16+offY,m_pStage->GetDream()*10*10,8,
						 false,true,1,1,1);
	totalScore+=m_pStage->GetDream()*10;
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x0c*16+offY,m_pStage->m_curNGraze*5*10,8,
						 false,true,1,1,1);
	totalScore+=m_pStage->m_curNGraze*5;
	m_pStage->m_pStageRes->DrawNumber(0x28*8+offX,0x0e*16+offY,m_pStage->m_curNPointItem,5,
						 false,true,1,1,1);
	totalScore*=m_pStage->m_curNPointItem;
	if (bNoMiss)
	{
		m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x10*16+offY,(CGame::GVar().m_playStage*5000+10000)*10,8,
							 false,true,1,1,1);
		totalScore+=CGame::GVar().m_playStage*5000+10000;
	}
	if (bNoBomb)
	{
		m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x11*16+offY,(CGame::GVar().m_playStage*5000+10000)*10,8,
							 false,true,1,1,1);
		totalScore+=CGame::GVar().m_playStage*5000+10000;
	}
	if (m_pStage->m_pBoss->m_bSuccessDefeat==false)
	{
		totalScore=0;
		DrawItem(0,0x14,true);
	}
	else
	{
		if (m_pStage->m_nLifeOnContinue>=4)
		{
			totalScore=totalScore*(15-m_pStage->m_nLifeOnContinue*2)/10;
			DrawItem(7-m_pStage->m_nLifeOnContinue,0x12,true);
		}
		if (m_pStage->m_nContinueUsed>0)
		{
			totalScore=totalScore*(10-m_pStage->m_nContinueUsed*2)/10;
			DrawItem(3+m_pStage->m_nContinueUsed,0x13,true);
		}
		int scale[]={5,10,12,14};
		totalScore=totalScore*scale[CGame::GVar().m_playDifficulty]/10;
		if (scale[CGame::GVar().m_playDifficulty]>=10)
			DrawItem(7+CGame::GVar().m_playDifficulty,0x14,false);
		else
			DrawItem(7+CGame::GVar().m_playDifficulty,0x14,true);
	}
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x15*16+offY,totalScore*10,8,false,true,1,1,1);
}

void CSummary::DrawEndGameSummary()
{
	bool bNoMiss=(m_pStage->m_nMiss==0);
	bool bNoBomb=(m_pStage->m_nBomb==0&&bNoMiss);
	m_pStage->m_curScrFade=60;
	int offX=m_pStage->m_playAreaUpperLeftX-32;
	int offY=m_pStage->m_playAreaUpperLeftY-16;

	const unsigned char extCongratulation[]={0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0};
	CTh5ExtFont::DrawExtString(extCongratulation,100,0x13*8+offX,0x04*16+offY,1,1,1);
	CPC98Font::DrawString(strAllClear,100,0x6*8+offX,0x6*16+offY,1,1,1);
	CPC98Font::DrawString(strDreamBonus,100,0x6*8+offX,0x8*16+offY,1,1,1);
	CPC98Font::DrawString(strGraze_x50,100,0x6*8+offX,0x0a*16+offY,1,1,1);
	CPC98Font::DrawString(strRemainingLifeX10000,100,0x6*8+offX,0x0c*16+offY,1,1,1);
	CPC98Font::DrawString(strPointItem,100,0x6*8+offX,0x0e*16+offY,1,1,1);

	if (bNoMiss)
	{
		CPC98Font::DrawString(strPlusNoMissBonus,100,0x6*8+offX,0x0f*16+offY,0,1,1);
	}
	if (bNoBomb)
	{
		CPC98Font::DrawString(strPlusNoBombBonus,100,0x6*8+offX,0x10*16+offY,0,1,1);
	}

	CPC98Font::DrawString(strTotalPointItemBonus,100,0x6*8+offX,0x11*16+offY,0,1,1);
	CPC98Font::DrawString(strTotal,100,0x6*8+offX,0x15*16+offY,1,1,1);

	int totalScore=1000;
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x06*16+offY,1000*10,8,
						 false,true,1,1,1);
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x08*16+offY,m_pStage->GetDream()*10*10,8,
						 false,true,1,1,1);
	totalScore+=m_pStage->GetDream()*10;
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x0a*16+offY,m_pStage->m_curNGraze*5*10,8,
						 false,true,1,1,1);
	totalScore+=m_pStage->m_curNGraze*5;
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x0c*16+offY,(m_pStage->m_nCurLife-1)*1000*10,8,
						 false,true,1,1,1);
	totalScore+=(m_pStage->m_nCurLife-1)*1000;
	m_pStage->m_pStageRes->DrawNumber(0x28*8+offX,0x0e*16+offY,m_pStage->m_curNPointItem,5,
						 false,true,1,1,1);
	totalScore*=m_pStage->m_curNPointItem;
	if (bNoMiss)
	{
		m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x0f*16+offY,50000*10,8,
							 false,true,1,1,1);
		totalScore+=50000;
	}
	if (bNoBomb)
	{
		m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x10*16+offY,50000*10,8,
							 false,true,1,1,1);
		totalScore+=50000;
	}
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x11*16+offY,m_pStage->m_totalNPointItem*250*10,8,
						 false,true,1,1,1);
	totalScore+=m_pStage->m_totalNPointItem*250;

	if (m_pStage->m_pBoss->m_bSuccessDefeat==false)
	{
		totalScore=0;
		DrawItem(0,0x14,true);
	}
	else
	{
		if (m_pStage->m_nLifeOnContinue>=4)
		{
			totalScore=totalScore*(15-m_pStage->m_nLifeOnContinue*2)/10;
			DrawItem(7-m_pStage->m_nLifeOnContinue,0x12,true);
		}
		if (m_pStage->m_nContinueUsed>0)
		{
			totalScore=totalScore*(10-m_pStage->m_nContinueUsed*2)/10;
			DrawItem(3+m_pStage->m_nContinueUsed,0x13,true);
		}
		int scale[]={5,10,12,14,20};
		totalScore=totalScore*scale[CGame::GVar().m_playDifficulty]/10;
		if (scale[CGame::GVar().m_playDifficulty]>=10)
			DrawItem(7+CGame::GVar().m_playDifficulty,0x14,false);
		else
			DrawItem(7+CGame::GVar().m_playDifficulty,0x14,true);
	}
	m_pStage->m_pStageRes->DrawNumber(0x22*8+offX,0x15*16+offY,totalScore*10,8,false,true,1,1,1);
}

void CSummary::Draw()
{
	if (m_bRunning==false)
		return;
	if (CGame::GVar().m_playStage<5)
		DrawNormalSummary();
	else
		DrawEndGameSummary();
}

}

