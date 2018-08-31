#ifndef __FILE_UTIL__
#define __FILE_UTIL__
#include <Windows.h>

class FileUtil
{
public:
	FileUtil();
	~FileUtil();
	static bool appToFile(char* fileName, WCHAR* cont);
	static bool writeIniFile(string appName, string keyName, string keyValue, string filePath);
	static string readIniFileString(string appName, string keyName, string defaultValue,string filePath);
	static int readIniFileInt(string appName, string keyName, string defaultValue, string filePath);
	static double readIniFileDouble(string appName, string keyName, string defaultValue, string filePath);
	static bool readIniFileBool(string appName, string keyName, int defaultValue, string filePath);
private:

};








#endif // !__FILE_UTIL__
