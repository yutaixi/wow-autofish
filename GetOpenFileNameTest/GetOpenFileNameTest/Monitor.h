#ifndef __MONITOR__
#define __MONITOR__
#include <Windows.h>   
#include "sysconfig.h"
#include "sysUtil.h"
//#include "wow.h"
 
class Monitor
{
public:
	Monitor();
	~Monitor();
	  void monitorInit();
	  bool isWowWindowFocus();
	  bool checkWowWindowFocus();
	  void getWowWindowRect(RECT *rect);
	  bool checkWindowSize(RECT *rect);
	  void getFrontWindowID();
	  void getWowProcessID();
	  void startMonitor();
	  void stopMonitor();  
	  bool refreshWowWindowInfo();
	  void startLicExpirationCheckThread();
	   
	  DWORD frontWindowProcessID=0; 
	  HANDLE hThread = NULL;
	  HANDLE timerThread = NULL;
	  HANDLE checkLicExpiredThread = NULL;
	  bool isMonitorRunning = false;

	  bool pauseMonitor();
	  bool continueMonitor();
	  //按键定时线程
	  HANDLE keyPressThread1 = NULL;
	  bool keyPressTimer1PauseStatus = true;
	  HANDLE keyPressThread2 = NULL;
	  bool keyPressTimer2PauseStatus = true;
	  void  initMqTimer(HWND hwnd); 
private:
	bool  startKeyPressTimer();
	bool stopKeyPressTimer();
};


#endif