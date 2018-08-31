#ifndef __TIME_COST_DEBUG_H__
#define __TIME_COST_DEBUG_H__

#include <string>
#include <windows.h>
class CTimeCostWatch 
{
public:
	CTimeCostWatch();
	~CTimeCostWatch();

	BOOL Start();
	BOOL Stop();

	double GetInterval() const;

protected:
	unsigned __int64 m_nStartTimer;
	unsigned __int64 m_nStopTimer;

	static unsigned __int64 s_nFrequency;
};

class CTimeCostDebug 
{
public:
	CTimeCostDebug(LPCTSTR lpszInfo);
	~CTimeCostDebug();

protected:
	std::wstring m_strHead;
	CTimeCostWatch m_watch;
	BOOL m_bByForce;
};


#endif //__TIME_COST_DEBUG_H__