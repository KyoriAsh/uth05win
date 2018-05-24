#pragma once

namespace th5w{

#define STAGE2BG_SPRITE_N_RECORD 64
typedef struct strSTAGE2BG_SPRITE{
	unsigned char flag;
	float angle;
	float curX,curY;
	float lastX,lastY;
	float velX,velY;
	int age;
}STAGE2BG_SPRITE;

class CStage;

class CStage2BG
{
friend class CStage;

protected:
	CStage *m_pStage;

protected:
	int m_scrScale;
	STAGE2BG_SPRITE m_bgSprite[STAGE2BG_SPRITE_N_RECORD];
	int m_nSprite;
	int m_spriteColorScale;
	int m_spriteColorDeltaScale;


protected:
	void StepSprites();
	void RegenerateSprite(int recordIdx);
public:
	void Initialize();
	void Step();
	void Draw();

public:
	CStage2BG(CStage *pStage);
	~CStage2BG(void);
};

}