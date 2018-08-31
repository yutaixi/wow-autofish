#include "stdafx.h"
#include "superadminDlg.h"
#include "resource.h"
#include "sysconfig.h"
#include <Commctrl.h>
#include "AudioRecognize.h"
#include "voiceprint.h"
#include "dlgHelper.h"

extern SysConfig sys; 
 
void initSuperadminDlg(HWND hwnd);

void onGetVoiceIDButton(HWND hwnd);
void onIdentifyButton(HWND hwnd);
void onExportButton(HWND hwnd);


BOOL CALLBACK superadminDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_INITDIALOG:
		initSuperadminDlg(hwnd);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_SUPERADMIN_OK:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		case IDC_SUPERADMIN_CANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		case IDC_CHECK_SAVE_CURSOR_IMG:
			sys.saveCursorImg = IsDlgButtonChecked(hwnd, IDC_CHECK_SAVE_CURSOR_IMG) == BST_CHECKED ? true : false;
			break;
		case IDC_CHECK_SAVE_FISH_STATUS_AREA_IMG:
			sys.saveCheckFishStatusImg = IsDlgButtonChecked(hwnd, IDC_CHECK_SAVE_FISH_STATUS_AREA_IMG) == BST_CHECKED ? true : false;
			break;
		case IDC_CHECK_SAVE_FISHING_FLOAT_AREA_IMG:
			sys.saveFishingFloatCheckAreaImg = IsDlgButtonChecked(hwnd, IDC_CHECK_SAVE_FISHING_FLOAT_AREA_IMG) == BST_CHECKED ? true : false;
			break;
		case IDC_CHECK_SAVE_MATCH_TEMPLATE_FISHING_FLOAT:
			sys.saveMatchTemplateFishingFloatImg = IsDlgButtonChecked(hwnd, IDC_CHECK_SAVE_MATCH_TEMPLATE_FISHING_FLOAT) == BST_CHECKED ? true : false;
			break;
		case IDC_CHECK_SAVE_MATCH_TEMPLATE_FISH_STATUS:
			sys.saveMatchTemplateFishStatusImg = IsDlgButtonChecked(hwnd, IDC_CHECK_SAVE_MATCH_TEMPLATE_FISH_STATUS) == BST_CHECKED ? true : false;
			break;
		case IDC_SLIDER_FISH_STATUS_ACCURACY:
			sys.checkScreenFishStatusBoundaryValue = (double)SendMessageW(GetDlgItem(hwnd, IDC_SLIDER_FISH_STATUS_ACCURACY), TBM_GETPOS, 0, 0)/100; //value即是当前控件的值 
			break;
		case IDC_CHECK_SAVE_MATCH_TEMPLATE_CURSOR:
			sys.saveMatchTemplateCursorFishingFloatImg= IsDlgButtonChecked(hwnd, IDC_CHECK_SAVE_MATCH_TEMPLATE_CURSOR) == BST_CHECKED ? true : false;
			break;
		case IDC_CHECK_DISPLAY_MATCH_TEMPLAE_SCREEN_FISH_STATUS:
			sys.isDisplayMatchTemplateCheckFishStatusLog= IsDlgButtonChecked(hwnd, IDC_CHECK_DISPLAY_MATCH_TEMPLAE_SCREEN_FISH_STATUS) == BST_CHECKED ? true : false;
			break;

		case IDC_CHECK_DISPLAY_MATCH_TEMPLATE_CURSOR:
			sys.isDisplayMatchTemplateCursorLog = IsDlgButtonChecked(hwnd, IDC_CHECK_DISPLAY_MATCH_TEMPLATE_CURSOR) == BST_CHECKED ? true : false;
			break;
		case IDC_CHECK_DISPALY_WEB_TIME_LOG:
			sys.isDisplayWebTime= IsDlgButtonChecked(hwnd, IDC_CHECK_DISPALY_WEB_TIME_LOG) == BST_CHECKED ? true : false;
			break;
		case IDC_BUTTON_GET_VOICE_ID:
			onGetVoiceIDButton(hwnd);
			break; 
		case IDC_BUTTON_EXPORT_GMM:
			onExportButton(hwnd);
			break;
		case IDC_BUTTON_IDENTIFY:
			onIdentifyButton(hwnd);
			break; 
		case SC_CLOSE:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		case IDCANCEL:
			EndDialog(hwnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break; 
	}

	return FALSE;
	 
}

void initSuperadminDlg(HWND hwnd)
{
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_SAVE_CURSOR_IMG, sys.saveCursorImg); 
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_SAVE_FISH_STATUS_AREA_IMG, sys.saveCheckFishStatusImg); 
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_SAVE_FISHING_FLOAT_AREA_IMG, sys.saveFishingFloatCheckAreaImg); 
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_SAVE_MATCH_TEMPLATE_FISHING_FLOAT, sys.saveMatchTemplateFishingFloatImg); 
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_SAVE_MATCH_TEMPLATE_FISH_STATUS, sys.saveMatchTemplateFishStatusImg); 
    DlgHelper::initCheckBox(hwnd, IDC_CHECK_SAVE_MATCH_TEMPLATE_CURSOR, sys.saveMatchTemplateCursorFishingFloatImg);
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_DISPLAY_MATCH_TEMPLAE_SCREEN_FISH_STATUS, sys.isDisplayMatchTemplateCheckFishStatusLog);
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_DISPLAY_MATCH_TEMPLATE_CURSOR, sys.isDisplayMatchTemplateCursorLog);
	DlgHelper::initCheckBox(hwnd, IDC_CHECK_DISPALY_WEB_TIME_LOG,sys.isDisplayWebTime);
}
 
void onGetVoiceIDButton(HWND hwnd)
{ 
	AudioRecognize::stopVoice(hwnd);
	AudioRecognize::startBuild(hwnd);

}
void onIdentifyButton(HWND hwnd)
{
	AudioRecognize::stopVoice(hwnd);
	AudioRecognize::identify(hwnd);
}

void onExportButton(HWND hwnd)
{
	AudioRecognize::exportGMM(hwnd,sys.EXPORT_GMM_FILE);
	MessageBox(hwnd, TEXT("保存成功！"), szAppVersion, 0);
}