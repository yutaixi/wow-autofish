#include "stdafx.h"
#include "wow.h" 
#include "global.h" 
#include "mouseHook.h" 
#include "resource.h"
#include "logger.h" 
Wow::Wow()
{  

}

Wow::~Wow()
{

}
void Wow::startFish(bool displayLog)
{
	LOG(INFO) << "抛竿，正在帮您钓鱼";
	check_is_fishing = true;
	IS_FISHING = true;
	FIND_CUSOR = sys.not_started_searching_cursor_pos;
	RECOGNISE_SOUND = false;
	if (displayLog)
	{
		Logger::displayLog("抛竿，正在帮您钓鱼\r\n");
	} 
	//抛竿
	PostMessage(hwndMainDlg, WM_START_FISH, 0, 0);
	//开始鼠标互动检测 
	stopStatus = false;
	lastFinishFishTime = time(NULL);
	 
}
void Wow::finishFish(POINT * fishFloatPoint,RECT *widowLocation)
{
	LOG(INFO) << "收竿，成功钓到一条鱼";
	Logger::displayLog("收竿，成功钓到一条鱼\r\n"); 
	IS_FISHING = false;
	RECOGNISE_SOUND = true;
	/*
	if (hThread != NULL)
	{
	CloseHandle(hThread);
	hThread = NULL;
	stopStatus = true;
	}
	*/
	if (FIND_CUSOR == sys.located_cursor_pos)
	{
		//point->x = fish_point.x;
		//point->y = fish_point.y;
		//MessageBox(NULL, TEXT("已找到"), TEXT("欢迎"), 0);
		//ClientToScreen(wowWindowInfo.hWnd, point);
		//MouseMove(point->x, point->y);
		MouseHook::mouseMoveClientToScreen(&fish_point, widowLocation);
		
	}
	//点击鼠标取鱼 shift+鼠标左键单击，自动拾取
	//MouseHook::shiftAndMouseLClick();
	if (sys.isLeftMouseBtnPickup)
	{
		MouseHook::mouseLClick();
	}
	else
	{
		MouseHook::mouseRClick();
	}
	lastFinishFishTime = time(NULL);
	FIND_CUSOR = sys.not_started_searching_cursor_pos;
	//关闭鼠标互动检测
	stopStatus = true;
	Sleep(sys.FINISH_FISH_DELAY);
}
