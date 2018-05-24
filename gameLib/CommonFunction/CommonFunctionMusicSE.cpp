#include ".\commonfunctionmusicse.h"

namespace th5w{

CCommonFunctionMusicSE::CCommonFunctionMusicSE(void)
{
}

CCommonFunctionMusicSE::~CCommonFunctionMusicSE(void)
{
}

bool CCommonFunctionMusicSE::LoadMusicToPMDFromDat(Cth5DatFile *pDatFile,char *musicFileName)
{
	int musicFileIdx;
	musicFileIdx=pDatFile->GetChildFileIndex(musicFileName);
	if (musicFileIdx==-1)
		return false;

	if (th5w::CPMDPlayer::LoadPMDData(pDatFile->GetChildFileContentPointer(musicFileIdx),
										pDatFile->GetChildFileLength(musicFileIdx))==false)
		return false;

	return true;
}

}
