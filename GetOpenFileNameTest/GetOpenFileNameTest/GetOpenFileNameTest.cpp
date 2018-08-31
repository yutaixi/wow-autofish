// GetOpenFileNameTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GetOpenFileNameTest.h"
#include "OpenFileDialog.h"
#include <commdlg.h>
#include "sysconfig.h"
#include "wow.h"
#include "Monitor.h"
#include "global.h"
#include "MainDlg.h"
#include "setupDlg.h"
#include "superadminDlg.h" 
#include "logger.h"
#include "dlgHelper.h"
#include "MQ.h"

SysConfig sys;
Wow wow;
Monitor monitor;
MQ mq;

#define MAX_LOADSTRING 100
   
bool getMessage(MSG *msg);

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GETOPENFILENAMETEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
	//检查软件是否已经激活
	sys.checkIsActive();
	sys.loadSysconfig();
	monitor.startLicExpirationCheckThread();
    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GETOPENFILENAMETEST));

    MSG msg;

    // Main message loop:
    while (getMessage(&msg))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

bool getMessage(MSG *msg)
{
	DWORD error;
	try
	{
		return GetMessage(msg, nullptr, 0, 0);
	}
	catch (...)
	{
		error = GetLastError();
		Logger::displayLog("GetMessage 异常");
	}
	return true;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GETOPENFILENAMETEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GETOPENFILENAMETEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   RECT    rect;
   //  hwndMainWin = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    //  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   hwndMainWin = CreateWindow(szWindowClass, szAppVersion,
	   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX,
	   CW_USEDEFAULT, CW_USEDEFAULT,
	   CW_USEDEFAULT, CW_USEDEFAULT,
	   NULL, NULL, hInstance, NULL);
   if (!hwndMainWin)
   {
      return FALSE;
   }
   //创建主窗口
   /**/
   hwndMainDlg = CreateDialogParam(hInstance, szAppName, hwndMainWin, MainDlg, nCmdShow);


   //创建高级设置窗口
   //setupWindow = CreateDialog(hInstance, TEXT("SETUPWINDOW"), hwndMainWin, setupDlg);
   //hWndCursorSpeedSlider = GetDlgItem(setupWindow, IDC_SLIDER_CURSOR_FISH_FLOAT_SPEED); //m_hWnd为父窗口的句柄
																						//初始化鼠标移动速度
  // DlgHelper::initSliderContrl(hWndCursorSpeedSlider, 0, sys.cursorMoveSpeedsMaxliderAccuracy, sys.cursorMoveSpeedsliderAccuracy);
   //初始化声音匹配灵敏度
   //hWndFishSoundMatchAccuracySlider = GetDlgItem(setupWindow, IDC_SLIDER_FISH_SOUND_MATCH_ACCURACY); //m_hWnd为父窗口的句柄
   //DlgHelper::initSliderContrl(hWndFishSoundMatchAccuracySlider, 1, 10, 10 - sys.fishSoundMatchAccuracy);
   //初始化屏幕钓鱼状态检测灵敏度
   //hWndScreenFishStatusCheckAccuracySlider = GetDlgItem(setupWindow, IDC_SLIDER_SCREEN_FISH_STATUS_CHECK_ACCURACY); //m_hWnd为父窗口的句柄
   //DlgHelper::initSliderContrl(hWndScreenFishStatusCheckAccuracySlider, 0, 100, sys.checkScreenFishStatusBoundaryValue * 100);
   //ShowWindow(setupWindow, SW_HIDE);
    
   //创建炒鸡admin窗口
  // superadminWindow = CreateDialog(hInstance, TEXT("SUPERADMIN"), hwndMainWin, superadminDlg);
   //ShowWindow(superadminWindow, SW_HIDE);
    
   GetWindowRect(hwndMainDlg, &rect);
   AdjustWindowRect(&rect, WS_CAPTION | WS_BORDER, FALSE);

   //SetWindowPos(hwndMainWin, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
   SetWindowPos(hwndMainWin, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
   ShowWindow(hwndMainDlg, SW_SHOW);
   
   ShowWindow(hwndMainWin, nCmdShow);
   UpdateWindow(hwndMainWin);
   if (!sys.isActive)
   {
	   DlgHelper::disaleAutoFish();
   }
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		//CreateControlButtons(hWnd); 
		srand((unsigned int)time(NULL));
		InitializeCriticalSection(&cs);
		sys.init();
	}
	break;
	case WM_SETFOCUS:
		SetFocus(hwndMainDlg);
		return TRUE;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About); 
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteCriticalSection(&cs); // 删除临界区
		PostQuitMessage(0);
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

 

 

