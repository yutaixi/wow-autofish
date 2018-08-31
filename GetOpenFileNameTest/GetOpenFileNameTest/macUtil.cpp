#include "stdafx.h"
#include "macUtil.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <nb30.h>
#pragma comment(lib,"netapi32.lib")

MacUtil::MacUtil()
{

}

MacUtil::~MacUtil()
{
}

string MacUtil::getFirstMac()
{
	return getMac(true);

} 
string MacUtil::getAllMac()
{
	return getMac(false);

}


string MacUtil::getMac(bool firstOnly)
{
	char temp[32];
	ASTAT Adapter;
	NCB Ncb;
	UCHAR uRetCode;
	LANA_ENUM lenum;
	int i = 0;
	string mac;
	bool addComma=false;
	std::stringstream strStream;


	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);

	uRetCode = Netbios(&Ncb);
	//printf("The NCBENUM return adapter number is: %d \n ", lenum.length);
	for (i = 0; i < lenum.length; i++)
	{
		ZeroMemory(temp, sizeof(temp));
		memset(&Ncb, 0, sizeof(Ncb));
		Ncb.ncb_command = NCBRESET;
		Ncb.ncb_lana_num = lenum.lana[i];
		uRetCode = Netbios(&Ncb);

		memset(&Ncb, 0, sizeof(Ncb));
		Ncb.ncb_command = NCBASTAT;
		Ncb.ncb_lana_num = lenum.lana[i];
		strcpy_s((char *)Ncb.ncb_callname, NCBNAMSZ, "* ");
		Ncb.ncb_buffer = (unsigned char *)&Adapter;
		Ncb.ncb_length = sizeof(Adapter);
		uRetCode = Netbios(&Ncb);

		if (uRetCode == 0 )
		{ 
			//sprintf(mac, "%02x-%02x-%02x-%02x-%02x-%02x ",
			sprintf_s(temp, "%02X%02X%02X%02X%02X%02X",
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]
			); 
			if (addComma)
			{
				strStream << ";" << temp;
			}
			else
			{
				strStream << temp;
				addComma = true;
			}
			if (firstOnly)
			{
				break;
			}
			
			//printf( "The Ethernet Number on LANA %d is: %s\n ", lenum.lana[i], mac);
		}
	}
	mac = strStream.str();
	strStream.str("");
	strStream.clear();
	return mac;
}
