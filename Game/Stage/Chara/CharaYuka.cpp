#include ".\charayuka.h"
#include "../Stage.h"
#include "../CharaBullet/CharaBullet.h"
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

CCharaYuka::CCharaYuka(void)
{
	m_moveSpeed=56.0f;
	m_moveSpeedDiagonal=40.0f;
	m_bombInvulnerableTime=224;
	m_bombClearEnemyBulletTime=192;
}

CCharaYuka::~CCharaYuka(void)
{
}

void CCharaYuka::StepBomb()
{
	if (m_bombTimer>=32&&m_bombTimer<160)
	{
		DecreaseDreamInBomb();
		if (m_bombTimer<=64)
			m_pStage->m_curScrFade=196-(m_bombTimer-32)*3;
		if (m_bombTimer>64)
		{
			if(m_pStage->m_curFrame%4==0)
			{
				m_pStage->m_bPlaySound[9]=true;
				m_pStage->m_curScrFade=150;
			}
			if(m_pStage->m_curFrame%4==2)
				m_pStage->m_curScrFade=100;
		}

		if (m_bombTimer==32)
		{
			m_bombAniRadius=16.0f*16.0f;
			m_bombAniBaseAngle=2.0f;
		}
		m_bombAniRadius+=2.0f*16.0f;
		m_bombAniBaseAngle+=2.0f;
	}
	if (m_bombTimer>=160)
	{
		if (m_bombTimer==160)
		{
			m_pStage->m_bPlaySound[15]=true;
			m_pStage->m_bInCharaBombForItem=false;
		}
		if (m_bombTimer!=192)
			m_pStage->m_curScrFade=192-(m_bombTimer-160)*3;
		else
		{
			m_pStage->m_bInCharaBomb=false;
			m_pStage->m_curScrFade=100;
		}
	}
	m_bombTimer++;
}

void CCharaYuka::DrawBomb()
{
	m_bombTimer--;
	if (m_bombTimer<32)
	{
		m_pStage->m_pStageRes->DrawPlayAreaMask(m_pStage->m_pStageRes->m_pCharaBombBB->GetMaskPtr(m_bombTimer/2),
								   m_pStage->m_pStageRes->m_palette[15*3]/255.0f,
								   m_pStage->m_pStageRes->m_palette[15*3+1]/255.0f,
								   m_pStage->m_pStageRes->m_palette[15*3+2]/255.0f);
	}
	else
		if (m_bombTimer<160)
		{
			m_pStage->m_pStageRes->ClearPlayArea(m_pStage->m_pStageRes->m_palette[14*3]/255.0f,
									m_pStage->m_pStageRes->m_palette[14*3+1]/255.0f,
									m_pStage->m_pStageRes->m_palette[14*3+2]/255.0f);
			C2DImage *pImage=m_pStage->m_pStageRes->m_spriteArray.GetImagePtr(0);
			pImage->Draw((float)(96+m_pStage->m_playAreaUpperLeftX-32),(float)(16+m_pStage->m_playAreaUpperLeftY-16));
			for (int i=0;i<16;i++)
			{
				C2DImage *pImage=m_pStage->m_pStageRes->m_patternArray.GetImagePtr(179);
				float coordX=(float)cos((m_bombAniBaseAngle/256.0f+i/(float)16)*2*PI)*m_bombAniRadius;
				float coordY=(float)sin((m_bombAniBaseAngle/256.0f+i/(float)16)*2*PI)*m_bombAniRadius;
				coordX+=192.0f*16.0f;
				coordY+=160.0f*16.0f+16.0f*16.0f;
				float c[3];
				c[0]=m_pStage->m_pStageRes->m_palette[15*3]/255.0f;
				c[1]=m_pStage->m_pStageRes->m_palette[15*3+1]/255.0f;
				c[2]=m_pStage->m_pStageRes->m_palette[15*3+2]/255.0f;
				m_pStage->m_pStageRes->DrawStageObjectImageColorScaled(pImage,coordX,coordY,c[0],c[1],c[2]);
			}
		}
	m_bombTimer++;
}

int CCharaYuka::GetBombDamage(float ctrX,float ctrY,float halfWidth,float halfHeight,bool isBoss)
{
	if (m_pStage->m_curFrame%4==0)
		return 3;
	return 0;
}

void CCharaYuka::ShootBullet()
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
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,200,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,184,6);
		}
		if (m_charaShootTimer%6==0)
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,184+(float)(rand()%16),9);
	    break;
	case 3:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,200,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,184,6);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,8);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,8);
		}
	    break;
	case 4:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,196+(float)(rand()%8),6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,180+(float)(rand()%8),6);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,186,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,198,7);
		}
		break;
	case 5:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,178,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,186,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,198,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,206,6);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,186,6);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,6);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,198,6);
		}
		break;
	case 6:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,176,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,184,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,200,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,208,6);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,186,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,198,7);
		}
	    break;
	case 7:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,176,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,183,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,190,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,194,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,201,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,208,6);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX-8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX+8.0f*16.0f,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,186,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,198,7);
		}
	    break;
	case 8:
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,176,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,183,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,190,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,194,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,201,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,208,6);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,180,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,186,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,198,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,204,7);
		}
		break;
	case 9 :
		if (m_charaShootTimer%18==0)
		{
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,168,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,176,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,184,6);
			GetCharaBullet()->AddBullet(m_curX-24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,193,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,191,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,200,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,208,6);
			GetCharaBullet()->AddBullet(m_curX+24.0f*16.0f,m_curY,22,CHARA_BULLET_TYPE_NORMAL,0,0,216,6);
		}
		if (m_charaShootTimer%6==0)
		{
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,180,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,186,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,192,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,198,7);
			GetCharaBullet()->AddBullet(m_curX,m_curY,20,CHARA_BULLET_TYPE_NORMAL,0,0,204,7);
		}
		break;
	}
}

}
