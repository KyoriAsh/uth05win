#include ".\bftfile.h"
#include <string.h>

namespace th5w{

unsigned char BFTdefaultPalette[48]={0x00,0x00,0x00,
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
CBFTFile::CBFTFile(unsigned char *content,int contentLen)
{
	m_content=content;
	m_contentLen=contentLen;

	m_bValidFormat=ParseHeader();
}

CBFTFile::~CBFTFile(void)
{
}

bool CBFTFile::ParseHeader()
{
	if (m_content[0]!='B'||m_content[1]!='F'||m_content[2]!='N'||m_content[3]!='T')
		return false;
	if (m_content[4]!=0x1a)
		return false;

	if (m_content[5]%128!=3)
		return false;
	
	m_bHasPalette=m_content[5]>128;

	m_width=*(unsigned short *)(m_content+8);
	m_height=*(unsigned short *)(m_content+10);
	m_nFile=*(unsigned short *)(m_content+14);
	m_nFile-=*(unsigned short *)(m_content+12);
	m_nFile++;

	if (m_content[30]!=0||m_content[31]!=0)
		return false;

	if (m_bHasPalette)
	{
		memcpy(&m_palette,m_content+32,48);
		for (int i=0;i<16;i++)
		{
			unsigned char tmp=m_palette[i*3];
			m_palette[i*3]=m_palette[i*3+1];
			m_palette[i*3+1]=m_palette[i*3+2];
			m_palette[i*3+2]=tmp;
			m_palette[i*3]>>=4;
			m_palette[i*3+1]>>=4;
			m_palette[i*3+2]>>=4;
			m_palette[i*3]*=17;
			m_palette[i*3+1]*=17;
			m_palette[i*3+2]*=17;
		}
		m_imgData=m_content+80;
	}
	else
		m_imgData=m_content+32;

	m_recordSize=m_width*m_height/2;

	return true;
}

bool CBFTFile::ExtractImageAndPalette(unsigned char *outImage, unsigned char *outPalette,int recordIdx,
									  unsigned char *inPalette,int transparentColorIdx)
{
	if (m_bValidFormat==false)
		return false;
	if (recordIdx<0||recordIdx>=NPattern())
		return false;

	unsigned char *palette;
	if (inPalette!=NULL)
		palette=inPalette;
	else
		if (m_bHasPalette)
			palette=m_palette;
		else
			palette=BFTdefaultPalette;

	unsigned char *srcPtr=m_imgData+recordIdx*m_recordSize;
	for (int i=0;i<m_height;i++)
		for (int j=0;j<m_width;j++)
		{
			unsigned char cidx=srcPtr[(i*m_width+j)/2];
			if (j%2==1)
				cidx%=16;
			else
				cidx/=16;
			int outIdx=(m_height-1-i)*m_width*4+j*4;
			outImage[outIdx]=palette[cidx*3];
			outImage[outIdx+1]=palette[cidx*3+1];
			outImage[outIdx+2]=palette[cidx*3+2];
			if (cidx==transparentColorIdx)
				outImage[outIdx+3]=0;
			else
				outImage[outIdx+3]=255;
		}

	return true;
}

}


















