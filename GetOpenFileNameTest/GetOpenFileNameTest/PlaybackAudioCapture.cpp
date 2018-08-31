#include "stdafx.h"
#include "PlaybackAudioCapture.h"
#include "ClassRegister.h"
#include "TimeCostDebug.h"
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <process.h>
#include <avrt.h>
#include "voiceprint.h" 
#include "AudioConfig.h"
#include "resource.h"
#include <mmsystem.h>
#include <conio.h> 
#include "logger.h"  

extern SysConfig sys;

#define AUDIO_CAPTURE_CLASS _T("audio_cpature_message_class")
#define REFTIMES_PER_SEC  1000000
 
#pragma comment(lib, "winmm.lib") 
bool CreateWaveFile(WAVEFORMATEX *sampleFormat, char* fileName, HMMIO *m_hFile);
char *  outfilename;
//HMMIO		m_hFile;
MMCKINFO    m_MMCKInfoData;
MMCKINFO	m_MMCKInfoParent;
MMCKINFO	m_MMCKInfoChild;
 

enum CAPTURE_STATUS 
{
	CAPTURE_START,
	CAPTURE_STOP,
	CAPTURE_ERROR
};

#define WM_CAPTUE_STATUS		WM_USER+100
#define WM_WAVE_FORMAT			WM_USER+101
#define WM_CAPTURE_DATA			WM_USER+102

////////////////////  CPlaybackCaptureImpl  /////////////////
struct capture_thread_data
{
	HANDLE hEventStarted;
	HANDLE hEventStop;
	HWND hWndMessage;
	IMMDevice* pDevice=NULL;
};


class CPlaybackCaptureImpl
{
public:
	CPlaybackCaptureImpl();
	~CPlaybackCaptureImpl();

	BOOL Initialize(IPlaybackCaptureEvent* pHandler);
	VOID Destroy();

	BOOL Start();
	VOID Stop();

	DWORD getDevcieFrequency();

	BOOL IsInited() const;
	BOOL IsCapturing() const;
	BOOL IsDeviceOK() const;
	IPlaybackCaptureEvent* GetEventHandler() const	{ return m_pEventHandler;}
	VOID OnThreadEnd();

private:
	IMMDevice* GetDefaultDevice();

private:
	HWND m_hWndMessage;
	HANDLE m_hEventStarted;
	HANDLE m_hEventStop;
	IMMDevice* m_pDevice=NULL;

	HANDLE m_hThreadCapture;

	static CClassRegister m_sClassRegister;
	BOOL m_bInited;

	IPlaybackCaptureEvent* m_pEventHandler=NULL;
};

LRESULT CALLBACK AudioCaptureMessageProc(HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled(FALSE);
	LRESULT lRet(0);

	IPlaybackCaptureEvent* pEventHandler = NULL;
	CPlaybackCaptureImpl* pThis = (CPlaybackCaptureImpl*)GetWindowLong(hWnd, GWL_USERDATA);
	if(pThis != NULL)
	{
		pEventHandler = pThis->GetEventHandler();
	}

	switch(uMsg)
	{
	case WM_NCCREATE:
		{
			CREATESTRUCT* pSC = (CREATESTRUCT*)lParam;
			if(pSC != NULL)
			{
				SetWindowLong(hWnd, GWL_USERDATA, (LONG)pSC->lpCreateParams);
			}
		}
		break;

	case WM_CAPTUE_STATUS:
		{
			if(wParam == CAPTURE_START)
			{
				if(pEventHandler != NULL) pEventHandler->OnCatpureStart(lParam);
			}
			else if(wParam == CAPTURE_STOP)
			{
				if(pEventHandler != NULL) pEventHandler->OnCaptureStop();
				if(pThis != NULL) pThis->OnThreadEnd();
			}

			bHandled = TRUE;
		}
		break;

	case WM_WAVE_FORMAT:
		{
			if(pEventHandler != NULL)
			{
				pEventHandler->OnAdjustCaptureFormat((WAVEFORMATEX*)lParam);
			}

			bHandled = TRUE;
		}
		break;

	case WM_CAPTURE_DATA:
		{
			if(pEventHandler != NULL)
			{
				pEventHandler->OnCatpureData((LPBYTE)lParam, wParam);
			}

			bHandled = TRUE;
		}
		break;

	default:
		break;
	}

	if(!bHandled)
	{
		lRet = ::DefWindowProc(hWnd, uMsg, wParam, lParam);	
	}

	if (pEventHandler != NULL)
	{
		pEventHandler = NULL;
	}
	if (pThis != NULL)
	{
		pThis = NULL;
	}

	return lRet;
}

CClassRegister CPlaybackCaptureImpl::m_sClassRegister(AUDIO_CAPTURE_CLASS, AudioCaptureMessageProc);

static VOID NotifyStatus(HWND hWndMesasge, CAPTURE_STATUS eStatus, DWORD dwUserData = 0)
{
	::SendMessage(hWndMesasge, WM_CAPTUE_STATUS, (WPARAM)eStatus, dwUserData);
}

static VOID NotifyWaveFormat(HWND hWndMessage, WAVEFORMATEX* pFormat)
{
	::SendMessage(hWndMessage, WM_WAVE_FORMAT, 0, (LPARAM)(WAVEFORMATEX*)pFormat);
}

static VOID NotifyData(HWND hWndMessage, LPBYTE pData, INT nDataLen)
{
	::SendMessage(hWndMessage, WM_CAPTURE_DATA, nDataLen, (LPARAM)pData);
}

 

BOOL AdjustFormatTo16Bits(WAVEFORMATEX *pwfx)
{
	BOOL bRet(FALSE); 
	
	if(pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
        pwfx->wFormatTag = WAVE_FORMAT_PCM;
        pwfx->wBitsPerSample = WBITSPERSAMPLE;
		//pwfx->nSamplesPerSec = SAMPLE_FREQUENCY;
		//pwfx->nChannels = NCHANNELS;
        pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
        pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;

		bRet = TRUE;
	}
	else if(pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
        PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
        if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat))
		{
           pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
           pEx->Samples.wValidBitsPerSample = WBITSPERSAMPLE;
           pwfx->wBitsPerSample = WBITSPERSAMPLE;
		   //pwfx->nSamplesPerSec = SAMPLE_FREQUENCY;
		   //pwfx->nChannels = NCHANNELS;
           pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
           pwfx->nAvgBytesPerSec =  pwfx->nSamplesPerSec*pwfx->nChannels*pwfx->wBitsPerSample/8;
		   
		   bRet = TRUE;
        } 
	}

	return bRet;
}

UINT CaptureAudio(HWND hWndMessage, IMMDevice* pDevice, HANDLE hEventStarted, HANDLE hEventStop)
{
	LOG(INFO) << "CaptureAudio start";
	HRESULT hr;
	IAudioClient *pAudioClient = NULL;
	WAVEFORMATEX *pwfx = NULL;
	REFERENCE_TIME hnsDefaultDevicePeriod(0);
	HANDLE hTimerWakeUp = NULL; 
	IAudioCaptureClient *pAudioCaptureClient = NULL;
	DWORD nTaskIndex = 0;
	HANDLE hTask = NULL;
	BOOL bStarted(FALSE);
	HMMIO m_hFile=NULL; 
	try
	{
		do
		{

			if (pDevice == NULL)
			{
				LOG(ERROR) << "异常：CaptureAudio pDevice为空 break重新录音";
				PostMessage(hwndMainDlg, WM_RESTART_RECORD, 0, 0);
				break;
			}
			
			hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
			if (FAILED(hr)) {
				LOG(ERROR) << "异常：CaptureAudio pDevice->Activate failed";
				break;
			}  
			LOG(INFO) << "CaptureAudio pDevice->Activate OK";
			hr = pAudioClient->GetMixFormat(&pwfx);   
			if (FAILED(hr)) {
			
				LOG(ERROR) << "异常：CaptureAudio pAudioClient->GetMixFormat failed";
				break;
			}  
			LOG(INFO) << "CaptureAudio pAudioClient->GetMixFormat OK";
			if (!AdjustFormatTo16Bits(pwfx)) {
				LOG(ERROR) << "异常：CaptureAudio AdjustFormatTo16Bits failed";
				break;
			}
			LOG(INFO) << "CaptureAudio AdjustFormatTo16Bits(pwfx) OK";
			hTimerWakeUp = CreateWaitableTimer(NULL, FALSE, NULL);
			if (hTimerWakeUp == NULL) {
			
				LOG(ERROR) << "异常：CaptureAudio CreateWaitableTimer failed";
				break;
			} 
			LOG(INFO) << "CaptureAudio CreateWaitableTimer OK";
			SetEvent(hEventStarted);
			LOG(INFO) << "CaptureAudio SetEvent hEventStarted OK";
			REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
			NotifyWaveFormat(hWndMessage, pwfx);
			LOG(INFO) << "CaptureAudio NotifyWaveFormat OK";
			//WAVEFORMATEX**  ppClosestMatch = NULL;
			//pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwfx, ppClosestMatch);
			hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, hnsRequestedDuration, pwfx, 0);
			if (FAILED(hr)) {
				LOG(ERROR) << "异常：CaptureAudio pAudioClient->Initialize failed";
				break;
			}
			LOG(INFO) << "CaptureAudio pAudioClient->Initialize OK";
			hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
			LOG(INFO) << "CaptureAudio pAudioClient->GetDevicePeriod OK";


			hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);
			if (FAILED(hr)) {
				LOG(ERROR) << "异常：CaptureAudio pAudioClient->GetService failed";
				break;
			}
			LOG(INFO) << "CaptureAudio pAudioClient->GetService OK";


			hTask = AvSetMmThreadCharacteristics(L"Capture", &nTaskIndex);
			if (NULL == hTask)
			{
				LOG(ERROR) << "异常：CaptureAudio AvSetMmThreadCharacteristics failed";
				break;
			}
			LOG(INFO) << "CaptureAudio AvSetMmThreadCharacteristics OK";
			LARGE_INTEGER liFirstFire;
			liFirstFire.QuadPart = -hnsDefaultDevicePeriod / 2; // negative means relative time
			LONG lTimeBetweenFires = (LONG)hnsDefaultDevicePeriod / 2 / (10 * 1000); // convert to milliseconds

			//liFirstFire.QuadPart = -hnsRequestedDuration / 2; // negative means relative time
			//LONG lTimeBetweenFires = (LONG)hnsRequestedDuration / 2 / (10 * 1000); // convert to milliseconds

			BOOL bOK = SetWaitableTimer(hTimerWakeUp, &liFirstFire, lTimeBetweenFires, NULL, NULL, FALSE);
			if (!bOK)
			{
				LOG(ERROR) << "异常：CaptureAudio SetWaitableTimer failed";
				break;
			}
			LOG(INFO) << "CaptureAudio SetWaitableTimer OK";
			hr = pAudioClient->Start();  
			if (FAILED(hr)) {
			
				LOG(ERROR) << "异常：CaptureAudio pAudioClient->Start failed";
				break;
			}
			LOG(INFO) << "CaptureAudio  pAudioClient->Start OK";
			NotifyStatus(hWndMessage, CAPTURE_START, lTimeBetweenFires);
			bStarted = TRUE;

			HANDLE waitArray[2] = { hEventStop, hTimerWakeUp };
			DWORD dwWaitResult;
			UINT32 nNextPacketSize(0);
			BYTE *pData = NULL;
			UINT32 nNumFramesToRead;
			DWORD dwFlags;
			int byteCount = 0;
			int recFrame = 0;
			int temp = 0;
			//m_pDC = GetDC(hwndMainDlg); 
			sprintf_s(my_voiceprint.voice_mdl.voice_file.szFileName, "%s./config\\record\\vi%I64d.wav", sys.workPath.c_str(),time(NULL));
			 CreateWaveFile(pwfx, my_voiceprint.voice_mdl.voice_file.szFileName,&m_hFile);
			LOG(INFO) << "CaptureAudio  loop";
			while (TRUE)
			{ 
				dwWaitResult = WaitForMultipleObjects(sizeof(waitArray) / sizeof(waitArray[0]), waitArray, FALSE, INFINITE);
				if (WAIT_OBJECT_0 == dwWaitResult)
				{
					//停止录音
					break;
				}
				if (WAIT_OBJECT_0 + 1 != dwWaitResult)
				{
					NotifyStatus(hWndMessage, CAPTURE_ERROR);
					break;
				}

				hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);

				if (FAILED(hr))
				{
					NotifyStatus(hWndMessage, CAPTURE_ERROR);
					LOG(ERROR) << "异常：CaptureAudio pAudioCaptureClient->GetNextPacketSize failed";
					break;
				}

				if (nNextPacketSize == 0) continue;

				hr = pAudioCaptureClient->GetBuffer(
					&pData,
					&nNumFramesToRead,
					&dwFlags,
					NULL,
					NULL
				);
				if (FAILED(hr))
				{ 
					LOG(ERROR) << "异常：CaptureAudio pAudioCaptureClient->GetBuffer failed";
					NotifyStatus(hWndMessage, CAPTURE_ERROR);
					break;
				}

				//char cont[1024];
				//sprintf_s(cont, "nNumFramesToRead= %d \r\n hnsDefaultDevicePeriod= %d\r\n  ",(int)nNumFramesToRead,(long)hnsDefaultDevicePeriod);
				//LOG(INFO) << cont;

				if (0 != nNumFramesToRead)
				{
					NotifyData(hWndMessage, pData, nNumFramesToRead * pwfx->nBlockAlign);
					temp = 0;
					
					//BYTE * paramData = pData;
					//PostMessageA(hwndMainDlg, WM_BUILD_WAVE, NULL, LPARAM(paramData));
					for (int i = 0; i <nNumFramesToRead * pwfx->nBlockAlign; i++)
					{
						temp += abs(pData[i]);
					}
					//if (temp > nNumFramesToRead * pwfx->nBlockAlign * 128 + 1 * nNumFramesToRead * pwfx->nBlockAlign/100)
					if (temp > nNumFramesToRead * pwfx->nBlockAlign*50)
					{
						char * dest = (char *)pData;
						int outputlenth = mmioWrite(m_hFile, dest, nNumFramesToRead * pwfx->nBlockAlign);
						byteCount+= nNumFramesToRead;
						//delete dest;
						//memcpy(my_voiceprint.voice_mdl.pVoice  , pData, nNumFramesToRead * pwfx->nBlockAlign);
						//fwrite(pData, sizeof(BYTE), nNumFramesToRead * pwfx->nBlockAlign, my_voiceprint.voice_mdl.file_stream);
						//BYTE * paramData =  pData;
						//PostMessageA(hwndMainDlg, WM_BUILD_WAVE, NULL, LPARAM(paramData));
						//DrawWaveIn((char *)pData, hWndMessage);
						recFrame++;
						if (outputlenth != nNumFramesToRead * pwfx->nBlockAlign)
						{
							LOG(ERROR) << "outputlenth != nNumFramesToRead * pwfx->nBlockAlign";
						}
						//if (recFrame >= g_build_buff_num)
						if (byteCount >= (int)(AUDIO_RECORD_TIME_MILION_SECOND * (double)pwfx->nSamplesPerSec/1000))
						{
							if (GetFileSize(m_hFile, 0)<1)
							{
								recFrame = 0;
								continue;
							}
							else
							{
								mmioAscend(m_hFile, &m_MMCKInfoChild, 0);
								mmioAscend(m_hFile, &m_MMCKInfoParent, 0);
								mmioClose(m_hFile, 0);
								m_hFile = NULL;
								LOG(INFO) << "CaptureAudio 录音结束";
								//fclose(my_voiceprint.voice_mdl.file_stream);
								PostMessage(hwndMainDlg, WM_BUILD_GMM, 0, 0); 
							    //bool r=mq.push(NULL); 
								//LOG(INFO) <<"MQ push结果= "<< (int)r; 
								break;
							}

						}
					}


				}

				pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);


			}




		} while (FALSE);
	}
	catch (...)
	{
		LOG(ERROR) << "异常：录音异常，即将重新录音";
		PostMessage(hwndMainDlg, WM_RESTART_RECORD, 0, 0);
	}
	
 
	if (m_hFile)
	{
		mmioClose(m_hFile, 0);
		m_hFile = NULL;
	}

	if(hTask != NULL)
	{
		AvRevertMmThreadCharacteristics(hTask);
		hTask = NULL;
	}

	if(pAudioCaptureClient != NULL)
	{
		pAudioCaptureClient->Release(); 
		pAudioCaptureClient = NULL;
	}

	if(pwfx != NULL)
	{
		CoTaskMemFree(pwfx);
		pwfx = NULL;
	}

	if(hTimerWakeUp != NULL)
	{
		CancelWaitableTimer(hTimerWakeUp);
		CloseHandle(hTimerWakeUp);
		hTimerWakeUp = NULL;
	}

	if(pAudioClient != NULL)
	{
		if(bStarted)
		{
			pAudioClient->Stop();
			NotifyStatus(hWndMessage, CAPTURE_STOP);
		}

		pAudioClient->Release();
		pAudioClient = NULL;
	}

	return 0;
}

UINT __stdcall CaptureTheadProc(LPVOID param)
{
	CoInitialize(NULL); 
	LOG(INFO) << "CaptureTheadProc CoInitialize";
	capture_thread_data* pData = (capture_thread_data*)param;
	//HWND hWndMessage = pData->hWndMessage; 
	//HANDLE hEventStop = pData->hEventStop;
	//IMMDevice* pDevice = pData->pDevice;
	//HANDLE hEventStarted = pData->hEventStarted; 
	UINT nRet = CaptureAudio(pData->hWndMessage, pData->pDevice, pData->hEventStarted, pData->hEventStop);
	LOG(INFO) << "CaptureTheadProc CaptureAudio";
	CoUninitialize(); 
	LOG(INFO) << "CaptureTheadProc CoUninitialize";
	return nRet;
}

CPlaybackCaptureImpl::CPlaybackCaptureImpl()
	:m_hWndMessage(NULL), m_bInited(FALSE), m_pDevice(NULL), 
	m_pEventHandler(NULL), m_hEventStarted(NULL), m_hEventStop(NULL)
{

}

CPlaybackCaptureImpl::~CPlaybackCaptureImpl()
{
	if(m_bInited) Destroy();
}

VOID CPlaybackCaptureImpl::OnThreadEnd()
{
	if(m_hThreadCapture != NULL)
	{
		CloseHandle(m_hThreadCapture);
		m_hThreadCapture = NULL;
	}
}

IMMDevice* CPlaybackCaptureImpl::GetDefaultDevice()
{
	IMMDevice* pDevice = NULL;
	IMMDeviceEnumerator *pMMDeviceEnumerator = NULL;
	HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, 
        __uuidof(IMMDeviceEnumerator),
        (void**)&pMMDeviceEnumerator);
	if(FAILED(hr)) return NULL;

    hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pMMDeviceEnumerator->Release();
	
	return pDevice;
}

DWORD CPlaybackCaptureImpl::getDevcieFrequency()
{
	IMMDevice* a_device = NULL;
	IAudioClient *pAudioClient = NULL;
	WAVEFORMATEX *pwfx = NULL;
	DWORD defaultFrequency = 0;
	HRESULT hr; 
	CoInitialize(NULL); 
	try
	{
		do
		{
			a_device = GetDefaultDevice();
			if (a_device == NULL) {
				LOG(ERROR) << " 异常：CPlaybackCaptureImpl:: getDevcieFrequency a_device== NULL";
				break;
			}
			hr = a_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
			if (FAILED(hr)) {
				LOG(ERROR) << "异常：getDevcieFrequency pDevice->Activate failed";
				break;
			}
			hr = pAudioClient->GetMixFormat(&pwfx);
			if (FAILED(hr)) {

				LOG(ERROR) << "异常：getDevcieFrequency pAudioClient->GetMixFormat failed";
				break;
			}
			defaultFrequency = pwfx->nSamplesPerSec;
		} while (false);
		
	}
	catch (const std::exception&e)
	{
		LOG(ERROR) << "异常：getDevcieFrequency 异常"<<e.what();
	} 
	CoUninitialize();
	if (pwfx != NULL)
	{
		CoTaskMemFree(pwfx);
		pwfx = NULL;
	}
	if (pAudioClient != NULL)
	{
		pAudioClient->Release();
		pAudioClient = NULL;
	}

	if (m_pDevice != NULL)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	return defaultFrequency;
}

BOOL CPlaybackCaptureImpl::Initialize(IPlaybackCaptureEvent* pHandler)
{
	if (m_bInited) {
		LOG(ERROR) << " 异常：CPlaybackCaptureImpl::Initialize m_bInited ==true";
		return TRUE;
	} 

	m_pEventHandler = pHandler;

	do
	{
		if(!m_sClassRegister.IsRegistered())
		{
			m_sClassRegister.Register();
		}

		m_hWndMessage = CreateWindow(AUDIO_CAPTURE_CLASS, NULL, WS_POPUP, 
		0, 0, 0, 0, HWND_MESSAGE, NULL, g_hInstance, this);
		if (m_hWndMessage == NULL) {
			LOG(ERROR) << " 异常：CPlaybackCaptureImpl::Initialize CreateWindow m_hWndMessage == NULL";
			break;
		} 

		m_pDevice = GetDefaultDevice();
		if (m_pDevice == NULL) {
			LOG(ERROR) << " 异常：CPlaybackCaptureImpl::Initialize GetDefaultDevice == NULL";
			break;
		} 

		m_hEventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_hEventStop == NULL) {
			LOG(INFO) << " 异常：CPlaybackCaptureImpl::Initialize CreateEvent m_hEventStop== NULL";
			break;
		} 

		m_hEventStarted = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_hEventStarted == NULL)
		{
			LOG(ERROR) << " 异常：CPlaybackCaptureImpl::Initialize CreateEvent m_hEventStarted== NULL";
			break;
		}

		m_bInited = TRUE;
	
	}while(FALSE);

	if(!m_bInited)
	{
		LOG(ERROR) << " 异常：CPlaybackCaptureImpl::Initialize 初始化失败 Destroy";
		Destroy();
	}

	return m_bInited;
}

VOID CPlaybackCaptureImpl::Destroy()
{
	if(m_hWndMessage != NULL
		&& ::IsWindow(m_hWndMessage))
	{
		DestroyWindow(m_hWndMessage);
	}

	m_hWndMessage = NULL;

	if(m_pDevice != NULL)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	if(m_hEventStop != NULL)
	{
		CloseHandle(m_hEventStop);
		m_hEventStop = NULL;
	}

	if(m_hEventStarted != NULL)
	{
		CloseHandle(m_hEventStarted);
		m_hEventStarted = NULL;
	}

	m_bInited = FALSE;
}

BOOL CPlaybackCaptureImpl::IsInited() const
{
	return m_bInited;
}

BOOL CPlaybackCaptureImpl::IsCapturing() const
{
	return m_hThreadCapture != NULL ;
}


BOOL CPlaybackCaptureImpl::IsDeviceOK() const
{
	return m_pDevice != NULL && m_hEventStop != NULL && m_hWndMessage != NULL && m_hEventStarted != NULL;
}

BOOL CPlaybackCaptureImpl::Start()
{
	if (!m_bInited) 
	{
		LOG(ERROR) << " 异常：CPlaybackCaptureImpl :Start m_bInited=false return " ;
		return FALSE;
	} 
	if (m_hThreadCapture != NULL) {
		LOG(INFO) << "CPlaybackCaptureImpl::Start m_hThreadCapture != NULL";
		return TRUE;
	}

	if (m_hEventStop == NULL || m_hWndMessage == NULL || m_pDevice == NULL || m_hEventStarted == NULL)
	{
		LOG(INFO) << "CPlaybackCaptureImpl::Start m_hEventStop == NULL || m_hWndMessage == NULL || m_pDevice == NULL || m_hEventStarted == NULL";
		return FALSE;
	}
	capture_thread_data data;
	data.hEventStop = m_hEventStop;
	data.hWndMessage = m_hWndMessage;
	data.pDevice = m_pDevice;
	data.hEventStarted = m_hEventStarted;

	try
	{
		m_hThreadCapture = (HANDLE)_beginthreadex(NULL, 0, &CaptureTheadProc, &data, 0, NULL);
	}
	catch (const std::exception&)
	{
		LOG(ERROR) << "异常：CPlaybackCaptureImpl::Start _beginthreadex 失败";
		return FALSE;
	} 
	if (m_hThreadCapture == NULL) {
		LOG(INFO) << "CPlaybackCaptureImpl::Start m_hThreadCapture == NULL";
		return FALSE;
	}
	LOG(INFO) << "CPlaybackCaptureImpl::Start _beginthreadex";
	HANDLE arWaits[2] = {m_hEventStarted, m_hThreadCapture};
	DWORD dwWaitResult = WaitForMultipleObjects(sizeof(arWaits)/sizeof(arWaits[0]), arWaits, FALSE, INFINITE);
	if(dwWaitResult != WAIT_OBJECT_0)
	{
		Stop();
		LOG(INFO) << " CPlaybackCaptureImpl :Start dwWaitResult != WAIT_OBJECT_0 Stop()";
		return FALSE;
	}

	return TRUE;
}

VOID CPlaybackCaptureImpl::Stop()
{
	if (!m_bInited) {
		LOG(INFO) << "CPlaybackCaptureImpl::Stop m_bInited=false";
		return;
	}

	if(m_hEventStop != NULL
		&& m_hThreadCapture != NULL)
	{
		try
		{
			SetEvent(m_hEventStop);
		}
		catch (...)
		{
			LOG(ERROR) << "异常:CPlaybackCaptureImpl::Stop SetEvent 异常";
		} 
		OnThreadEnd();
		LOG(INFO) << "CPlaybackCaptureImpl::Stop 停止事件，清理m_hThreadCapture";
	}
}

////////////////////  CPlaybackCaptureImpl  /////////////////


////////////////////  CPlaybackCapture ////////////////

CPlaybackCapture::CPlaybackCapture()
{
	m_pImpl = new CPlaybackCaptureImpl();
	//cout << "" << endl;
}

CPlaybackCapture::~CPlaybackCapture()
{
	if(m_pImpl != NULL)
	{
		delete m_pImpl;
	}
	
}

DWORD CPlaybackCapture::getDevcieFrequency()
{

	if (m_pImpl != NULL)
	{
		LOG(INFO) << " CPlaybackCapture::getDevcieFrequency";
		return m_pImpl->getDevcieFrequency();
	}

	else
	{
		LOG(ERROR) << " 异常：CPlaybackCapture::getDevcieFrequency m_pImpl == NULL";
		return 0;
	}

}


BOOL CPlaybackCapture::Initialize(IPlaybackCaptureEvent* pHandler)
{
	if (m_pImpl != NULL)
	{
		LOG(INFO) << " CPlaybackCapture::Initialize m_pImpl != NULL";
		return m_pImpl->Initialize(pHandler);
	}
		
	else
	{
		LOG(ERROR) << " 异常：CPlaybackCapture::Initialize m_pImpl == NULL";
		return FALSE;
	}
		
}

VOID CPlaybackCapture::Destroy()
{
	if(m_pImpl != NULL)
		 m_pImpl->Destroy();
}

BOOL CPlaybackCapture::Start()
{
	if (m_pImpl != NULL)
	{
		LOG(INFO) << "CPlaybackCapture ::Start m_pImpl != NULL ";
		return m_pImpl->Start();
	}
		
	else
	{
		LOG(INFO) << "CPlaybackCapture ::Start m_pImpl == NULL ";
		return FALSE;
	}
		
}

VOID CPlaybackCapture::Stop()
{
	if (m_pImpl != NULL)
	{ 
		m_pImpl->Stop();
	}
		
}

BOOL CPlaybackCapture::IsInited() const
{
	if(m_pImpl != NULL)
		return m_pImpl->IsInited();
	else
		return FALSE;
}

BOOL CPlaybackCapture::IsCapturing() const
{
	if(m_pImpl != NULL)
		return m_pImpl->IsCapturing()  ;
	else
		return FALSE;	
}

BOOL CPlaybackCapture::IsDeviceOK() const
{
	if (m_pImpl != NULL)
		return m_pImpl->IsDeviceOK();
	else
		return FALSE;
}

////////////////////  CPlaybackCapture ////////////////

bool CreateWaveFile(WAVEFORMATEX *sampleFormat,char* fileName, HMMIO *m_hFile)
{
	MMRESULT    mmResult = 0;

	ZeroMemory(&m_MMCKInfoParent, sizeof(MMCKINFO));
	ZeroMemory(&m_MMCKInfoChild, sizeof(MMCKINFO));
	ZeroMemory(&m_MMCKInfoData, sizeof(MMCKINFO));
	if (*m_hFile != NULL)
	{
		mmioClose(*m_hFile, 0);
		delete m_hFile; 
	}
	LPWSTR lpszPath = new WCHAR[strlen(fileName) + 1];
	ConvertCharToLPWSTR(fileName, lpszPath); 
	*m_hFile = mmioOpen(lpszPath, NULL, MMIO_CREATE | MMIO_WRITE | MMIO_EXCLUSIVE | MMIO_ALLOCBUF);
	delete lpszPath;
	lpszPath = NULL;
	if (*m_hFile == NULL)
	{
		return NULL;
	}
	/**/
	m_MMCKInfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmResult = mmioCreateChunk(*m_hFile, &m_MMCKInfoParent, MMIO_CREATERIFF);
	m_MMCKInfoChild.ckid = mmioFOURCC('f', 'm', 't', ' ');
	m_MMCKInfoChild.cksize = sizeof(WAVEFORMATEX) + sampleFormat->cbSize;
	mmResult = mmioCreateChunk(*m_hFile, &m_MMCKInfoChild, 0);
	//mmResult = mmioWrite(m_hFile, (char*)&sampleFormat, sizeof(WAVEFORMATEX) + sampleFormat->cbSize);
	mmResult = mmioWrite(*m_hFile, (char*)sampleFormat, sizeof(WAVEFORMATEX) + sampleFormat->cbSize);
	mmResult = mmioAscend(*m_hFile, &m_MMCKInfoChild, 0);
	m_MMCKInfoChild.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmResult = mmioCreateChunk(*m_hFile, &m_MMCKInfoChild, 0);
	
	return true;
}
