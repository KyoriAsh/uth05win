#pragma once

namespace th5w{

typedef struct strCD2CDG_HEADER{
	unsigned short recordBasicSize;
	unsigned short spriteWidth;
	unsigned short spriteHeight;
	unsigned short spriteLastLineOffset;
	unsigned short lengthEachRowIn4Bytes;
	unsigned char nSprite;
	unsigned char formatFlag;
	unsigned int padding;
}CD2CDG_HEADER;

class CCD2CDGFile
{
protected:
	unsigned char *m_content;
	int m_contentLen;
	int m_recordSize;

	int m_nFile;
	int m_width,m_height;

	CD2CDG_HEADER *m_pHeader;
public:
	bool m_bValidFormat;

protected:
	bool ValidateFormat();

public:
	__forceinline int NSprite() {return m_nFile;}
	__forceinline int GetWidth() {return m_width;}
	__forceinline int GetHeight() {return m_height;}
	bool ExtractImage(unsigned char *outBuf, int recordIdx,unsigned char *palette);

public:
	CCD2CDGFile(unsigned char *content,int contentLen);
	~CCD2CDGFile(void);
};

}