#include ".\charareimu.h"
#include "../Stage.h"
#include "../Circle/Circle.h"
#include "../CharaBullet/CharaBullet.h"
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

CCharaReimu::CCharaReimu(void)
{
	m_moveSpeed=56.0f;
	m_moveSpeedDiagonal=40.0f;
	m_bombInvulnerableTime=192;
	m_bombClearEnemyBulletTime=192;
}

CCharaReimu::~CCharaReimu(void)
{
}

void CCharaReimu::StepBomb()
{
	if (m_bombTimer>=32&&m_bombTimer<128)
	{
		DecreaseDreamInBomb();
		if (m_bombTimer<=64)
			m_pStage->m_curScrFade=196-(m_bombTimer-32)*3;
		if (m_bombTimer>64&&m_bombTimer<=112&&m_pStage->m_curFrame%4==0)
			m_pStage->m_bPlaySound[9]=true;

		if (m_bombTimer==32)
		{
			//a bug in the PC98 ver cause the records not be properly initialized, fixed here
			for (int i=0;i<REIMU_BOMB_ANI_N_RECORD;i++)
			{
				m_bombAni[i].x[0]=(float)(rand()%5120+512);
				m_bombAni[i].y[0]=(float)(rand()%5888);
				for (int j=1;j<8;j++)
				{
					m_bombAni[i].x[j]=m_bombAni[i].x[0];
					m_bombAni[i].y[j]=m_bombAni[i].y[0];
				}
				float speed=(float)(160+rand()%128);
				float angle=(float)(40+rand()%48);	//the range is not horizontally symmetric, same as PC98 ver
				m_bombAni[i].velX=(float)(cos(angle/256.0f*2*PI)*speed);
				m_bombAni[i].velY=(float)(sin(angle/256.0f*2*PI)*speed);
			}
		}
		for (int i=0;i<REIMU_BOMB_ANI_N_RECORD;i++)
		{
			for (int j=7;j>=1;j--)
			{
				m_bombAni[i].x[j]=m_bombAni[i].x[j-1];
				m_bombAni[i].y[j]=m_bombAni[i].y[j-1];
			}
			m_bombAni[i].x[0]+=m_bombAni[i].velX;
			m_bombAni[i].y[0]+=m_bombAni[i].velY;
			if (m_bombAni[i].x[0]<=32.0f*16.0f||m_bombAni[i].x[0]>=352.0f*16.0f)
				m_bombAni[i].velX=-m_bombAni[i].velX;
			if (m_bombAni[i].y[0]>=400.f*16.0f)
				m_bombAni[i].y[0]-=400.f*16.0f;
			if (m_bombTimer<=112)
			{
				if (m_pStage->m_curFrame%8==i)
				{
					m_pStage->m_pCircle->AddCircle(m_bombAni[i].x[0],m_bombAni[i].y[0]+16.0f*16.0f,
												   m_pStage->m_pStageRes->m_palette[9*3]/255.0f,
												   m_pStage->m_pStageRes->m_palette[9*3+1]/255.0f,
												   m_pStage->m_pStageRes->m_palette[9*3+2]/255.0f,true);
				}
			}
		}
	}
	if (m_bombTimer>=128)
	{
		if (m_bombTimer==128)
		{
			m_pStage->m_bPlaySound[15]=true;
			m_pStage->m_bInCharaBombForItem=false;
		}
		if (m_bombTimer!=160)
			m_pStage->m_curScrFade=192-(m_bombTimer-128)*3;
		else
		{
			m_pStage->m_bInCharaBomb=false;
			m_pStage->m_curScrFade=100;
		}
	}
	m_bombTimer++;
}

void CCharaReimu::DrawBomb()
{
	m_bombTimer--;			//PC98 ver put StepBomb and DrawBomb in one function and inc m_bombTimer at end of function.
							//as we divide the code into two functions and put inc m_bombTimer at end of StepBomb
							//it must dec 1 at start of DrawBomb and inc 1 at end of DrawBomb to work correctly
	if (m_bombTimer<32)
	{
		m_pStage->m_pStageRes->DrawPlayAreaMask(m_pStage->m_pStageRes->m_pCharaBombBB->GetMaskPtr(m_bombTimer/2),
												m_pStage->m_pStageRes->m_palette[15*3]/255.0f,
												m_pStage->m_pStageRes->m_palette[15*3+1]/255.0f,
												m_pStage->m_pStageRes->m_palette[15*3+2]/255.0f);
	}
	else
		if (m_bombTimer<128)
		{
			m_pStage->m_pStageRes->ClearPlayArea(m_pStage->m_pStageRes->m_palette[15*3]/255.0f,
												 m_pStage->m_pStageRes->m_palette[15*3+1]/255.0f,
												 m_pStage->m_pStageRes->m_palette[15*3+2]/255.0f);
			C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(0);
			pImage->Draw((float)(80+m_pStage->m_playAreaUpperLeftX-32),(float)(16+m_pStage->m_playAreaUpperLeftY-16));
			for (int i=0;i<REIMU_BOMB_ANI_N_RECORD;i++)
			{
				C2DImage *pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(179);
				float c[3];
				c[0]=m_pStage->m_pStageRes->m_palette[9*3]/255.0f;
				c[1]=m_pStage->m_pStageRes->m_palette[9*3+1]/255.0f;
				c[2]=m_pStage->m_pStageRes->m_palette[9*3+2]/255.0f;
				m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(pImage,m_bombAni[i].x[6],m_bombAni[i].y[6],c[0],c[1],c[2]);
				m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(pImage,m_bombAni[i].x[4],m_bombAni[i].y[4],c[0],c[1],c[2]);
				m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(pImage,m_bombAni[i].x[2],m_bombAni[i].y[2],c[0],c[1],c[2]);
				c[0]=m_pStage->m_pStageRes->m_palette[8*3]/255.0f;
				c[1]=m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f;
				c[2]=m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f;
				m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(pImage,m_bombAni[i].x[0],m_bombAni[i].y[0],c[0],c[1],c[2]);
			}
		}

	m_bombTimer++;
}

int CCharaReimu::GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss)
{
	if (m_pStage->m_curFrame%4==0)
		return 3;
	return 0;
}

void CCharaReimu::ShootBullet()
{
	switch(m_powerLevel)
	{
	case 0:
		ShootPowerLevel0Common();
		break;
	case 1:
		ShootPowerLevel1Common();
		break;
	case 2:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,200,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,184,4);
		}
		if (m_charaShootTimer%6==0)
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,184+(float)(rand()%16),10);
	    break;
	case 3:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,200,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,184,4);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,9);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,9);
		}
	    break;
	case 4:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,200,3);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,184,3);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,186,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,198,9);
		}
		break;
	case 5:					//power level 5 and 6 shoot the same way
	case 6:
		if (m_charaShootTimer%9==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,
										198+(float)(rand()%4),3);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,
										182+(float)(rand()%4),3);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,186,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,198,9);
		}
	    break;
	case 7:
		if (m_charaShootTimer%9==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,
										198+(float)(rand()%4),3);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,
										182+(float)(rand()%4),3);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,182,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,187,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,197,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,202,9);
		}
	    break;
	case 8:
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,
										198+(float)(rand()%4),3);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,
										182+(float)(rand()%4),3);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,182,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,187,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,197,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,202,9);
		}
		break;
	case 9 :
		if (m_charaShootTimer%9==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,214,2);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,170,2);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,200,2);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_HOMING,0,0,184,2);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,182,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,187,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,197,9);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,202,9);
		}
		break;
	}
}

}
