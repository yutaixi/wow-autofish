//////////////////////////////////////////////////////////////////////////
//Copyright(C) Dake，2010-05
//编程方式：win32 SDK C语言编程
//文件名：MainDlg.c
//描述：
//主要函数：
//版本：V1.1
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"   
#include <avrt.h>
#include <assert.h>
#include "mouseHook.h" 
#include "MainDlg.h"   
#include "AudioConfig.h" 
#include "Monitor.h" 
#include "resource.h"
#include "logger.h"
#include "timeUtil.h"  
#include "kbHook.h" 
#include "wow.h"
#include <Commctrl.h> 
#include "AudioRecognize.h"
#include "qqContactDlg.h"
#include "helpDlg.h"
#include <commdlg.h> 
#include "OpenFileDialog.h"
#include "setupDlg.h"
#include "dlgHelper.h"
#include "superadminDlg.h"
#include "Security.h"
#include "macUtil.h"
#include "CharacterHelper.h"
#include "imgEx.h"

extern Monitor monitor;
extern Wow wow;
extern SysConfig sys;
 
static void OnInitDlg(HWND hwnd); 
static void UpdateDlg(HWND hwnd, LPARAM lParam);
static BOOL OnBuildGMM(HWND hwnd);  
static void OnStopVoice(HWND hwnd);
static void OnRecordBeg(HWND hwnd, LPARAM lParam);
static void OnRecordEnd(HWND hwnd); 
static void OnIdentifyBtn(HWND hwnd); 
static void OnImportGMMBtn(HWND hwnd);
static void DelVoiceFile(void);
void onBtnSetup();
void onBtnStopFish();
void onBtnSuperAdmin();
void onBtnStartFish();
void  DrawWaveIn(LPARAM lParam, HWND hwnd);
bool onActiveBtn(HWND hwnd);
bool onBtnGetESN();
void onScrollScreenFishStatus(WPARAM wParam);
void onScrollScreenFishStatusXOffset(WPARAM wParam);
void onScrollScreenFishStatusYOffset(WPARAM wParam);
void onResetScreenFishStatusOffset();
//声明全局线程变量 
// 全局Hook ID
HHOOK g_HookHwnd = NULL;
LRESULT CALLBACK kbHookFun(int nCode, WPARAM wParam, LPARAM lParam);   
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
 
BOOL CALLBACK MainDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{ 
	case WM_INITDIALOG:
		OnInitDlg(hwnd);
		
		return TRUE;

	case WM_RECORD_BEG:
		OnRecordBeg(hwnd, lParam);
		return TRUE;

	case WM_RECORD_END:
		OnRecordEnd(hwnd);
		return TRUE;
	case WM_BUILD_WAVE:
		//DrawWaveIn(lParam,hwnd);
		return true;
	case WM_RESTART_RECORD:
		OnStopVoice(hwnd);
		DelVoiceFile();
		OnIdentifyBtn(hwnd);
		break;
	case WM_BUILD_GMM:  // 建立GMM及生成识别阈值
		LOG(INFO) << "WM_BUILD_GMM";
						// 先结束录音
		OnStopVoice(hwnd);
		OnBuildGMM(hwnd);
		DelVoiceFile();
		if (wow.start_rec_sys_sound)
		{
			OnIdentifyBtn(hwnd);
		} 
		return TRUE;

	case WM_START_FISH: 
		KBHook::pressKeyboard('1');
		return TRUE; 
	
	case WM_COMMAND: 
		switch (LOWORD(wParam))
		{ 
		case IDC_BUTTON_STOP:
			onBtnStopFish();
			break;
		case  IDC_BUTTON_ACTIVE:
			onActiveBtn(hwnd);//点击激活按钮
			break;
		case IDC_BUTTON_CONTACT_ME:  
			DialogBox(hInst, MAKEINTRESOURCE(DLG_QQCONTACT), hwndMainWin, qqContactDlg);
			break;
		case IDC_BUTTON_HELP:
			DialogBox(hInst, MAKEINTRESOURCE(DLG_HELP_WINDOW), hwndMainWin, helpDlg); 
			break; 
		case IDC_CHECK_SHOW_LOG:
			break;
		case IDC_BUTTON_SETUP:  
			onBtnSetup();
			break;
		case IDC_BUTTON_SUPERADMIN:
			onBtnSuperAdmin();
			break;
		case IDC_START_FISH: 
			onBtnStartFish();
			break;
		case IDC_BTN_GET_ESN:
			onBtnGetESN();
			break; 
		case IDC_BUTTON_RESET_SCREEN_FISH_STATUS_OFFSET:
			onResetScreenFishStatusOffset();
			break;
		case IDCANCEL:    // 关闭对话框
			AudioRecognize::freeVoicePrint();
			monitor.stopMonitor(); 
			sys.releaseFishStatusTemplate();
			QuitMyWindow();
			return TRUE;
		} // switch (LOWORD(wParam))
		break;
	case WM_VSCROLL:
	{
		if ((HWND)lParam == GetDlgItem(hwnd, IDC_SCROLL_SCREEN_FISH_STATUS_BOUNDARY_VALUE))
		{
			onScrollScreenFishStatus(wParam);
		}else if ((HWND)lParam == GetDlgItem(hwnd, IDC_SCROLL_CHECK_SCREEN_FISH_STATUS_Y_OFFSET))
		{
			onScrollScreenFishStatusYOffset(wParam);
		}
	}
		break;
	case WM_HSCROLL:
	{
		if ((HWND)lParam == GetDlgItem(hwnd, IDC_SCROLL_CHECK_SCREEN_FISH_STATUS_X_OFFSET))
		{
			onScrollScreenFishStatusXOffset(wParam);
		}
	}
		break;
	case WM_SYSCOMMAND:
		switch (LOWORD(wParam))
		{
		case SC_CLOSE:
			SendMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
			return TRUE; 
		}
		break;
	}

	return FALSE;
}
 
void onResetScreenFishStatusOffset()
{ 
	sys.checkScreenFishStatusXOffset = 0;
	sys.checkScreenFishStatusYOffset = 0;
	ImgEx::redrawAndRefreshFishStatusPic();
}
//IDC_SCROLL_CHECK_SCREEN_FISH_STATUS_Y_OFFSET

void onScrollScreenFishStatusXOffset(WPARAM wParam)
{
	int nScrollCode = (int)LOWORD(wParam);
	if (nScrollCode == 0)
	{
		sys.checkScreenFishStatusXOffset += -1;
	}
	else if (nScrollCode == 1)
	{
		sys.checkScreenFishStatusXOffset += 1;
	}
	 
	ImgEx::redrawAndRefreshFishStatusPic();

}
void onScrollScreenFishStatusYOffset(WPARAM wParam)
{
	int nScrollCode = (int)LOWORD(wParam);
	if (nScrollCode == 0)
	{
		sys.checkScreenFishStatusYOffset += -1;
	}
	else if (nScrollCode == 1)
	{
		sys.checkScreenFishStatusYOffset += 1;
	}
	ImgEx::redrawAndRefreshFishStatusPic();

}
void onScrollScreenFishStatus(WPARAM wParam)
{
	int nScrollCode = (int)LOWORD(wParam);
	if (nScrollCode == 0)
	{
		sys.checkScreenFishStatusBoundaryValue += 0.01;
	}
	else if (nScrollCode == 1)
	{
		sys.checkScreenFishStatusBoundaryValue -= 0.01;
	}
	DlgHelper::refreshCheckScreenFishStatusBoundaryValueItem(NULL);

}

void onBtnStartFish()
{
	/*
	monitor.monitorInit();
	if (!monitor.refreshWowWindowInfo())
	{
		 
	}

	ImgEx::checkScreenFishStatus();
	*/
	if (!sys.isActive)
	{
		return ;
	}
	if (!monitor.isMonitorRunning)
	{ 
		monitor.startMonitor();
	}
	
	
}
void onBtnStopFish()
{
	if (!sys.isActive)
	{
		return;
	}
	monitor.isMonitorRunning = false;
	Logger::displayLog("助手已关闭\r\n");
	monitor.stopMonitor();
}

void onBtnSetup()
{
	if (!sys.isActive)
	{
		return;
	}
	DialogBox(hInst, MAKEINTRESOURCE(DLG_SETUP_WINDOW), hwndMainWin, setupDlg);

}

void onBtnSuperAdmin()
{
	if (!sys.isActive)
	{
		return;
	}
	DialogBox(hInst, MAKEINTRESOURCE(DLG_SUPERADMIN), hwndMainWin, superadminDlg);
}

static BOOL OnBuildGMM(HWND hwnd)
{
	if (!sys.isActive)
	{
		return FALSE;
	}
	return AudioRecognize::buildGMM(hwnd);
}

 

static void OnInitDlg(HWND hwnd)
{ 
	Logger::initGlog();
	// 初始化文件对话框 
	AudioRecognize::refreshSoundDeviceFrequency();
	AudioRecognize::initVoicePrint();   
	OnImportGMMBtn(hwnd);//导入声纹
	LOG(INFO) << "已导入声纹"; 
	sys.getSystemDisplayZoom();
	LOG(INFO) << "getSystemDisplayZoom";  
	// 安装钩子
	g_HookHwnd = SetWindowsHookEx(WH_KEYBOARD_LL, kbHookFun, hInst, 0);
	// 测试.同上
	assert(g_HookHwnd);
	DlgHelper::refreshDeadLineTime(hwnd);
	DlgHelper::refreshCheckScreenFishStatusBoundaryValueItem(hwnd);

	//SetScrollRange(GetDlgItem(hwnd, IDC_SCROLL_SCREEN_FISH_STATUS_BOUNDARY_VALUE), SB_CTL,1,0,true);
	HANDLE hBitmap;
	hBitmap = LoadImageA(hInst,sys.FISHING_STATUS_IMGID_FILE.c_str(), IMAGE_BITMAP, 150, 20, LR_LOADFROMFILE);
	HWND hPic = GetDlgItem(hwnd, IDC_SOUND_DISPLAY);
	SendMessage(hPic, STM_SETIMAGE, IMAGE_BITMAP, LPARAM(hBitmap));
	//DeleteObject(hBitmap);
	DeleteObject(hPic);
	
	if (!sys.debugMode)
	{
		ShowWindow(GetDlgItem(hwnd, IDC_BUTTON_SUPERADMIN), SW_HIDE);
	}
	monitor.initMqTimer(hwnd);
	


} 
static void UpdateDlg(HWND hwnd, LPARAM lParam)
{
	int m = 6;
	int percent = 19;
	int frame_num = 106;  
	AudioRecognize::updateVoicePrint(percent,m, frame_num,lParam == IDC_IDENTIFY); 
 
}
 
static void OnStopVoice(HWND hwnd)
{
	AudioRecognize::stopVoice(hwnd);
	if (AudioRecognize::hasVoiceId())
	{
		SetWindowText(GetDlgItem(hwnd, IDC_VOICE_STOP), TEXT("身份验证"));
		SetWindowLong(GetDlgItem(hwnd, IDC_VOICE_STOP), GWL_ID, IDC_IDENTIFY);
	}
	else
	{
		SetWindowText(GetDlgItem(hwnd, IDC_VOICE_STOP), TEXT("提取声纹"));
		SetWindowLong(GetDlgItem(hwnd, IDC_VOICE_STOP), GWL_ID, IDC_BUILD_GMM);
	} 
	 
}



static void OnRecordBeg(HWND hwnd, LPARAM lParam)
{
	if (!sys.isActive)
	{
		return;
	}
	if (AudioRecognize::beginRecord(hwnd))
	{ 
		SetWindowText(GetDlgItem(hwnd, lParam), TEXT("停止"));
		SetWindowLong(GetDlgItem(hwnd, lParam), GWL_ID, IDC_VOICE_STOP);
	}
}

static void OnRecordEnd(HWND hwnd)
{
	AudioRecognize::endRecord(WM_RECORD_END); 
}

 
static void OnIdentifyBtn(HWND hwnd)
{
	if (!sys.isActive)
	{
		return;
	}
	AudioRecognize::identify(hwnd);
}
 
static void OnImportGMMBtn(HWND hwnd)
{
	
	AudioRecognize::importGMM(hwnd, sys.soundDevcieFrequency);
}
 
/* 删除临时语音转储文件 */
static void DelVoiceFile(void)
{
	AudioRecognize::deleteVoiceFile();
}
  
 
void  DrawWaveIn(LPARAM lParam, HWND hwnd)
{ 
	
 	char *buf = (char *)lParam;
	HWND picCtr = GetDlgItem(hwnd, IDC_SOUND_DISPLAY);
	HDC m_pDC = GetDC(picCtr);
    RECT  clientRect = {0,0,0,0};  
	int m_beginX = 20;
	int m_beginY = 25; 
	GetWindowRect(picCtr, &clientRect);
	//CRect clientRect;
	//RECT clientRect = { 20, 25, 293, 150 } 
	//GetWindowRect(dicSound,&clientRect);
	//GetDlgItem(IDC_SOUND)->GetWindowRect(&clientRect); 
	//ScreenToClient(dicSound,clientRect);
	//画中轴线
	COLORREF bkColor = RGB(0, 0, 0); 
	SetBkColor(m_pDC, bkColor);
	HBRUSH  brush= CreateSolidBrush(bkColor);
	
	//m_pDC->FillRect(clientRect, &brush);
	FillRect(m_pDC, &clientRect, brush);
	HPEN pen1= CreatePen(0, 1, RGB(0, 255, 0));
	
	//CPen *oldPen1 = m_pDC->SelectObject(&pen1);
	HGDIOBJ pen2=SelectObject(m_pDC, pen1);
	//int size = m_SoundIn.m_BufferSize / 2;
	int size = 480*2 / 2;
	int yy = (clientRect.bottom - clientRect.top) / 2;
	 
	//MoveTo(m_beginX, int(clientRect.Height() / 2 + m_beginY));
	MoveToEx(m_pDC, m_beginX, int((clientRect.bottom - clientRect.top) / 2 + m_beginY),NULL);
	int height = yy;
	int x, y;
	int low, high, data_true;
	short sample;
	for (int register i = 0; i <(long)size; i+=4)	//to draw first channel
	{
		//sample = (((*((short*)buf + i))-128)*h) / (65535 / 2);
		low = *(buf + i);
		high = *(buf + i+1);
		data_true = high * 256 + low;
		if (high / 128 == 1)
		{
			data_true = data_true - 65535;
		}
		sample = (data_true*height) / (65535 / 2);
		//sample = ((*((short*)buf + i)) *h) / (65535 / 2);
		x = int(((float)i / size)*(clientRect.right- clientRect.left));
		y = yy - sample;
		//m_pDC->LineTo(m_beginX + x, y + m_beginY);
		LineTo(m_pDC, m_beginX + x, y + m_beginY);
	} 

	DeleteObject(pen2);
	DeleteObject(brush);
	DeleteObject(pen1);
	ReleaseDC(hwnd,m_pDC); 
	delete(buf);
	 
}
  

// 钩子子程
LRESULT CALLBACK kbHookFun(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!sys.isActive)
	{
		return NULL;
	}
	return KBHook::kbHookFun(nCode, wParam, lParam, g_HookHwnd, &monitor);
}




bool onActiveBtn(HWND hwnd)
{ 
	   
	OpenFileDialog* fileDlg = new OpenFileDialog(); 
	fileDlg->FilterIndex = 1;
	fileDlg->Flags |= OFN_SHOWHELP;
	fileDlg->InitialDir = _T("D:\\");
	fileDlg->Title = _T("激活");
	TCHAR szFilter[] = TEXT("ASCII Files (*.lic)\0*.lic\0") \
		TEXT("All Files (*.*)\0*.*\0\0");
	fileDlg->Filter = szFilter;
	if (fileDlg->ShowDialog())
	{
		//Security::isActive();
		/**/
		if (Security::activeTool(fileDlg->FileName))
		{
			DlgHelper::enableAutoFish();
			DlgHelper::refreshDeadLineTime(hwnd);
			MessageBox(hwnd, TEXT("激活成功！"), szAppVersion, 0);
		}
		else
		{
			MessageBox(hwnd, TEXT("激活失败！"), szAppVersion, 0); 
		}
		
	} 
	 
	return true;

}

bool onBtnGetESN()
{
	//获取设备ESN
	string mac = MacUtil::getAllMac();
	mac = CAPECharacterHelper::connectStr(mac,"\r\n");
	Logger::displayLog((char*)mac.c_str());
	return true;
}