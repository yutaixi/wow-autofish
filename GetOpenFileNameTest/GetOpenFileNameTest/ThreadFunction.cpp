#include "stdafx.h"
#include "ThreadFunction.h"
#include "Security.h"
#include"dlgHelper.h"  
#include "kbHook.h"
#include "MQ.h" 

extern Monitor monitor;
extern Wow wow;
extern SysConfig sys;
extern MQ mq;

bool validateFishingFloatPoint(POINT *point);
bool  findFishingFloatWithinRect(int offset);
void monitorStartCountDown(bool display);

void processMqMsg(MqMsg msg);
void checkFishIntervalTime();

DWORD WINAPI  Fun(LPVOID lpParamter)
{
	
	monitorStartCountDown(true);
	if (monitor.isMonitorRunning)
	{
		wow.startFish(true);
	}
	
	while (!wow.find_cursor_thread_destroy && monitor.isMonitorRunning)
	{
		
		LOG(INFO) << "Fun线程循环";
		int matchNum = 0; 
		if (!wow.stopStatus)
		{ 
			Sleep(sys.delayCursorFishFlotCheck);
			LOG(INFO) << "开始检测鼠标互动";
			wow.FIND_CUSOR_FAILED_NUM = 0;
			wow.FIND_CUSOR = sys.searching_cursor_pos;
			int matchTemplate = 0;
			matchNum = 0;
			while (matchTemplate > 0 && monitor.isMonitorRunning)
			{
				ImgEx::getFishingFloatFromCheckArea(&(wow.fish_point));
				//displayLog("图像模板识别点 %d %d \r\n", fish_point.x, fish_point.y);
				MouseHook::mouseMoveClientToScreen(&(wow.fish_point), &(sys.wowWindowRect));
				if (validateFishingFloatPoint(&(wow.fish_point)))
				{
					matchNum++;
					if (matchNum >= 2)
					{
						//displayLog("使用图像模板识别点 %d %d \r\n", fish_point.x, fish_point.y);
						LOG(INFO) << "使用图像模板识别点";
						MouseHook::mouseMoveClientToScreen(&(wow.fish_point), &(sys.wowWindowRect));
						//CloseHandle(hThread);
						//hThread = NULL;
						wow.stopStatus = true;
						wow.FIND_CUSOR = sys.located_cursor_pos;
						break;
					}

				}
				Sleep(500);
				matchTemplate--;
			}

			while (wow.FIND_CUSOR == sys.searching_cursor_pos && !wow.stopStatus &&  monitor.isMonitorRunning)
			{
				if (findFishingFloatWithinRect(wow.FIND_CUSOR_FAILED_NUM == 0 ? 0 : sys.SEARCH_FISHING_CURSOR_Y_OFFSET / 2))
				{
					//displayLog("使用图像汉明顿距离识别点 %d %d \r\n", fish_point.x, fish_point.y);
					LOG(INFO) << "使用图像汉明顿距离识别点";
					MouseHook::mouseMoveClientToScreen(&(wow.fish_point), &(sys.wowWindowRect));
					wow.stopStatus = true;
					wow.FIND_CUSOR = sys.located_cursor_pos;
					break;
				}
				//区域内循环一次后，未找到互动焦点
				wow.FIND_CUSOR_FAILED_NUM++;
				//查找互动焦点失败2次后，停止寻找
				if (wow.FIND_CUSOR_FAILED_NUM >= 2)
				{
					LOG(INFO) << "未找到鼠标互动点";
					wow.FIND_CUSOR = sys.not_find_cursor_pos;
					wow.stopStatus = true;
					break;
				}

			}

		}
		//if (FIND_CUSOR = wating_for_start_fish && check_is_fishing)
		//{
		//	displayLog("check_is_fishing 未处于钓鱼状态,重新检测鼠标互动\r\n");
		//	FIND_CUSOR = not_started_searching_cursor_pos;
		//	stopStatus = false;
		//} 
		Sleep(1000);
	}
	return 1;
}

bool  findFishingFloatWithinRect(int offset)
{
	int x, y;
	for (y = sys.SEARCH_FISHING_CURSOR_Y_MIN + offset; y < sys.SEARCH_FISHING_CURSOR_Y_MAX && !wow.stopStatus; y += sys.SEARCH_FISHING_CURSOR_Y_OFFSET)
	{
		for (x = sys.SEARCH_FISHING_CURSOR_X_MIN; x < sys.SEARCH_FISHING_CURSOR_X_MAX && !wow.stopStatus; x += sys.SEARCH_FISHING_CURSOR_X_OFFSET)
		{
			if (wow.stopStatus || !monitor.isMonitorRunning)
			{
				//displayLog("check_is_fishing 未处于钓鱼状态\r\n"); 
				return false;
			}
			wow.fish_point.x = x;
			wow.fish_point.y = y;

			//当前点是鼠标互动点
			if (validateFishingFloatPoint(&(wow.fish_point)))
			{
				return true;
			}
			Sleep(20);
		}
	}
	return false;
}
bool validateFishingFloatPoint(POINT *point)
{
	MouseHook::mouseMoveClientToScreen(point, &(sys.wowWindowRect));
	Sleep(sys.cursorMoveSpeedsliderAccuracy);
	return  ImgEx::checkFishingFloatPoint();
}


DWORD WINAPI fishTimer(LPVOID lpParamter)
{
	monitorStartCountDown(false);
	while (!wow.timer_destroy &&  monitor.isMonitorRunning)
	{
		if (!wow.fishTimerPauseStatus)
		{
			LOG(INFO) << "fishTimer 循环";
			//checkWowWindowFocus(); 
			if (!monitor.refreshWowWindowInfo())
			{
				monitor.stopMonitor();
				return 0;
			}
			wow.check_is_fishing = ImgEx::checkScreenFishStatus();
			//double scale1 = (double)(sys.wowWindowRect.right - sys.wowWindowRect.left) / (double)(sys.wowWindowRect.bottom - sys.wowWindowRect.top);
			//double scale2 =((double)sys.maxWindowWidht / (double)sys.maxWindowHeight);
			//Logger::displayLog("比例为%f  %f\r\n", scale1, scale2);
			//check_is_fishing = false;
			if (!wow.check_is_fishing && !wow.fishTimerPauseStatus)
			{
				//实际未处于钓鱼状态，声音识别失败，或者鼠标互动查找失败两次，重新抛竿
				if (!(wow.FIND_CUSOR == sys.searching_cursor_pos) && (wow.IS_FISHING && !wow.RECOGNISE_SOUND && (wow.FIND_CUSOR == sys.located_cursor_pos || wow.FIND_CUSOR == sys.wating_for_start_fish) || wow.FIND_CUSOR_FAILED_NUM >= 2) || wow.FIND_CUSOR == sys.located_cursor_pos)
				{
					//Logger::displayLog("fishTimer检测未处于钓鱼状态，重新抛竿  \r\n");
					LOG(INFO) << "fishTimer检测未处于钓鱼状态，重新抛竿";
					wow.startFish(false);
				}
			}
		}
		
		 
		Sleep(1500);

	}
	return 1;
}

void monitorStartCountDown(bool display)
{
	int i = 0;
	while (i < sys.START_MONITOR_DELAY && !wow.find_cursor_thread_destroy)
	{
		if (display)
		{
			Logger::displayLog("%d秒后启动\r\n", sys.START_MONITOR_DELAY - i);
		} 
		Sleep(1000);
		i++;
	}
	if (display&& !wow.find_cursor_thread_destroy)
	{
		Logger::displayLog("助手已开启\r\n", sys.START_MONITOR_DELAY - i);
	} 
}


DWORD WINAPI licExpireCheck(LPVOID lpParamter)
{

	while (1)
	{
		 
		if (!sys.isActive)
		{
			MessageBox(NULL, L"尚未激活", L"到期提醒", 0); 
			LOG(ERROR) << "软件尚未激活";
			return 0;
		}

		if (Security::isLicExpiredThreadCheck())
		{
			DlgHelper::disaleAutoFish();
			monitor.stopMonitor();
			MessageBox(NULL, L"licence已过期", L"到期提醒", 0);
			Logger::displayLog("licence已过期\r\n");
			LOG(ERROR) << "licence已过期";
			return 0;
		} 

		Sleep(30000);
	}
	


}
/*

DWORD WINAPI keyPressTimer1(LPVOID lpParamter)
{ 
	    monitorStartCountDown(false);
		while (1)
		{
			if (monitor.keyPressTimer1PauseStatus)
			{
				return 1;
			}
			Logger::displayLog("keyPressTimer1循环\r\n");
			if (sys.timerThread1_Enable)
			{
				
				monitor.pauseMonitor();
				//按键
				KBHook::pressKeyboard(sys.timerThread1_keyName.c_str()[0]); 
				//停顿 
				Sleep(sys.timerThread1_pauseTime * 1000);
				if (monitor.keyPressTimer1PauseStatus)
				{
					return 1;
				}
				monitor.continueMonitor();
				Sleep(sys.timerThread1_intervalSeconds * 1000);
				 
			}else
			{
				Sleep(3000);
			}
		}  
		return 1;
}

DWORD WINAPI keyPressTimer2(LPVOID lpParamter)
{ 
	monitorStartCountDown(false);
	while (1)
	{ 
		if (monitor.keyPressTimer2PauseStatus)
		{
			return 1;
		}
		Logger::displayLog("keyPressTimer2循环\r\n");
		if (sys.timerThread2_Enable )
		{
			
			monitor.pauseMonitor();
			//按键
			KBHook::pressKeyboard(sys.timerThread2_keyName.c_str()[0]);
			//停顿 
			Sleep(sys.timerThread2_pauseTime * 1000);
			if (monitor.keyPressTimer2PauseStatus)
			{
				return 1;
			}
			monitor.continueMonitor();
			Sleep(sys.timerThread2_intervalSeconds * 1000);
			 
		}
		else
		{
			Sleep(3000);
		}
	}
	return 1;
}
*/

void pushDataBeforeKeyPressTimerStart()
{
	LOG(INFO) << "pushDataBeforeKeyPressTimerStart";
	if (sys.timerThread1_Enable)
	{ 
		mq.push(MqMsg(operationEnum::PAUSE_AND_PRESS_KEY, sys.timerThread1_keyName, sys.timerThread1_pauseTime));
	}
	if (sys.timerThread2_Enable)
	{ 
		mq.push(MqMsg(operationEnum::PAUSE_AND_PRESS_KEY, sys.timerThread2_keyName, sys.timerThread2_pauseTime));
	}

}
void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	switch (nTimerid)
	{
	case sys.timer1Id:
		if (sys.timerThread1_Enable)
		{
			//Logger::displayLog("timer1\r\n");
			mq.push(MqMsg(operationEnum::PAUSE_AND_PRESS_KEY,sys.timerThread1_keyName,sys.timerThread1_pauseTime));
		} 
		break;
	case sys.timer2Id:
		if (sys.timerThread2_Enable)
		{
			//Logger::displayLog("timer2\r\n");
			mq.push(MqMsg(operationEnum::PAUSE_AND_PRESS_KEY, sys.timerThread2_keyName, sys.timerThread2_pauseTime));
		} 
		break;
	case  sys.initMqTimerId:
	      { 
		       pushDataBeforeKeyPressTimerStart();
			   KillTimer(hWnd, sys.initMqTimerId);
			   LOG(INFO) << "pushDataBeforeKeyPressTimerStart and KillTimer";
	      }
		break;
	case sys.fishIntervalCheckTimerId:
		checkFishIntervalTime();
		break;
	default:
		break;
	} 
}
void CALLBACK MqTimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{

	switch (nTimerid)
	{
	case sys.mqTimerId:
		if (!mq.isEmpty())
		{
			monitor.pauseMonitor();
			while (!mq.isEmpty())
			{
				wow.lastFinishFishTime = time(NULL);
				processMqMsg(mq.pop());
			}
			monitor.continueMonitor();
		} 
		break;
	default:
		break;
	}

}

void processMqMsg(MqMsg msg)
{
	switch (msg.operation)
	{
	case operationEnum::PAUSE_AND_PRESS_KEY:
	    { 
		  if (!msg.msg.empty() && msg.data>=0 )
		  {
			//按键
			KBHook::pressKeyboard(msg.msg.c_str()[0]);
			//停顿 
			Sleep(msg.data * 1000);
		  } 
	    }
		break;
	default:
		break;
	}

}

void checkFishIntervalTime()
{

	time_t now = time(NULL);
	if (now - wow.lastFinishFishTime >= (23+sys.FINISH_FISH_DELAY/1000)  )
	{
		wow.startFish(true);
		wow.lastFinishFishTime = time(NULL);
	}
	return;

}