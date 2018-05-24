#pragma once

namespace th5w{

class CMPNFile
{
protected:
	unsigned char *m_content;
	int m_contentLen;

	unsigned char m_palette[48];

	int m_nImage;

public:
	bool m_bValidFormat;

protected:
	bool ParseHeader();

public:
	__forceinline int NImage() {return m_nImage;}
	bool ExtractImageAndPalette(unsigned char *outImage, unsigned char *outPalette,int recordIdx);
public:
	CMPNFile(unsigned char *content,int contentLen);
	~CMPNFile(void);
};

}