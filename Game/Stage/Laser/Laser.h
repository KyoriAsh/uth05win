#pragma once

namespace th5w{

#define LASER_N_RECORD 32
#define LASER_MODE_NONE 0
#define LASER_MODE_SHOOT_OUT 1
#define LASER_MODE_WAIT_TO_GROW 2
#define LASER_MODE_GROW 3
#define LASER_MODE_PRESENT 4
#define LASER_MODE_SHRINK 5
#define LASER_MODE_SHRINK_AND_WAIT_TO_GROW 6
#define LASER_MODE_SHOOT_OUT_END 7

typedef struct strLASER{
	unsigned char mode;
	float color[3];
	float srcX,srcY;
	float lenLower,lenUpper;
	float angle;
	float width;
	float speed;
	int age;
	int ageThres;
	int shootAgeThres;
	float expectedWidth;
}LASER;

class CStage;

class CLaser
{
friend class CStage;
protected:
	CStage *m_pStage;
protected:
	LASER m_laser[LASER_N_RECORD];

public:
	void AddLaserFixed(int laserIdx,float srcX,float srcY,float color[3],float expectedWidth,float angle,
					   int ageThres=-1,int shootAgeThres=-1);
	void AddLaserShootOut(float srcX,float srcY,float color[3],float angle,int ageThres,float speed);
	void StartGrow(int laserIdx);
	void SetMode(int laserIdx,unsigned char newMode);
	void RemoveLaser(int laserIdx);
	__forceinline float& LaserAngle(int laserIdx) {return m_laser[laserIdx].angle;}
	__forceinline void SetLaserSrc(int laserIdx,float srcX,float srcY)
	{m_laser[laserIdx].srcX=srcX;m_laser[laserIdx].srcY=srcY;}
	__forceinline unsigned char GetMode(int laserIdx) {return m_laser[laserIdx].mode;}
protected:
	void TestCrash(int laserIdx);
public:
	void Initialize();
	void Step();
	void Draw();
public:
	CLaser(CStage *pStage);
	~CLaser(void);
};

}
