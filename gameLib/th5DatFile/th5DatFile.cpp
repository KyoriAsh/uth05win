#include ".\th5datfile.h"

#define COMPRESS_NONE 0xF388
#define COMPRESS_RLE  0x9595
#define EXTRACTED_FILE_SIZE_LIMIT 2000000

namespace th5w{

Cth5DatFile::Cth5DatFile(void)
{
	Reset();
}

Cth5DatFile::~Cth5DatFile(void)
{
	Reset();
}

void Cth5DatFile::Reset()
{
	for (int i=0;i<(int)m_childFileContent.size();i++)
		if (m_childFileContent[i]!=NULL)
			delete m_childFileContent[i];
	m_childFileContent.clear();
	m_childFileName.clear();
	m_childFilePointer.clear();
	m_childFileLength.clear();
}

void Cth5DatFile::DecodeFileTable(unsigned char *pFileTable,unsigned short fileTableLen,unsigned char fileTableKey)
{
	for (int i=0;i<(int)fileTableLen;i++)
	{
		pFileTable[i]^=fileTableKey;
		fileTableKey-=pFileTable[i];
	}
}

bool Cth5DatFile::DecodeSingleFile(unsigned char *controlBlock,int curFileIdx,FILE *fpParent)
{
	unsigned short compressMode=*(unsigned short*)controlBlock;
	unsigned char xorByte=controlBlock[2];
	char *fileName=(char *)(controlBlock+3);
	unsigned short compressedFileSize=*(unsigned short *)(controlBlock+16);
	unsigned short unknownField12h=*(unsigned short *)(controlBlock+18);
	unsigned int origFileOffset=*(unsigned int *)(controlBlock+20);
	unsigned int extractedFileSize=0;

	if (compressMode!=COMPRESS_NONE&&compressMode!=COMPRESS_RLE)
		return false;
	if (compressMode==COMPRESS_RLE)
	{
		unsigned char *outBuffer=new unsigned char[EXTRACTED_FILE_SIZE_LIMIT];
		if (outBuffer==NULL)
			return false;
		unsigned char *inBuffer=new unsigned char[compressedFileSize];
		if (inBuffer==NULL)
		{
			delete outBuffer;
			return false;
		}
		fseek(fpParent,origFileOffset,SEEK_SET);
		if (fread(inBuffer,1,compressedFileSize,fpParent)!=compressedFileSize)
		{
			delete outBuffer;
			delete inBuffer;
			return false;
		}

		int inCur=0,outCur=0;
		unsigned char lastchar=255;
		bool firstChar=true;
		while(inCur!=compressedFileSize)
		{
			unsigned char nextchar=inBuffer[inCur]^xorByte;
			inCur++;
			if (nextchar!=lastchar||inCur==compressedFileSize||firstChar)
			{
				outBuffer[outCur++]=nextchar;
				lastchar=nextchar;
				firstChar=false;
			}
			else
			{
				for (int i=0;i<(int)((inBuffer[inCur]^xorByte)+1);i++)
					outBuffer[outCur++]=nextchar;
				inCur++;
			}
		}
		extractedFileSize=outCur;

		unsigned char *outBufferFinal=new unsigned char[extractedFileSize];
		if (outBufferFinal==NULL)
		{
			delete inBuffer;
			delete outBuffer;
			return false;
		}

		m_childFileLength.push_back(extractedFileSize);
		m_childFileName.push_back(fileName);
		m_childFilePointer.push_back(0);
		memcpy(outBufferFinal, outBuffer,extractedFileSize);
		m_childFileContent.push_back(outBufferFinal);

		delete inBuffer;
		delete outBuffer;
		return true;
	}
	if (compressMode==COMPRESS_NONE)
	{
		unsigned char *outBuffer=new unsigned char[compressedFileSize];
		if (outBuffer==NULL)
			return false;
		unsigned char *inBuffer=new unsigned char[compressedFileSize];
		if (inBuffer==NULL)
		{
			delete outBuffer;
			return false;
		}
		fseek(fpParent,origFileOffset,SEEK_SET);
		if (fread(inBuffer,1,compressedFileSize,fpParent)!=compressedFileSize)
		{
			delete outBuffer;
			delete inBuffer;
			return false;
		}

		for (int i=0;i<compressedFileSize;i++)
			outBuffer[i]=inBuffer[i]^xorByte;
		extractedFileSize=compressedFileSize;

		m_childFileLength.push_back(extractedFileSize);
		m_childFileName.push_back(fileName);
		m_childFilePointer.push_back(0);
		m_childFileContent.push_back(outBuffer);

		return true;
	}

	return false;
}

bool Cth5DatFile::LoadFile(const char *fileName)
{
	Reset();

	FILE *fpi=fopen(fileName,"rb");
	if (fpi==NULL)
		return false;

#define CLOSE_RETFALSE(x) {fclose(x);return false;}

	unsigned char header[16];
	if (fread(header,1,16,fpi)!=16)
		CLOSE_RETFALSE(fpi);

	unsigned short fileTableSize=*(unsigned short *)header;
	unsigned char fileTableKey=header[6];
	unsigned char *fileTable=new unsigned char[fileTableSize];

#define CLOSE_DELETE_RETFALSE(x,y) {fclose(x);delete y;return false;}

	if (fread(fileTable,1,fileTableSize,fpi)!=fileTableSize)
		CLOSE_DELETE_RETFALSE(fpi,fileTable);

	DecodeFileTable(fileTable,fileTableSize,fileTableKey);

	int fileTablePtr=0;

	while(fileTable[fileTablePtr]!=0)
	{
		if (DecodeSingleFile(fileTable+fileTablePtr,fileTablePtr/32,fpi)==false)
		{
			Reset();
			CLOSE_DELETE_RETFALSE(fpi,fileTable);
		}
		fileTablePtr+=32;
	}

#undef CLOSE_DELETE_RETFALSE
#undef CLOSE_RETFALSE

	delete fileTable;
	fclose(fpi);

	return true;
}

int Cth5DatFile::NFile()
{
	return (int)m_childFileContent.size();
}

string Cth5DatFile::GetChildFileName(int childFileIdx)
{
	if (childFileIdx>=NFile()||childFileIdx<0)
		return "";
	return m_childFileName[childFileIdx];
}

int Cth5DatFile::GetChildFileLength(int childFileIdx)
{
	if (childFileIdx>=NFile()||childFileIdx<0)
		return -1;
	return m_childFileLength[childFileIdx];
}

int Cth5DatFile::Childftell(int childFileIdx)
{
	if (childFileIdx>=NFile()||childFileIdx<0)
		return -1;
	return m_childFilePointer[childFileIdx];
}

int Cth5DatFile::Childfseek(int childFileIdx,int offset, int origin)
{
	if (childFileIdx>=NFile()||childFileIdx<0)
		return 1;
	if (origin==SEEK_SET)
		m_childFilePointer[childFileIdx]=offset;
	if (origin==SEEK_CUR)
		m_childFilePointer[childFileIdx]+=offset;
	if (origin==SEEK_END)
		m_childFilePointer[childFileIdx]=GetChildFileLength(childFileIdx)-offset;
	if (m_childFilePointer[childFileIdx]<0)
		m_childFilePointer[childFileIdx]=0;
	if (m_childFilePointer[childFileIdx]>GetChildFileLength(childFileIdx))
		m_childFilePointer[childFileIdx]=GetChildFileLength(childFileIdx);

	return 0;
}

char* Cth5DatFile::Childfgets(char *str,int count,int childFileIdx)
{
	if (childFileIdx>=NFile()||childFileIdx<0)
		return NULL;
	if (m_childFilePointer[childFileIdx]>=GetChildFileLength(childFileIdx))
		return NULL;
	int &offset=m_childFilePointer[childFileIdx];
	int nRet=0;
	while(offset<GetChildFileLength(childFileIdx)&&nRet<count)
	{
		str[nRet++]=m_childFileContent[childFileIdx][offset++];
		if (str[nRet-1]=='\n')
		{
			nRet--;
			str[nRet]=0;
			if (nRet>=1&&str[nRet-1]==0x0d)
			{
				nRet--;
				str[nRet]=0;
			}
			break;
		}
	}

	return str;
}

int Cth5DatFile::Childfread(void *buffer,int size,int count,int childFileIdx)
{
	if (childFileIdx>=NFile()||childFileIdx<0||size==0||count==0||buffer==NULL)
		return 0;
	int nRead=(GetChildFileLength(childFileIdx)-Childftell(childFileIdx))/size;
	if (nRead>count)
		nRead=count;
	if (nRead==0)
		return 0;
	memcpy(buffer,m_childFileContent[childFileIdx]+Childftell(childFileIdx),nRead*size);
	return nRead;
}

int Cth5DatFile::GetChildFileIndex(const char *childFileName)
{
	char ucFileName[1000];
	strcpy(ucFileName,childFileName);
	strupr(ucFileName);
	for (int i=0;i<NFile();i++)
		if (GetChildFileName(i)==ucFileName)
			return i;
	return -1;
}

unsigned char* Cth5DatFile::GetChildFileContentPointer(int childFileIdx)
{
	if (childFileIdx>=NFile()||childFileIdx<0)
		return NULL;
	return m_childFileContent[childFileIdx];
}
}


















