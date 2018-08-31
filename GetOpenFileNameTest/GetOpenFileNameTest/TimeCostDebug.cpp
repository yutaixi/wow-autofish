#include "stdafx.h"
#include "TimeCostDebug.h"

unsigned __int64 CTimeCostWatch::s_nFrequency = 0;

CTimeCostWatch::CTimeCostWatch()
:m_nStartTimer(0), m_nStopTimer(0)
{
	if(s_nFrequency == 0)
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&s_nFrequency);
	}
}

CTimeCostWatch::~CTimeCostWatch()
{

}

BOOL CTimeCostWatch::Start()
{
	BOOL bRet = QueryPerformanceCounter((LARGE_INTEGER*)&m_nStartTimer);
	return bRet;
}

BOOL CTimeCostWatch::Stop()
{
	BOOL bRet = QueryPerformanceCounter((LARGE_INTEGER*)&m_nStopTimer);
	return bRet;
}

double CTimeCostWatch::GetInterval() const
{
	if(s_nFrequency != 0)
	{
		return 1000 * ((double)(long)(m_nStopTimer - m_nStartTimer)) / (double)(long)s_nFrequency;
	}

	return 0;
}
/////////////////   CTimeCostWatch   //////////////////////


/////////////////   CTimeCostDebug   //////////////////////

CTimeCostDebug::CTimeCostDebug(LPCTSTR lpszHead)
{

	if(lpszHead != NULL)
	{
		m_strHead = lpszHead;
	}

	m_watch.Start();
}

CTimeCostDebug::~CTimeCostDebug()
{
	m_watch.Stop();

	TCHAR szBuffer[256] = {0};
	_sntprintf_s(szBuffer, 256, _T("%s cost time: %f ms \r\n"), m_strHead.c_str(), m_watch.GetInterval());
	OutputDebugString(szBuffer);
}
