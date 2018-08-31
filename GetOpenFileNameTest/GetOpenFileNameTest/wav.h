#ifndef __WAV__
#define __WAV__
#include <string>
#include <Windows.h>
using namespace std;
class WavFile
{
public:
	WavFile();
	~WavFile();

	virtual void Serialize(string fileName);

	int   num;               // 样本数
	LONG *data = NULL;                // 样本数据
	LONG *Ldata = NULL, *Rdata = NULL;     // 双声道数据
	WORD  BytesPerSample, wChannel;     // 一个样本的字节数,  声道数
	DWORD SampleRate;
	DWORD ByteRate;
	WORD BlockAlign;
	WORD wBitsPerSample;   //样本位数 
	DWORD Len;          // 数据块大小,  BYTE 
	WORD extendSize;


private:

};
#endif // !__WAV__

