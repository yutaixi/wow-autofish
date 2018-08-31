#include "stdafx.h"
#include "AudioRecognize.h"
#include "voiceprint.h"
#include "logger.h" 
#include "wow.h" 

#include "global.h"
#include "AudioConfig.h"  
#include "AudioManager.h" 


extern VOICE_PRINT my_voiceprint; 
extern Wow wow;
extern SysConfig sys;

//录音
class CPlaybackEventHandler : public IPlaybackEvent
{
public:
	virtual VOID OnPlaybackEnd()
	{

	}
};



CPlaybackEventHandler g_playbackEventHandler;
CAudioManager g_audioMgr(&g_playbackEventHandler);




AudioRecognize::AudioRecognize()
{
}

AudioRecognize::~AudioRecognize()
{
}

 void AudioRecognize::voicePrintIdentify(HWND hwnd)
{

	LOG(INFO) << "VoicePrintIdentify";
	int i;
	int temp_num = 0;
	double temp = 0;
	double percent = my_voiceprint.voice_mdl.percent / 100.0; 
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		temp = (1 + percent) * my_voiceprint.voice_mdl.value[i][0]; 
		temp >= my_voiceprint.voice_mdl.value[i][1] ? ++temp_num : temp_num;
	} 

	//Logger::displayLog("语音匹配次数：%d\r\n", temp_num);
	if (temp_num >= sys.fishSoundMatchAccuracy)
	{
		LOG(INFO) << "VoicePrintIdentify 识别成功,识别出次数="<< temp_num;
		
		if (wow.start_rec_sys_sound )
		{
			if (!wow.pauseSoundRecognize)
			{
				//收杆
				wow.finishFish(&(wow.fish_point), &(sys.wowWindowRect));
				//抛竿 
				wow.startFish(true);
			}
			
		}
		else
		{
			MessageBox(NULL, L"识别成功", szAppVersion, 0);
		} 
	}
	else
	{
		LOG(INFO) << "VoicePrintIdentify 识别失败,识别次数="<< temp_num;
	}
	 
}
BOOL AudioRecognize::buildGMM(HWND hwnd)
{
	LOG(INFO) << "OnBuildGMM";
	int buff_num = g_build_buff_num;
	//MessageBox(hwnd, TEXT("语音已经采集！"), szAppVersion, 0);
	// 文件探测 
	CheckFileSize(&my_voiceprint.voice_mdl.voice_file);
	//my_voiceprint.voice_mdl.voice_file.f_CheckFileSize(&my_voiceprint.voice_mdl.voice_file);
	if (my_voiceprint.voice_mdl.voice_file.file_size == -1)
	{
		LOG(ERROR) << "异常：声纹提取文件不存在！";
		return FALSE;
	}
	else if ((!my_voiceprint.voice_mdl.bValued && my_voiceprint.voice_mdl.voice_file.file_size<BUFFLEN*buff_num)
		|| (my_voiceprint.voice_mdl.bValued && my_voiceprint.voice_mdl.voice_file.file_size<BUFFLEN*IDENTIFY_BUFF_NUM))
	{
		LOG(ERROR) << "异常：语音数据过少！！";
		return FALSE;
	}
	/**/
	BOOL getUsrGMM = false;

	try
	{
		getUsrGMM = GetUsrGMM(&my_voiceprint, my_voiceprint.voice_mdl.bValued ? E_IDENTIFY : E_BUILD);
		LOG(ERROR) << "getUsrGMM";
	}
	catch (const std::exception&)
	{
		LOG(ERROR) << "异常：getUsrGMM异常";
		getUsrGMM = false;
	}
	catch (...)
	{
		LOG(ERROR) << "异常：getUsrGMM异常";
		getUsrGMM = false;
	}


	if (getUsrGMM)
	{
		if (!my_voiceprint.voice_mdl.bValued)
		{
			my_voiceprint.voice_mdl.bValued = TRUE;
			LOG(INFO) << "建模成功！";
			MessageBox(hwnd, TEXT("建模成功！"), szAppVersion, 0);

		}
		else
		{
			voicePrintIdentify(hwnd);
		}
	}
	else
	{
		LOG(ERROR) << "建模失败！";
	}

	return TRUE;
}

 
 


 

void AudioRecognize::stopVoice(HWND hwnd)
{
	LOG(INFO) << "stopVoice";
	g_buildGMM = FALSE;
	LOG(INFO) << "stopVoice g_buildGMM = FALSE"; 
	stopRecord();
	LOG(INFO) << "stopRecord";
}


 
 

void AudioRecognize::startBuild(HWND hwnd)
{ 
	//if (!g_buildGMM && !my_voiceprint.voice_mdl.bValued)
	if (!g_buildGMM )
	{
		my_voiceprint.voice_mdl.gmm.f_FreeGMM(&my_voiceprint.voice_mdl.gmm);
		VoicePrintInit(&my_voiceprint);
		/* 启动录音 */
		MessageBox(hwnd, TEXT("建模开始\n开始录音..."), szAppVersion, 0);
		g_buildGMM = TRUE;
		startRecord();
	}
	else if (my_voiceprint.voice_mdl.bValued) // 已经建模过	
	{
		MessageBox(hwnd, TEXT("已有声纹模型，请导出后重置！"), szAppVersion, 0);
	}
}

 void AudioRecognize::identify(HWND hwnd)
{
	LOG(INFO) << "OnIdentifyBtn";
	char cont[100];
	sprintf_s(cont, "g_buildGMM= %d \r\n my_voiceprint.voice_mdl.bValued= %d\r\n  ", (int)g_buildGMM, (int)my_voiceprint.voice_mdl.bValued);
	LOG(INFO) << cont;
	if (!g_buildGMM && !my_voiceprint.voice_mdl.bValued)
	{
		MessageBox(hwnd, TEXT("请先提取或导入声纹！"), szAppVersion, 0);
	}
	else if (g_buildGMM && !my_voiceprint.voice_mdl.bValued)
	{
		MessageBox(hwnd, TEXT("正在提取声纹！"), szAppVersion, 0);
	}
	else  if (!g_buildGMM && my_voiceprint.voice_mdl.bValued)
	{

		g_buildGMM = TRUE;
		startRecord();
		LOG(INFO) << "OnIdentifyBtn StartRecordmgt"; 
	}
}

 void AudioRecognize::exportGMM(HWND hwnd ,string exportFilePath)
{
	int i;
	FILE * fsave = NULL;
	VOICE_MODEL * p_data = NULL;  
	if (!my_voiceprint.voice_mdl.bValued)
	{
		MessageBox(hwnd, TEXT("尚无声纹模型！"), szAppVersion, 0);
		return;
	}  
	fopen_s(&fsave, exportFilePath.c_str(), "wb");
	if (!fsave)
	{
		MessageBox(hwnd, TEXT("创建文件失败！"), szAppVersion, 0);
		return;
	}
	p_data = &my_voiceprint.voice_mdl;

	// 写入GMM模型数据
	fwrite(&p_data->frame_num, sizeof(p_data->frame_num), 1, fsave);
	fwrite(&p_data->m, sizeof(p_data->m), 1, fsave);
	fwrite(&p_data->percent, sizeof(p_data->percent), 1, fsave);
	fwrite(p_data->gmm.p, sizeof(p_data->gmm.p[0]), p_data->m, fsave);
	for (i = 0; i < p_data->m; ++i)
	{
		fwrite(p_data->gmm.u[i], sizeof(p_data->gmm.u[0][0]), D, fsave);
	}
	for (i = 0; i < p_data->m; ++i)
	{
		fwrite(p_data->gmm.cMatrix[i], sizeof(p_data->gmm.cMatrix[0][0]), D, fsave);
	}

	// 写入识别阈值
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		fwrite(&p_data->value[i][0], sizeof(p_data->value[0][0]), 1, fsave);
	}
	fclose(fsave); 
}


  

void AudioRecognize::importGMM(HWND hwnd,DWORD sampleFrequency)
{ 
	int i;
	FILE * f_open = NULL;
	VOICE_MODEL * p_data = NULL;
	/*
	FILE_CLASS open_file = { FileClassInit };
	FileClassInit(&open_file);
	FileInitialize(hwnd);
	FileOpenDlg(hwnd, &open_file);
	*/
	string gmmFile=sys.getStdGMMFilePath(sampleFrequency);

	fopen_s(&f_open, gmmFile.c_str(), "rb");
	if (!f_open)
	{
		MessageBox(hwnd, TEXT("打开文件失败！"), szAppVersion, 0);
		return;
	}
	p_data = &my_voiceprint.voice_mdl;
	p_data->gmm.f_FreeGMM(&p_data->gmm);

	// 读入GMM模型数据
	fread(&p_data->frame_num, sizeof(p_data->frame_num), 1, f_open);
	fread(&p_data->m, sizeof(p_data->m), 1, f_open);
	fread(&p_data->percent, sizeof(p_data->percent), 1, f_open);
	p_data->gmm.f_Init(&p_data->gmm, p_data->m);
	p_data->gmm.f_CallocGMM(&p_data->gmm);
	fread(p_data->gmm.p, sizeof(p_data->gmm.p[0]), p_data->m, f_open);
	for (i = 0; i < p_data->m; ++i)
	{
		fread(p_data->gmm.u[i], sizeof(p_data->gmm.u[0][0]), D, f_open);
	}
	for (i = 0; i < p_data->m; ++i)
	{
		fread(p_data->gmm.cMatrix[i], sizeof(p_data->gmm.cMatrix[0][0]), D, f_open);
	}

	// 读入识别阈值
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		fread(&p_data->value[i][0], sizeof(p_data->value[0][0]), 1, f_open);
	}
	fclose(f_open);
	p_data->bValued = TRUE;
	p_data = NULL;
	//-------------------------------------------------------------------------------------------------
	// 刷新界面显示
	//  
	g_build_buff_num = my_voiceprint.voice_mdl.frame_num * FRAME_LEN / BUFFLEN; 
}


/* 删除临时语音转储文件 */
void AudioRecognize::deleteVoiceFile()
{
	// 文件探测 
	CheckFileSize(&my_voiceprint.voice_mdl.voice_file);
	if (my_voiceprint.voice_mdl.voice_file.file_size == -1)
	{
		return;
	}
	if (my_voiceprint.voice_mdl.file_stream)
	{
		fclose(my_voiceprint.voice_mdl.file_stream);
	}
	if (remove(my_voiceprint.voice_mdl.voice_file.szFileName))
	{
		LOG(ERROR) << "删除文件"<< my_voiceprint.voice_mdl.voice_file.szFileName<<"失败";
	}
}


void AudioRecognize::reset(HWND hwnd)
{
	my_voiceprint.g_hRecThread = NULL;
	my_voiceprint.g_RecThreadID = 0;
	g_buildGMM = FALSE;
	my_voiceprint.voice_mdl.bValued = FALSE;
	my_voiceprint.voice_mdl.gmm.f_FreeGMM(&my_voiceprint.voice_mdl.gmm);
	my_voiceprint.f_Init(&my_voiceprint);
	my_voiceprint.hwnd = hwnd; 
}

void AudioRecognize::freeVoicePrint()
{
	g_buildGMM = FALSE;
	my_voiceprint.voice_mdl.gmm.f_FreeGMM(&my_voiceprint.voice_mdl.gmm);

}

void AudioRecognize::initVoicePrint()
{
	VoicePrintInit(&my_voiceprint);

}

void AudioRecognize::updateVoicePrint(int percent,int m,int frame_num,bool identify)
{
	my_voiceprint.voice_mdl.percent = percent;
	if (identify)
	{
		return;
	}
	my_voiceprint.voice_mdl.m = m;
	my_voiceprint.voice_mdl.frame_num = frame_num;
	g_build_buff_num = frame_num* FRAME_LEN / BUFFLEN;

}

BOOL AudioRecognize::hasVoiceId()
{ 
	return my_voiceprint.voice_mdl.bValued; 
}

bool AudioRecognize::beginRecord(HWND hwnd)
{
	if (!my_voiceprint.myRecord && !my_voiceprint.g_RecThreadID)
	{
		my_voiceprint.myRecord = CreateRecord(hwnd); 
		return true;
	}
	return false;
}
bool AudioRecognize::endRecord(int msg)
{
	if (my_voiceprint.myRecord && my_voiceprint.g_RecThreadID)
	{ 
		// 向录音线程发送结束消息
		PostThreadMessage(my_voiceprint.g_RecThreadID, msg, 0, 0);
		my_voiceprint.myRecord = NULL;
		return true;
	}
	return false;
}

//录音

void AudioRecognize::startRecord()
{
	if (g_audioMgr.StartCapture())
	{
		LOG(INFO) << "g_audioMgr.StartCapture()";
	}
}

void AudioRecognize::stopRecord()
{
	g_audioMgr.StopCapture();
	LOG(INFO) << "停止录音";

}

void AudioRecognize::startPlay()
{
	if (g_audioMgr.CanPlay())
	{
		if (g_audioMgr.StartPlayback())
		{

		}
	}
}

void AudioRecognize::stopPlay()
{
	g_audioMgr.StopPlayback();
}

bool  AudioRecognize::refreshSoundDeviceFrequency()
{

	DWORD deviceFrequency=g_audioMgr.getDeviceFrequency();
	if (deviceFrequency == 0)
	{
		return false;
	}
	
	sys.soundDevcieFrequency = deviceFrequency;
	return true;


}