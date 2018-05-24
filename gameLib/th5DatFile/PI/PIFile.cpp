#include ".\pifile.h"
#include <stdio.h>
#include <string.h>

namespace th5w{

unsigned char PIdefaultPalette[48]={0x00,0x00,0x00,
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

CPIFile::CPIFile(unsigned char *content,int contentLen)
{
	m_content=content;
	m_contentLen=contentLen;
	m_bValidFormat=ParseHeader();
}

CPIFile::~CPIFile(void)
{
}

bool CPIFile::ParseHeader()
{
	if (m_content[0]!='P'||m_content[1]!='i')
		return false;

	unsigned char *ptr=m_content;
	while((*ptr)!=26) ptr++;
	while((*ptr)!=0) ptr++;
	ptr++;
	if ((*ptr)&(1<<7))
		m_bHasPalette=false;
	else
		m_bHasPalette=true;
	ptr++;
	ptr+=2;
	if (*ptr!=4)
		return false;
	ptr++;
	ptr+=4;
	ptr+=2+(*ptr)*256+(*(ptr+1));
	m_width=(*ptr)*256+(*(ptr+1));
	ptr+=2;
	m_height=(*ptr)*256+(*(ptr+1));
	ptr+=2;
	if (m_bHasPalette)
	{
		m_palette=ptr;
		for (int i=0;i<48;i++)
		{
			m_palette[i]>>=4;
			m_palette[i]*=17;
		}
        ptr+=48;
	}
	else
		m_palette=NULL;
	m_imgData=ptr;

	return true;
}

bool CPIFile::ExtractImageAndPalette(unsigned char *outImage, unsigned char *outPalette,
									 unsigned char *inPalette, unsigned char transparentColorIdx)
{
	if (m_bValidFormat==false)
		return false;

	if (outImage==NULL)
	{
		if (outPalette!=NULL)
		{
			if (m_bHasPalette==false)
				return false;
			else
				memcpy(outPalette,m_palette,48);
		}
		return true;
	}

	for (int i=0;i<16;i++)
		for (int j=0;j<16;j++)
			table[i][j]=(i-j+16)%16;
	int datalen=m_contentLen-(int)(m_imgData-m_content);
	bitstream=new unsigned char[datalen*8];
	for (int i=0;i<datalen;i++)
	{
		bitstream[i*8]=(((m_imgData[i]&(1<<7)))?1:0);
		bitstream[i*8+1]=(((m_imgData[i]&(1<<6)))?1:0);
		bitstream[i*8+2]=(((m_imgData[i]&(1<<5)))?1:0);
		bitstream[i*8+3]=(((m_imgData[i]&(1<<4)))?1:0);
		bitstream[i*8+4]=(((m_imgData[i]&(1<<3)))?1:0);
		bitstream[i*8+5]=(((m_imgData[i]&(1<<2)))?1:0);
		bitstream[i*8+6]=(((m_imgData[i]&(1<<1)))?1:0);
		bitstream[i*8+7]=(((m_imgData[i]&(1<<0)))?1:0);
	}
	bsoff=0;

	endidx=m_width*(m_height+2);

	bmpbuf=new unsigned char[m_width*(m_height+2)];

	Decode();

	delete bitstream;

	unsigned char *palette;
	if (m_bHasPalette)
		palette=m_palette;
	else
		if (inPalette!=NULL)
			palette=inPalette;
		else
			palette=PIdefaultPalette;

	if (outPalette!=NULL&&m_bHasPalette)
		memcpy(outPalette,m_palette,48);

	//convert format and apply palette
	for (int i=0;i<m_width*m_height;i++)
	{
		int x=i%m_width;
		int y=i/m_width;
		int j=(m_height-1-y)*m_width+x;
		unsigned char idx=bmpbuf[m_width*2+i];
		outImage[j*4]=palette[idx*3];
		outImage[j*4+1]=palette[idx*3+1];
		outImage[j*4+2]=palette[idx*3+2];
		if (idx==transparentColorIdx)
			outImage[j*4+3]=0;
		else
			outImage[j*4+3]=255;
	}

	delete bmpbuf;

	return true;
}


unsigned char CPIFile::getnextbit()
{
	return bitstream[bsoff++];
}

int CPIFile::getbits(int len)
{
	int ret=0;
	for (int i=0;i<len;i++)
		ret=ret*2+getnextbit();
	return ret;
}

unsigned char CPIFile::getposition()
{
	unsigned char ret=0;
	ret=ret*2+getnextbit();
	ret=ret*2+getnextbit();
	if (ret==3)
		ret=ret+getnextbit();
	return ret;
}

unsigned char CPIFile::getcolor()
{
	unsigned char ret=0;
	ret=getnextbit();
	if (ret==1)
		return getbits(1);
	ret=getnextbit();
	if (ret==0)
		return 2+getbits(1);
	ret=getnextbit();
	if (ret==0)
		return 4+getbits(2);
	return 8+getbits(3);
}

unsigned char CPIFile::getrealcolor(unsigned char lastcolor)
{
	unsigned char color=getcolor();
	unsigned char realcolor=table[lastcolor][color];
	for (int z=color;z>0;z--) table[lastcolor][z]=table[lastcolor][z-1];
	table[lastcolor][0]=realcolor;
	return realcolor;
}

int CPIFile::getlen()
{
	if (getnextbit()==0)
		return 1;
	if (getnextbit()==0)
		return 2+getnextbit();
	if (getnextbit()==0)
		return 4+getbits(2);
	if (getnextbit()==0)
		return 8+getbits(3);
	int a=16,b=4;
	while(1)
	{
		if (getnextbit()==0)
			break;
		a*=2;b++;
	}
	return a+getbits(b);
}

void CPIFile::Decode()
{
	int bmpoffset=m_width*2;
	unsigned char lastcolor=0;
	unsigned char realcolor=getrealcolor(lastcolor);
	lastcolor=realcolor;
	bmpbuf[bmpoffset]=realcolor;
	realcolor=getrealcolor(lastcolor);
	lastcolor=realcolor;
	bmpbuf[bmpoffset+1]=realcolor;

	for (int i=0;i<m_width;i++)
	{
		bmpbuf[i*2]=bmpbuf[bmpoffset];
		bmpbuf[i*2+1]=bmpbuf[bmpoffset+1];
	}

	int lastposition=6;
	while(1)
	{
		unsigned char position=getposition();
		//printf("%d %d\n",position,lastposition);
		if (position==lastposition)
		{
			lastposition=6;
			realcolor = getrealcolor(lastcolor);
			lastcolor = realcolor;
			unsigned char realcolor1 = getrealcolor(lastcolor);
			lastcolor = realcolor1;
			bmpbuf[bmpoffset] = realcolor;
			bmpbuf[bmpoffset+1] = realcolor1;
			bmpoffset+=2;
			//printf("S %d %d\t\t%d %d\n",bsoff,bmpoffset,realcolor,realcolor1);
			if (bmpoffset>=endidx) return;
			while (getnextbit()==1)
			{
				realcolor = getrealcolor(lastcolor);
				lastcolor = realcolor;
				unsigned char realcolor1 = getrealcolor(lastcolor);
				lastcolor = realcolor1;
				bmpbuf[bmpoffset] = realcolor;
				bmpbuf[bmpoffset+1] = realcolor1;
				bmpoffset+=2;
				//printf("S %d %d\t\t%d %d\n",bsoff,bmpoffset,realcolor,realcolor1);
				if (bmpoffset>=endidx) return;
			}
		}
		else
		{
			int len=getlen()*2;
			int readoffset=0;
			if (position==0)
			{
				if (bmpbuf[bmpoffset-1]!=bmpbuf[bmpoffset-2])
					readoffset=-4;
				else
					readoffset=-2;
			}
			if (position==1)
				readoffset=-m_width;
			if (position==2)
				readoffset=-m_width*2;
			if (position==3)
				readoffset=-m_width+1;
			if (position==4)
				readoffset=-m_width-1;
			for (int i=0;i<len;i++)
				bmpbuf[bmpoffset+i]=bmpbuf[bmpoffset+readoffset+i];
			//memcpy(bmpbuf+bmpoffset,bmpbuf+bmpoffset+readoffset,len);
			bmpoffset+=len;
			//printf("L %d %d\n",bsoff,bmpoffset);
			if (bmpoffset>=endidx) return;
			lastcolor=bmpbuf[bmpoffset-1];
			lastposition=position;
		}
	}
}

}



























