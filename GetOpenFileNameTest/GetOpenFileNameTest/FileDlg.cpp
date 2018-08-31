//////////////////////////////////////////////////////////////////////////
//Copyright(C) Dake，2010-11
//编程方式：win32 SDK C语言编程
//文件名：FileDlg.c
//描述：文件对话框的相关操作
//主要函数：
//版本：V1.1
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FileDlg.h"
#include <commdlg.h>
//#include "userdata.h"

//FILE_CLASS voice_file = {FileClassInit};
static OPENFILENAME ofn;

void FileClassInit(FILE_CLASS * pfile)
{
	pfile->szFileName[0] = '\0';
	pfile->szTitleName[0] = '\0';
	pfile->file_size = 0;
	pfile->f_FileInitialize = FileInitialize;
	pfile->f_FileOpenDlg = FileOpenDlg;
	pfile->f_FileSaveDlg = FileSaveDlg;
	pfile->f_CheckFileSize = CheckFileSize;
}
//文件对话框的初始化
void FileInitialize(HWND hwnd)
{
	static TCHAR szFilter[] = TEXT("ASCII Files (*.GMM)\0*.gmm\0") \
		TEXT("Text Files (*.TXT)\0*.txt\0") \
		TEXT("All Files (*.*)\0*.*\0\0");

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFilter;      // 设置格式过滤选项
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = NULL;          // Set in Open and Close functions
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;          // Set in Open and Close functions
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = 0;             // Set in Open and Close functions
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = TEXT("gmm");
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
}

BOOL FileOpenDlg(HWND hwnd, FILE_CLASS * myfile)
{
	ofn.hwndOwner = hwnd; 
	LPWSTR lpszPath = new WCHAR[strlen(myfile->szFileName) + 1];
	ConvertCharToLPWSTR(myfile->szFileName, lpszPath);
	ofn.lpstrFile = lpszPath;
	ofn.lpstrFileTitle = lpszPath;
	delete lpszPath;
	lpszPath = NULL;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

	return GetOpenFileName(&ofn);
}

BOOL FileSaveDlg(HWND hwnd, FILE_CLASS * myfile)
{
	ofn.hwndOwner = hwnd;
	LPWSTR lpszPath = new WCHAR[strlen(myfile->szFileName) + 1];
	ConvertCharToLPWSTR(myfile->szFileName, lpszPath);
	
	ofn.lpstrFile = lpszPath;
	ofn.lpstrFileTitle = lpszPath;
	 delete lpszPath;
	 lpszPath = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	return GetSaveFileName(&ofn);
}


// 返回文件大小(>=0)，错误时，返回-1
int CheckFileSize(FILE_CLASS * myfile)
{
	struct _stat buf;
	int result;

	result = _stat(myfile->szFileName, &buf);
	// Check if statistics are valid: 
	if (result != 0)
	{
		switch (errno)
		{
		case ENOENT:
			//xprint(TEXT("File %s not found."), myfile->szFileName);
			break;
		case EINVAL:
			//xprint(TEXT("Invalid parameter to _stat."));
			break;
		default:
			/* Should never be reached. */
			//xprint(TEXT("Unexpected error in _stat."));
			break;
		}
		myfile->file_size = -1;
		return -1;
	}
	myfile->file_size = buf.st_size;
	return buf.st_size;
}
 