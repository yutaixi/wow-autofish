#ifndef __SECURITY__
#define __SECURITY__

#include <string>
using namespace std;

class Security
{
public:
	Security();
	~Security();
	static bool isActive(); 
	static bool activeTool(LPTSTR licPath);
	static bool isLicExpired(bool runningLic);
	static bool isMacValid(bool runningLic, bool signvalid);
	static bool isLicExpiredThreadCheck();
private:
	static bool getLicInfo(LPTSTR licFilePath, bool runningLic);
	static bool validateSignedString(LPTSTR licFilePath, bool runningLic);
};

 

#endif // !__SECURITY__

