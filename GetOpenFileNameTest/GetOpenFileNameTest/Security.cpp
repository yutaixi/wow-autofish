#include "stdafx.h"
#include "Security.h"
#include "MyRSA.h"
#include <sstream>
#include "CharacterHelper.h"
#include "cryptopp\secblock.h"
#include "timeUtil.h"
#include "macUtil.h"
#include "logger.h"


extern SysConfig sys;

Security::Security()
{
}

Security::~Security()
{
}

void afterActiveSuccess(bool runningLic)
{
	sys.isActive = true;
	if (!runningLic)
	{
		sys.esn = sys.esnTemp;
		sys.date = sys.dateTemp;
		sys.sign = sys.signTemp;
		sys.debug = sys.debugTemp;
		sys.signedString = sys.signedStringTemp;
	}
	if (!sys.debug.empty())
	{
		sys.debugMode = true;
	}
	
}

bool Security::isActive()
{
	bool signValid = validateSignedString(NULL, true);
	if (!signValid)
	{
		return false;
	}
	if (!isMacValid(true, signValid))
	{
		return false;
	}
	if (isLicExpired(true))
	{
		return false;
	} 
	afterActiveSuccess(true);
	return true;
}


bool Security::isLicExpired(bool runningLic)
{ 
	time_t licTime = timeUtil::stringToTime_t(runningLic?sys.date:sys.dateTemp);
	time_t now = time(NULL);//这句返回的只是一个时间cuo
	return licTime < now;
}

bool Security::isLicExpiredThreadCheck()
{
	time_t licTime = timeUtil::stringToTime_t(sys.date); 
	time_t now = timeUtil::getCurrentTimeFromIP();
	if (sys.isDisplayWebTime)
	{
		Logger::displayLog("网络时间：%I64d\r\n", now);
	} 
	if (now < 0)
	{
		now= time(NULL);
	}
	return licTime < now;
}



bool Security::getLicInfo(LPTSTR licFilePath, bool runningLic)
{
	if (licFilePath == NULL)
	{
		licFilePath = sys.licFilePath;
	} 
	WCHAR esn[1024];
	WCHAR date[50];
	WCHAR sign[1024];
	WCHAR debug[20];
	GetPrivateProfileString(L"DATA", L"esn", NULL, esn, 1024, licFilePath);
	GetPrivateProfileString(L"DATA", L"date", NULL, date, 50, licFilePath);
	GetPrivateProfileString(L"SIGN", L"sign", NULL, sign, 2048, licFilePath);
	GetPrivateProfileString(L"DATA", L"debug",NULL, debug,sizeof(debug), licFilePath);
	string signedStr;
	if (runningLic)
	{
		sys.esn = CAPECharacterHelper::GetANSIFromUTF16(esn);
		sys.date = CAPECharacterHelper::GetANSIFromUTF16(date);
		sys.sign = CAPECharacterHelper::GetANSIFromUTF16(sign);
		sys.debug = CAPECharacterHelper::GetANSIFromUTF16(debug);
		signedStr.append(sys.esn).append(sys.date).append(sys.debug);
		sys.signedString = signedStr;
	}
	else
	{
		sys.esnTemp = CAPECharacterHelper::GetANSIFromUTF16(esn);
		sys.dateTemp = CAPECharacterHelper::GetANSIFromUTF16(date);
		sys.signTemp = CAPECharacterHelper::GetANSIFromUTF16(sign);
		sys.debugTemp = CAPECharacterHelper::GetANSIFromUTF16(debug);
		signedStr.append(sys.esnTemp).append(sys.dateTemp).append(sys.debugTemp);
		sys.signedStringTemp = signedStr;
	}
	 
	return true;
}
SecByteBlock HexDecodeString(const char *hex) {
	StringSource ss(hex, true, new HexDecoder);
	SecByteBlock result((size_t)ss.MaxRetrievable());
	ss.Get(result, result.size());
	return result;
}
bool Security::validateSignedString(LPTSTR licFilePath, bool runningLic)
{ 
	getLicInfo(licFilePath, runningLic);

	if (runningLic)
	{
		if (sys.sign.empty() || sys.signedString.empty())
		{
			return false;
		}
	}
	else
	{
		if (sys.signTemp.empty() || sys.signedStringTemp.empty())
		{
			return false;
		}
	}
	
	MyRSA rsa;  
	SecByteBlock signature = HexDecodeString(runningLic?sys.sign.c_str():sys.signTemp.c_str());
	return rsa.VerifyString(sys.pubKeyPath, runningLic?sys.signedString.c_str():sys.signedStringTemp.c_str(), signature);
}

bool Security::activeTool(LPTSTR licPath)
{
	if (!licPath)
	{
		return false;
	} 
	bool signValid = validateSignedString(licPath, false);
	if (!signValid)
	{
		return false;
	}
	if (isLicExpired(false))
	{
		return false;
	}

	if (!isMacValid(false, signValid))
	{
		return false;
	}
	sys.isActive = true;
	afterActiveSuccess(false);
	CopyFile(licPath, sys.licFilePath, FALSE);//false代表覆盖，true不覆盖
	return true;
	
}

bool Security::isMacValid(bool runningLic,bool signvalid)
{
	if (!signvalid)
	{
		return false;
	}
	if (strstr(runningLic ? sys.esn.c_str() : sys.esnTemp.c_str(),"runonany") > 0 || strstr(signvalid ? sys.esn.c_str() : sys.esnTemp.c_str(), "RUNONANY") > 0)
	{
		return true;
	}

	string mac = MacUtil::getFirstMac();
	if (strstr(runningLic?sys.esn.c_str():sys.esnTemp.c_str(), mac.c_str())>0)
	{
		return true;
	}
	return false;

}