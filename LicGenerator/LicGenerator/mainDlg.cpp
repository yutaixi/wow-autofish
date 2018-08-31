#include "stdafx.h"
#include "mainDlg.h"
#include "generator.h"
#include "resource.h"
#include "cUtil.h"
#include <sstream>

bool onGenerateBtn(HWND hwnd);
BOOL CALLBACK MainDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{ 
	switch (message)
	{
	case WM_INITDIALOG: 
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{ 
		  case IDC_BTN_GENERATE:
			  onGenerateBtn(hwnd);
			break;
		} // switch (LOWORD(wParam))
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

bool onGenerateBtn(HWND hwnd)
{
	WCHAR esn[200];
	WCHAR date[200];
	WCHAR time[200];
	std::stringstream strStream; 

	ZeroMemory(esn,sizeof(esn));
	ZeroMemory(date, sizeof(date));
	ZeroMemory(time, sizeof(time));

	GetDlgItemText(hwnd, IDC_EDIT_ESN, esn, 200); 
	GetDlgItemText(hwnd, IDC_LIC_DATE, date, 200); 
	GetDlgItemText(hwnd, IDC_LIC_TIME, time, 200);

	bool debugMode=IsDlgButtonChecked(hwnd, IDC_CHECK_DEBUG_MODE_ENABLE)==  BST_CHECKED ? true : false;

	string esnStr=CUtil::GetANSIFromUTF16(esn);
	string dateStr = CUtil::GetANSIFromUTF16(date);
	string timeStr = CUtil::GetANSIFromUTF16(time);
	strStream << dateStr << " " << timeStr;
	string expireDate = strStream.str();

	Generator::generateLic(esnStr,expireDate, debugMode,"");

	MessageBox(hwnd,L"创建成功",L"Generater",0);
	return true;
}