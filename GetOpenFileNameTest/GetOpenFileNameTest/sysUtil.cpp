#include "stdafx.h" 
#include "sysUtil.h"    

SysUtil::SysUtil()
{
}

SysUtil::~SysUtil()
{
}
BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam)//枚举所有进程
{
	DWORD dwProcId;
	GetWindowThreadProcessId(hWnd, &dwProcId);
	LPWNDINFO pInfo = (LPWNDINFO)lParam;
	if (dwProcId == pInfo->dwProcessId)
	{
		// 在此添加更多限制条件
		CString strTmp;

		GetClassName(hWnd, strTmp.GetBuffer(200), 200);
		strTmp.ReleaseBuffer();

		if (!strTmp.Compare(L"LButton"))	// 按钮, 编辑框TEdit等, 具体可使用Spy++查看
		{
			pInfo->hWnd = hWnd;
			return FALSE;
		}

		if (IsWindowVisible(hWnd)) // 当前窗口是否可见
		{
			pInfo->hWnd = hWnd;		// 获取到第一个窗口句柄

			return FALSE;
		}
	}
	return TRUE;
}

bool SysUtil::GetProcessHwnd(DWORD proccessId, WNDINFO *wi)
{

	EnumWindows(MyEnumProc, (LPARAM)wi);
	// EnumChildWindows(hWnPar, MyEnumProc, (LPARAM)&wi); // 枚举窗口的子窗口句柄, MFC中的控作等
	return true;
}

bool SysUtil::GetProcessInfo(CString processName, WNDINFO *wi)
{
	wi->dwProcessId = 0;
	wi->hWnd = NULL;
	//创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//PROCESSENTRY32进程快照的结构体       
	PROCESSENTRY32 pe;
	//实例化后使用Process32First获取第一个快照的进程前必做的初始化操作
	pe.dwSize = sizeof(PROCESSENTRY32);
	//下面的IF效果同:       
	//if(hProcessSnap == INVALID_HANDLE_VALUE)   无效的句柄  
	if (!Process32First(hSnapShot, &pe))
	{
		::CloseHandle(hSnapShot);
		DeleteObject(hSnapShot);
		return false;
	}
	processName.MakeLower();
	BOOL clearprocess = FALSE;
	//如果句柄有效  则一直获取下一个句柄循环下去 
	while (Process32Next(hSnapShot, &pe))
	{
		//pe.szExeFile获取当前进程的可执行文件名称 
		CString scTmp = pe.szExeFile;
		scTmp.MakeLower();
		char modPath[MAX_PATH] = { 0 };
		if (!scTmp.Compare(processName))
		{
			//从快照进程中获取该进程的PID(即任务管理器中的PID)
			DWORD dwProcessID = pe.th32ProcessID;

			//获取进程的句柄
			//HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
			//获取第一个可视窗口的句柄
			wi->dwProcessId = dwProcessID;
			DeleteObject(wi->hWnd);
			GetProcessHwnd(dwProcessID, wi);

			//RECT rect = { 0 };
			//GetWindowRect(hWnd, &rect);
			//drawFishHdc(hWnd); 

			break;
			//获取文件路径
			//MODULEENTRY32 me = { sizeof(me) };
			//HANDLE hpro;
			//hpro = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);
			//Module32First(hpro, &me);
			//TRACE(processName + " Path: " + me.szExePath);
			//::CloseHandle(hpro);

		}
	}
	::CloseHandle(hSnapShot);
	DeleteObject(hSnapShot);
	return true;
}