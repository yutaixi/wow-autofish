#include "stdafx.h" 
#include "opencv2/opencv.hpp" 
#include "hdcMap.h"
#include "logger.h" 
 
extern SysConfig sys;   

HBITMAP  drawCursor(HWND hwnd) { 
	CURSORINFO pci;//定义光标结构体信息
				   //获取结构体大小，这一步必须要。否则后面就无法获取句柄
	pci.cbSize = sizeof(CURSORINFO);
	HDC hdc = GetDC(hwnd);
	if (hdc == NULL)
	{
		DWORD error=GetLastError();
		
			LPVOID lpMsgBuf;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0, // Default language
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
			);

			MessageBox(NULL, (LPCTSTR)lpMsgBuf, L"Error", MB_OK | MB_ICONINFORMATION);
		
		return NULL;
	}
	GetCursorInfo(&pci);//获取光标信息 
	ICONINFO iconinfo;
	GetIconInfo(pci.hCursor, &iconinfo);//主要用于获取光标对应的位图句柄，后面会在屏幕画出。 
	HDC memdc1 = CreateCompatibleDC(hdc);
	HDC memdc2 = CreateCompatibleDC(hdc);


	//创建与屏幕DC兼容的DIB位图，初始化尺寸为屏幕尺寸 
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, 40, 40);
	SelectObject(memdc2, bitmap);
	SelectObject(memdc1, iconinfo.hbmColor);//黑底彩色鼠标

	BitBlt(memdc2, 0, 0, 40, 40, memdc1, 0, 0, SRCPAINT);//将带有鼠标的屏幕位图抓取
	//SaveBitmapToFile(bitmap, "D:\\client\\fishingFloat\\fishingFloat.bmp");
	 //IplImage* iplImage = hBitmapToIpl(bitmap);
	::ReleaseDC(hwnd, hdc);
	hdc = NULL;
	::DeleteDC(memdc2);
	memdc2 = NULL;
	::DeleteDC(memdc1);
	memdc1 = NULL;
	return bitmap; 
}

bool getCurrentCursorBitmap(CURSORINFO *pci, ICONINFO *iconinfo,string fileName)
{
	//CURSORINFO pci;//定义光标结构体信息
	//ICONINFO iconinfo;
	//获取结构体大小，这一步必须要。否则后面就无法获取句柄
	//pci.cbSize = sizeof(CURSORINFO);
	try
	{
		GetCursorInfo(pci);//获取光标信息 
		DWORD error = GetLastError();
		if (pci->hCursor == NULL)
		{
			LOG(ERROR) << "异常：getCurrentCursorBitmap GetCursorInfo pci.hCursor = NULL";
			return false;
		}

		GetIconInfo(pci->hCursor, iconinfo);
		if (iconinfo->hbmColor == NULL)
		{
			LOG(ERROR) << "异常：getCurrentCursorBitmap GetIconInfo iconinfo.hbmColor = NULL";
			return false;
		}
	}
	catch (const std::exception&)
	{
		LOG(ERROR) << "异常：获取鼠标信息时发生异常";
	} 
	if (!fileName.empty())
	{
		SaveBitmapToFile(iconinfo->hbmColor, fileName.append("_").append(to_string(time(NULL))).append(".bmp"));
	}
	//SaveBitmapToFile(iconinfo.hbmColor, "D:\\client\\cursor\\dllCursor2.bmp");
	//::DeleteObject(iconinfo.hbmMask);
	//::DeleteObject(pci.hCursor);
	
	return true;
}


HBITMAP  drawFishHdc() {

	HWND hWndDeskTop = ::GetDesktopWindow();
	HDC hdc = GetDC(hWndDeskTop);
	HDC memdc = CreateCompatibleDC(hdc);//创建兼容屏幕的内存DC 
	int srcxsize = GetSystemMetrics(SM_CXFULLSCREEN);//获取屏幕设备尺寸信息
	int srcysize = GetSystemMetrics(SM_CYFULLSCREEN);

	//HBITMAP  hOldBitmap;
	int px, px2, py, py2;
	px = sys.FISH_STATUS_CHECK_FIELD_X; //原815 1550 * 838 检测起始点x=(x-x*285/1550)/2   
	py = sys.FISH_STATUS_CHECK_FIELD_y;//原775 检测起始点y=y*775/1550
	px2 = sys.FISH_STATUS_CHECK_FIELD_WIDTH;//检测区域宽度x*285/1550 
	py2 = sys.FISH_STATUS_CHECK_FIELD_HEIGHT;//检测高度y*50/838
	int width = px2;
	int heigt = py2;


	//创建与屏幕DC兼容的DIB位图，初始化尺寸为屏幕尺寸
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, heigt);
	 //bitmap = CreateCompatibleBitmap(hdc, width, heigt);
	//将屏幕位图拷贝到兼容内存DC中。
	 SelectObject(memdc, bitmap);


	BOOL bRet;
	BOOL bProcessed = FALSE; 

	if (0 && (px + px2 > srcxsize || py + py2 > srcysize))
	{
		DEVMODE curDevMode;
		memset(&curDevMode, 0, sizeof(curDevMode));
		curDevMode.dmSize = sizeof(DEVMODE);
		BOOL bEnumRet = ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMode);


		if (bEnumRet && srcxsize < curDevMode.dmPelsWidth)
		{
			bProcessed = TRUE;
			::SetStretchBltMode(memdc, STRETCH_HALFTONE);
			bRet = ::StretchBlt(memdc, 0, 0, width, heigt, hdc, px, py, curDevMode.dmPelsWidth,
				curDevMode.dmPelsHeight, SRCCOPY | CAPTUREBLT);
		}
	}

	if (!bProcessed)
	{
		bRet = ::BitBlt(memdc, 0, 0, width, heigt, hdc, px, py, SRCCOPY | CAPTUREBLT);  // CAPTUREBLT - 该参数保证能够截到透明窗口
	}

	//bitmap = (HBITMAP)SelectObject(memdc, hOldBitmap);
	//SaveBitmapToFile(bitmap, RECORD_FISHING_STATUS_FILE);
	//iplImage = hBitmapToIpl(bitmap);
	::ReleaseDC(hWndDeskTop, hdc);
	::DeleteDC(memdc); 
	//::DeleteObject(bitmap); 
	return bitmap;
}

 

bool  drawHdc(HWND hwnd, int px, int py, int width, int height, string fileName, HBITMAP *bitmap) {

	HDC hdc = GetDC(hwnd);
	if (!hdc)
	{
		DWORD error = GetLastError();

		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0, // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL
		);

		MessageBox(NULL, (LPCTSTR)lpMsgBuf, L"Error", MB_OK | MB_ICONINFORMATION);
		LOG(ERROR) << "异常：drawHdc GetDC hdc = NULL";
		return false;
	}
	HDC memdc = CreateCompatibleDC(hdc);//创建兼容屏幕的内存DC 
	HDC memdcFullScreen = CreateCompatibleDC(hdc);//创建兼容屏幕的内存DC 
	if (memdc == NULL|| memdcFullScreen == NULL)
	{
		LOG(ERROR) << "异常：CreateCompatibleDC memdc == NULL|| memdcFullScreen == NULL";
		return false;
	} 
	int srcxsize = GetSystemMetrics(SM_CXFULLSCREEN);//获取屏幕设备尺寸信息
	int srcysize = GetSystemMetrics(SM_CYFULLSCREEN); 
	if (srcxsize < 10 || srcysize < 10)
	{
		LOG(ERROR) << "异常：GetSystemMetrics GetSystemMetrics srcxsize < 10 || srcysize < 10";
		return false;
	}
	HBITMAP  hOldBitmap; 
	//创建与屏幕DC兼容的DIB位图，初始化尺寸为屏幕尺寸
	*bitmap = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP bitmapFullScreen = CreateCompatibleBitmap(hdc, srcxsize, srcysize);
	//将屏幕位图拷贝到兼容内存DC中。
	hOldBitmap = (HBITMAP)SelectObject(memdc, *bitmap);
	SelectObject(memdcFullScreen, bitmapFullScreen);
	BOOL bRet;
	BOOL bProcessed = FALSE;

	//if (0 && (px + width > srcxsize || py + height > srcysize))
	{
		DEVMODE curDevMode;
		memset(&curDevMode, 0, sizeof(curDevMode));
		curDevMode.dmSize = sizeof(DEVMODE);
		BOOL bEnumRet = ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMode);


		if (bEnumRet && srcxsize < curDevMode.dmPelsWidth)
		{
			bProcessed = TRUE;
			::SetStretchBltMode(memdcFullScreen, STRETCH_HALFTONE);
			//bRet = ::StretchBlt(memdc, 0, 0, width, height, hdc, px, py, curDevMode.dmPelsWidth,
			//	curDevMode.dmPelsHeight, SRCCOPY | CAPTUREBLT);
			bRet = ::StretchBlt(memdcFullScreen, 0, 0, srcxsize, srcysize, hdc, 0, 0, curDevMode.dmPelsWidth,
			//bRet = ::StretchBlt(memdcFullScreen, 0, 0, srcxsize, (int)((double)srcxsize/ curDevMode.dmPelsWidth*curDevMode.dmPelsHeight), hdc, 0, 0, curDevMode.dmPelsWidth,
				curDevMode.dmPelsHeight, SRCCOPY | CAPTUREBLT);
		}
	}

	if (!bProcessed)
	{
		bRet = ::BitBlt(memdc, 0, 0, width, height, hdc, px, py, SRCCOPY | CAPTUREBLT);  // CAPTUREBLT - 该参数保证能够截到透明窗口
	}
	else
	{
		bRet = ::BitBlt(memdc, 0, 0, width, height, memdcFullScreen, px, py, SRCCOPY | CAPTUREBLT);  // CAPTUREBLT - 该参数保证能够截到透明窗口
	}

	*bitmap = (HBITMAP)SelectObject(memdc, hOldBitmap);
	if (!fileName.empty()) 
	{
		char temp[MAX_PATH];
		ZeroMemory(temp,sizeof(temp));
		sprintf_s(temp, "%s_zoom_%f_%d_%d_", fileName.c_str(), sys.system_display_zoom,sys.wowWindowRect.right- sys.wowWindowRect.left, sys.wowWindowRect.bottom- sys.wowWindowRect.top);
		 
		SaveBitmapToFile(*bitmap, string(temp).append(to_string(time(NULL))).append(".bmp"));
	} 
	//IplImage* iplImage = hBitmapToIpl(bitmap);
	::ReleaseDC(hwnd, hdc);
	hdc = NULL;
	::DeleteDC(memdcFullScreen);
	memdcFullScreen = NULL;
	::DeleteDC(memdc);
	memdc = NULL;
	::DeleteObject(bitmapFullScreen);
	bitmapFullScreen = NULL;
	//::DeleteObject(bitmap);
	::DeleteObject(hOldBitmap);
	hOldBitmap = NULL;
	
	return true;
}
bool  drawFishingFloatField(bool saveToFile, HBITMAP *bitmap)
{
	//HWND hWndDeskTop = ::GetDesktopWindow();
	return drawHdc(NULL, sys.SEARCH_FISHING_CURSOR_X_MIN, sys.SEARCH_FISHING_CURSOR_Y_MIN,//起始点
		sys.SEARCH_FISHING_CURSOR_X_MAX - sys.SEARCH_FISHING_CURSOR_X_MIN, //宽度
		sys.SEARCH_FISHING_CURSOR_Y_MAX - sys.SEARCH_FISHING_CURSOR_Y_MIN, //高度
		sys.saveFishingFloatCheckAreaImg?sys.RECORD_FISHING_FLOAT_CHECK_AREA_FILE:"", //保存文件地址
		bitmap);
		//RECORD_FISHING_FLOAT_CHECK_AREA_FILE);//保存文件地址

}
bool  drawFishStatusHdc(bool saveToFile, HBITMAP *bitmap) {

	//HWND hWndDeskTop = ::GetDesktopWindow(); 

	return drawHdc(NULL, sys.FISH_STATUS_CHECK_FIELD_X+sys.checkScreenFishStatusXOffset, sys.FISH_STATUS_CHECK_FIELD_y + sys.checkScreenFishStatusYOffset,//起始点
		sys.FISH_STATUS_CHECK_FIELD_WIDTH, //宽度
		sys.FISH_STATUS_CHECK_FIELD_HEIGHT,//高度
		sys.saveCheckFishStatusImg?sys.RECORD_FISHING_STATUS_FILE:"", //保存文件地址
		bitmap);
		//RECORD_FISHING_STATUS_FILE);//保存文件地址
}

int SaveBitmapToFile(HBITMAP hBitmap, string lpFileName)
{
	//lpFileName 为位图文件名  
	HDC hDC;
	//设备描述表  
	int iBits;
	//当前显示分辨率下每个像素所占字节数  
	WORD wBitCount;
	//位图中每个像素所占字节数  
	//定义调色板大小， 位图中像素字节大小 ， 位图文件大小 ， 写入文件字节数  
	DWORD dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;
	BITMAP Bitmap;
	//位图属性结构  
	BITMAPFILEHEADER bmfHdr;
	//位图文件头结构  
	BITMAPINFOHEADER bi;
	//位图信息头结构  
	LPBITMAPINFOHEADER lpbi;
	//指向位图信息头结构  
	HANDLE fh, hDib, hPal;
	HPALETTE hOldPal = NULL;
	//定义文件，分配内存句柄，调色板句柄  

	//计算位图文件每个像素所占字节数  
	hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) *
		GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 32;
	//计算调色板大小  
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

	//设置位图信息头结构  
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	//为位图内容分配内存  
	/*xxxxxxxx计算位图大小分解一下(解释一下上面的语句)xxxxxxxxxxxxxxxxxxxx
	//每个扫描行所占的字节数应该为4的整数倍，具体算法为:
	1int biWidth = (Bitmap.bmWidth*wBitCount) / 32;
	if((Bitmap.bmWidth*wBitCount) % 32)
	biWidth++; //不是整数倍的加1
	biWidth *= 4;//到这里，计算得到的为每个扫描行的字节数。
	dwBmBitsSize = biWidth * Bitmap.bmHeight;//得到大小
	xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/

	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	if (hDib == 0)
	{
		LOG(ERROR) << "异常：SaveBitmapToFile GlobalAlloc hDib=0";
	}
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板  
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值  
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	//恢复调色板  
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	WCHAR fileName[256];
	memset(fileName, 0, sizeof(fileName));
	MultiByteToWideChar(CP_ACP, 0, lpFileName.c_str(), strlen(lpFileName.c_str()) + 1, fileName,
		sizeof(fileName) / sizeof(fileName[0]));

	//创建位图文件  
	//fh = CreateFile(TEXT("D:\\client\\fishstatus\\fishstatus2.bmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	fh = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;
	// 设置位图文件头  
	bmfHdr.bfType = 0x4D42; // "BM"  
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	// 写入位图文件头  
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// 写入位图文件其余内容  
	WriteFile(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize, &dwWritten, NULL);
	//清除  
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}



IplImage* hBitmap2Ipl(HBITMAP hBmp)
{
	BITMAP bmp;

	::GetObject(hBmp, sizeof(BITMAP), &bmp);

	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

	IplImage* img = cvCreateImageHeader(cvSize(bmp.bmWidth, bmp.bmHeight)
		, depth, nChannels);

	img->imageData =
		(char*)malloc(bmp.bmHeight*bmp.bmWidth*nChannels * sizeof(char));
	memcpy(img->imageData, (char*)(bmp.bmBits), bmp.bmHeight*bmp.bmWidth*nChannels);

	return img;
}

IplImage * hBitmapToIpl(HBITMAP hBmp)
{
	BITMAP bmp;

	ZeroMemory(&bmp, sizeof(bmp));
	IplImage* img = NULL;
	BYTE *pBuffer = NULL;
	IplImage *dst = NULL;
	if (!hBmp)
	{
		return NULL;
	}

	try
	{
		GetObject(hBmp, sizeof(BITMAP), &bmp);
		int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
		if (nChannels <= 0 || bmp.bmWidth <= 0 || bmp.bmHeight <= 0)
		{
			return NULL;
		}
		int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

		img = cvCreateImage(cvSize(bmp.bmWidth, bmp.bmHeight), depth, nChannels);
		if (!img)
		{
			return NULL;
		}
		pBuffer = new BYTE[bmp.bmHeight*bmp.bmWidth*nChannels];
		GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, pBuffer);
		memcpy(img->imageData, pBuffer, bmp.bmHeight*bmp.bmWidth*nChannels); 
		if (img->nChannels >= 3)
		{ 
			try
			{
				dst = cvCreateImage(cvGetSize(img), img->depth, 3);
				cvCvtColor(img, dst, CV_BGRA2BGR);
				cvReleaseImage(&img);
			}
			catch (const std::exception&)
			{
				cvReleaseImage(&img);
			}

			return dst;
		}
	}
	catch (...)
	{
		LOG(ERROR) << "hBitmapToIpl ERROR";

		if(img!=NULL)
			cvReleaseImage(&img);
		img = NULL;
		if(dst!=NULL)
			cvReleaseImage(&dst);
		dst = NULL;
		if (pBuffer != NULL)
			delete[] pBuffer;
			pBuffer = NULL;
			return NULL;
	}

	delete[] pBuffer;
	 
	return img;
}