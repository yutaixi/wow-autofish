#ifndef ___SOCKET_EXCEPTION__
#define ___SOCKET_EXCEPTION__
#include <string>
using namespace std;

class SocketException
{
public:
	SocketException();
	~SocketException();
	int status=0;
	string msg;
private:

};





#endif // !___SOCKET_EXCEPTION__
