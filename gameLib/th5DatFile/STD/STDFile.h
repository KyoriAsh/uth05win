#pragma once
#include <vector>

namespace th5w{

class CSTDFile
{
public:
	unsigned char *m_content;
	int m_contentLen;

	char *m_stageName;
	char *m_stageBGMName;
	char *m_stageBossBGMName;

	int m_nBGBlockIdx;
	unsigned char *m_pBGBlockIdxArray;
	int m_nBGScroolSpeed;
	unsigned char *m_pBGScrollSpeedArray;

	int m_nEnemyType;
	std::vector<int> m_enemyScriptLen;
	std::vector<unsigned char *> m_enemyScript;
	unsigned char *m_pEnemyTable;

protected:
	void Parse();
public:
	CSTDFile(unsigned char *content,int len);
	~CSTDFile(void);
};

}