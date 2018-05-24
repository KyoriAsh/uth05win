#include ".\charamima.h"
#include "../Stage.h"
#include "../CharaBullet/CharaBullet.h"
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#define MIMA_BOMB_ANI_RECORD_RADIUS_UNUSED 64537.0f		//value taken from PC98 ver

namespace th5w{

CCharaMima::CCharaMima(void)
{
	m_moveSpeed=72.0f;
	m_moveSpeedDiagonal=52.0f;
	m_bombInvulnerableTime=144;
	m_bombClearEnemyBulletTime=96;
}

CCharaMima::~CCharaMima(void)
{
}

void CCharaMima::StepBomb()
{
	if (m_bombTimer<64)
	{
		if (m_bombTimer==1)
		{
			m_pStage->m_curScrFade=100;		//yes, this assign is useless because there's another outside if.
											//kept here just to make it the same as PC98 ver
			for (int i=0;i<MIMA_BOMB_ANI_N_RECORD;i++)
				m_bombAni[i].radius=MIMA_BOMB_ANI_RECORD_RADIUS_UNUSED;
		}
		//PC98 ver does not have m_bombTimer>=16 condition, which may cause array index out range. fixed here
		if (m_bombTimer%4==0&&m_bombTimer>=16)
		{
			m_bombAni[(m_bombTimer-16)/4%8].radius=256.0f*16.0f;
			m_bombAni[(m_bombTimer-16)/4%8].angle=(float)(rand()%256);
		}
		for (int i=0;i<MIMA_BOMB_ANI_N_RECORD;i++)
			if (m_bombAni[i].radius!=MIMA_BOMB_ANI_RECORD_RADIUS_UNUSED)
			{
				m_bombAni[i].radius-=16.0f*16.0f;
				if (m_bombAni[i].radius<4.0f*16.0f)
					m_bombAni[i].radius=MIMA_BOMB_ANI_RECORD_RADIUS_UNUSED;
			}
		m_pStage->m_curScrFade=100+m_bombTimer;
		DecreaseDreamInBomb();
	}
	else
	{
		if (m_bombTimer==64)
		{
			m_pStage->m_bPlaySound[15]=true;
			m_pStage->m_bInCharaBombForItem=false;
		}
		if (m_bombTimer>=80)
		{
			m_pStage->m_bInCharaBomb=false;
			m_pStage->m_curScrFade=100;
		}
		else
			m_pStage->m_curScrFade=192-(m_bombTimer-64)*6;
	}
	m_bombTimer++;
}

void CCharaMima::DrawBomb()
{
	m_bombTimer--;
	if (m_bombTimer<64)
	{
		m_pStage->m_pStageRes->ClearPlayArea(m_pStage->m_pStageRes->m_palette[1*3]/255.0f,
								m_pStage->m_pStageRes->m_palette[1*3+1]/255.0f,
								m_pStage->m_pStageRes->m_palette[1*3+2]/255.0f);
		float ctrX=192.0f*16.0f;
		float width=32.0f*16.0f;
		float color[3];
		color[0]=m_pStage->m_pStageRes->m_palette[6*3]/255.0f;
		color[1]=m_pStage->m_pStageRes->m_palette[6*3+1]/255.0f;
		color[2]=m_pStage->m_pStageRes->m_palette[6*3+2]/255.0f;
		m_pStage->m_pStageRes->DrawRectangle(ctrX-width/2.0f,0,width,368.0f*16.0f,color[0],color[1],color[2],true);
		width-=2*16.0f;
		color[0]=m_pStage->m_pStageRes->m_palette[7*3]/255.0f;
		color[1]=m_pStage->m_pStageRes->m_palette[7*3+1]/255.0f;
		color[2]=m_pStage->m_pStageRes->m_palette[7*3+2]/255.0f;
		m_pStage->m_pStageRes->DrawRectangle(ctrX-width/2.0f,0,width,368.0f*16.0f,color[0],color[1],color[2],true);
		width-=2*16.0f;
		color[0]=m_pStage->m_pStageRes->m_palette[15*3]/255.0f;
		color[1]=m_pStage->m_pStageRes->m_palette[15*3+1]/255.0f;
		color[2]=m_pStage->m_pStageRes->m_palette[15*3+2]/255.0f;
		m_pStage->m_pStageRes->DrawRectangle(ctrX-width/2.0f,0,width,368.0f*16.0f,color[0],color[1],color[2],true);
		C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(0);
		pImage->Draw((float)(64+m_pStage->m_playAreaUpperLeftX-32),(float)(64+m_pStage->m_playAreaUpperLeftY-16));

		if (m_bombTimer>=16)
		{
			for (int i=0;i<MIMA_BOMB_ANI_N_RECORD;i++)
				if (m_bombAni[i].radius!=MIMA_BOMB_ANI_RECORD_RADIUS_UNUSED)
				{
					float coordX=(224-32)*16.0f;
					float coordY=(117-16)*16.0f;
					coordX+=(float)cos(m_bombAni[i].angle/256.0f*2*PI)*m_bombAni[i].radius;
					coordY+=(float)sin(m_bombAni[i].angle/256.0f*2*PI)*m_bombAni[i].radius;
					m_pStage->m_pStageRes->DrawCircle(coordX,coordY,m_bombAni[i].radius/2,color[0],color[1],color[2],true);
				}
		}
		{
			float coordX=(224-32)*16.0f;
			float coordY=(117-16)*16.0f;
			m_pStage->m_pStageRes->DrawCircle(coordX,coordY,(float)(224-m_bombTimer)*16,color[0],color[1],color[2],false);
		}
	}
	m_bombTimer++;
}

int CCharaMima::GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss)
{
	if (m_pStage->m_curFrame%4==0)
		return 15;
	return 0;
}

void CCharaMima::ShootBullet()
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
		if (m_charaShootTimer%6==0)
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,184+(float)(rand()%16),8);
		if (m_charaShootTimer%3==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,5);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,5);
		}
	    break;
	case 3:
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
		}
		if (m_charaShootTimer%3==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,5);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,5);
		}
	    break;
	case 4:
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
		}
		if (m_charaShootTimer%3==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,16,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-16,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
		}
		break;
	case 5:
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,185,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,199,7);
		}
		if (m_charaShootTimer%3==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-16,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,16,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
		}
		break;
	case 6:
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,185,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,199,7);
		}
		if (m_charaShootTimer%3==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-8,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,8,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,16,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-16,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
		}
	    break;
	case 7:
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,185,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,199,7);
		}
		if (m_charaShootTimer%3==0)
		{
			GetCharaBullet()->AddBullet(m_curX+28.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,16,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-28.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-16,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-8,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,8,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
		}
	    break;
	case 8:
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,185,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,199,7);
		}
		if (m_charaShootTimer%3==0)
		{
			GetCharaBullet()->AddBullet(m_curX+28.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,28,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-28.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-28,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-8,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,8,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,-320,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
		}
		break;
	case 9 :
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,184,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,200,7);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
		}
		if (m_charaShootTimer%3==0)
		{
			GetCharaBullet()->AddBullet(m_curX+32.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,42,-288,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-32.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-42,-288,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,8,-192,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,8,-192,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-8,-192,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,-8,-192,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,4);
		}
		break;
	}
}


}
