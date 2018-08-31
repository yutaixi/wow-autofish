//////////////////////////////////////////////////////////////////////////
//Copyright(C) Dake，2010-06
//编程方式：win32 SDK C语言编程
//文件名：voiceprint.c
//描述：
//主要函数：
//版本：
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "voiceprint.h"
#include "CharacterHelper.h" 
#include "AudioConfig.h"
#include <fstream>
#include "wav.h"
#include "resource.h" 
extern SysConfig sys;

static CAPECharacterHelper chelper;
VOICE_PRINT my_voiceprint = { VoicePrintInit };
 
void  DrawRawWave(LONG* lParam, int num, HWND hwnd);

void VoiceModelInit(VOICE_MODEL * pvmodel)
{
	int i;
	int j;

	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		for (j = 0; j < 2; ++j)
		{
			pvmodel->value[i][j] = 0;
		}
	}
	pvmodel->gmm.f_Init = InitGMMClass;
	pvmodel->gmm.f_Init(&pvmodel->gmm, M);
	pvmodel->percent = PERCENT;
	pvmodel->frame_num = GOOD_FRAME_NUM;
	pvmodel->m = M;
	pvmodel->bValued = FALSE;
	pvmodel->file_stream = NULL;
	pvmodel->pVoice = NULL;
	pvmodel->voice_file.f_Init = FileClassInit;
	pvmodel->voice_file.f_Init(&pvmodel->voice_file);
	pvmodel->f_GetUsrGMM = GetUsrGMM;
}


void VoicePrintInit(VOICE_PRINT * pvprint)
{
	int i;

	pvprint->voice_mdl.f_Init = VoiceModelInit;
	pvprint->voice_mdl.f_Init(&pvprint->voice_mdl);
	pvprint->hwnd = NULL;
	pvprint->myRecord = NULL;
	pvprint->g_hRecThread = NULL;
	pvprint->g_RecThreadID = 0;
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		/*this->rec_frame[i] = IDENTIFY_FRAME_NUM / (IDENTIFY_NUM - i);*/
		pvprint->rec_frame[i] = IDENTIFY_FRAME_NUM / IDENTIFY_NUM;
	}
}

// mod取E_BUILD是建模，取E_IDENTIFY是识别
BOOL GetUsrGMM(VOICE_PRINT * pUsr, BUILD_MODE mod)
{
	int i;
	//FILE * fi = NULL;
	int frame_num;
	//int buff_num=0;
	int buff_size;
	BOOL bRes;
	//BYTE * pRawVoice = NULL;   //指向原始语音数据
	double ** MFCC = NULL;
	double temp;

	switch (mod)
	{
	case E_BUILD: // 建模模式
		frame_num = pUsr->voice_mdl.frame_num;
		//buff_num = g_build_buff_num;
		break;

	case E_IDENTIFY: // 识别模式
		frame_num = IDENTIFY_FRAME_NUM;
		//buff_num = IDENTIFY_BUFF_NUM;
		break;

	default:
		break;
	}

	//buff_size = buff_num * BUFFLEN;
	//fopen_s(&fi, pUsr->voice_mdl.voice_file.szFileName, "rb");
	//if (!fi)
	//{
		//xprint(TEXT("打开声纹数据文件出错"));
		//return FALSE;
	//}
	
	//LONG *pRawVoice;
	WavFile wavFile;
	if (mod == E_BUILD)
	{
		if (sys.soundDevcieFrequency == 48000)
		{
			wavFile.Serialize(sys.FISHING_TRIGER_STD48k_WAV);
		}
		else if (sys.soundDevcieFrequency == 44100)
		{
			wavFile.Serialize(sys.FISHING_TRIGER_STD44k_WAV);
		}
		
	}
	else
	{
		wavFile.Serialize(pUsr->voice_mdl.voice_file.szFileName);
	} 
	if (wavFile.num < 1)
	{
		return false;
	}
	int dataLen = wavFile.num / wavFile.wChannel; 
	frame_num = dataLen / FRAME_LEN;
	//buff_num = dataLen / BUFFLEN;
	//buff_num = dataLen / BUFFLEN;
	//pRawVoice = wavFile.Ldata;
	/*
	BYTE * lrRawVoice,*pRawVoice;
	lrRawVoice= (BYTE *)malloc(buff_size*4);//含左右声道原始数据
	fread(lrRawVoice, sizeof(BYTE), buff_size*4, fi);
	
	pRawVoice = (BYTE *)malloc(buff_size * 2);
	if (!pRawVoice)
	{
		//xprint(TEXT("分配读文件缓冲失败"));
		fclose(fi);
		return FALSE;
	}
	int lindex = 0;
	for (int index = 0; index < buff_size*4; index+=4)
	{
		pRawVoice[lindex] = lrRawVoice[index];
		pRawVoice[lindex+1] = lrRawVoice[index+1];
		lindex++;
	}
	
	*/
	//pRawVoice = (BYTE *)malloc(buff_size);
	
	//fread(pRawVoice, sizeof(BYTE), buff_size, fi);
	//fclose(fi);
	//DrawRawWave(pRawVoice, dataLen, hwndMainDlg);

	MFCC = (double **)malloc(frame_num * sizeof(double *));
	for (i = 0; i < frame_num; ++i)
	{
		MFCC[i] = (double *)malloc(D * sizeof(double));
	}
	
	if (voiceToMFCC(wavFile.Ldata, dataLen, MFCC, frame_num))
	{  
		//delete pRawVoice;
		//free(pRawVoice);
		if (mod == E_BUILD)
		{
			//xprint(TEXT("训练参数: %d %d"), pUsr->voice_mdl.frame_num, pUsr->voice_mdl.m);
			if (!GMMs(MFCC, &pUsr->voice_mdl.gmm, frame_num, pUsr->voice_mdl.m))
			{
				MessageBox(pUsr->hwnd, TEXT("GMMs失败！"), szAppVersion, 0);
				goto RETURN_FALSE;
			}
		}
		// TODO 判断GMM_identify返回值
		for (i = 0; i < IDENTIFY_NUM; ++i)
		{
			//bRes = GMM_identify(&MFCC[i*(IDENTIFY_FRAME_NUM / IDENTIFY_NUM)], &temp, &(pUsr->voice_mdl.gmm), pUsr->rec_frame[i], pUsr->voice_mdl.m);
			bRes = GMM_identify(&MFCC[i*(IDENTIFY_FRAME_NUM / IDENTIFY_NUM)], &temp, &(pUsr->voice_mdl.gmm), IDENTIFY_FRAME_NUM /IDENTIFY_NUM, pUsr->voice_mdl.m);
			if (bRes)
			{
				pUsr->voice_mdl.value[i][mod] = fabs(temp);
			}
			else
			{
				//MessageBox(pUsr->hwnd, TEXT("GMM_identify失败！"), szAppVersion, 0);
			RETURN_FALSE:
				for (i = 0; i < frame_num; ++i)
				{
					free(MFCC[i]);
				}
				free(MFCC);
				MFCC = NULL;
				return FALSE;
			}
		}
		if (mod == E_BUILD) // 打印模型阈值
		{
			PrintThresholdValue(pUsr->hwnd);
		}
		for (i = 0; i < frame_num; ++i)
		{
			free(MFCC[i]);
		}
		free(MFCC);
		MFCC = NULL;
	}
	else
	{
		//MessageBox(pUsr->hwnd, TEXT("MFCC失败！"), szAppVersion, 0);
		for (i = 0; i < frame_num; ++i)
		{
			free(MFCC[i]);
		}
		free(MFCC);
		MFCC = NULL;
		//free(pRawVoice);
		 
		return FALSE;
	}
	return TRUE;
}

void PrintThresholdValue(HWND hwnd)
{
	//int i;
	//HWND hwndPrint; 
	//hwndPrint = GetDlgItem(hwnd, IDC_TEXTOUT);
	//EditPrintf(hwndPrint, "声纹识别阈值:\r\n");
	//for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		//EditPrintf(hwndPrint, "第%d个%d帧:  %lf\r\n", i + 1, my_voiceprint.rec_frame[i], my_voiceprint.voice_mdl.value[i][0]);
	}
}

void  DrawRawWave(LONG* lParam, int num,HWND hwnd)
{  
	HDC m_pDC = GetDC(hwnd);
	//CRect clientRect;
	RECT clientRect = { 20, 25, 293, 150 };
	int m_beginX = 20;
	int m_beginY = 25;
	HWND dicSound = GetDlgItem(hwnd, IDC_SOUND);
	//GetWindowRect(dicSound,&clientRect);
	//GetDlgItem(IDC_SOUND)->GetWindowRect(&clientRect); 
	//ScreenToClient(dicSound,clientRect);
	//画中轴线
	COLORREF bkColor = RGB(0, 0, 0);
	SetBkColor(m_pDC, bkColor);
	HBRUSH  brush = CreateSolidBrush(bkColor);

	//m_pDC->FillRect(clientRect, &brush);
	FillRect(m_pDC, &clientRect, brush);
	HPEN pen1 = CreatePen(0, 1, RGB(0, 255, 0));

	//CPen *oldPen1 = m_pDC->SelectObject(&pen1);
	HGDIOBJ pen2 = SelectObject(m_pDC, pen1);
	//int size = m_SoundIn.m_BufferSize / 2;
	int size = num;
	int yy = (clientRect.bottom - clientRect.top) / 2;

	//MoveTo(m_beginX, int(clientRect.Height() / 2 + m_beginY));
	MoveToEx(m_pDC, m_beginX, int((clientRect.bottom - clientRect.top) / 2 + m_beginY), NULL);
	int height = yy;
	int x, y; 
	short sample;
	int gap = num / (clientRect.right - clientRect.left);
	for (int register i = 0; i <(long)size; i+= gap)	//to draw first channel
	{
		//sample = (((*((short*)buf + i))-128)*h) / (65535 / 2);
		 

		sample = (lParam[i]-128)*height /(256 / 2) ;
		//sample = ((*((short*)buf + i)) *h) / (65535 / 2);
		x = int(((float)i / size)*(clientRect.right - clientRect.left));
		y = yy - sample;
		//m_pDC->LineTo(m_beginX + x, y + m_beginY);
		LineTo(m_pDC, m_beginX + x, y + m_beginY);
	}
	DeleteObject(brush);
	DeleteObject(pen1);
	DeleteObject(pen2);
	ReleaseDC(hwnd,m_pDC); 
}