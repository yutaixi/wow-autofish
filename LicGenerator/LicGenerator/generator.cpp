#include "stdafx.h"
#include "generator.h"
#include <string>
#include <iostream>
#include <sstream>
#include "MyRSA.h"
#include "cUtil.h"
#include "global.h"

using namespace std;
string*  byteToHexStr(BYTE byte_arr[], int arr_len)
{
	string* hexstr = new string;
	for (int i = 0; i<arr_len; i++)
	{
		char hex1;
		char hex2;
		int value = byte_arr[i];
		int v1 = value / 16;
		int v2 = value % 16;
		//将商转换为字母   
		if (v1 >= 0 && v1 <= 9)
		{
			hex1 = (char)(48 + v1);
		}
		else
		{
			hex1 = (char)(55 + v1);
		}
		//将余数转成字母   
		if (v2 >= 0 && v2 <= 9)
		{
			hex2 = (char)(48 + v2);
		}
		else
		{
			hex2 = (char)(55 + v2);
		}
		//将字母连成一串   
		*hexstr = *hexstr + hex1 + hex2;
	}
	int t = (*hexstr).size();
	return hexstr;
}
Generator::Generator()
{
}

Generator::~Generator()
{
}

bool Generator::generateLic(string esn,string date,bool debugMode,string path)
{
	if (path.empty())
	{ 
		path.append("./lic\\key_").append(esn).append(".lic");
	}
	std::stringstream strStream;  
	time_t tt = time(NULL);
	MyRSA rsa; 
	strStream << esn << date;
	if (debugMode)
	{
		strStream << to_string(tt);
	} 
	string plainText = strStream.str();
	SecByteBlock signature = rsa.SignString(privFilename, plainText.c_str());

	string *sign =  byteToHexStr(signature, signature.size());

	LPTSTR lpPath = CUtil::GetUTF16FromANSI(path.c_str());
	WCHAR *esnWchar = CUtil::GetUTF16FromANSI(esn.c_str());
	WCHAR *dateWchar = CUtil::GetUTF16FromANSI(date.c_str());
	WCHAR *signWchar = CUtil::GetUTF16FromANSI((*sign).c_str());
	WritePrivateProfileString(L"DATA", L"esn", esnWchar, lpPath);
	WritePrivateProfileString(L"DATA", L"date", dateWchar, lpPath);
	WritePrivateProfileString(L"SIGN", L"sign", signWchar, lpPath); 
	if (debugMode)
	{ 
		WritePrivateProfileStringA("DATA", "debug", to_string(tt).c_str(), path.c_str());
	}
	delete esnWchar;
	delete dateWchar;
	delete signWchar;
	delete sign;
	return true;
}