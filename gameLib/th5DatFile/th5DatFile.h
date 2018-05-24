/*
	This class is NOT thread-safe because I don't see any need to make it thread-safe.
	a) load must always come first.
	b) no write support hence read/write lock is not necessary.
	c) yes, simultaneous reads from multiple threads may lead to chaos. But even if we set critical section
	   here, these reads still lead to chaos.
	d) this class is designed to copy the functions of FILE*, not a structure to hold the contents.
	   Imagine what happens if multiple threads share the same FILE* and use it to access the file: chaos, right?
*/
#pragma once
#include <vector>
#include <string>
using namespace std;

namespace th5w{

class Cth5DatFile
{
protected:
	vector<unsigned char *> m_childFileContent;
	vector<int> m_childFileLength;
	vector<string> m_childFileName;
	vector<int> m_childFilePointer;

protected:
	static void DecodeFileTable(unsigned char *pFileTable,unsigned short fileTableLen,unsigned char fileTableKey);
	bool DecodeSingleFile(unsigned char *controlBlock,int curFileIdx,FILE *fpParent);
public:
	bool LoadFile(const char *fileName);
	void Reset();

	int NFile();
	string GetChildFileName(int childFileIdx);					//return "" on fail
	int GetChildFileLength(int childFileIdx);					//return -1 on fail
	int Childftell(int childFileIdx);							//return -1 on fail
	int Childfseek(int childFileIdx,int offset, int origin);	//when seeked outside boundary, clamped
	int Childfread(void *buffer,int size,int count,int childFileIdx);
	char* Childfgets(char *str,int count,int childFileIdx);		//read a line. unlike fgets in standard c library,
																//'\n' is REMOVED FROM the end of returned string here.
	int GetChildFileIndex(const char *childFileName);			//return -1 if not found

	unsigned char* GetChildFileContentPointer(int childFileIdx);	//this function is not recommended, use it only
																	//when you know what you are doing as it may
																	//cause chaos if there're multiple threads
																	//operating on the same child file

public:
	Cth5DatFile(void);
	~Cth5DatFile(void);
};

}