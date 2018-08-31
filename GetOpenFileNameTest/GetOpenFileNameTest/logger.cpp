#include "stdafx.h"
#include "logger.h"
#include "stdio.h"
#include "global.h"
#include "CharacterHelper.h"
#include "timeUtil.h"
#include <atlstr.h>
#include "resource.h"
#include "fileUtil.h" 

extern SysConfig sys;

void selectStr(WCHAR *src, WCHAR *des, int start, int end);


Logger::Logger()
{
}

Logger::~Logger()
{
}


int Logger::displayLog(char* format, ...)
{
	int i = 0; 
	char str_tmp[1024];
	WCHAR text[1024*15] = {0};  
	 
	va_list vArgList; // 定义一个va_list型的变量, 这个变量是指向参数的指针. 
	va_start(vArgList, format);                 //用va_start宏初始化变量,这个宏的第二个参数是第一个可变参数的前一个参数,是一个固定的参数
	i = _vsnprintf_s(str_tmp, 1024, format, vArgList); //注意,不要漏掉前面的_
	va_end(vArgList);                            //用va_end宏结束可变参数的获取 
	 
	HWND textOut = GetDlgItem(hwndMainDlg, IDC_TEXTOUT); //获取输出区域句柄
	   
	GetWindowText(textOut, text, 1024 * 15);//获取当前输出区域内容
	if (wcslen(text) >= 1024 * 14)//如果内容超长，则情况内容；
	{  
		SetDlgItemText(hwndMainDlg, IDC_TEXTOUT, L""); 
	}

	//获取当前时间
	string now = getFormatedDate();
	//将时间和内容拼接
	now.append(" ").append(str_tmp);

	SendMessage(textOut, EM_SETSEL, (WPARAM)-1, (LPARAM)-1); 
	str_utf16 * msg=CAPECharacterHelper::GetUTF16FromANSI(now.c_str());
	SendMessage(textOut, EM_REPLACESEL, FALSE, (LPARAM)msg);
	SendMessage(textOut, EM_SCROLLCARET, 0, 0); 
	delete[] msg; 
	DeleteObject(textOut);
	
	return i;                                    //返回参数的字符个数中间有逗号间隔
}

void selectStr(WCHAR *src, WCHAR *des, int start, int end)
{
	int index = 0;
	for (int i = start-1; i < end; i++)
	{
		des[index] = src[i];
		index++;
	}
}

void Logger::initGlog()
{ 
	google::InitGoogleLogging(sys.glogHead);
	google::SetLogDestination(google::GLOG_INFO, sys.glogDestination.c_str()); 
	//单个日志文件最大，单位M
	FLAGS_max_log_size = sys.glogMaxSize;
	FLAGS_minloglevel = sys.glogLevel;
}

bool Logger::setGlogLevel(int level)
{
	sys.glogLevel = level;
	FLAGS_minloglevel = level; 
	return true;
}

string Logger::getFormatedDate()
{ 
	return timeUtil::localTimeToString("%m-%d %H:%M:%S");
}