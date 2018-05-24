#include ".\item.h"
#include "../Stage.h"
#include "../Chara/Chara.h"
#include <string.h>
#include <math.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace th5w{

CItem::CItem(CStage *pStage)
{
	m_pStage=pStage;
	Initialize();
}

CItem::~CItem(void)
{
}

void CItem::Initialize()
{
	memset(m_item,0,sizeof(m_item));
	memset(m_itemSplash,0,sizeof(m_itemSplash));
	m_clipXLow=-128;
	m_clipXHigh=6272;
	m_clipYLow=-128;
	m_clipYHigh=6016;
}

void CItem::OnMissItem(unsigned char itemType)
{
	if (itemType==ITEM_POINT&&m_pStage->GetDream()>1&&m_pStage->GetDream()<128)	//point item
		m_pStage->SetDream(m_pStage->GetDream()-1);
	if (itemType==ITEM_BOMB)													//bomb item
		m_pStage->ChangePlayerPerformance(-2);
	if (itemType==ITEM_1UP)													//1UP item
		m_pStage->ChangePlayerPerformance(-4);
}

void CItem::AddSplash(float ctrX,float ctrY)
{
	for (int i=0;i<ITEM_SPLASH_N_RECORD;i++)
		if (m_itemSplash[i].flag==0)
		{
			m_itemSplash[i].flag=1;
			m_itemSplash[i].ctrX=ctrX;
			m_itemSplash[i].ctrY=ctrY;
			m_itemSplash[i].radius=32.0f;
			m_itemSplash[i].lastRadius=32.0f;
			break;
		}
}

void CItem::AddItem(float itemX,float itemY,unsigned char itemType,float velX,float velY,bool bSplash,bool bConvertInBomb)
{
	if (bConvertInBomb&&m_pStage->m_bInCharaBombForItem)
	{
		if (itemType<=2||itemType==0xff)		//power or point or dream or follow table
			itemType=ITEM_POWER;				//all convert to power
	}
	for (int i=0;i<ITEM_N_RECORD;i++)
		if (m_item[i].flag==0)
		{
			m_item[i].flag=1;
			m_item[i].curX=itemX;
			m_item[i].curY=itemY;
			m_item[i].velX=velX;
			m_item[i].velY=velY;
			m_item[i].itemType=itemType;
			m_item[i].imgIdx=itemType+36;
			m_item[i].bLastFrameInBomb=false;
			if (bSplash)
				AddSplash(itemX,itemY);
			break;
		}
}

void CItem::Step()
{
	for (int i=0;i<ITEM_N_RECORD;i++)
	{
		if (m_item[i].flag==2||m_item[i].flag==0)
		{
			m_item[i].flag=0;
			continue;
		}
		if (m_pStage->m_bInCharaBombForItem)
		{
			m_item[i].bLastFrameInBomb=true;
			float angle=m_pStage->m_pChara->AngleToChara(m_item[i].curX,m_item[i].curY);
			m_item[i].velX=(float)cos(angle/256.0f*2*PI)*160;
			m_item[i].velY=(float)sin(angle/256.0f*2*PI)*160;
		}
		else
		{
			if (m_item[i].bLastFrameInBomb)
			{
				m_item[i].bLastFrameInBomb=false;
				m_item[i].velX=0;
				m_item[i].velY=0;
			}
		}
		m_item[i].lastX=m_item[i].curX;
		m_item[i].lastY=m_item[i].curY;
		m_item[i].curX+=m_item[i].velX;
		m_item[i].curY+=m_item[i].velY;
		if (m_item[i].curX<=m_clipXLow||m_item[i].curX>=m_clipXHigh||m_item[i].curY>=m_clipYHigh)
		{
			m_item[i].flag=2;
			OnMissItem(m_item[i].itemType);
			continue;
		}
		if (m_item[i].curY<m_clipYLow)
			m_item[i].curY=m_clipYLow;
		if (m_item[i].velY>=0)
			m_item[i].velX=0;
		if (m_pStage->m_pChara->GetNextLifeComeTimeLeft()==0)
		{
			if (m_item[i].curX>=m_pStage->m_pChara->GetCurX()-24.0f*16.0f)
				if (m_item[i].curX<=m_pStage->m_pChara->GetCurX()+24.0f*16.0f)
					if (m_item[i].curY>=m_pStage->m_pChara->GetCurY()-14.0f*16.0f)
						if (m_item[i].curY<=m_pStage->m_pChara->GetCurY()+24.0f*16.0f)
						{
							m_pStage->m_pChara->CaughtItem(m_item[i].curX,m_item[i].curY,m_item[i].itemType);
							m_pStage->m_bPlaySound[11]=true;
							m_item[i].flag=2;
						}
		}
		m_item[i].velY+=1.0f;
	}
	for (int i=0;i<ITEM_SPLASH_N_RECORD;i++)
	{
		if (m_itemSplash[i].flag==0||m_itemSplash[i].flag==2)
		{
			m_itemSplash[i].flag=0;
			continue;
		}
		m_itemSplash[i].lastRadius=m_itemSplash[i].radius;
		m_itemSplash[i].radius+=32.0f;
		if (m_itemSplash[i].radius>=512.0f)
			m_itemSplash[i].flag=2;
	}
}

void CItem::Draw()
{
	for (int i=0;i<ITEM_SPLASH_N_RECORD;i++)
		if (m_itemSplash[i].flag==1)
		{
			for (int j=0;j<256;j+=8)
			{
				float drawX=m_itemSplash[i].ctrX+(float)cos(j/256.0f*2*PI)*m_itemSplash[i].radius;
				float drawY=m_itemSplash[i].ctrY+(float)sin(j/256.0f*2*PI)*m_itemSplash[i].radius;
				float color[3];
				color[0]=m_pStage->m_pStageRes->m_palette[15*3]/255.0f;
				color[1]=m_pStage->m_pStageRes->m_palette[15*3+1]/255.0f;
				color[2]=m_pStage->m_pStageRes->m_palette[15*3+2]/255.0f;
				m_pStage->m_pStageRes->DrawPoint(drawX,drawY,color[0],color[1],color[2]);
			}
		}
	for (int i=0;i<ITEM_N_RECORD;i++)
		if (m_item[i].flag==1)
			m_pStage->m_pStageRes->DrawStageObjectImage(m_pStage->m_pStageRes->m_patternArray.GetImagePtr(m_item[i].imgIdx),
										   m_item[i].curX,m_item[i].curY);
}

}






















