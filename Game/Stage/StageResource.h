#pragma once
#include "../../gameLib/Graphic/2DImageArray.h"
#include "../../gameLib/CommonFunction/CommonFunctionGraphic.h"
#include "../../gameLib/CommonFunction/CommonFunctionMusicSE.h"
#include "../../gameLib/CommonFunction/CommonFunctionInput.h"
#include "../../gameLib/th5DatFile/bb/BBFile.h"
#include "../../gameLib/th5DatFile/mpn/MPNFile.h"
#include "../../gameLib/th5DatFile/std/STDFile.h"

namespace th5w{

class CStage;

class CStageResource
{
public:
	CStage *m_pStage;

//images
public:
	C2DImageArray m_patternArray;
	C2DImageArray m_spriteArray;
	C2DImage *m_pImgDotBullet;
	C2DImage *m_pImgDotBulletShadow;
	C2DImage *m_pImgSpark[8];
	C2DImage *m_pImgNumber[10];

	CBBFile *m_pCharaBombBB;
	CBBFile *m_pStageBB;
	C2DImage *m_pImgLogo;

	char m_translatedStageName[7][1000];
	char m_translatedStageBGMName[7][1000];
	char m_translatedStageBossBGMName[7][1000];
	char m_stage4BossPart2BGMNames[2][1000];
	char m_crossRow[1000];

public:
	unsigned char m_palette[48];

public:
	C2DImageArray m_mpnArray;
	CSTDFile *m_pStageStd;
	unsigned char *m_stageMap;
public:
	void InitImages();
	void InitSpecificImages();
	void LoadStageDataFiles();

public:
	void Initialize();
	void InitTranslation();

public:
	void DrawGauge(int curValue,int fullValue,int drawX,int drawY,float colorR,float colorG,float colorB);
	void DrawNumber(int numX,int numY,int number,int nDigit,bool leadingZero,bool rightAlign,
					float colorR,float colorG,float colorB);
	void DrawStageObjectImage(C2DImage *pImage,float gameCoordX,float gameCoordY);
	void DrawStageObjectImageColorScaled(C2DImage *pImage,float gameCoordX,float gameCoordY,
										 float colorR,float colorG,float colorB);
	void DrawStageObjectImageWhiteMask(C2DImage *pImage,float gameCoordX,float gameCoordY);
	void ClearPlayArea(float colorR,float colorG,float colorB);
	void DrawCircle(float gameCoordX,float gameCoordY,float radius,
					float colorR,float colorG,float colorB,bool bSolid,float alpha=1.0f);
	void DrawLine(float gameCoordSrcX,float gameCoordSrcY,float gameCoordTarX,float gameCoordTarY,
				  float colorR,float colorG,float colorB,float width);
	void DrawRectangle(float gameCoordX,float gameCoordY,float width,float height,
					   float colorR,float colorG,float colorB,bool bSolid,float alpha=1.0f);
	void DrawPoint(float gameCoordX,float gameCoordY,float colorR,float colorG,float colorB);
	void DrawPlayAreaMask(unsigned char *pMask,float colorR,float colorG,float colorB);


public:
	C2DImageArray m_txtBBArray;

public:
	CStageResource(CStage *pStage);
	~CStageResource(void);
};

}
