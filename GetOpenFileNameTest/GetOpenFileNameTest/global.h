#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED 
/** @file
********************************************************************************
<PRE>
模块名: 声纹识别
文件名: global.h
相关文件:
文件实现功能: 全局资源声明及定义
作者: Dake
版本: V0.0.0.0
编程方式: ANSI C语言编程
授权方式: Copyright(C) Dake
联系方式: chen423@yeah.net
生成日期: 2010-11-16
--------------------------------------------------------------------------------
多线程安全性: <是/否>[，说明]
异常时安全性: <是/否>[，说明]
--------------------------------------------------------------------------------
备注: <其它说明>
--------------------------------------------------------------------------------
修改记录:
日 期        版本     修改人              修改内容
YYYY/MM/DD   X.Y      <作者或修改者名>    <修改内容>
</PRE>
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h> 
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
//#include "resource.h"
#pragma comment(lib,"winmm.lib")
//#define _DEBUG   //DEBUG选项

#define PI   3.1415926L
#define _2PI 6.2831853L
#define GET_ARRAY_LEN(array,len) {len = (sizeof(array) / sizeof(array[0]));}

#define BUFFLEN  480  //!<(512 * 2)
/* 录音缓冲数量 */
#define BUFF_NUM 10

#define PERCENT 19              /*< 识别范围 */
#define IDENTIFY_NUM 10          /*< 识别次数 */
#define IDENTIFY_FRAME_NUM 80  /*< 识别帧数 */
#define IDENTIFY_BUFF_NUM ((IDENTIFY_FRAME_NUM/2 +10)/BUFFLEN*FRAME_LEN) /*< 识别缓冲数 */

#ifdef _DEBUG
extern HWND hwndDebugDlg;   /*< 调试使用 */
extern HWND hwndEdit;
#define X_PRINT EditPrintf
#define X_HANDLE hwndEdit
#endif

extern int g_build_buff_num; /*< 建模缓冲数 */
extern BOOL bExit;           /*< 退出登录（关闭程序） */
extern BOOL g_bStopRecv;     /*< 标记是否继续接收对方发来的音频数据 */
extern BOOL killThreading;   /*< 正在关闭子线程 */
extern BOOL g_buildGMM;      /*< 标记提取声纹 */
extern TCHAR * szAppName;
extern TCHAR * szAppVersion ;
extern HINSTANCE hInst;      /*< 存储当前程序句柄 */
extern HWND hwndMainWin;
extern HWND hwndMainDlg;
extern HWND hWndCursorSpeedSlider;
extern HWND hWndFishSoundMatchAccuracySlider ;
extern HWND hWndScreenFishStatusCheckAccuracySlider;

extern HWND qqContactWin ;
extern HWND helpWin ;
extern HWND setupWindow;
extern HWND superadminWindow;
extern HWND checkScreenFishStatusBoundaryValueSlider;


extern CRITICAL_SECTION cs;  /*< 临界区 */

void EditPrintf(HWND hwndEdit, char * szFormat, ...);
void QuitMyWindow();

bool  ConvertCharToLPWSTR(const char * szString, LPWSTR lpszPath);
//LPWSTR  ConvertCharToLPWSTR(const char * szString);
#endif // GLOBAL_H_INCLUDED
