#ifndef __WOW__
#define __WOW__
#include <Windows.h> 
#include "sysconfig.h"
extern SysConfig sys;

class Wow
{
public:
	Wow();
	~Wow();
	  void startFish(bool displayLog);
	  void finishFish(POINT * fishFloatPoint, RECT *widowLocation); 
	//fish status  
	  bool find_cursor_thread_destroy = false;
	
	  bool start_find_cursor = false;
	  bool timer_destroy = false;//定时检测状态线程销毁标识
	  bool  IS_FISHING = false; //是否正在钓鱼
	  POINT fish_point = { 0,0 };//钓鱼鼠标互动点坐标  

	  int FIND_CUSOR = sys.not_started_searching_cursor_pos;//是否已经找到钓鱼鼠标互动点 0未找到，1找到，-1正在找
	  bool check_is_fishing = false;
	  bool RECOGNISE_SOUND = false;//是否已经识别出钓到鱼的声音
	  bool fish_status_matching = false;
	  bool start_rec_sys_sound = false;//已经开启语音识别线程
	  int FIND_CUSOR_FAILED_NUM = 0;
	   //状态控制
	  bool stopStatus = false;//鼠标检测
	  bool fishTimerPauseStatus = false;
	  bool pauseSoundRecognize = false;

	  time_t lastFinishFishTime=0;
private:

};

#endif