#include "stdafx.h"
#include "setupDlg.h"
#include "resource.h" 
#include <Commctrl.h> 
#include "dlgHelper.h"
#include "logger.h"

extern SysConfig sys;
bool initSetupDlg(HWND hwnd);
bool initTimerThreadBtnGroup(HWND hwnd);
bool test = true;
INT_PTR CALLBACK setupDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_INITDIALOG:
		initSetupDlg(hwnd);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{ 
		case ID_SETUP_CANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		case SC_CLOSE:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;

		case IDC_CHECK_DETAIL_LOG_ENABLE:
			sys.detailLogEnable = IsDlgButtonChecked(hwnd, IDC_CHECK_DETAIL_LOG_ENABLE) == BST_CHECKED ? true : false;
			if (sys.detailLogEnable)
			{
				Logger::setGlogLevel(0);
			}
			else
			{
				Logger::setGlogLevel(2);
			}
			break;
		case IDC_RADIO_MOUSE_LEFT_BUTTON_PICKUP:
			 
			sys.isLeftMouseBtnPickup= IsDlgButtonChecked(hwnd, IDC_RADIO_MOUSE_LEFT_BUTTON_PICKUP) == BST_CHECKED ? true : false; 
			break;
		case IDC_RADIO_MOUSE_RIGHT_BUTTON_PICKUP: 
			sys.isLeftMouseBtnPickup = IsDlgButtonChecked(hwnd, IDC_RADIO_MOUSE_RIGHT_BUTTON_PICKUP) == BST_CHECKED ? false : true;
			break;
		case IDC_RADIO_CURSOR_SHAP_STD:
			sys.FISHING_CURSOR_IMGID_FILE_NAME = sys.fishCursorStdImgId;
			break;
		case IDC_RADIO_CURSOR_SHAP_GEAR:
			sys.FISHING_CURSOR_IMGID_FILE_NAME = sys.fishCursorGearImgId;
			break;
		case IDC_CHECK_TIME_THREAD1_ENABLE:
			sys.timerThread1_Enable = IsDlgButtonChecked(hwnd, IDC_CHECK_TIME_THREAD1_ENABLE) == BST_CHECKED ? true : false;
			break;
		case IDC_EDIT_TIME_THREAD1_KEY_NAME:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				sys.timerThread1_keyName = DlgHelper::onStringEditControlChanged(hwnd, IDC_EDIT_TIME_THREAD1_KEY_NAME);
			} 
			break;
		case IDC_EDIT_TIME_THREAD1_INTERVEL_TIME:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				sys.timerThread1_intervalSeconds = DlgHelper::onIntEditControlChanged(hwnd, IDC_EDIT_TIME_THREAD1_INTERVEL_TIME);
			} 
			break;
		case IDC_EDIT_TIME_THREAD1_PAUSE_TIME:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				sys.timerThread1_pauseTime = DlgHelper::onIntEditControlChanged(hwnd, IDC_EDIT_TIME_THREAD1_PAUSE_TIME);
			} 
			break;   


		case IDC_CHECK_TIME_THREAD2_ENABLE:
			sys.timerThread2_Enable = IsDlgButtonChecked(hwnd, IDC_CHECK_TIME_THREAD2_ENABLE) == BST_CHECKED ? true : false;
			break;
		case IDC_EDITK_TIME_THREAD2_KEY_NAME:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				sys.timerThread2_keyName = DlgHelper::onStringEditControlChanged(hwnd, IDC_EDITK_TIME_THREAD2_KEY_NAME);
			}
			break;
		case IDC_EDITK_TIME_THREAD2_INTERVEL_TIME:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				sys.timerThread2_intervalSeconds = DlgHelper::onIntEditControlChanged(hwnd, IDC_EDITK_TIME_THREAD2_INTERVEL_TIME);
			}
			break;
		case IDC_EDITK_TIME_THREAD2_PAUSE_TIME:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				sys.timerThread2_pauseTime = DlgHelper::onIntEditControlChanged(hwnd, IDC_EDITK_TIME_THREAD2_PAUSE_TIME);
			}
			break;

		case IDCANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE; 
		}
		break; 
	case WM_HSCROLL: 
		  sys.cursorMoveSpeedsliderAccuracy = (int)SendMessageW(hWndCursorSpeedSlider, TBM_GETPOS, 0, 0); //value即是当前控件的值 
	      sys.fishSoundMatchAccuracy = 10 - (int)SendMessageW(hWndFishSoundMatchAccuracySlider, TBM_GETPOS, 0, 0); //value即是当前控件的值 
		  sys.checkScreenFishStatusBoundaryValue = (double)SendMessageW(hWndScreenFishStatusCheckAccuracySlider, TBM_GETPOS, 0, 0) / 100; //value即是当前控件的值 
		  DlgHelper::refreshDlgItemText(hwnd, STATIC_SOUND_RECOGNIZE_REFRESH_VALUE);
		  DlgHelper::refreshDlgItemText(hwnd, STATIC_CURSOR_MOVE_SPEED_REFRESH_VALUE);
		  DlgHelper::refreshDlgItemText(hwnd, STATIC_CHECK_SCREEN_FISH_STATUS_REFRESH_VALUE);
		  break; 
	}

	return FALSE;



}

bool initSetupDlg(HWND hwnd)
{
	hWndCursorSpeedSlider = GetDlgItem(hwnd, IDC_SLIDER_CURSOR_FISH_FLOAT_SPEED);
	DlgHelper::initSliderContrl(hWndCursorSpeedSlider, 0, sys.cursorMoveSpeedsMaxliderAccuracy, sys.cursorMoveSpeedsliderAccuracy);
	hWndFishSoundMatchAccuracySlider = GetDlgItem(hwnd, IDC_SLIDER_FISH_SOUND_MATCH_ACCURACY);
	DlgHelper::initSliderContrl(hWndFishSoundMatchAccuracySlider, 1, 10, 10 - sys.fishSoundMatchAccuracy);
	hWndScreenFishStatusCheckAccuracySlider = GetDlgItem(hwnd, IDC_SLIDER_SCREEN_FISH_STATUS_CHECK_ACCURACY);
	DlgHelper::initSliderContrl(hWndScreenFishStatusCheckAccuracySlider, 0, 100, sys.checkScreenFishStatusBoundaryValue * 100);
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_DETAIL_LOG_ENABLE, sys.detailLogEnable);

	DlgHelper::refreshDlgItemText(hwnd, STATIC_SOUND_RECOGNIZE_REFRESH_VALUE);
	DlgHelper::refreshDlgItemText(hwnd, STATIC_CURSOR_MOVE_SPEED_REFRESH_VALUE);
	DlgHelper::refreshDlgItemText(hwnd, STATIC_CHECK_SCREEN_FISH_STATUS_REFRESH_VALUE);

	DlgHelper::initCheckBox(hwnd, IDC_RADIO_MOUSE_LEFT_BUTTON_PICKUP, sys.isLeftMouseBtnPickup);
	DlgHelper::initCheckBox(hwnd, IDC_RADIO_MOUSE_RIGHT_BUTTON_PICKUP, !sys.isLeftMouseBtnPickup);

	
	DlgHelper::initCheckBox(hwnd, IDC_RADIO_CURSOR_SHAP_STD, sys.FISHING_CURSOR_IMGID_FILE_NAME== sys.fishCursorStdImgId);
	DlgHelper::initCheckBox(hwnd, IDC_RADIO_CURSOR_SHAP_GEAR, sys.FISHING_CURSOR_IMGID_FILE_NAME == sys.fishCursorGearImgId);
	

	HANDLE hBitmap;
	hBitmap = LoadImageA(hInst,string(sys.FISHING_CURSOR_IMGID_FILE_PATH).append(sys.fishCursorStdImgId).c_str(), IMAGE_BITMAP, 20,20, LR_LOADFROMFILE);
	HWND stdCursorPic = GetDlgItem(hwnd, PIC_CURSOR_SHAP_RADIO_BTN_STD);
	SendMessage(stdCursorPic, STM_SETIMAGE, IMAGE_BITMAP, LPARAM(hBitmap));
	 
	hBitmap = LoadImageA(hInst, string(sys.FISHING_CURSOR_IMGID_FILE_PATH).append(sys.fishCursorGearImgId).c_str(), IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);
	HWND gearCursorPic = GetDlgItem(hwnd, PIC_CURSOR_SHAP_RADIO_BTN_GEAR);
	SendMessage(gearCursorPic, STM_SETIMAGE, IMAGE_BITMAP, LPARAM(hBitmap));

	initTimerThreadBtnGroup(hwnd);

	return true;
}

bool initTimerThreadBtnGroup(HWND hwnd)
{

	//定时器1
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_TIME_THREAD1_ENABLE, sys.timerThread1_Enable); 
	SetDlgItemTextA(hwnd, IDC_EDIT_TIME_THREAD1_KEY_NAME, sys.timerThread1_keyName.c_str());
	SetDlgItemTextA(hwnd, IDC_EDIT_TIME_THREAD1_INTERVEL_TIME, to_string(sys.timerThread1_intervalSeconds).c_str());
	SetDlgItemTextA(hwnd, IDC_EDIT_TIME_THREAD1_PAUSE_TIME, to_string(sys.timerThread1_pauseTime).c_str()); 

	//定时器2  
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_TIME_THREAD2_ENABLE, sys.timerThread2_Enable);
	SetDlgItemTextA(hwnd, IDC_EDITK_TIME_THREAD2_KEY_NAME, sys.timerThread2_keyName.c_str());
	SetDlgItemTextA(hwnd, IDC_EDITK_TIME_THREAD2_INTERVEL_TIME, to_string(sys.timerThread2_intervalSeconds).c_str());
	SetDlgItemTextA(hwnd, IDC_EDITK_TIME_THREAD2_PAUSE_TIME, to_string(sys.timerThread2_pauseTime).c_str());
	return true;
}

