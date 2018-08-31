#ifndef __TIME_UTIL__
#define __TIME_UTIL__

class timeUtil
{
public:
	timeUtil();
	~timeUtil();
	static long getCurrentTimeMilliseconds();
	static char * getCurrentTimeStr();
	static time_t stringToTime_t(const std::string & time_string);
	static bool compareTime(time_t t1, time_t t2);

	static string formatedLocalTimeToString();
	static string formatedGmtTimeToString();
	static string localTimeToString(char* format);
	static string gmtTimeToString(char* format);
	static time_t SystemTimeToTimet(SYSTEMTIME st);

	static time_t timeUtil::getCurrentTimeFromIP();
private:
	static SYSTEMTIME getTimeFromULONG(ULONG ulTime); 
	

};





#endif // !__TIME_UTIL__


