#pragma once
#include "../Graphic/2DImage.h"
#include "../Graphic/2DImageArray.h"
#include "../th5DatFile/bft/BFTFile.h"
#include "../th5DatFile/CD2CDG/CD2CDGFile.h"
#include "../th5DatFile/pi/PIFile.h"
#include "../th5DatFile/th5DatFile.h"


namespace th5w{

class CCommonFunctionGraphic
{
public:
	static bool LoadPIFromDat(C2DImage** pRet,unsigned char *outPalette,Cth5DatFile *pDatFile,char *piFileName);
	static bool LoadCD2CDGFromDat(C2DImage** pRet,Cth5DatFile *pDatFile,char *cd2cdgFileName,int spriteIdx, unsigned char *inPalette);
	static bool LoadCD2CDGAllSpriteFromDat(C2DImageArray *pOutImgArray,Cth5DatFile *pDatFile,
										   char *cd2cdgFileName,unsigned char *inPalette);
	static bool LoadBFTFromDat(C2DImage** pRet,unsigned char *outPalette,Cth5DatFile *pDatFile,
							   char *bftFileName,int patternIdx,int transparentColorIdx);
	static bool LoadBFTAllPatternFromDat(C2DImageArray *pOutImgArray,unsigned char *outPalette,
										 Cth5DatFile *pDatFile,char *bftFileName,int transparentColorIdx,
										 unsigned char *inPalette=NULL);
	static bool LoadBinaryImageFile(C2DImage **pRet,unsigned char transparentColor[3], const char *fileName);
	static void ScreenFade(float fadeScale);	//0: black, 100: normal, 200: white, value between: linearly interpolated
	static void DrawRectangle(float XLow,float YLow,float XHigh,float YHigh,
							  float colorR,float colorG,float colorB,float alpha=1.0f,bool bSolid=true);
public:
	CCommonFunctionGraphic(void);
	~CCommonFunctionGraphic(void);
};

}
