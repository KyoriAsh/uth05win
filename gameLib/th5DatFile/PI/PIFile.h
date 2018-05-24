#pragma once

namespace th5w{

class CPIFile
{
protected:
	unsigned char *m_content;
	int m_contentLen;

	int m_width,m_height;
	unsigned char *m_palette;
	unsigned char *m_imgData;
	bool m_bHasPalette;
public:
	bool m_bValidFormat;

//temporary variable for decoding
protected:
	unsigned char table[16][16];
	unsigned char *bitstream;
	unsigned char *bmpbuf;
	int bsoff;
	int endidx;

protected:
	unsigned char getnextbit();
	int getbits(int len);
	unsigned char getposition();
	unsigned char getcolor();
	unsigned char getrealcolor(unsigned char lastcolor);
	int getlen();
	void Decode();

protected:
	bool ParseHeader();

public:
	__forceinline int GetWidth() {return m_width;}
	__forceinline int GetHeight() {return m_height;}
public:
	bool ExtractImageAndPalette(unsigned char *outImage, unsigned char *outPalette,
								unsigned char *inPalette, unsigned char transparentColorIdx=-1);
public:
	CPIFile(unsigned char *content,int contentLen);
	~CPIFile(void);
};

}
