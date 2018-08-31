#ifndef __SYSCONFIG__
#define __SYSCONFIG__

#include "sysUtil.h"
#include <opencv2/core/core.hpp> 
using namespace std;
class SysConfig
{
public:
	SysConfig();
	~SysConfig();
	bool init();
	bool chooseImgID();
	//系统配置
	string workPath; 
	bool debugMode=false;
	DWORD soundDevcieFrequency = 48000;
	//网络时间
	bool isDisplayWebTime = false;

	//软件是否已经激活
	bool isActive = false;
	void checkIsActive(); 
	//激活后license的值存储；
	string esn ;
	string date ;
	string sign ;
	string debug;
	string signedString ; 
	//验证过程中使用的变量，验证成功后复制到实际使用的变量。
	string esnTemp;
	string dateTemp;
	string debugTemp;
	string signTemp;
	string signedStringTemp;
	//license文件地址
	LPTSTR licFilePath ;  
	//F:\My Document\workspace\Visual Studio 2015\GetOpenFileNameTest\GetOpenFileNameTest\config\key
	string pubKeyPath = "./config\\key\\key.pub";
	//const char* pubKeyPath = "F:\\My Document\\workspace\\Visual Studio 2015\\GetOpenFileNameTest\\GetOpenFileNameTest\\config\\key\\key.pub";
	string privateKeyPath = "./config\\key\\key.pri";

	//const char * STD_GMM_FILE = "./config\\gmm\\fishTrigerStd.GMM"; 
	string STD_GMM_48k_FILE;
	string STD_GMM_44k_FILE;
	//string supportedDeviceFrequency = "48000,44100";
	DWORD supportedDeviceFrequency[2]= {
		48000,
		44100
	};
	string getStdGMMFilePath(DWORD SampleFrequency);

	bool isSampleFrequencySupported(DWORD SampleFrequency);
	string EXPORT_GMM_FILE = "./config\\record\\train.GMM";
	string FISHING_TRIGER_STD48k_WAV = "./config\\soundID\\fishTriger48k.wav";
	string FISHING_TRIGER_STD44k_WAV = "./config\\soundID\\fishTriger44k.wav";
	 
	//钓鱼按键设置
	bool isLeftMouseBtnPickup = false;
	 
	char* WOW_EXE_64_NAME = "wow-64.exe";
	char* WOW_EXE_32_NAME = "wow.exe";
	string otherWowName;
	int searching_cursor_pos = -1;
	int not_find_cursor_pos = -2;
    int wating_for_start_fish = -3;
	int located_cursor_pos = 1;
	int not_started_searching_cursor_pos = 0;

	//钓鱼屏幕互动检测鼠标移动间隔时间
	int cursorMoveSpeedsliderAccuracy = 25;
	int cursorMoveSpeedsMaxliderAccuracy = 50;
	//启动抛竿后，延迟检测鼠标鱼漂时间,单位毫秒
	int delayCursorFishFlotCheck = 1000;
	int glogPrintLevel = 0;

	 
	//是否保存鼠标形状图片
	bool saveCursorImg = false;
	bool isDisplayMatchTemplateCursorLog = false;
	double checkCursorFishingFloatBoundaryValue = 0.7;
	string RECORD_FISHING_CURSOR_FILE = "config\\record\\cursor.bmp";
	//钓鱼鼠标形状
	string fishCursorStdImgId= "fishingCursorStd.bmp";
	string fishCursorGearImgId = "fishingCursorGear.bmp";
	string FISHING_CURSOR_IMGID_FILE_PATH = "./config\\imgID\\default\\";
	string FISHING_CURSOR_IMGID_FILE_NAME = "fishingCursorStd.bmp"; 
	bool saveMatchTemplateCursorFishingFloatImg = false;
	string RECORD_MATCH_TEMPLAE_CURSOR_RESULT_PATH = "config\\record\\matchTemplateCursor.bmp";
	//是否保存检测鱼漂区域图片
	bool saveFishingFloatCheckAreaImg = false;
	bool isDisplayMatchTemplateFishingFloatAreaLog = false;
	float checkScreenFishingFloatBoundaryValue = 0.75;
	string RECORD_FISHING_FLOAT_CHECK_AREA_FILE = "config\\record\\fishingFloatCheckArea.bmp";
	string FISHING_FLOAT_IMGID_PATH = "config\\imgID\\default\\fishingFloatImgStd.bmp";
	//是否保存图像模板匹配后鼠标的图像匹配结果
	bool saveMatchTemplateFishingFloatImg = false;
	string RECORD_MATCH_TEMPLATE_FISHING_FLOAT_AREA_RESULT_PATH = "config\\record\\matchTemplateFishingFloat.bmp";
	//是否保存检测钓鱼状态区域图片 
	bool saveCheckFishStatusImg = false;
	bool isDisplayMatchTemplateCheckFishStatusLog = false;
	float checkScreenFishStatusBoundaryValue = 0.79;

	int checkScreenFishStatusXOffset = 0;
	int checkScreenFishStatusYOffset = 0;
	string RECORD_FISHING_STATUS_FILE = "config\\record\\fishstatus.bmp";
	std::string FISHING_STATUS_IMGID_FILE = "config\\imgID\\default\\fishStatusStd.bmp";
	std::string FISHING_STATUS2_IMGID_FILE = "config\\imgID\\default\\fishStatusStd2.bmp";
	std::string FISHING_STATUS3_IMGID_FILE = "config\\imgID\\default\\fishStatusStd3.bmp";
	std::string FISHING_STATUS4_IMGID_FILE = "config\\imgID\\default\\fishStatusStd4.bmp";
	std::string FISHING_STATUS5_IMGID_FILE = "config\\imgID\\default\\fishStatusStd5.bmp";
	IplImage* resizedFishStatusTemplate = NULL;
	IplImage* resizedFishStatusTemplate2 = NULL;
	IplImage* resizedFishStatusTemplate3 = NULL;
	IplImage* resizedFishStatusTemplate4 = NULL;
	IplImage* resizedFishStatusTemplate5 = NULL;
	int maxWindowWidht = 1550;
	int maxWindowHeight=838;
	void releaseFishStatusTemplate();
	//是否保存图像模板匹配后钓鱼状态的图像匹配结果
	bool saveMatchTemplateFishStatusImg = false;
	string RECORD_MATCH_TEMPLAE_FISH_STATUS_RESULT_PATH = "config\\record\\matchTemplateFishStatus.bmp";
	
	 

	//监视器启动延迟时间,单位秒
	int START_MONITOR_DELAY = 3;

	//屏幕搜索鱼漂区域
	int SEARCH_FISHING_CURSOR_X_MIN = 700; //1550*838  搜索宽度=x*0.065  x起始点=(x-x*0.065)/2=x*0.935/2
	int SEARCH_FISHING_CURSOR_X_MAX = 800;//SEARCH_FISHING_CURSOR_X_MIN+x*0.065
	int SEARCH_FISHING_CURSOR_Y_MIN = 300;//搜索高度=y*0.15  y起始点=y*0.5-y*0.12=y*0.38
	int SEARCH_FISHING_CURSOR_Y_MAX = 400;//终点y/2
	int SEARCH_FISHING_CURSOR_X_OFFSET = 15;//x*0.00968
	int SEARCH_FISHING_CURSOR_Y_OFFSET = 15;//y*0.0179  
	void freshSearchField(RECT *frameRect);

	//屏幕检索钓鱼状态区域
	int FISH_STATUS_CHECK_FIELD_X = 562;//1550 * 838 检测起始点x=(x-x*285/1550)/2    //x*3/4/2
	int FISH_STATUS_CHECK_FIELD_y = 560;//检测起始点y=y*775/838
	int FISH_STATUS_CHECK_FIELD_WIDTH = 285;//检测区域宽度x*285/1550  x/4
	int FISH_STATUS_CHECK_FIELD_HEIGHT = 50;//检测高度y*50/838    y/3
	void freshFishStatusCheckFeild(RECT frameRect, bool client2screen);

	//系统屏幕缩放比例，例如win10
	double system_display_zoom = 1;
	double system_display_zoom_y = 1;
	double last_system_display_zoom = 1;
	//系统屏幕原始大小
	int windowOrignalWidth = 1920;
	int windowOrignalHeight = 1080;
	int afterZoomWindowWidth=0;
	int afterZoomWindowHeight = 0;
	void getSystemDisplayZoom();
	//钓鱼收杆延迟时间，单位ms
	int FINISH_FISH_DELAY = 2000;

	//声音识别 //钓鱼声音检测灵敏度
	//识别次数  /*< 识别次数 */
	int fishSoundMatchAccuracy = 3;//取值范围1~10,1最灵敏，但是出错率高，10最不灵敏，出错率低

	//日志设置
	bool detailLogEnable = false;
	int glogLevel = 2;
	int glogMaxSize = 10;
	string glogDestination;
	char* glogHead = "Auto Fish Tool";
	//Wow 屏幕信息;
	RECT wowLastWindowRect = { 0,0,0,0 };
	RECT wowWindowRect = { 0,0,0,0 };
	WNDINFO wowWindowInfo;
	bool SysConfig::wowWindowChanged();
	bool synWowWindowInfo();
	void loadSysconfig();


	//定时按键（鱼饵设置）
	//定时器1
	const static  int timer1Id = 1;
	const static  int mqTimerId = 9;
	const static  int initMqTimerId = 8;
	bool timerThread1_Enable = false;
	string timerThread1_keyName ="2";
	int timerThread1_intervalSeconds = 0;
	int timerThread1_pauseTime = 0;
	//定时器2
	const  static   int timer2Id = 2;
	bool timerThread2_Enable = false;
	string timerThread2_keyName="3";
	int timerThread2_intervalSeconds = 0;
	int timerThread2_pauseTime = 0;
	//定时器 检测钓鱼间隔，超时重新抛竿
	const static  int fishIntervalCheckTimerId = 7;

private: 
	
	void saveSysconfig(); 
	string configFilePath; 
	int screenSize[13][2] = { //屏幕尺寸
		{ 1920,1080 },
		{ 1024,768 },
		{ 1280,600 },
		{ 1280,720 },
		{ 1280,768 },
		{ 1280,800 },
		{ 1280,960 },
		{ 1280,1024 },
		{ 1366,768 },
		{ 1400,900 },
		{ 1400,1050 },
		{ 1600,900 },
		{ 1680,1050 }
	};
	int maxWindowSize_0_8[13][2] = {
		{ 1550, 838},// 1980,1020 
		{ 834,589},//1024,768
		{ 1296, 576},//1280,600 *1
		{ 1296, 696},//1280,720  *1
		{ 1038 , 589 },//1280,768
		{ 1038 , 614},//1280,800
		{ 1038 , 742 },//1280,960
		{ 1038 ,794 },//1280,1024
		{ 1107 , 589 },//1366,768
		{ 1166 , 694 },//1400,900 
		{ 1134 ,814 },//1400,1050 
		{ 1294 , 694 },//1600,900  
		{ 1358 ,814 }//1680,1050
	};
	int maxWindowSize_1_0[13][2] = {
		{ 1936 , 1056 },// 1980,1020 
		{ 1040 , 744 },//1024,768
		{ 1296 , 576 },//1280,600
		{ 1296,  696},//1280,720
		{ 1296 , 744 },//1280,768
		{ 1296 , 776 } ,//1280,800
		{ 1296 , 936}, //1280,960
		{ 1296 , 1000 },//1280,1024
		{ 1382 , 744 },// 1366,768
		{ 1456 , 876 },//1400,900
		{ 1416 , 1026 },//1400,1050
		{ 1616 , 876 },//1600,900 
		{ 1696 , 1026 }//1680,1050
	};


	
};
 

#endif // !__SYSCONFIG__
