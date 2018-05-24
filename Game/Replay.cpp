#include ".\replay.h"
#include <stdio.h>
#include <time.h>
#include "game.h"

namespace th5w{

CReplay::CReplay(void)
{
	m_opStage=-1;
	Reset();
}

CReplay::~CReplay(void)
{
}

bool CReplay::Reset()
{
	if (m_opStage!=-1)
		return false;

	m_nInitLife=0;
	m_nInitBomb=0;
	m_playDifficulty=0;
	m_playChara=0;
	m_bPracticeMode=false;
	memset(m_stageFlag,0,sizeof(m_stageFlag));
	memset(m_nCurLife,0,sizeof(m_nCurLife));
	memset(m_nCurBomb,0,sizeof(m_nCurBomb));
	memset(m_curPower,0,sizeof(m_curPower));
	memset(m_powerOverflowLevel,0,sizeof(m_powerOverflowLevel));
	memset(m_curDream,0,sizeof(m_curDream));
	memset(m_curScore,0,sizeof(m_curScore));
	memset(m_totalNPointItem,0,sizeof(m_totalNPointItem));
	memset(m_playerPerformance,0,sizeof(m_playerPerformance));
	memset(m_stageScore,0,sizeof(m_stageScore));
	memset(m_randSeed,0,sizeof(m_randSeed));
	for (int i=0;i<7;i++)
        m_keyState[i].clear();

	memset(m_playerName,0,sizeof(m_playerName));

	m_opIdx=0;
	return true;
}

bool CReplay::GetRepFileInfo(REPLAY_BRIEF_INFO* pOut, const char *fileName)
{
#define FAIL_CLOSE_SETERRORCODE_RETURN(x,y) if (x) {fclose(fp);pOut->infoRes=y;return false;}
	FILE *fp=fopen(fileName,"rb");
	if (fp==NULL)
	{
		pOut->infoRes=InfoRes_FileNotExist;
		return false;
	}

	char sig[]="RPY ";
	unsigned char buf[16];
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(buf,1,4,fp)!=4,InfoRes_InvalidReplay)
	FAIL_CLOSE_SETERRORCODE_RETURN((*(int*)buf)!=(*(int*)sig),InfoRes_InvalidReplay)
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(buf,1,4,fp)!=4,InfoRes_InvalidReplay)
	FAIL_CLOSE_SETERRORCODE_RETURN((*(int*)buf)!=CGame::GVar().m_gameVersion,InfoRes_GameVersionMismatch)
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(buf,1,16,fp)!=16,InfoRes_InvalidReplay)
	for (int i=0;i<16;i++)
		FAIL_CLOSE_SETERRORCODE_RETURN(buf[i]!=CGame::GVar().m_modMD5[i],InfoRes_ModFileMisMatch)

	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->playerName,1,8,fp)!=8,InfoRes_InvalidReplay)
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->saveTime,4,1,fp)!=1,InfoRes_InvalidReplay)

	fseek(fp,8,SEEK_CUR);
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->playDifficulty,4,1,fp)!=1,InfoRes_InvalidReplay)
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->playChara,4,1,fp)!=1,InfoRes_InvalidReplay)
	fseek(fp,1,SEEK_CUR);

	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->stageFlag,1,7,fp)!=7,InfoRes_InvalidReplay)

	fseek(fp,224,SEEK_CUR);
	FAIL_CLOSE_SETERRORCODE_RETURN(fread(&pOut->stageScore,4,7,fp)!=7,InfoRes_InvalidReplay)

	fclose(fp);
	pOut->infoRes=InfoRes_ValidReplay;
	return true;
#undef FAIL_CLOSE_SETERRORCODE_RETURN
}

bool CReplay::SaveFile(const char *fileName)
{
#define FAIL_CLOSE_RETURN(x) if (x) {fclose(fp);return false;}
	FILE *fp=fopen(fileName,"wb");
	if (fp==NULL)
		return false;

	FAIL_CLOSE_RETURN(fwrite("RPY ",1,4,fp)!=4)
	FAIL_CLOSE_RETURN(fwrite(&CGame::GVar().m_gameVersion,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(CGame::GVar().m_modMD5,1,16,fp)!=16)

	FAIL_CLOSE_RETURN(fwrite(&m_playerName,1,8,fp)!=8)
	m_saveTime=(unsigned int)time(NULL);
	FAIL_CLOSE_RETURN(fwrite(&m_saveTime,4,1,fp)!=1)

	FAIL_CLOSE_RETURN(fwrite(&m_nInitLife,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(&m_nInitBomb,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(&m_playDifficulty,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(&m_playChara,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fwrite(&m_bPracticeMode,1,1,fp)!=1)

	FAIL_CLOSE_RETURN(fwrite(m_stageFlag,1,7,fp)!=7)

	FAIL_CLOSE_RETURN(fwrite(m_nCurLife,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_nCurBomb,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_curPower,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_powerOverflowLevel,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_curDream,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_curScore,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_totalNPointItem,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_playerPerformance,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fwrite(m_stageScore,4,7,fp)!=7)

	FAIL_CLOSE_RETURN(fwrite(m_randSeed,4,7,fp)!=7)
	for (int i=0;i<7;i++)
		if (m_stageFlag[i]!=0)
		{
			int nkey=(int)m_keyState[i].size();
			FAIL_CLOSE_RETURN(fwrite(&nkey,4,1,fp)!=1)
			FAIL_CLOSE_RETURN(fwrite(&m_keyState[i][0],2,nkey,fp)!=nkey)
		}
	fclose(fp);
	return true;
#undef FAIL_CLOSE_RETURN
}

bool CReplay::LoadFile(const char *fileName)
{
#define FAIL_CLOSE_RETURN(x) if (x) {fclose(fp);return false;}
	FILE *fp=fopen(fileName,"rb");
	if (fp==NULL)
		return false;

	char sig[]="RPY ";
	unsigned char buf[16];
	FAIL_CLOSE_RETURN(fread(buf,1,4,fp)!=4)
	FAIL_CLOSE_RETURN((*(int*)buf)!=(*(int*)sig))
	FAIL_CLOSE_RETURN(fread(buf,1,4,fp)!=4)
	FAIL_CLOSE_RETURN((*(int*)buf)!=CGame::GVar().m_gameVersion)
	FAIL_CLOSE_RETURN(fread(buf,1,16,fp)!=16)
	for (int i=0;i<16;i++)
		FAIL_CLOSE_RETURN(buf[i]!=CGame::GVar().m_modMD5[i])

	FAIL_CLOSE_RETURN(fread(&m_playerName,1,8,fp)!=8)
	FAIL_CLOSE_RETURN(fread(&m_saveTime,4,1,fp)!=1)

	FAIL_CLOSE_RETURN(fread(&m_nInitLife,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fread(&m_nInitBomb,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fread(&m_playDifficulty,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fread(&m_playChara,4,1,fp)!=1)
	FAIL_CLOSE_RETURN(fread(&m_bPracticeMode,1,1,fp)!=1)

	FAIL_CLOSE_RETURN(fread(m_stageFlag,1,7,fp)!=7)

	FAIL_CLOSE_RETURN(fread(m_nCurLife,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_nCurBomb,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_curPower,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_powerOverflowLevel,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_curDream,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_curScore,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_totalNPointItem,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_playerPerformance,4,7,fp)!=7)
	FAIL_CLOSE_RETURN(fread(m_stageScore,4,7,fp)!=7)

	FAIL_CLOSE_RETURN(fread(m_randSeed,4,7,fp)!=7)
	for (int i=0;i<7;i++)
		if (m_stageFlag[i]!=0)
		{
			int nkey;
			FAIL_CLOSE_RETURN(fread(&nkey,4,1,fp)!=1)
			m_keyState[i].resize(nkey);
			FAIL_CLOSE_RETURN(fread(&m_keyState[i][0],2,nkey,fp)!=nkey)
		}
	fclose(fp);
	return true;
#undef FAIL_CLOSE_RETURN
}

bool CReplay::OpenStage(int stageIdx,bool bRead)
{
	if (stageIdx<0||stageIdx>6)
		return false;
	if (m_opStage!=-1)
		return false;
	if (bRead&&m_stageFlag[stageIdx]==0)
		return false;
	if (bRead==false&&m_stageFlag[stageIdx]!=0)
		return false;

	if (!bRead)
	{
		m_nCurLife[stageIdx]=CGame::GVar().m_nCurLife;
		m_nCurBomb[stageIdx]=CGame::GVar().m_nCurBomb;
		m_curPower[stageIdx]=CGame::GVar().m_curPower;
		m_powerOverflowLevel[stageIdx]=CGame::GVar().m_powerOverflowLevel;
		m_curDream[stageIdx]=CGame::GVar().m_curDream;
		m_curScore[stageIdx]=CGame::GVar().m_curScore;
		m_totalNPointItem[stageIdx]=CGame::GVar().m_totalNPointItem;
		m_playerPerformance[stageIdx]=CGame::GVar().m_playerPerformance;
		m_stageScore[stageIdx]=CGame::GVar().m_curScore;
		m_randSeed[stageIdx]=CGame::GVar().m_randomSeed[stageIdx];
	}

	m_opStage=stageIdx;
	m_opIdx=0;
	m_bOpRead=bRead;
	return true;
}

bool CReplay::CloseStage(bool bCleared)
{
	if (m_opStage==-1)
		return false;
	if (!m_bOpRead)
	{
		m_stageFlag[m_opStage]=bCleared?2:1;
		m_stageScore[m_opStage]=CGame::GVar().m_curScore;
	}
	m_opStage=-1;
	return true;
}

bool CReplay::GetNextData(unsigned short *pOut)
{
	if (m_opStage==-1)
		return false;
	if (!m_bOpRead)
		return false;
	if (m_opIdx>=(int)m_keyState[m_opStage].size())
		return false;
	(*pOut)=m_keyState[m_opStage][m_opIdx];
	m_opIdx++;
	return true;
}

bool CReplay::PutNextdata(unsigned short data)
{
	if (m_opStage==-1)
		return false;
	if (m_bOpRead)
		return false;
	m_keyState[m_opStage].push_back(data);
	m_opIdx++;
	return true;
}

}


















