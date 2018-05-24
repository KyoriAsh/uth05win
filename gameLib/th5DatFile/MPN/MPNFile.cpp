#include ".\mpnfile.h"
#include <stdio.h>
#include <string.h>

namespace th5w{

CMPNFile::CMPNFile(unsigned char *content,int contentLen)
{
	m_content=content;
	m_contentLen=contentLen;
	m_bValidFormat=ParseHeader();
}

CMPNFile::~CMPNFile(void)
{
}

bool CMPNFile::ParseHeader()
{
	if (m_content[0]!='M'||m_content[1]!='P'||m_content[2]!='T'||m_content[3]!='N')
		return false;
	m_nImage=m_content[4]+m_content[5]*256;
	memcpy(m_palette,m_content+6,48);

	return true;
}

bool CMPNFile::ExtractImageAndPalette(unsigned char *outImage, unsigned char *outPalette,int recordIdx)
{
	if (outPalette!=NULL)
		memcpy(outPalette,m_palette,48);
	if (recordIdx<0||recordIdx>=m_nImage)
		return false;

	memset(outImage,0,1024);
	unsigned char *ptr=m_content+6+48+recordIdx*128;
	for (int bitplane=0;bitplane<4;bitplane++)
	{
		for (int i=0;i<256;i++)
		{
			int v=ptr[30-i/16*2+i%16/8]&(1<<(7-i%8));
			if (v!=0)
				outImage[i*4]|=(1<<bitplane);
		}
		ptr+=32;
	}
	for (int i=0;i<256;i++)
	{
		outImage[i*4+3]=255;
		outImage[i*4+2]=m_palette[outImage[i*4]*3+2];
		outImage[i*4+1]=m_palette[outImage[i*4]*3+1];
		outImage[i*4]=m_palette[outImage[i*4]*3];

	}

	return true;
}

}
