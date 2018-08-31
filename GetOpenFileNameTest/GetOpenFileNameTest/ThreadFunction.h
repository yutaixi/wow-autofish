#ifndef __THREAD_FUNCTION__
#define __THREAD_FUNCTION__


#include <Windows.h>
#include "stdafx.h"
#include "Monitor.h"
#include "logger.h" 
#include "mouseHook.h"
#include "sysconfig.h"
#include "imgEx.h"

#include "wow.h"
 
 
 
bool  findFishingFloatWithinRect(int offset);

bool validateFishingFloatPoint(POINT *point);
DWORD WINAPI  Fun(LPVOID lpParamter);
DWORD WINAPI fishTimer(LPVOID lpParamter);
DWORD WINAPI licExpireCheck(LPVOID lpParamter);


//DWORD WINAPI keyPressTimer1(LPVOID lpParamter); 
//DWORD WINAPI keyPressTimer2(LPVOID lpParamter);


void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);
void CALLBACK MqTimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);
void pushDataBeforeKeyPressTimerStart();
#endif