#pragma once

namespace th5w{

class CBBFile
{
protected:
	unsigned char *m_content;
	int m_contentLen;
public:
	unsigned char* GetMaskPtr(int bbFrame);
public:
	CBBFile(unsigned char *content,int contentLen);
	~CBBFile(void);
};

}