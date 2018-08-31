#pragma once
#include <Windows.h> 
#include <atlstr.h>
#include "tlhelp32.h"  
 
typedef struct tagWNDINFO
{
	DWORD dwProcessId;
	HWND hWnd;
} WNDINFO, *LPWNDINFO;

// 保存数据的结构体
typedef struct tagRpcData
{
	char	m_szData[MAX_PATH];
	void*	m_pfunCall = NULL;
}stRpcData, *LPRPCDATA;

class SysUtil
{
public:
	SysUtil();
	~SysUtil();
	static bool GetProcessHwnd(DWORD proccessId, WNDINFO *wi);
	static bool GetProcessInfo(CString processName, WNDINFO *wi);
private:

};

