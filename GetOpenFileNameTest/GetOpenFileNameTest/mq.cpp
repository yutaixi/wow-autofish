#include "stdafx.h"
#include "MQ.h"
#include "logger.h"

MQ::MQ()
{ 
	::InitializeCriticalSection(&mqCs);
}

MQ::~MQ()
{
	::DeleteCriticalSection(&mqCs);
}

bool MQ::isEmpty()
{
	return query.size() <= 0;
}

bool MQ::push(MqMsg msg)
{
	try
	{
		//判断长度
		if (query.size() >= mqMaxSize)
		{
			return false;
		}
		//加锁
		lock();
		query.push_back(msg);
		//解锁
		unLock();
	}
	catch (const std::exception&)
	{
		LOG(ERROR) << "PUSH ERROR";
	}
	 
	return true;
}
MqMsg MQ::pop()
{
	MqMsg msg ;
	//判断长度，是否为空
	if (query.size() <= 0)
	{
		return msg;
	}
	//加锁
	lock();
	msg = query.front();
	query.erase(query.begin());
	//解锁
	unLock();
	return msg;
}
bool MQ::clear()
{
	//判断长度，是否为空
	if (query.size() == 0)
	{
		return true;
	}
	//加锁
	lock();
	query.clear();
	//解锁
	unLock();
	return true;
}

bool MQ::lock()
{
	::EnterCriticalSection(&mqCs);
	return true; 
}
bool MQ::unLock()
{
	::LeaveCriticalSection(&mqCs);
	return true;
}

 

 