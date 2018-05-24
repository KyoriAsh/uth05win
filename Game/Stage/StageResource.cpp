#include ".\stageresource.h"
#include "../Game.h"
#include "Stage.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include "../../gameLib/Graphic/PC98Font.h"

#define SAFE_DELETE(x) {if (x) delete x;x=NULL;}
#define SAFE_DESTROY(x) {if (x) x->Destroy();x=NULL;}
#define SAFE_ARRAY_DESTROY(x,y) {for (int i=0;i<y;i++) if (x[i]) {x[i]->Destroy();x[i]=NULL;}}

namespace th5w{

CStageResource::CStageResource(CStage *pStage)
{
	m_pStage=pStage;

	m_pStageStd=NULL;
	m_stageMap=NULL;

	m_pImgDotBullet=NULL;
	m_pImgDotBulletShadow=NULL;
	for (int i=0;i<8;i++)
		m_pImgSpark[i]=NULL;
	for (int i=0;i<10;i++)
		m_pImgNumber[i]=NULL;
	m_pImgLogo=NULL;

	m_pCharaBombBB=NULL;
	m_pStageBB=NULL;

	Initialize();
}

CStageResource::~CStageResource(void)
{
	SAFE_DELETE(m_pStageStd);

	SAFE_DESTROY(m_pImgDotBullet);
	SAFE_DESTROY(m_pImgDotBulletShadow);
	SAFE_ARRAY_DESTROY(m_pImgSpark,8);
	SAFE_ARRAY_DESTROY(m_pImgNumber,10);
	SAFE_DESTROY(m_pImgLogo);

	SAFE_DELETE(m_pCharaBombBB);
	SAFE_DELETE(m_pStageBB);
}

void CStageResource::InitTranslation()
{
	if (CGame::GVar().m_bUseMod==false)
		return;

	memset(m_translatedStageName,0,sizeof(m_translatedStageName));
	memset(m_translatedStageBGMName,0,sizeof(m_translatedStageBGMName));
	memset(m_translatedStageBossBGMName,0,sizeof(m_translatedStageBossBGMName));
	memset(m_stage4BossPart2BGMNames,0,sizeof(m_stage4BossPart2BGMNames));

	int fileIdx=CGame::s_pCurGame->m_modDat.GetChildFileIndex("names.txt");
	if (fileIdx!=-1)
	{
		CGame::s_pCurGame->m_modDat.Childfseek(fileIdx,0,SEEK_SET);
		char emptyLine[10];
		for (int i=0;i<7;i++)
		{
			CGame::s_pCurGame->m_modDat.Childfgets(m_translatedStageName[i],1000,fileIdx);
			if (m_translatedStageName[i][strlen(m_translatedStageName[i])-1]=='\n')
				m_translatedStageName[i][strlen(m_translatedStageName[i])-1]=0;
			CGame::s_pCurGame->m_modDat.Childfgets(m_translatedStageBGMName[i],1000,fileIdx);
			if (m_translatedStageBGMName[i][strlen(m_translatedStageBGMName[i])-1]=='\n')
				m_translatedStageBGMName[i][strlen(m_translatedStageBGMName[i])-1]=0;
			CGame::s_pCurGame->m_modDat.Childfgets(m_translatedStageBossBGMName[i],1000,fileIdx);
			if (m_translatedStageBossBGMName[i][strlen(m_translatedStageBossBGMName[i])-1]=='\n')
				m_translatedStageBossBGMName[i][strlen(m_translatedStageBossBGMName[i])-1]=0;
			CGame::s_pCurGame->m_modDat.Childfgets(emptyLine,10,fileIdx);
		}
		for (int i=0;i<2;i++)
		{
			CGame::s_pCurGame->m_modDat.Childfgets(m_stage4BossPart2BGMNames[i],1000,fileIdx);
			if (m_stage4BossPart2BGMNames[i][strlen(m_stage4BossPart2BGMNames[i])-1]=='\n')
				m_stage4BossPart2BGMNames[i][strlen(m_stage4BossPart2BGMNames[i])-1]=0;
		}
		m_pStageStd->m_stageName=m_translatedStageName[CGame::GVar().m_playStage];
		m_pStageStd->m_stageBGMName=m_translatedStageBGMName[CGame::GVar().m_playStage];
		m_pStageStd->m_stageBossBGMName=m_translatedStageBossBGMName[CGame::GVar().m_playStage];
	}

	//copy default cross row string to m_crossRow
	{
		unsigned char cross[]={0x81,0x40,0x81,0x40,0x81,0x7e,0x81,0x40,0x81,0x40,0};
		strcpy(m_crossRow,(char*)cross);
	}
	fileIdx=CGame::s_pCurGame->m_modDat.GetChildFileIndex("marks.txt");
	if (fileIdx!=-1)
	{
		CGame::s_pCurGame->m_modDat.Childfseek(fileIdx,0,SEEK_SET);
		CGame::s_pCurGame->m_modDat.Childfgets(m_crossRow,1000,fileIdx);
		if (m_crossRow[strlen(m_crossRow)-1]=='\n')
			m_crossRow[strlen(m_crossRow)-1]=0;
	}
}

void CStageResource::Initialize()
{
	LoadStageDataFiles();				//make sure this call is before InitImages as palette in MPN file
										//is loaded as m_palette
										//also make sure this call is before initialization of any stage
										//component as they may require stage data that is loaded by this function

	strcpy(m_stage4BossPart2BGMNames[0],"— Ø‚è‚Ì­—@` Judas Kiss");
	strcpy(m_stage4BossPart2BGMNames[1],"^g‚Ì­—@` Crimson Dead!!");

	InitTranslation();					//load translation data to overwrite the data loaded

	InitImages();
	//load chara bomb bb
	{
		char charaBBFileName[]="BB0.BB";
		charaBBFileName[2]=48+CGame::GVar().m_playChara;
		int charaBombBBIdx=CGame::s_pCurGame->m_th5Dat2.GetChildFileIndex(charaBBFileName);
		m_pCharaBombBB=new CBBFile(CGame::s_pCurGame->m_th5Dat2.GetChildFileContentPointer(charaBombBBIdx),
								   CGame::s_pCurGame->m_th5Dat2.GetChildFileLength(charaBombBBIdx));
	}
	//load stage bb
	{
		char stageBBFileName[]="ST00.BB";
		if (CGame::GVar().m_playStage!=6)
			stageBBFileName[3]=48+CGame::GVar().m_playStage;
		else
			stageBBFileName[3]=48+3;
		int stageBBIdx=CGame::s_pCurGame->m_th5Dat2.GetChildFileIndex(stageBBFileName);
		m_pStageBB=new CBBFile(CGame::s_pCurGame->m_th5Dat2.GetChildFileContentPointer(stageBBIdx),
							   CGame::s_pCurGame->m_th5Dat2.GetChildFileLength(stageBBIdx));
	}

}

void CStageResource::LoadStageDataFiles()
{
	//load MPN
	if (CGame::GVar().m_playStage!=5)
	{
		char mpnFileName[]="ST00.MPN";
		mpnFileName[3]=CGame::GVar().m_playStage+48;
		int mpnIdx=CGame::s_pCurGame->m_th5Dat2.GetChildFileIndex(mpnFileName);
		CMPNFile mpnFile(CGame::s_pCurGame->m_th5Dat2.GetChildFileContentPointer(mpnIdx),
						 CGame::s_pCurGame->m_th5Dat2.GetChildFileLength(mpnIdx));
		for (int i=0;i<mpnFile.NImage();i++)
		{
			C2DImage *pImage=C2DImage::Create2DImage(16,16);
			mpnFile.ExtractImageAndPalette(pImage->GetDataPointer(),m_palette,i);
			pImage->UploadToTexture();
			m_mpnArray.AddImage(pImage);
		}
	}

	//load STD
	{
		char stdFileName[]="ST00.STD";
		stdFileName[3]=CGame::GVar().m_playStage+48;
		int stdIdx=CGame::s_pCurGame->m_th5Dat2.GetChildFileIndex(stdFileName);
		m_pStageStd=new CSTDFile(CGame::s_pCurGame->m_th5Dat2.GetChildFileContentPointer(stdIdx),
								 CGame::s_pCurGame->m_th5Dat2.GetChildFileLength(stdIdx));
	}

	//load MAP
	if (CGame::GVar().m_playStage!=5)
	{
		char mapFileName[]="ST00.MAP";
		mapFileName[3]=CGame::GVar().m_playStage+48;
		int mapIdx=CGame::s_pCurGame->m_th5Dat2.GetChildFileIndex(mapFileName);
		m_stageMap=CGame::s_pCurGame->m_th5Dat2.GetChildFileContentPointer(mapIdx)+8;
	}
}

void CStageResource::InitSpecificImages()
{
	//generate a dot bullet image and dot bullet shadow image
	//this image was obtained from screen shot of PC98 ver
	unsigned char img[]={0,0,1,1,1,1,0,0,
						 0,1,1,1,1,1,1,0,
						 1,1,1,1,1,1,1,1,
						 1,1,1,1,1,1,1,2,
						 1,1,1,1,1,1,1,2,
						 2,1,1,1,1,1,2,2,
						 0,2,2,2,2,2,2,0,
						 0,0,2,2,2,2,0,0};

	m_pImgDotBullet=C2DImage::Create2DImage(8,8);
	unsigned char *ptr=m_pImgDotBullet->GetDataPointer();
	for (int i=0;i<8;i++)
		for (int j=0;j<8;j++)
			if (img[(7-i)*8+j]>=1)
				ptr[i*8*4+j*4]=ptr[i*8*4+j*4+1]=ptr[i*8*4+j*4+2]=ptr[i*8*4+j*4+3]=255;
			else
				ptr[i*8*4+j*4]=ptr[i*8*4+j*4+1]=ptr[i*8*4+j*4+2]=ptr[i*8*4+j*4+3]=0;
	m_pImgDotBullet->UploadToTexture();

	m_pImgDotBulletShadow=C2DImage::Create2DImage(8,8);
	ptr=m_pImgDotBulletShadow->GetDataPointer();
	for (int i=0;i<8;i++)
		for (int j=0;j<8;j++)
			if (img[(7-i)*8+j]>=2)
				ptr[i*8*4+j*4]=ptr[i*8*4+j*4+1]=ptr[i*8*4+j*4+2]=ptr[i*8*4+j*4+3]=255;
			else
				ptr[i*8*4+j*4]=ptr[i*8*4+j*4+1]=ptr[i*8*4+j*4+2]=ptr[i*8*4+j*4+3]=0;
	m_pImgDotBulletShadow->UploadToTexture();

	//generate spark images
	//these bitmasks were obtained from data segment of main.exe
	unsigned char sparkImgBitmask[8][8]={0x10,0x10,0x10,0x1f,0xf8,0x8,0x8,0x8,
										 0x40,0x21,0x12,0x1c,0x38,0x48,0x84,0x2,
										 0,0x42,0x24,0x18,0x18,0x24,0x42,0,
										 0,0,0x24,0x18,0x18,0x24,0,0,
										 0x40,0x21,0x12,0x1c,0x38,0x48,0x84,0x2,
										 0,0x42,0x24,0x18,0x18,0x24,0x42,0,
										 0,0,0x24,0x18,0x18,0x24,0,0,
										 0x3c,0x42,0x81,0x99,0x99,0x81,0x42,0x3c};
	for (int i=0;i<8;i++)
	{
		m_pImgSpark[i]=C2DImage::Create2DImage(8,8);
		unsigned char *ptr=m_pImgSpark[i]->GetDataPointer();
		for (int j=0;j<8;j++)
			for (int k=0;k<8;k++)
			{
				if ((sparkImgBitmask[i][7-j]&(1<<(7-k)))!=0)
					ptr[0]=ptr[1]=ptr[2]=ptr[3]=255;
				else
					ptr[0]=ptr[1]=ptr[2]=ptr[3]=0;
				ptr+=4;
			}
		m_pImgSpark[i]->UploadToTexture();
	}

	//generate number images
	//these bitmasks were obtained from data segment of main.exe
	unsigned char numberImgBitMask[10][8]={0,0x38,0x44,0x82,0x82,0x82,0x44,0x38,
										   0,0,0,0x6,0x7e,0,0,0,
										   0,0x0c,0x3c,0,0,0x6,0x7e,0,
										   0x6,0x7e,0,0x6,0x3c,0,0x3,0xff,
										   0,0x7e,0x5a,0x7e,0x42,0x42,0x7e,0,
                                           0,0x7e,0x8,0x10,0x7c,0x24,0x44,0xff,
										   0,0x18,0x7e,0,0,0x24,0x42,0,
										   0,0x20,0x7e,0x20,0x20,0x3c,0,0,
										   0,0x4,0x24,0x22,0x42,0x41,0,0,
										   0,0x10,0x3e,0x24,0x26,0x4a,0x8e,0};
	for (int i=0;i<10;i++)
	{
		m_pImgNumber[i]=C2DImage::Create2DImage(8,8);
		unsigned char *ptr=m_pImgNumber[i]->GetDataPointer();
		for (int j=0;j<8;j++)
			for (int k=0;k<8;k++)
			{
				if ((numberImgBitMask[i][7-j]&(1<<(7-k)))!=0)
					ptr[0]=ptr[1]=ptr[2]=ptr[3]=255;
				else
					ptr[0]=ptr[1]=ptr[2]=ptr[3]=0;
				ptr+=4;
			}
		m_pImgNumber[i]->UploadToTexture();
	}

	//generate character fade out/in images from txtBB files.
	{
		unsigned char dataBuf[1024*3];
		int idx;
		idx=CGame::s_pCurGame->m_th5Dat2.GetChildFileIndex("TXT1.BB");
		CGame::s_pCurGame->m_th5Dat2.Childfread(dataBuf,1,2048,idx);
		idx=CGame::s_pCurGame->m_th5Dat2.GetChildFileIndex("TXT2.BB");
		CGame::s_pCurGame->m_th5Dat2.Childfread(dataBuf+2048,1,1024,idx);
		for (int i=0;i<24;i++)
		{
			unsigned char *ptr=dataBuf+i*128;
			C2DImage *pImg=C2DImage::Create2DImage(32,32);
			unsigned char *outPtr=pImg->GetDataPointer();
			for (int j=0;j<32;j++)
				for (int k=0;k<32;k++)
				{
					unsigned char v=ptr[(31-j)*4+k/8]&(1<<(7-k%8));
					if (v!=0) v=255;
					outPtr[0]=outPtr[1]=outPtr[2]=outPtr[3]=v;
					outPtr+=4;
				}
			pImg->UploadToTexture();
			m_txtBBArray.AddImage(pImg);
		}
	}
}

void CStageResource::InitImages()
{
	//load palette
	if (CGame::GVar().m_playStage==5)
	{
		int rgbIdx=CGame::s_pCurGame->m_th5Dat2.GetChildFileIndex("EYE.RGB");
		CGame::s_pCurGame->m_th5Dat2.Childfread(m_palette,1,48,rgbIdx);
		for (int i=0;i<48;i++)
			m_palette[i]*=17;
	}

#define LOAD_DAT2_BFT(x) CCommonFunctionGraphic::LoadBFTAllPatternFromDat(&m_patternArray,NULL,&CGame::s_pCurGame->m_th5Dat2,x,0,m_palette)
#define LOAD_DAT2_CD2G(x) CCommonFunctionGraphic::LoadCD2CDGAllSpriteFromDat(&m_spriteArray,&CGame::s_pCurGame->m_th5Dat2,x,m_palette)

	//load patterns
	char charaImageFileName[][10]={"REIMU.BFT","MARI.BFT","MIMA.BFT","YUKA.BFT"};
	LOAD_DAT2_BFT(charaImageFileName[CGame::GVar().m_playChara]);
	LOAD_DAT2_BFT("MIKOD.BFT");
	LOAD_DAT2_BFT("MIKO32.BFT");
	char charaBulletImageFileName[][20]={"REIMU16.BFT","MARI16.BFT","MIMA16.BFT","YUKA16.BFT"};
	LOAD_DAT2_BFT(charaBulletImageFileName[CGame::GVar().m_playChara]);
	LOAD_DAT2_BFT("MIKO16.BFT");

	char charaBombBFT[]="BOMB0.BFT";
	if (CGame::GVar().m_playChara!=2)
		charaBombBFT[4]=48+CGame::GVar().m_playChara;
	LOAD_DAT2_BFT(charaBombBFT);

	char stageEnemyBFT[]="ST00.BFT";
	stageEnemyBFT[3]=48+CGame::GVar().m_playStage;
	if (CGame::GVar().m_playStage==5)
		stageEnemyBFT[3]=48+4;
	LOAD_DAT2_BFT(stageEnemyBFT);

	if (CGame::GVar().m_playStage!=5)
	{
		char stageMidBossBMT[]="ST00.BMT";
		stageMidBossBMT[3]=48+CGame::GVar().m_playStage;
		LOAD_DAT2_BFT(stageMidBossBMT);
	}

	char charaBombCDG[]="BB0.CDG";
	charaBombCDG[2]=48+CGame::GVar().m_playChara;
	LOAD_DAT2_CD2G(charaBombCDG);
	m_spriteArray.AddImage(NULL);

	char charaFaceCDG[]="KAO0.CD2";
	charaFaceCDG[3]=48+CGame::GVar().m_playChara;
	LOAD_DAT2_CD2G(charaFaceCDG);

	while(m_spriteArray.NImage()<8)
		m_spriteArray.AddImage(NULL);

	char bossFaceCDG[]="BSS0.CD2";
	bossFaceCDG[3]=48+CGame::GVar().m_playStage;
	LOAD_DAT2_CD2G(bossFaceCDG);

	while(m_spriteArray.NImage()<16)
		m_spriteArray.AddImage(NULL);

	char bossBGCDG[]="ST00BK.CDG";
	bossBGCDG[3]=48+CGame::GVar().m_playStage;
	if (CGame::GVar().m_playStage==0)		//hack for the palette change
		m_palette[0]=m_palette[2]=85;
	LOAD_DAT2_CD2G(bossBGCDG);
	if (CGame::GVar().m_playStage==0)		//restore hacked palette
		m_palette[0]=m_palette[2]=0;

	if (CGame::GVar().m_playStage==5)		//hack for the palette change
	{
		m_palette[0]=96;
		m_palette[1]=0;
		m_palette[2]=0;
		LOAD_DAT2_CD2G("ST05BK2.CDG");
		m_palette[0]=m_palette[1]=m_palette[2]=0;
	}

	CCommonFunctionGraphic::LoadCD2CDGFromDat(&m_pImgLogo,&CGame::s_pCurGame->m_th5Dat2,"EYE.CDG",0,m_palette);

	InitSpecificImages();
}

void CStageResource::DrawGauge(int curValue,int fullValue,int drawX,int drawY,float colorR,float colorG,float colorB)
{
	if (curValue<=0) return;
	if (curValue>fullValue) curValue=fullValue;
	unsigned char str[8];
	if (curValue==fullValue)
		for (int i=0;i<8;i++)
			str[i]=0x30+i;
	else
	{
		curValue=curValue*128/fullValue;
		for (int i=0;i<curValue/16;i++)
			str[i]=0x30;
		for (int i=curValue/16;i<8;i++)
			str[i]=2;
		if (curValue%16)
			str[curValue/16]=curValue%16+0x20-1;
	}
	CTh5ExtFont::DrawExtString(str,8,drawX,drawY,colorR,colorG,colorB);
}

void CStageResource::DrawNumber(int numX,int numY,int number,int nDigit,bool leadingZero,bool rightAlign,float colorR,float colorG,float colorB)
{
	unsigned char digit[20];
	for (int i=0;i<nDigit;i++)
	{
		digit[i]=number%10;
		number/=10;
	}
	digit[nDigit-1]+=number*10;		//this is to simulate "score overflow to hex value" of PC98 ver
	int startDigit=nDigit-1;
	if (leadingZero==false)
	{
		startDigit=0;
		for (int i=0;i<nDigit;i++)
			if (digit[i]!=0)
				startDigit=i;
	}
	if (rightAlign)
		numX+=16*(nDigit-1-startDigit);
	for (int i=startDigit;i>=0;i--)
	{
		CTh5ExtFont::DrawExtChar(digit[i]+0xa0,numX,numY,colorR,colorG,colorB);
		numX+=16;
	}
}

void CStageResource::DrawCircle(float gameCoordX,float gameCoordY,float radius,float colorR,float colorG,float colorB,bool bSolid,float alpha)
{
	gameCoordX=gameCoordX/16.0f+m_pStage->m_playAreaUpperLeftX;
	gameCoordY=gameCoordY/16.0f+m_pStage->m_playAreaUpperLeftY;
	gameCoordY=CGame::s_pCurGame->m_gameWindow.m_height-1-gameCoordY;

	radius/=16.0f;

	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
			   0,CGame::s_pCurGame->m_windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor4f(colorR,colorG,colorB,alpha);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	if (bSolid==false)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBegin(GL_POLYGON);
	int nSeg=(int)radius;
	if (nSeg<16) nSeg=16;
	for (int i=0;i<nSeg;i++)
	{
		float x=(float)(gameCoordX+cos(i*2*PI/nSeg)*radius);
		float y=(float)(gameCoordY+sin(i*2*PI/nSeg)*radius);
		glVertex2f(x,y);
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void CStageResource::DrawLine(float gameCoordSrcX,float gameCoordSrcY,float gameCoordTarX,float gameCoordTarY,
							  float colorR,float colorG,float colorB,float width)
{
	gameCoordSrcX=gameCoordSrcX/16.0f+m_pStage->m_playAreaUpperLeftX;
	gameCoordSrcY=gameCoordSrcY/16.0f+m_pStage->m_playAreaUpperLeftY;
	gameCoordSrcY=CGame::s_pCurGame->m_gameWindow.m_height-1-gameCoordSrcY;
	gameCoordTarX=gameCoordTarX/16.0f+m_pStage->m_playAreaUpperLeftX;
	gameCoordTarY=gameCoordTarY/16.0f+m_pStage->m_playAreaUpperLeftY;
	gameCoordTarY=CGame::s_pCurGame->m_gameWindow.m_height-1-gameCoordTarY;
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
			   0,CGame::s_pCurGame->m_windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor4f(colorR,colorG,colorB,1);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glLineWidth(width);
    glBegin(GL_LINES);
		glVertex2f(gameCoordSrcX,gameCoordSrcY);
		glVertex2f(gameCoordTarX,gameCoordTarY);
	glEnd();
	glLineWidth(1);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void CStageResource::DrawPoint(float gameCoordX,float gameCoordY,float colorR,float colorG,float colorB)
{
	gameCoordX=gameCoordX/16.0f+m_pStage->m_playAreaUpperLeftX;
	gameCoordY=gameCoordY/16.0f+m_pStage->m_playAreaUpperLeftY;
	gameCoordY=CGame::s_pCurGame->m_gameWindow.m_height-1-gameCoordY;

	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,CGame::s_pCurGame->m_windowWidth,
			   0,CGame::s_pCurGame->m_windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor4f(colorR,colorG,colorB,1);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
    glBegin(GL_POINTS);
		glVertex2f(gameCoordX,gameCoordY);
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void CStageResource::DrawRectangle(float gameCoordX,float gameCoordY,float width,float height,
						   float colorR,float colorG,float colorB,bool bSolid,float alpha)
{
	gameCoordX=gameCoordX/16.0f+m_pStage->m_playAreaUpperLeftX;
	gameCoordY=gameCoordY/16.0f+m_pStage->m_playAreaUpperLeftY;
	width/=16.0f;
	height/=16.0f;
	CCommonFunctionGraphic::DrawRectangle(gameCoordX,gameCoordY,gameCoordX+width-1,gameCoordY+height-1,
										  colorR,colorG,colorB,alpha,bSolid);
}

void CStageResource::DrawPlayAreaMask(unsigned char *pMask,float colorR,float colorG,float colorB)
{
	for (int i=0;i<23;i++)
		for (int j=0;j<24;j++)
			if ((pMask[i*4+j/8]&(1<<(7-j%8)))!=0)
			{
				float x,y;
				x=(float)(m_pStage->m_playAreaUpperLeftX+j*16);
				y=(float)(m_pStage->m_playAreaUpperLeftY+i*16);
				CCommonFunctionGraphic::DrawRectangle(x,y,x+16,y+16,colorR,colorG,colorB);
			}
}

void CStageResource::DrawStageObjectImage(C2DImage *pImage,float gameCoordX,float gameCoordY)
{
	pImage->Draw(gameCoordX/16-pImage->m_width/2+m_pStage->m_playAreaUpperLeftX,
				 gameCoordY/16-pImage->m_height/2+m_pStage->m_playAreaUpperLeftY);
}
void CStageResource::DrawStageObjectImageColorScaled(C2DImage *pImage,float gameCoordX,float gameCoordY,
												   float colorR,float colorG,float colorB)
{
	pImage->DrawColorScaled(gameCoordX/16-pImage->m_width/2+m_pStage->m_playAreaUpperLeftX,
							gameCoordY/16-pImage->m_height/2+m_pStage->m_playAreaUpperLeftY,
							colorR,colorG,colorB);
}
void CStageResource::DrawStageObjectImageWhiteMask(C2DImage *pImage,float gameCoordX,float gameCoordY)
{
	pImage->DrawWhiteMask(gameCoordX/16-pImage->m_width/2+m_pStage->m_playAreaUpperLeftX,
						  gameCoordY/16-pImage->m_height/2+m_pStage->m_playAreaUpperLeftY);
}
void CStageResource::ClearPlayArea(float colorR,float colorG,float colorB)
{
	CCommonFunctionGraphic::DrawRectangle((float)(m_pStage->m_playAreaUpperLeftX),
										  (float)(m_pStage->m_playAreaUpperLeftY),
										  (float)(m_pStage->m_playAreaUpperLeftX+384),
										  (float)(m_pStage->m_playAreaUpperLeftY+368),
										  colorR,colorG,colorB);
}

}
