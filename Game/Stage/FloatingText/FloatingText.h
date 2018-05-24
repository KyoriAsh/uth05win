#pragma once

namespace th5w{

#define FLOATING_TEXT_NONE -1
#define FLOATING_TEXT_HISCORE 0
#define FLOATING_TEXT_EXTEND 1
#define FLOATING_TEXT_BONUS 2
#define FLOATING_TEXT_FULLPOWER 3
#define FLOATING_TEXT_DREAMMAX 4

class CStage;

class CFloatingText
{
friend class CStage;

protected:
	CStage *m_pStage;

protected:
	int m_textType;
	int m_textAge;
	int m_textNumber;
	int m_textLen;
	int m_textCurX;
	int m_textDestX;
	bool m_floating;

public:
	void Initialize();
	void Step();
	void Draw();

	void SetTextType(int textType,int number=0);		//number is only needed for FLOATING_TEXT_BONUS

public:
	CFloatingText(CStage *pStage);
	~CFloatingText(void);
};

}