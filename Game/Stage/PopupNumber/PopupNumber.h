#pragma once

namespace th5w{

#define POPUP_NUMBER_MAX_DIGIT 6
#define POPUP_NUMBER_N_RECORD 280	//PC98 ver uses 200 records for white and 80 records for yellow,
									//here we just limit the total number but not each
typedef struct strPOPUP_NUMBER{
	unsigned char flag;
	unsigned char age;
	float curX;
	float curY;
	float lastY;
	unsigned char nDigit;
	unsigned char digits[POPUP_NUMBER_MAX_DIGIT];
	float color[3];
}POPUP_NUMBER;

class CStage;

class CPopupNumber
{
friend class CStage;

protected:
	CStage *m_pStage;
	POPUP_NUMBER m_numbers[POPUP_NUMBER_N_RECORD];

public:
	void AddNumber(float origX,float origY,int number,float colorR,float colorG,float colorB);
	void Initialize();
	void Step();
	void Draw();

public:
	CPopupNumber(CStage *pStage);
	~CPopupNumber(void);
};

}
