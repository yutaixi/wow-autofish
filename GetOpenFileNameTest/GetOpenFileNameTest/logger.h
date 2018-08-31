#ifndef __LOGGER__
#define __LOGGER__

#define GLOG_NO_ABBREVIATED_SEVERITIES 

#include "glog\logging.h"

class Logger
{
public:
	Logger();
	~Logger();
	static int displayLog(char* format, ...);
	static void initGlog();
	static bool setGlogLevel(int level);
private:
	static string getFormatedDate();
};




#endif // !__LOGGER__

