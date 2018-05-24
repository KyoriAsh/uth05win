#pragma once
#include "Scheme.h"

namespace th5w{

class CSchemeSwitcher
{
public:
	static bool SwitchScheme(CScheme** ppOutNextScheme,int *pOutNextSchemeID, int curSchemeID, int curSchemeExitValue);
	static bool GetFirstScheme(CScheme** ppOutNextScheme,int *pOutNextSchemeID);
public:
	CSchemeSwitcher(void);
	~CSchemeSwitcher(void);
};

}