#include "stdafx.h"
#include "kbHook.h" 
#include "logger.h"
#include "timeUtil.h"

//keypress time record
LONG VK_F9_AND_CTRL_LAST_PRESS_TIME = 0;
LONG VK_F10_AND_CTRL_LAST_PRESS_TIME = 0; 

KBHook::KBHook()
{
}

KBHook::~KBHook()
{
}
void KBHook::pressKeyboard(char key)
{
	keybd_event(key, 0, 0, 0);
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);

}

void KBHook::pressKeyboardDown(char key)
{
	keybd_event(key, 0, 0, 0); 
}

void KBHook::keyboardUp(char key)
{
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}

LRESULT KBHook::kbHookFun(int nCode, WPARAM wParam, LPARAM lParam, HHOOK g_HookHwnd, Monitor *monitor)
{
	// 这个Structure包含了键盘的信息
	/*typedef struct {
	DWORD vkCode;
	DWORD scanCode;
	DWORD flags;
	DWORD time;
	ULONG_PTR dwExtraInfo;
	} KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;*/
	// 我们只需要那个vkCode
	PKBDLLHOOKSTRUCT pVirKey = (PKBDLLHOOKSTRUCT)lParam;
	int ctrlState = 0;
	// MSDN说了,nCode < 0的时候别处理
	if (nCode >= 0)
	{
		// 按键消息
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			switch (pVirKey->vkCode)
			{
			case VK_LWIN:
			case VK_RWIN:
				return 1;  // 吃掉消息
				break;
			case VK_F9:
				//F9和Ctrl同时按下，开启钓鱼监控、声音监控

				if (ctrlState = GetKeyState(VK_LCONTROL) < 0)
				{
					//if (VK_F9_AND_CTRL_LAST_PRESS_TIME == 0 || (getCurrentTimeMilliseconds() - VK_F9_AND_CTRL_LAST_PRESS_TIME > 50))
					if(!monitor->isMonitorRunning)
					{  
						monitor->stopMonitor();
						monitor->startMonitor(); 
					}

				}
				break;
			case VK_F10:
				//F10和Ctrl同时按下，关闭钓鱼监控、声音监控
				if (ctrlState = GetKeyState(VK_LCONTROL) < 0)
				{
					//if (VK_F10_AND_CTRL_LAST_PRESS_TIME == 0 || (getCurrentTimeMilliseconds() - VK_F10_AND_CTRL_LAST_PRESS_TIME > 100))
					if(monitor->isMonitorRunning)
					{
						monitor->isMonitorRunning = false;
						Logger::displayLog("助手已关闭\r\n");
						monitor->stopMonitor();
						//VK_F10_AND_CTRL_LAST_PRESS_TIME = getCurrentTimeMilliseconds();
					} 
				}

				break;
			}
			break;
		}
	}

	return CallNextHookEx(g_HookHwnd, nCode, wParam, lParam);


}