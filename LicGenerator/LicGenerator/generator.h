#ifndef __GENERATOR__
#define __GENERATOR__
#include <string>
using namespace std;
class Generator
{
public:
	Generator();
	~Generator();
	static bool generateLic(string esn, string date,bool debugMode,string path);
private:

};



#endif // !__GENERATOR__
