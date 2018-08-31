#ifndef __DLG_HELPER__
#define __DLG_HELPER__

class DlgHelper
{
public:
	DlgHelper();
	~DlgHelper();
	static void initSliderContrl(HWND dlg, int min, int max, int default); 
	static void disaleAutoFish();
	static void enableAutoFish();
	static bool refreshDeadLineTime(HWND hwnd);
	static void initCheckBox(HWND dlg, int id, bool checked);
	static bool  refreshCheckScreenFishStatusBoundaryValueItem(HWND hwnd);
	static bool   refreshCheckScreenFishStatusValueItem(HWND hwnd, double value);
	static bool  refreshDlgItemText(HWND hwnd, int item);
	static bool refreshMatchTemplateFishStatusPic(HBITMAP *bitmap);
	static string onStringEditControlChanged(HWND hwnd, int controlId);
	static int onIntEditControlChanged(HWND hwnd, int controlId);
private:

};


#endif // !__DLG_HELPER__

