#include "stdafx.h"
#include "helpDlg.h"
#include "resource.h"

//BOOL CALLBACK helpDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
INT_PTR CALLBACK helpDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_HELP_CANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		case IDC_BTN_HELP_TEST:
			return (INT_PTR)TRUE;
		case IDCANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		default:
			break;
		} 
		break;
	}
	return (INT_PTR)FALSE;
	 
}