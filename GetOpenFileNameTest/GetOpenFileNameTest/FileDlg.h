#include "global.h"
#include <string>
using  namespace std;

#ifndef _FILEDLG_H_
#define _FILEDLG_H_

typedef struct FILE_CLASS
{
	void(*f_Init)(struct FILE_CLASS *);
	char szFileName[MAX_PATH];  // ����·��
	char szTitleName[60];      // �ļ���
	int file_size;              // �ļ���С
	void(*f_FileInitialize)(HWND);
	BOOL(*f_FileOpenDlg)(HWND, struct FILE_CLASS *);
	BOOL(*f_FileSaveDlg)(HWND, struct FILE_CLASS *);
	int(*f_CheckFileSize)(struct FILE_CLASS *);
	BOOL(*f_SetFileName)(struct FILE_CLASS *);
}FILE_CLASS;

//extern FILE_CLASS voice_file;

void FileClassInit(FILE_CLASS * pfile);
void FileInitialize(HWND hwnd);
BOOL FileOpenDlg(HWND hwnd, FILE_CLASS * myfile);
BOOL FileSaveDlg(HWND hwnd, FILE_CLASS * myfile);
int CheckFileSize(FILE_CLASS * myfile); 
#endif //_FILEDLG_H_