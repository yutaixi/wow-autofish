#include "stdafx.h"
#include "dlgHelper.h"
#include <Commctrl.h>
#include "resource.h" 
#include "global.h"

#include "CharacterHelper.h"

extern SysConfig sys;

DlgHelper::DlgHelper()
{
}

DlgHelper::~DlgHelper()
{
}

void DlgHelper::initSliderContrl(HWND dlg, int min, int max, int default)
{
	SendMessageW(dlg, TBM_SETRANGEMIN, (WPARAM)TRUE, (LPARAM)(int)(min));//设置最小值 
	SendMessageW(dlg, TBM_SETRANGEMAX, (WPARAM)TRUE, (LPARAM)(int)(max));//设置范围最大值
	SendMessageW(dlg, TBM_SETPOS, TRUE, (LPARAM)(int)(default));//设置slider控件  
}

void DlgHelper::initCheckBox(HWND dlg,int id,bool checked)
{
	if (checked)
	{
		CheckDlgButton(dlg, id, BST_CHECKED);
	}
	else
	{
		CheckDlgButton(dlg, id,BST_UNCHECKED );
	}
	
}


void DlgHelper::disaleAutoFish()
{
	EnableWindow(GetDlgItem(hwndMainDlg, IDC_BUTTON_STOP), FALSE);
	EnableWindow(GetDlgItem(hwndMainDlg, IDC_BUTTON_SETUP), FALSE);
	EnableWindow(GetDlgItem(hwndMainDlg, IDC_BUTTON_SUPERADMIN), FALSE);
	EnableWindow(GetDlgItem(hwndMainDlg, IDC_START_FISH), FALSE); 
	if (sys.debugMode)
	{
		ShowWindow(GetDlgItem(hwndMainDlg, IDC_BUTTON_SUPERADMIN), SW_HIDE);
	}
	 
}

void DlgHelper::enableAutoFish()
{
	EnableWindow(GetDlgItem(hwndMainDlg, IDC_BUTTON_STOP), TRUE);
	EnableWindow(GetDlgItem(hwndMainDlg, IDC_BUTTON_SETUP), TRUE);
	EnableWindow(GetDlgItem(hwndMainDlg, IDC_BUTTON_SUPERADMIN), TRUE);
	EnableWindow(GetDlgItem(hwndMainDlg, IDC_START_FISH), TRUE);
	if (sys.debugMode)
	{
		ShowWindow(GetDlgItem(hwndMainDlg, IDC_BUTTON_SUPERADMIN), SW_SHOW);
	}
	else
	{
		ShowWindow(GetDlgItem(hwndMainDlg, IDC_BUTTON_SUPERADMIN), SW_HIDE);
	}

}
bool DlgHelper::refreshDeadLineTime(HWND hwnd)
{
	if (sys.date.empty())
	{
		return false;
	}
	if (hwnd == NULL)
	{
		hwnd = hwndMainDlg;
	}
	WCHAR *dlTime = CAPECharacterHelper::GetUTF16FromANSI(sys.date.c_str());
	SetDlgItemText(hwnd, DEAD_LINE_TIME_CHAR, dlTime); 
	return true;
}

bool  DlgHelper::refreshCheckScreenFishStatusBoundaryValueItem(HWND hwnd)
{ 
	if (hwnd == NULL)
	{
		hwnd = hwndMainDlg;
	} 
	SetDlgItemTextA(hwnd, STATIC_CHECK_SCREEN_FISH_STATUS_STD,to_string((int)(sys.checkScreenFishStatusBoundaryValue*100)).c_str());
	return true;
}
bool  DlgHelper::refreshCheckScreenFishStatusValueItem(HWND hwnd,double value)
{
	if (hwnd == NULL)
	{
		hwnd = hwndMainDlg;
	}
	SetDlgItemTextA(hwnd, STATIC_CHECK_SCREEN_FISH_STATUS_VALUE, to_string((int)(value * 100)).c_str());
	return true;
}

bool  DlgHelper::refreshDlgItemText(HWND hwnd, int item)
{
	if (hwnd == NULL)
	{
		hwnd = hwndMainDlg;
	}
	switch (item)
	{
	case STATIC_CURSOR_MOVE_SPEED_REFRESH_VALUE:
		SetDlgItemTextA(hwnd, STATIC_CURSOR_MOVE_SPEED_REFRESH_VALUE, to_string(sys.cursorMoveSpeedsliderAccuracy).c_str());
		break;
	case STATIC_CHECK_SCREEN_FISH_STATUS_REFRESH_VALUE:
		SetDlgItemTextA(hwnd, STATIC_CHECK_SCREEN_FISH_STATUS_REFRESH_VALUE, to_string((int)(sys.checkScreenFishStatusBoundaryValue * 100)).c_str());
		break;
	case STATIC_SOUND_RECOGNIZE_REFRESH_VALUE:
		SetDlgItemTextA(hwnd, STATIC_SOUND_RECOGNIZE_REFRESH_VALUE, to_string(sys.fishSoundMatchAccuracy).c_str());
		break;
	default:
		break;
	}
	 
	return true;
}
 
bool DlgHelper::refreshMatchTemplateFishStatusPic(HBITMAP *bitmap)
{ 
	HWND hPic = GetDlgItem(hwndMainDlg, IDC_SOUND_DISPLAY);
	SendMessage(hPic, STM_SETIMAGE, IMAGE_BITMAP, LPARAM(*bitmap)); 
	return true;
}

string DlgHelper::onStringEditControlChanged(HWND hwnd, int controlId)
{
	char temp[20];
	GetDlgItemTextA(hwnd, controlId, temp, sizeof(temp) / sizeof(temp[0]));
	return temp;
}

int DlgHelper::onIntEditControlChanged(HWND hwnd, int controlId)
{
	char temp[20];
	GetDlgItemTextA(hwnd, controlId, temp, sizeof(temp) / sizeof(temp[0]));
	return atoi(temp);
}