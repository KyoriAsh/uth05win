#pragma once

namespace th5w{

class CBFTFile
{
protected:
	unsigned char *m_content;
	int m_contentLen;
	int m_recordSize;

	unsigned char m_palette[48];
	unsigned char *m_imgData;

	int m_nFile;
	int m_width,m_height;

	bool m_bHasPalette;
public:
	bool m_bValidFormat;

protected:
	bool ParseHeader();

public:
	__forceinline int NPattern() {return m_nFile;}
	__forceinline int GetWidth() {return m_width;}
	__forceinline int GetHeight() {return m_height;}
	bool ExtractImageAndPalette(unsigned char *outImage, unsigned char *outPalette,int recordIdx,unsigned char *inPalette,int transparentColorIdx);

public:
	CBFTFile(unsigned char *content,int contentLen);
	~CBFTFile(void);
};

}
