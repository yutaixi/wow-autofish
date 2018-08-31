#include "stdafx.h" 
#include "Monitor.h"
class KBHook
{
public:
	KBHook();
	~KBHook();
	static void KBHook::pressKeyboard(char key);
	static void KBHook::pressKeyboardDown(char key);
	static void KBHook::keyboardUp(char key);
	static LRESULT kbHookFun(int nCode, WPARAM wParam, LPARAM lParam, HHOOK g_HookHwnd, Monitor *monitor);
private:

};


