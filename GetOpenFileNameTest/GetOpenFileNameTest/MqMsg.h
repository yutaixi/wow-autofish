#ifndef __TIMER_MSG__
#define __TIMER_MSG__
enum  operationEnum { PAUSE_AND_PRESS_KEY };

class MqMsg
{
public:
	MqMsg();
	MqMsg(operationEnum operation,string msg, int data);
	~MqMsg(); 
	operationEnum operation;
	string msg;
	int data;
private:

};



#endif // !__TIMER_MSG__
