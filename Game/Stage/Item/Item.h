#pragma once

namespace th5w{

#define ITEM_POWER 0
#define ITEM_POINT 1
#define ITEM_DREAM 2
#define ITEM_BIGPOWER 3
#define ITEM_BOMB 4
#define ITEM_1UP 5
#define ITEM_FULLPOWER 6

#define ITEM_N_RECORD 40
#define ITEM_SPLASH_N_RECORD 8

typedef struct strITEM{
	unsigned char flag;
	//unsigned char age;		//PC98 ver has 1 byte left here which is never used, guess its for age but
								//age is unnecessary for items
	float curX,curY;
	float lastX,lastY;
	float velX,velY;
	unsigned char itemType;
	//unsigned char unknown;		//PC98 ver has 1 byte here which is always initialized to 0 but never used
	unsigned char imgIdx;			//always itemType+36;
	bool bLastFrameInBomb;
}ITEM;

typedef struct strITEM_SPLASH{
	unsigned char flag;
	//unsigned char unknown;	//PC98 ver has 1 byte here which is always initialized to 16 but never used
	float ctrX,ctrY;
	float radius;
	float lastRadius;
}ITEM_SPLASH;

class CStage;

class CItem
{
friend class CStage;
protected:
	CStage *m_pStage;
protected:
	float m_clipXLow,m_clipXHigh,m_clipYLow,m_clipYHigh;

protected:
	ITEM m_item[ITEM_N_RECORD];
	ITEM_SPLASH m_itemSplash[ITEM_SPLASH_N_RECORD];
protected:
	void AddSplash(float ctrX,float ctrY);
	void OnMissItem(unsigned char itemType);
public:
	void Initialize();
	void AddItem(float itemX,float itemY,unsigned char itemType,float velX,float velY,bool bSplash=true,bool bConvertInBomb=true);
	void Step();
	void Draw();
public:
	CItem(CStage *pStage);
	~CItem(void);
};

}