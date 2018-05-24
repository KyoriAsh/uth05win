#include ".\charamarisa.h"
#include "../Stage.h"
#include "../SparkEffect/SparkEffect.h"
#include "../CharaBullet/CharaBullet.h"
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#define MARISA_BOMB_ANI_RECORD_X_UNUSED 64537.0f		//value taken from PC98 ver


namespace th5w{

CCharaMarisa::CCharaMarisa(void)
{
	m_moveSpeed=64.0f;
	m_moveSpeedDiagonal=48.0f;
	m_bombInvulnerableTime=160;
	m_bombClearEnemyBulletTime=128;
}

CCharaMarisa::~CCharaMarisa(void)
{
}

void CCharaMarisa::StepBomb()
{
	if (m_bombTimer<80)
	{
		if (m_bombTimer<16)
			m_pStage->m_curScrFade=196-m_bombTimer*6;
		else
		{
			if (m_bombTimer==16)
			{
				m_pStage->m_curScrFade=100;
				for (int i=0;i<MARISA_BOMB_ANI_N_RECORD;i++)
					m_bombAni[i].x=MARISA_BOMB_ANI_RECORD_X_UNUSED;
			}
			if (m_bombTimer%4==0)
			{
				m_bombAni[m_bombTimer/4%8].x=m_curX;
				m_bombAni[m_bombTimer/4%8].y=m_curY;
				m_pStage->m_pSparkEffect->AddEffect(m_curX,m_curY,192,24,true);
				m_bombAni[m_bombTimer/4%8].width=24*16.0f;
				m_pStage->m_bPlaySound[15]=true;
				m_pStage->m_curScrFade=170;
			}
			else
				m_pStage->m_curScrFade=100;
			for (int i=0;i<MARISA_BOMB_ANI_N_RECORD;i++)
			{
				m_bombAni[i].width-=1.0f*16.0f;
				if (m_bombAni[i].width<3.999f)
					m_bombAni[i].x=MARISA_BOMB_ANI_RECORD_X_UNUSED;
			}
			m_charaShootTimer=255;
		}
		DecreaseDreamInBomb();
	}
	else
	{
		if (m_bombTimer==80)
		{
			m_pStage->m_bPlaySound[15]=true;
			m_pStage->m_bInCharaBombForItem=false;
		}
		if (m_bombTimer!=96)
			m_pStage->m_curScrFade=192-(m_bombTimer-80)*6;
		else
		{
			m_pStage->m_bInCharaBomb=false;
			m_pStage->m_curScrFade=100;
		}
	}
	m_bombTimer++;
}

void CCharaMarisa::DrawBomb()
{
	m_bombTimer--;

	if (m_bombTimer<80)
	{
		m_pStage->m_pStageRes->ClearPlayArea(m_pStage->m_pStageRes->m_palette[1*3]/255.0f,
								m_pStage->m_pStageRes->m_palette[1*3+1]/255.0f,
								m_pStage->m_pStageRes->m_palette[1*3+2]/255.0f);
		C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(0);
		pImage->Draw((float)(112+m_pStage->m_playAreaUpperLeftX-32),(float)(16+m_pStage->m_playAreaUpperLeftY-16));
		if (m_bombTimer>=16)
		{
			for (int i=0;i<MARISA_BOMB_ANI_N_RECORD;i++)
				if (m_bombAni[i].x!=MARISA_BOMB_ANI_RECORD_X_UNUSED)
				{
					float ctrX=m_bombAni[i].x;
					float width=m_bombAni[i].width;
					float color[3];
					color[0]=m_pStage->m_pStageRes->m_palette[8*3]/255.0f;
					color[1]=m_pStage->m_pStageRes->m_palette[8*3+1]/255.0f;
					color[2]=m_pStage->m_pStageRes->m_palette[8*3+2]/255.0f;
					m_pStage->m_pStageRes->DrawRectangle(ctrX-width/2.0f,0,width,m_bombAni[i].y,color[0],color[1],color[2],true);
					width-=2*16.0f;
					color[0]=m_pStage->m_pStageRes->m_palette[9*3]/255.0f;
					color[1]=m_pStage->m_pStageRes->m_palette[9*3+1]/255.0f;
					color[2]=m_pStage->m_pStageRes->m_palette[9*3+2]/255.0f;
					if (width>0.0001f)
						m_pStage->m_pStageRes->DrawRectangle(ctrX-width/2.0f,0,width,m_bombAni[i].y,color[0],color[1],color[2],true);
					width-=2*16.0f;
					color[0]=m_pStage->m_pStageRes->m_palette[15*3]/255.0f;
					color[1]=m_pStage->m_pStageRes->m_palette[15*3+1]/255.0f;
					color[2]=m_pStage->m_pStageRes->m_palette[15*3+2]/255.0f;
					if (width>0.0001f)
						m_pStage->m_pStageRes->DrawRectangle(ctrX-width/2.0f,0,width,m_bombAni[i].y,color[0],color[1],color[2],true);
					m_pStage->m_pStageRes->DrawCircle(ctrX,m_bombAni[i].y,m_bombAni[i].width,color[0],color[1],color[2],true);
				}
		}
	}

	m_bombTimer++;
}

int CCharaMarisa::GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss)
{
	int totalDmg=0;
	if (m_pStage->m_curFrame%4==0)
		totalDmg+=4;
	if (m_bombTimer>=16&&m_bombTimer<80&&m_bombTimer%4==0)
	{
		if (m_curX>=ctrX-halfWidth&&m_curX<=ctrX+halfWidth&&m_curY>=ctrY)
			totalDmg+=22;
	}
	return totalDmg;
}

void CCharaMarisa::ShootBullet()
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
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,26,32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,16);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-26,32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,16);
		}
		if (m_charaShootTimer%6==0)
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,184+(float)(rand()%16),8);
	    break;
	case 3:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,22,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,13);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-22,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,13);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,13);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,13);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
		}
	    break;
	case 4:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,22,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,12);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-22,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,12);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,16,-48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,12);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,-16,-48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,12);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-64,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,12);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-64,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,12);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,186,6);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,6);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,198,6);
		}
		break;
	case 5:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,22,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-22,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,16,-48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,-16,-48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,-8,-64,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,8,-64,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,185,6);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,6);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,199,6);
		}
		break;
	case 6:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,22,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-22,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,16,-48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,-16,-48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-80,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-80,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,5);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,5);
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,5);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,5);
		}
	    break;
	case 7:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,28,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-28,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-32,32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,32,32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,16,48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,-16,48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-80,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-80,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,11);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,5);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,5);
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,5);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,5);
		}
	    break;
	case 8:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,28,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-28,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-32,32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,32,32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,16,48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,-16,48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-80,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-80,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
		}
		break;
	case 9 :
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,32,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-32,36,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_LEFT,-32,32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_RIGHT,32,32,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,48,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,16,0,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,-16,0,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-128,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-128,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-64,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_MISSILE_STRAIGHT,0,-64,
										ADD_CHARA_BULLET_IGNORE_MOVEANGLE,10);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX+40.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
			GetCharaBullet()->AddBullet(m_curX-40.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,4);
		}
		break;
	}
}

}
