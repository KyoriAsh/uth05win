#pragma once

namespace th5w{
	
class CCommonFunctionSystem
{
public:
	static void CreateConsoleWindow();
	static bool GetFileMD5(unsigned char *retMD5,const char *fileName);
public:
	CCommonFunctionSystem(void);
	~CCommonFunctionSystem(void);
};

}
