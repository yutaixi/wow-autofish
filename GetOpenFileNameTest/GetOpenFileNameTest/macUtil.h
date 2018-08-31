#ifndef __MAC_UTIL__
#define __MAC_UTIL__
#include "stdafx.h"  
#include <nb30.h>
#pragma comment(lib,"netapi32.lib")
/*
Dev C++ -> include libnetapi32.a
BCC 5.5 or VC++ -> #pragma comment(lib,"netapi32.lib")
*/

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[30];
} ASTAT, *PASTAT;

class MacUtil
{
public:
	MacUtil();
	~MacUtil();
	static string getFirstMac();
	static string getAllMac();
private:
	static string getMac(bool firstOnly);
};




#endif // !__MAC_UTIL__
