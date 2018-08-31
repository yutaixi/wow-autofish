#ifndef __INIT_SOCK__
#define __INIT_SOCK__
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

class CInitSock
{
public:
	CInitSock(); 
	~CInitSock();
	static string getIpFromDN(string domainName, string port);
	static bool initSock();
	static bool releaseSock();
	static ULONG getNtpTimeFromNetwork();
private:
};




#endif // !__INIT_SOCK__

