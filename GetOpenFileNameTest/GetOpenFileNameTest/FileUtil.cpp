#include "stdafx.h"
#include "fileUtil.h"
#include <iostream>
#include <fstream> 
#include <string> 
#include "CharacterHelper.h"

using namespace std;
FileUtil::FileUtil()
{
}

FileUtil::~FileUtil()
{
}
bool FileUtil::appToFile(char* fileName, WCHAR* cont)
{
	ofstream out(fileName, ios::out | ios::app); 
	if (out.is_open())
	{ 
		out << *cont;    //message是程序中处理的数据
		out.close();
		return true;
	}
	else
	{ 
		//LOG(ERROR) << "不能打开文件!" << endl;
	}
	return false;
}

 
bool FileUtil::writeIniFile(string appName,string keyName,string keyValue,string filePath)
{
	//LPCWSTR wFilePath = CAPECharacterHelper::GetUTF16FromANSI(filePath.c_str());
	//LPCWSTR wAppName = CAPECharacterHelper::GetUTF16FromANSI(appName.c_str());
	//LPCWSTR wKeyName = CAPECharacterHelper::GetUTF16FromANSI(keyName.c_str());
	//LPCWSTR wKeyValue = CAPECharacterHelper::GetUTF16FromANSI(keyValue.c_str());

	WritePrivateProfileStringA(appName.c_str(), keyName.c_str(), keyValue.c_str(), filePath.c_str());

	//delete[] wFilePath;
	//delete[] wAppName;
	//delete[] wKeyName;
	//delete[] wKeyValue;

	return true; 
}


string FileUtil::readIniFileString(string appName, string keyName,string defaultValue, string filePath)
{
	char temp[100]; 
	GetPrivateProfileStringA(appName.c_str(),keyName.c_str(), defaultValue.c_str(), temp,sizeof(temp),filePath.c_str());
	 
	return string(temp);
}
bool  FileUtil::readIniFileBool(string appName, string keyName, int defaultValue, string filePath)
{
	int value=GetPrivateProfileIntA(appName.c_str(), keyName.c_str(), defaultValue, filePath.c_str());
	return value;
}

 double  FileUtil::readIniFileDouble(string appName, string keyName, string defaultValue, string filePath)
{
	 char temp[100];
	 GetPrivateProfileStringA(appName.c_str(), keyName.c_str(), defaultValue.c_str(), temp, sizeof(temp), filePath.c_str());
	 double value = atof(temp);
	 return value;

}

 int FileUtil::readIniFileInt(string appName, string keyName, string defaultValue, string filePath)
 { 
	 char temp[100];
	 GetPrivateProfileStringA(appName.c_str(), keyName.c_str(), defaultValue.c_str(), temp, sizeof(temp), filePath.c_str());
	 int value = atoi(temp);
	 return value;
 }