#include "stdafx.h"
#include "qqContactDlg.h"
#include "resource.h" 

//BOOL CALLBACK qqContactDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
INT_PTR CALLBACK qqContactDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{ 
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_QQ_CANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		case IDCANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		} 
		break;
	}
	return (INT_PTR)FALSE;


}