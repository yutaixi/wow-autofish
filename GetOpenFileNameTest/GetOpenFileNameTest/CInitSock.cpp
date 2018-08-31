#include "stdafx.h"
#include "CInitSock.h" 
#include <Ws2tcpip.h>
#include <stdio.h>
#include <string>
#include "SocketException.h"
#include "logger.h"  
#include "CharacterHelper.h"

string  nistIpList[17] = {
	"wwv.nist.gov",	//24.56.178.140
	"time.nist.gov",	//132.163.4.102
	"time-a.nist.gov",	//129.6.15.28
	"time-b.nist.gov",	//129.6.15.29
	"time-c.nist.gov",	//129.6.15.30
	"nist1-macon.macon.ga.us",	//98.175.203.200
	"wolfnisttime.com",	//66.199.22.67
	"nist.netservicesgroup.com",	//64.113.32.5
	"nisttime.carsoncity.k12.mi.us",	//198.111.152.100
	"nist1-lnk.binary.net",	//216.229.0.179 
	"time-a.timefreq.bldrdoc.gov",	//132.163.4.101
	"time-b.timefreq.bldrdoc.gov",	//132.163.4.102
	"time-c.timefreq.bldrdoc.gov",	//132.163.4.103 
	"utcnist.colorado.edu",	//128.138.140.44
	"utcnist2.colorado.edu",	//128.138.141.172
	"time-nw.nist.gov",	//131.107.13.100
	"nist-time - server.eoni.com"	//216.228.192.69
};



CInitSock::CInitSock()
{
	// 初始化WS2_32.dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (::WSAStartup(sockVersion, &wsaData) != 0)
	{
		SocketException skException;
		skException.msg = "socket init failed";
		throw skException;
	}
}
CInitSock::~CInitSock()
{
	::WSACleanup();
}
bool CInitSock::initSock()
{ 
	// 初始化WS2_32.dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (::WSAStartup(sockVersion, &wsaData) != 0)
	{
		return false;
	}
	return true;

}
bool CInitSock::releaseSock()
{ 
	::WSACleanup();
	return true;
}


string CInitSock::getIpFromDN(string domainName,string port)
{  
	
	//-----------------------------------------
	// Declare and initialize variables
	WSADATA wsaData;
	int iResult;
	INT iRetval;
	string ipv4;
	DWORD dwRetval;

	int i = 1;

	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;

	struct sockaddr_in  *sockaddr_ipv4;
	//    struct sockaddr_in6 *sockaddr_ipv6;
	LPSOCKADDR sockaddr_ip;
	WCHAR ipv4stringbuffer[16];
	WCHAR ipv6stringbuffer[46];
	DWORD ipbufferlength = 46;
	ZeroMemory(ipv4stringbuffer,sizeof(ipv4stringbuffer));
	ZeroMemory(ipv6stringbuffer, sizeof(ipv6stringbuffer));


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) { 
		LOG(ERROR) << "WSAStartup failed:" << iResult;
		return "";
	}

	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP; 
	LOG(INFO) << "Calling getaddrinfo with following parameters:tnodename =" << domainName<<",tservname (or port) ="<< port;
	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the result variable will hold a linked list
	// of addrinfo structures containing response
	// information
	dwRetval = getaddrinfo(domainName.c_str(), "37", &hints, &result);
	if (dwRetval != 0) { 
		LOG(ERROR) << "getaddrinfo failed with error:" << (int)dwRetval;
		WSACleanup();
		return "";
	} 
	LOG(INFO) << "getaddrinfo returned success" ;
	// Retrieve each address and print out the hex bytes
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		//printf("getaddrinfo response %d\n", i++);
		//printf("\tFlags: 0x%x\n", ptr->ai_flags);
		//printf("\tFamily: ");
		switch (ptr->ai_family) {
		case AF_UNSPEC:
			//printf("Unspecified\n");
			break;
		case AF_INET:
			//printf("AF_INET (IPv4)\n");
			sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr; 
			InetNtop(AF_INET, &(sockaddr_ipv4->sin_addr), ipv4stringbuffer,sizeof(ipv4stringbuffer));
			ipv4 = CAPECharacterHelper::GetANSIFromUTF16(ipv4stringbuffer);
			//printf("\tIPv4 address %s\n",
			//	ipv4);
			break;
		case AF_INET6:
			//printf("AF_INET6 (IPv6)\n");
			// the InetNtop function is available on Windows Vista and later
			// sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
			// printf("\tIPv6 address %s\n",
			//    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

			// We use WSAAddressToString since it is supported on Windows XP and later
			sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
			// The buffer length is changed by each call to WSAAddresstoString
			// So we need to set it for each iteration through the loop for safety
			ipbufferlength = 46;
			iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptr->ai_addrlen, NULL,
				ipv6stringbuffer, &ipbufferlength);
			//if (iRetval)
			//	printf("WSAAddressToString failed with %u\n", WSAGetLastError());
			//else
			//	printf("\tIPv6 address %s\n", ipv6stringbuffer);
			break;
		case AF_NETBIOS:
			//printf("AF_NETBIOS (NetBIOS)\n");
			break;
		default:
			//printf("Other %ld\n", ptr->ai_family);
			break;
		}

		/*
		printf("\tSocket type: ");
		switch (ptr->ai_socktype) {
		case 0:
			printf("Unspecified\n");
			break;
		case SOCK_STREAM:
			printf("SOCK_STREAM (stream)\n");
			break;
		case SOCK_DGRAM:
			printf("SOCK_DGRAM (datagram) \n");
			break;
		case SOCK_RAW:
			printf("SOCK_RAW (raw) \n");
			break;
		case SOCK_RDM:
			printf("SOCK_RDM (reliable message datagram)\n");
			break;
		case SOCK_SEQPACKET:
			printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
			break;
		default:
			printf("Other %ld\n", ptr->ai_socktype);
			break;
		}
		*/
		/*
		printf("\tProtocol: ");
		switch (ptr->ai_protocol) {
		case 0:
			printf("Unspecified\n");
			break;
		case IPPROTO_TCP:
			printf("IPPROTO_TCP (TCP)\n");
			break;
		case IPPROTO_UDP:
			printf("IPPROTO_UDP (UDP) \n");
			break;
		default:
			printf("Other %ld\n", ptr->ai_protocol);
			break;
		}

		*/
		//printf("\tLength of this sockaddr: %d\n", ptr->ai_addrlen);
		//printf("\tCanonical name: %s\n", ptr->ai_canonname);
	}

	freeaddrinfo(result);
	WSACleanup();
    result = NULL;
    ptr = NULL;


	return ipv4;

}


ULONG CInitSock::getNtpTimeFromNetwork()
{
	WSADATA wsaData;
	int iResult;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) { 
		LOG(ERROR) << "WSAStartup failed:" << iResult;
		return 0;
	}


	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		LOG(ERROR) << "socket  Failed INVALID_SOCKET";
		return 0;
	}

	// 填写远程地址信息，连接到时间服务器
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(37);
	// 这里使用的时间服务器是129.132.2.21，更多地址请参考http://tf.nist.gov/tf-cgi/servers.cgi
	string ip;
	bool connectOK = false;
	for (int index = 0; index < sizeof(nistIpList) / sizeof(nistIpList[0]); index++)
	{
		ip = CInitSock::getIpFromDN(nistIpList[index],"37");
		if (!ip.empty())
		{
			//InetPton(AF_INET, ip.c_str());
			
			inet_pton(AF_INET, ip.c_str(),&(servAddr.sin_addr.S_un.S_addr));
			//servAddr.sin_addr.S_un.S_addr =
		}
		if (::connect(s, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
		{
			LOG(ERROR) << " Failed connect to " << nistIpList[index];

		}
		else
		{
			connectOK = true;
			break;
		}
	}

	if (!connectOK)
	{
		return 0;
	}
	// 等待接收时间协议返回的时间。学习了Winsock I/O模型之后，最好使用异步I/O，以便设置超时
	ULONG ulTime = 0;
	int nRecv = ::recv(s, (char*)&ulTime, sizeof(ulTime), 0);
	if (nRecv > 0)
	{
		ulTime = ntohl(ulTime);
		return ulTime;
	}
	::closesocket(s); 
	WSACleanup();
	CInitSock::releaseSock();
	return 0;
}

