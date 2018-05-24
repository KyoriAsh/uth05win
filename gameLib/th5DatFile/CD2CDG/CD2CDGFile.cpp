#include ".\cd2cdgfile.h"
#include <stdio.h>

#define CD2CDG_FORMAT_MASK_ONLY 2
#define CD2CDG_FORMAT_MASK_AND_COLOR_INDEX 1
#define CD2CDG_FORMAT_COLOR_INDEX_ONLY 0


namespace th5w{
unsigned char CD2CDGdefaultPalette[48]={0x00,0x00,0x00,
										0x00,0x00,0x7f,
										0x7f,0x00,0x00,
										0x7f,0x00,0x7f,
										0x00,0x7f,0x00,
										0x00,0x7f,0x7f,
										0x7f,0x7f,0x00,
										0x7f,0x7f,0x7f,
										0x00,0x00,0x00,
										0x00,0x00,0xff,
										0xff,0x00,0x00,
										0xff,0x00,0xff,
										0x00,0xff,0x00,
										0x00,0xff,0xff,
										0xff,0xff,0x00,
										0xff,0xff,0xff,
										};
CCD2CDGFile::CCD2CDGFile(unsigned char *content,int contentLen)
{
	m_content=content;
	m_contentLen=contentLen;

	m_pHeader=(CD2CDG_HEADER *)content;
	m_bValidFormat=ValidateFormat();

	if (m_bValidFormat)
	{
		m_nFile=m_pHeader->nSprite;
		m_width=m_pHeader->spriteWidth;
		m_height=m_pHeader->spriteHeight;
		if (m_pHeader->formatFlag==CD2CDG_FORMAT_MASK_ONLY)
			m_recordSize=m_pHeader->recordBasicSize;
		if (m_pHeader->formatFlag==CD2CDG_FORMAT_MASK_AND_COLOR_INDEX)
			m_recordSize=m_pHeader->recordBasicSize*5;
		if (m_pHeader->formatFlag==CD2CDG_FORMAT_COLOR_INDEX_ONLY)
			m_recordSize=m_pHeader->recordBasicSize*4;
	}
}

CCD2CDGFile::~CCD2CDGFile(void)
{
}

bool CCD2CDGFile::ValidateFormat()
{
	if (m_pHeader->recordBasicSize*8!=m_pHeader->spriteWidth*m_pHeader->spriteHeight)
		return false;
	if (m_pHeader->spriteWidth%32!=0)
		return false;
	if (m_pHeader->spriteLastLineOffset!=(m_pHeader->spriteHeight-1)*80)
		return false;
	if (m_pHeader->lengthEachRowIn4Bytes*32!=m_pHeader->spriteWidth)
		return false;
	if (m_pHeader->formatFlag!=CD2CDG_FORMAT_MASK_ONLY&&
		m_pHeader->formatFlag!=CD2CDG_FORMAT_MASK_AND_COLOR_INDEX&&
		m_pHeader->formatFlag!=CD2CDG_FORMAT_COLOR_INDEX_ONLY)
		return false;

	return true;
}

bool CCD2CDGFile::ExtractImage(unsigned char *outBuf, int recordIdx,unsigned char *palette)
{
	if (m_bValidFormat==false)
		return false;

	if (recordIdx<0||recordIdx>=NSprite())
		return false;

	unsigned char *srcPtr=m_content+16+recordIdx*m_recordSize;

	if (m_pHeader->formatFlag==CD2CDG_FORMAT_MASK_ONLY)
	{
		unsigned char maskcolor[3];
		if (palette!=NULL)
		{
			maskcolor[0]=palette[0];
			maskcolor[1]=palette[1];
			maskcolor[2]=palette[2];
		}
		else
			maskcolor[0]=maskcolor[1]=maskcolor[2]=0;
		for (int i=0;i<m_recordSize;i++)
		{
			for (int j=0;j<8;j++)
				if ((srcPtr[i]&(1<<(7-j)))!=0)
				{
					outBuf[(i*8+j)*4]=maskcolor[0];
					outBuf[(i*8+j)*4+1]=maskcolor[1];
					outBuf[(i*8+j)*4+2]=maskcolor[2];
					outBuf[(i*8+j)*4+3]=255;
				}
				else
					outBuf[(i*8+j)*4]=outBuf[(i*8+j)*4+1]=outBuf[(i*8+j)*4+2]=outBuf[(i*8+j)*4+3]=0;
		}
		return true;
	}
	if (palette==NULL)
		palette=CD2CDGdefaultPalette;
	if (m_pHeader->formatFlag==CD2CDG_FORMAT_MASK_AND_COLOR_INDEX)
	{
		for (int i=0;i<m_recordSize/5;i++)
			for (int j=0;j<8;j++)
			{
				unsigned char bits[5];
				for (int k=0;k<5;k++)
				{
					bits[k]=srcPtr[i+m_recordSize/5*k]&(1<<(7-j));
					if (bits[k]&&k) bits[k]=1<<(k-1);
				}
				bits[1]+=bits[2]+bits[3]+bits[4];
				if (bits[0])
				{
					outBuf[(i*8+j)*4]=palette[bits[1]*3];
					outBuf[(i*8+j)*4+1]=palette[bits[1]*3+1];
					outBuf[(i*8+j)*4+2]=palette[bits[1]*3+2];
					outBuf[(i*8+j)*4+3]=255;
				}
				else
				{
					outBuf[(i*8+j)*4]=palette[0];
					outBuf[(i*8+j)*4+1]=palette[1];
					outBuf[(i*8+j)*4+2]=palette[2];
					outBuf[(i*8+j)*4+3]=0;
				}
			}
		return true;
	}
	if (m_pHeader->formatFlag==CD2CDG_FORMAT_COLOR_INDEX_ONLY)
	{
		for (int i=0;i<m_recordSize/4;i++)
			for (int j=0;j<8;j++)
			{
				unsigned char bits[4];
				for (int k=0;k<4;k++)
				{
					bits[k]=srcPtr[i+m_recordSize/4*k]&(1<<(7-j));
					if (bits[k]) bits[k]=1<<k;
				}
				bits[0]+=bits[1]+bits[2]+bits[3];
				outBuf[(i*8+j)*4]=palette[bits[0]*3];
				outBuf[(i*8+j)*4+1]=palette[bits[0]*3+1];
				outBuf[(i*8+j)*4+2]=palette[bits[0]*3+2];
				outBuf[(i*8+j)*4+3]=255;
			}
			return true;
	}

	return false;
}

}



















