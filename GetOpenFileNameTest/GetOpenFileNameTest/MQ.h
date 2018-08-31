#ifndef __MQ__
#define __MQ__
#include<vector>
#include "MqMsg.h"

using namespace std;
#define mqMaxSize 100
class MQ
{
public:
	MQ();
	~MQ();
	bool clear();
	bool push(MqMsg msg);
	MqMsg pop();
	bool  isEmpty();
private: 
	vector<MqMsg> query;
	bool lock();
	bool unLock();
	CRITICAL_SECTION mqCs;
};


#endif // !__MQ__


