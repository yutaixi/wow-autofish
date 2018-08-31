#include "stdafx.h"
#include "sysconfig.h" 
#include "opencv2/opencv.hpp"
#include <sstream>
#include "Security.h"
#include "CharacterHelper.h"
#include "fileUtil.h"
#include "MathUtil.h" 

SysConfig::SysConfig()
{ 
	WCHAR chpath[MAX_PATH]; 
	char* pChPath = NULL;
	ZeroMemory(chpath, sizeof(chpath)); 

	//获取当前工作路径
	GetCurrentDirectory(sizeof(chpath), chpath);
	//转换成char
	pChPath = CAPECharacterHelper::GetANSIFromUTF16(chpath); 
	string pathStr(pChPath) ; 
	//string licPath = CAPECharacterHelper::connectStr(pChPath, "./config\\key\\key.lic");
	string licPath = string(pChPath).append("./config\\key\\key.lic");

	//转换成wchar
	licFilePath = CAPECharacterHelper::GetUTF16FromANSI(licPath.c_str());
	 
	workPath = string(pChPath).append("");
	pubKeyPath = string(pChPath).append("./config\\key\\key.pub");
	privateKeyPath = string(pChPath).append("./config\\key\\key.pri");
	STD_GMM_48k_FILE = CAPECharacterHelper::connectStr(pChPath, "./config\\gmm\\fishTrigerStd48k.GMM");
	STD_GMM_44k_FILE = CAPECharacterHelper::connectStr(pChPath, "./config\\gmm\\fishTrigerStd44k.GMM");
	EXPORT_GMM_FILE =string(pChPath).append("./config\\record\\train.GMM"); 
	FISHING_TRIGER_STD48k_WAV =string(pChPath).append("./config\\soundID\\fishTriger48k.wav"); 
	FISHING_TRIGER_STD44k_WAV = string(pChPath).append("./config\\soundID\\fishTriger44k.wav");
	RECORD_FISHING_CURSOR_FILE =string(pChPath).append("./config\\record\\cursor.bmp"); 
	FISHING_CURSOR_IMGID_FILE_PATH =string(pChPath).append("./config\\imgID\\default\\");  
	RECORD_MATCH_TEMPLAE_CURSOR_RESULT_PATH =string(pChPath).append("./config\\record\\matchTemplateCursor.bmp"); 
	RECORD_FISHING_FLOAT_CHECK_AREA_FILE =string(pChPath).append("./config\\record\\fishingFloatCheckArea.bmp");
	FISHING_FLOAT_IMGID_PATH =string(pChPath).append("./config\\imgID\\default\\fishingFloatImgStd.bmp"); 
	RECORD_MATCH_TEMPLATE_FISHING_FLOAT_AREA_RESULT_PATH =string(pChPath).append("./config\\record\\matchTemplateFishingFloat.bmp");
	RECORD_FISHING_STATUS_FILE =string(pChPath).append("./config\\record\\fishstatus.bmp");
	FISHING_STATUS_IMGID_FILE =string(pChPath).append("./config\\imgID\\default\\fishStatusStd.bmp");
	FISHING_STATUS2_IMGID_FILE =string(pChPath).append("./config\\imgID\\default\\fishStatusStd2.bmp");
	FISHING_STATUS3_IMGID_FILE =string(pChPath).append("./config\\imgID\\default\\fishStatusStd3.bmp");
	FISHING_STATUS4_IMGID_FILE =string(pChPath).append("./config\\imgID\\default\\fishStatusStd4.bmp"); 
	RECORD_MATCH_TEMPLAE_FISH_STATUS_RESULT_PATH =string(pChPath).append("./config\\record\\matchTemplateFishStatus.bmp");
	glogDestination =string(pChPath).append("./log/log");
	configFilePath=string(pChPath).append("./config/config.ini"); 
	delete[] pChPath;
	pChPath = NULL;
}

SysConfig::~SysConfig()
{
	saveSysconfig();
}

void SysConfig::freshSearchField(RECT *frameRect)
{ 
	int searchWidth = (frameRect->right - frameRect->left) / 9;
	int searchHeight = (frameRect->bottom - frameRect->top) / 8;
	SEARCH_FISHING_CURSOR_X_MIN = ((frameRect->right - frameRect->left) - searchWidth)/2;
	SEARCH_FISHING_CURSOR_X_MAX = (SEARCH_FISHING_CURSOR_X_MIN+ searchWidth) ;
	
	SEARCH_FISHING_CURSOR_Y_MAX = (frameRect->bottom - frameRect->top) /2*7/8 ;
	SEARCH_FISHING_CURSOR_Y_MIN = SEARCH_FISHING_CURSOR_Y_MAX- searchHeight;
	SEARCH_FISHING_CURSOR_X_OFFSET = (frameRect->right - frameRect->left)*0.00968;
	SEARCH_FISHING_CURSOR_Y_OFFSET = (frameRect->bottom - frameRect->top)*0.0179;
}
void SysConfig::freshFishStatusCheckFeild(RECT frameRect,bool client2screen)
{ 
	 
	FISH_STATUS_CHECK_FIELD_X = (frameRect.right - frameRect.left) * 80/100 / 2 ;
	FISH_STATUS_CHECK_FIELD_y = (double)(frameRect.bottom - frameRect.top)  * 79/ 100 ;
	
	FISH_STATUS_CHECK_FIELD_X += frameRect.left; 
	 
	double scale = 1;
	if (system_display_zoom != 1)
	{
		scale = (double)windowOrignalWidth / windowOrignalHeight / ((double)afterZoomWindowWidth / afterZoomWindowHeight);
	} 
	FISH_STATUS_CHECK_FIELD_y = (FISH_STATUS_CHECK_FIELD_y+frameRect.top)*scale;
    
	FISH_STATUS_CHECK_FIELD_WIDTH = (frameRect.right - frameRect.left) *20/ 100; 
	FISH_STATUS_CHECK_FIELD_HEIGHT = (frameRect.bottom - frameRect.top)*scale *5/100; 
}

void SysConfig::getSystemDisplayZoom()
{
	RECT rect;
	HWND hWndDeskTop = ::GetDesktopWindow();
	GetWindowRect(hWndDeskTop, &rect);
	int srcxsize = GetSystemMetrics(SM_CXFULLSCREEN);//获取屏幕设备尺寸信息
	int srcysize = GetSystemMetrics(SM_CYFULLSCREEN);
	//cvMatchTemplateTest();
	DEVMODE curDevMode;
	memset(&curDevMode, 0, sizeof(curDevMode));
	curDevMode.dmSize = sizeof(DEVMODE);
	BOOL bEnumRet = ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMode);
	if (bEnumRet)
	{
		system_display_zoom = (double)(rect.right - rect.left)/curDevMode.dmPelsWidth; 
		system_display_zoom = MathUtil::Round(system_display_zoom, 1);
		system_display_zoom_y= (double)(rect.bottom - rect.top) / curDevMode.dmPelsHeight;
		//system_display_zoom_y= MathUtil::Round(system_display_zoom_y, 1);
		windowOrignalWidth = curDevMode.dmPelsWidth;
		windowOrignalHeight = curDevMode.dmPelsHeight; 

		afterZoomWindowWidth = srcxsize;
		afterZoomWindowHeight = srcysize;
	}
	DeleteObject(hWndDeskTop);
}

bool SysConfig::init()
{ 
	
	
	return true;

}

bool SysConfig::chooseImgID()
{   
	std::stringstream strStream;   

	int minAbsIndex = 0;
	int minAbs = INT_MAX;
	int index = 0;
	for (index = 0; index < sizeof(screenSize) / sizeof(screenSize[0]); index++)
	{
		if (abs(windowOrignalWidth - screenSize[index][0])+ abs(windowOrignalHeight - screenSize[index][1])< minAbs)
		{
			minAbsIndex = index;
			minAbs = abs(windowOrignalWidth - screenSize[index][0]) + abs(windowOrignalHeight - screenSize[index][1]);
		}
	} 
	
	strStream << workPath << "./config\\imgID\\" << system_display_zoom<<"\\" << screenSize[minAbsIndex][0] << "_" << screenSize[minAbsIndex][1] << "\\";
	std::string path = strStream.str();
	
	strStream.str("");
	strStream << path << "fishStatusStd.bmp";
	FISHING_STATUS_IMGID_FILE = strStream.str();

	strStream.str("");
	strStream << path << "fishStatusStd2.bmp";
	FISHING_STATUS2_IMGID_FILE = strStream.str();

	strStream.str("");
	strStream << path << "fishStatusStd3.bmp";
	FISHING_STATUS3_IMGID_FILE = strStream.str();

	strStream.str("");
	strStream << path << "fishStatusStd4.bmp";
	FISHING_STATUS4_IMGID_FILE = strStream.str();

	strStream.str("");
	strStream << path << "fishStatusStd5.bmp";
	FISHING_STATUS5_IMGID_FILE = strStream.str();

	strStream.str("");
	strStream.clear();

	if (system_display_zoom == 1)
	{
		maxWindowWidht = maxWindowSize_1_0[minAbsIndex][0];
		maxWindowHeight = maxWindowSize_1_0[minAbsIndex][1];
	}
	else if (system_display_zoom == 0.8)
	{
	     maxWindowWidht = maxWindowSize_0_8[minAbsIndex][0];
		 maxWindowHeight = maxWindowSize_0_8[minAbsIndex][1];
		//maxWindowWidht = maxWindowSize_1_0[minAbsIndex][0]* system_display_zoom;
		//maxWindowHeight = maxWindowSize_1_0[minAbsIndex][1]* system_display_zoom;
	}

	return true;

}

bool SysConfig::wowWindowChanged()
{
	if ((wowLastWindowRect.right - wowLastWindowRect.left) != (wowWindowRect.right - wowWindowRect.left) || last_system_display_zoom !=system_display_zoom)
	{
		return true;
	}
	return false;

}
bool SysConfig::synWowWindowInfo()
{
	wowLastWindowRect = wowWindowRect;
	last_system_display_zoom = system_display_zoom;
	return true;
}

void SysConfig::releaseFishStatusTemplate()
{
	cvReleaseImage(&resizedFishStatusTemplate);
	resizedFishStatusTemplate = NULL;

	cvReleaseImage(&resizedFishStatusTemplate2);
	resizedFishStatusTemplate2 = NULL;

	cvReleaseImage(&resizedFishStatusTemplate3);
	resizedFishStatusTemplate3 = NULL; 

	cvReleaseImage(&resizedFishStatusTemplate4);
	resizedFishStatusTemplate4= NULL;

	cvReleaseImage(&resizedFishStatusTemplate5);
	resizedFishStatusTemplate5 = NULL;
}


void SysConfig::checkIsActive()
{
	isActive=Security::isActive();  
}


void SysConfig::saveSysconfig()
{ 
	//---------------------CURSOR------------------------------
	//cursorMoveSpeedsliderAccuracy;
	FileUtil::writeIniFile("CURSOR", "cursorMoveSpeedsliderAccuracy", to_string(cursorMoveSpeedsliderAccuracy), configFilePath);
	//----------------screenFishStatus-------------------
	//checkScreenFishStatusBoundaryValue;
	FileUtil::writeIniFile("SCREENFISHSTATUS", "checkScreenFishStatusBoundaryValue", to_string(checkScreenFishStatusBoundaryValue), configFilePath);
	//checkScreenFishStatusXOffset
	FileUtil::writeIniFile("SCREENFISHSTATUS","checkScreenFishStatusXOffset", to_string(checkScreenFishStatusXOffset), configFilePath);
	//checkScreenFishStatusYOffset
	FileUtil::writeIniFile("SCREENFISHSTATUS", "checkScreenFishStatusYOffset", to_string(checkScreenFishStatusYOffset), configFilePath);

	//----------------------soundRecognize------------------------
	//fishSoundMatchAccuracy;
	FileUtil::writeIniFile("SOUNDRECOGNIZE", "fishSoundMatchAccuracy", to_string(fishSoundMatchAccuracy), configFilePath);

	//---------------------------log----------------------------
	//detailLogEnable;
	FileUtil::writeIniFile("LOG", "detailLogEnable", to_string(detailLogEnable), configFilePath);
	//FINISH_FISH_DELAY
	FileUtil::writeIniFile("LOG", "FINISH_FISH_DELAY", to_string(FINISH_FISH_DELAY), configFilePath);
	//otherWowName
	 FileUtil::writeIniFile("WOW", "otherWowName", otherWowName, configFilePath);
	 //isLeftMouseBtnPickup
	 FileUtil::writeIniFile("WOW", "isLeftMouseBtnPickup", to_string(isLeftMouseBtnPickup), configFilePath);
	 //FISHING_CURSOR_IMGID_FILE_NAME
	 FileUtil::writeIniFile("WOW", "FISHING_CURSOR_IMGID_FILE_NAME", FISHING_CURSOR_IMGID_FILE_NAME, configFilePath);


	 //Timer

	 //定时器1
	 //bool timerThread1_Enable = false;
	 FileUtil::writeIniFile("TIMER", "timerThread1_Enable", to_string(timerThread1_Enable), configFilePath);
	// string timerThread1_keyName;
	 FileUtil::writeIniFile("TIMER", "timerThread1_keyName", timerThread1_keyName, configFilePath);
	 //int timerThread1_intervalSeconds = 0;
	 FileUtil::writeIniFile("TIMER", "timerThread1_intervalSeconds", to_string(timerThread1_intervalSeconds), configFilePath);
	 //int timerThread1_pauseTime = 0;
	 FileUtil::writeIniFile("TIMER", "timerThread1_pauseTime", to_string(timerThread1_pauseTime), configFilePath);
	 //定时器2 
	 //bool timerThread2_Enable = false;
	 FileUtil::writeIniFile("TIMER", "timerThread2_Enable", to_string(timerThread2_Enable), configFilePath);
	 // string timerThread2_keyName;
	 FileUtil::writeIniFile("TIMER", "timerThread2_keyName", timerThread2_keyName, configFilePath);
	 //int timerThread2_intervalSeconds = 0;
	 FileUtil::writeIniFile("TIMER", "timerThread2_intervalSeconds", to_string(timerThread2_intervalSeconds), configFilePath);
	 //int timerThread2_pauseTime = 0;
	 FileUtil::writeIniFile("TIMER", "timerThread2_pauseTime", to_string(timerThread2_pauseTime), configFilePath);

	if (debugMode)
	{
		
		//saveCursorImg;
		FileUtil::writeIniFile("CURSOR", "saveCursorImg", to_string(saveCursorImg), configFilePath);
		//saveMatchTemplateCursorFishingFloatImg;
		FileUtil::writeIniFile("CURSOR", "saveMatchTemplateCursorFishingFloatImg", to_string(saveMatchTemplateCursorFishingFloatImg), configFilePath);

		
		//saveCheckFishStatusImg;
		FileUtil::writeIniFile("SCREENFISHSTATUS", "saveCheckFishStatusImg", to_string(saveCheckFishStatusImg), configFilePath);
		//saveMatchTemplateFishStatusImg;
		FileUtil::writeIniFile("SCREENFISHSTATUS", "saveMatchTemplateFishStatusImg", to_string(saveMatchTemplateFishStatusImg), configFilePath);

		//-----------------screenFishingfloat -----------------------
		//saveFishingFloatCheckAreaImg;
		FileUtil::writeIniFile("SCREENFISHINGFLOAT", "saveFishingFloatCheckAreaImg", to_string(saveFishingFloatCheckAreaImg), configFilePath);
		//saveMatchTemplateFishingFloatImg;
		FileUtil::writeIniFile("SCREENFISHINGFLOAT", "saveMatchTemplateFishingFloatImg", to_string(saveMatchTemplateFishingFloatImg), configFilePath);

		
		
		//isDisplayMatchTemplateCheckFishStatusLog; 
		FileUtil::writeIniFile("LOG", "isDisplayMatchTemplateCheckFishStatusLog", to_string(isDisplayMatchTemplateCheckFishStatusLog), configFilePath);
		//isDisplayMatchTemplateCursorLog;
		FileUtil::writeIniFile("LOG", "isDisplayMatchTemplateCursorLog", to_string(isDisplayMatchTemplateCursorLog), configFilePath);
		//isDisplayWebTime
		FileUtil::writeIniFile("LOG", "isDisplayWebTime", to_string(isDisplayWebTime), configFilePath);
	}

	return;

}
void SysConfig::loadSysconfig()
{ 
	//---------------------CURSOR------------------------------
	//cursorMoveSpeedsliderAccuracy;
	cursorMoveSpeedsliderAccuracy = FileUtil::readIniFileInt("CURSOR", "cursorMoveSpeedsliderAccuracy", to_string(cursorMoveSpeedsliderAccuracy), configFilePath);

	//----------------screenFishStatus-------------------
	//checkScreenFishStatusBoundaryValue;
	checkScreenFishStatusBoundaryValue = FileUtil::readIniFileDouble("SCREENFISHSTATUS", "checkScreenFishStatusBoundaryValue", to_string(checkScreenFishStatusBoundaryValue), configFilePath);
	//checkScreenFishStatusXOffset
	checkScreenFishStatusXOffset = FileUtil::readIniFileInt("SCREENFISHSTATUS","checkScreenFishStatusXOffset", to_string(checkScreenFishStatusXOffset), configFilePath);
	//checkScreenFishStatusYOffset
	checkScreenFishStatusYOffset = FileUtil::readIniFileInt("SCREENFISHSTATUS", "checkScreenFishStatusYOffset", to_string(checkScreenFishStatusYOffset), configFilePath);


	//----------------------soundRecognize------------------------
	//fishSoundMatchAccuracy;
	fishSoundMatchAccuracy=FileUtil::readIniFileInt("SOUNDRECOGNIZE", "fishSoundMatchAccuracy", to_string(fishSoundMatchAccuracy), configFilePath);
	//---------------------------log----------------------------
	//detailLogEnable;
	detailLogEnable=FileUtil::readIniFileBool("LOG", "detailLogEnable", detailLogEnable, configFilePath);
	//FINISH_FISH_DELAY
	FINISH_FISH_DELAY = FileUtil::readIniFileInt("LOG", "FINISH_FISH_DELAY", to_string(FINISH_FISH_DELAY), configFilePath);
	//otherWowName
	otherWowName = FileUtil::readIniFileString("WOW","otherWowName", otherWowName, configFilePath); 
	//isLeftMouseBtnPickup
	isLeftMouseBtnPickup=FileUtil::readIniFileBool("WOW", "isLeftMouseBtnPickup", isLeftMouseBtnPickup, configFilePath);
	//FISHING_CURSOR_IMGID_FILE_NAME
	FISHING_CURSOR_IMGID_FILE_NAME = FileUtil::readIniFileString("WOW", "FISHING_CURSOR_IMGID_FILE_NAME", FISHING_CURSOR_IMGID_FILE_NAME, configFilePath);
	
	//----------------------------------TIMER---------------------------------------------------------
	//定时器1
	//bool timerThread1_Enable = false;
	timerThread1_Enable=FileUtil::readIniFileBool("TIMER", "timerThread1_Enable", timerThread1_Enable, configFilePath);
	// string timerThread1_keyName;
	timerThread1_keyName=FileUtil::readIniFileString("TIMER", "timerThread1_keyName", timerThread1_keyName, configFilePath);
	//int timerThread1_intervalSeconds = 0;
	timerThread1_intervalSeconds=FileUtil::readIniFileInt("TIMER", "timerThread1_intervalSeconds", to_string(timerThread1_intervalSeconds), configFilePath);
	//int timerThread1_pauseTime = 0;
	timerThread1_pauseTime=FileUtil::readIniFileInt("TIMER", "timerThread1_pauseTime", to_string(timerThread1_pauseTime), configFilePath);
	//定时器2 
	//bool timerThread2_Enable = false;
	timerThread2_Enable=FileUtil::readIniFileBool("TIMER", "timerThread2_Enable", timerThread2_Enable, configFilePath);
	// string timerThread2_keyName;
	timerThread2_keyName=FileUtil::readIniFileString("TIMER", "timerThread2_keyName", timerThread2_keyName, configFilePath);
	//int timerThread2_intervalSeconds = 0;
	timerThread2_intervalSeconds=FileUtil::readIniFileInt("TIMER", "timerThread2_intervalSeconds", to_string(timerThread2_intervalSeconds), configFilePath);
	//int timerThread2_pauseTime = 0;
	timerThread2_pauseTime=FileUtil::readIniFileInt("TIMER", "timerThread2_pauseTime", to_string(timerThread2_pauseTime), configFilePath);
	
	if (debugMode)
	{ 
		
		//saveCursorImg;
		saveCursorImg = FileUtil::readIniFileBool("CURSOR", "saveCursorImg", saveCursorImg, configFilePath);
		//saveMatchTemplateCursorFishingFloatImg;
		saveMatchTemplateCursorFishingFloatImg = FileUtil::readIniFileBool("CURSOR", "saveMatchTemplateCursorFishingFloatImg", saveMatchTemplateCursorFishingFloatImg, configFilePath);

		
		//saveCheckFishStatusImg;
		saveCheckFishStatusImg = FileUtil::readIniFileBool("SCREENFISHSTATUS", "saveCheckFishStatusImg", saveCheckFishStatusImg, configFilePath);
		//saveMatchTemplateFishStatusImg;
		saveMatchTemplateFishStatusImg = FileUtil::readIniFileBool("SCREENFISHSTATUS", "saveMatchTemplateFishStatusImg", saveMatchTemplateFishStatusImg, configFilePath);

		//-----------------screenFishingfloat -----------------------
		//saveFishingFloatCheckAreaImg;
		saveFishingFloatCheckAreaImg = FileUtil::readIniFileBool("SCREENFISHINGFLOAT", "saveFishingFloatCheckAreaImg", saveFishingFloatCheckAreaImg, configFilePath);
		//saveMatchTemplateFishingFloatImg;
		saveMatchTemplateFishingFloatImg = FileUtil::readIniFileBool("SCREENFISHINGFLOAT", "saveMatchTemplateFishingFloatImg", saveFishingFloatCheckAreaImg, configFilePath);

		//isDisplayMatchTemplateCheckFishStatusLog; 
		isDisplayMatchTemplateCheckFishStatusLog = FileUtil::readIniFileBool("LOG", "isDisplayMatchTemplateCheckFishStatusLog", isDisplayMatchTemplateCheckFishStatusLog, configFilePath);
		//isDisplayMatchTemplateCursorLog;
		isDisplayMatchTemplateCursorLog = FileUtil::readIniFileBool("LOG", "isDisplayMatchTemplateCursorLog", isDisplayMatchTemplateCursorLog, configFilePath);
		//isDisplayWebTime
		isDisplayWebTime = FileUtil::readIniFileBool("LOG", "isDisplayWebTime", isDisplayWebTime, configFilePath);

	}
	  
}

bool SysConfig::isSampleFrequencySupported(DWORD SampleFrequency)
{ 
	for (int index =0; index < sizeof(supportedDeviceFrequency) / sizeof(supportedDeviceFrequency[0]); index++)
	{
		if (SampleFrequency == supportedDeviceFrequency[index])
		{ 
			return true;
		}
	}
	return false;
}
string SysConfig::getStdGMMFilePath(DWORD sampleFrequency)
{
	DWORD dstSampleFrequency;
	
	if (!isSampleFrequencySupported(sampleFrequency))
	{
		DWORD absMin=INT_MAX;
		DWORD absMinIndex = -1;
		for (int i = 0; i < sizeof(supportedDeviceFrequency) / sizeof(supportedDeviceFrequency[0]); i++)
		{
			if (abs((long)sampleFrequency - (long)supportedDeviceFrequency[i]) < absMin)
			{
				absMin = abs((long)sampleFrequency - (long)supportedDeviceFrequency[i]);
				absMinIndex = i;
			}
		}
		dstSampleFrequency = supportedDeviceFrequency[absMinIndex];

	}
	else
	{
		dstSampleFrequency = sampleFrequency;
	}

	switch (dstSampleFrequency)
	{
	case 48000:
		return STD_GMM_48k_FILE;
	case 44100:
		return STD_GMM_44k_FILE;
	default:
		return STD_GMM_48k_FILE;
	} 
}

 