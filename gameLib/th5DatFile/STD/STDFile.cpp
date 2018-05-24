#include ".\stdfile.h"
#include <stdio.h>
#include <string.h>

namespace th5w{

CSTDFile::CSTDFile(unsigned char *content,int len)
{
	m_content=content;
	m_contentLen=len;
	Parse();
}

CSTDFile::~CSTDFile(void)
{
}

void CSTDFile::Parse()
{
	unsigned char *ptr=m_content+2;
	m_stageName=(char *)ptr;
	ptr+=strlen(m_stageName)+1;
	m_stageBGMName=(char *)ptr;
	ptr+=strlen(m_stageBGMName)+1;
	m_stageBossBGMName=(char *)ptr;
	ptr+=strlen(m_stageBossBGMName)+1;

	m_nBGBlockIdx=*ptr;
	m_pBGBlockIdxArray=ptr+1;
	ptr+=1+m_nBGBlockIdx;
	m_nBGScroolSpeed=*ptr;
	m_pBGScrollSpeedArray=ptr+1;
	ptr+=1+m_nBGScroolSpeed;

	m_nEnemyType=*ptr;
	ptr++;
	for (int i=0;i<m_nEnemyType;i++)
	{
		m_enemyScriptLen.push_back(*ptr);
		m_enemyScript.push_back(ptr+1);
		ptr+=(*ptr)+1;
	}
	ptr++;
	m_pEnemyTable=ptr;
}

}
