#include ".\bbfile.h"
#include <stdio.h>

namespace th5w{

CBBFile::CBBFile(unsigned char *content,int contentLen)
{
	m_content=content;
	m_contentLen=contentLen;
}

CBBFile::~CBBFile(void)
{
}

unsigned char* CBBFile::GetMaskPtr(int bbFrame)
{
	if ((bbFrame+1)*128>m_contentLen)
		return NULL;
	return m_content+128*bbFrame;
}

}