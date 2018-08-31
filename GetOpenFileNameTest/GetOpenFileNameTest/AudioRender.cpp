#include "stdafx.h" 
#include "AudioRender.h"
#include "ClassRegister.h" 
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <process.h>
#include <avrt.h>
#pragma comment(lib, "avrt.lib")
#pragma comment(lib, "winmm.lib") 
#define AUDIO_RENDER_CLASS _T("audio_render_message_class")

enum RENDER_STATUS 
{
	RENDER_START,
	RENDER_STOP,
	RENDER_ERROR
};

#define WM_RENDER_STATUS		WM_USER+100
#define WM_WAVE_FORMAT			WM_USER+101
#define WM_RENDER_DATA			WM_USER+102

struct render_thread_data
{
	HANDLE hEventStarted;
	HANDLE hEventStop;
	HWND hWndMessage;
	IMMDevice* pDevice;
};

class CAudioRenderImpl
{
public:
	CAudioRenderImpl();
	~CAudioRenderImpl();

	BOOL Initialize(IAudioRenderEvent* pHandler);
	VOID Destroy();

	BOOL Start();
	VOID Stop();

	BOOL IsInited() const;
	BOOL IsRendering() const;

	IAudioRenderEvent* GetEventHandler() const	{ return m_pEventHandler;}
	VOID OnThreadEnd();

private:
	IMMDevice* GetDefaultDevice();

private:
	HWND m_hWndMessage;
	HANDLE m_hEventStarted;
	HANDLE m_hEventStop;
	IMMDevice* m_pDevice;

	HANDLE m_hThreadRender;

	static CClassRegister m_sClassRegister;
	BOOL m_bInited;

	IAudioRenderEvent* m_pEventHandler;
};

LRESULT CALLBACK AudioRenderMessageProc(HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled(FALSE);
	LRESULT lRet(0);

	IAudioRenderEvent* pEventHandler = NULL;
	CAudioRenderImpl* pThis = (CAudioRenderImpl*)GetWindowLong(hWnd, GWL_USERDATA);
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

	case WM_RENDER_STATUS:
		{
			if(wParam == RENDER_START)
			{
				if(pEventHandler != NULL) pEventHandler->OnRenderStart();
			}
			else if(wParam == RENDER_STOP)
			{
				if(pEventHandler != NULL) pEventHandler->OnRenderStop();
				if(pThis != NULL) pThis->OnThreadEnd();
			}


			bHandled = TRUE;
		}
		break;

	case WM_WAVE_FORMAT:
		{
			if(pEventHandler != NULL)
			{
				pEventHandler->OnAdjustRenderFormat((WAVEFORMATEX*)lParam);
			}

			bHandled = TRUE;
		}
		break;

	case WM_RENDER_DATA:
		{
			if(pEventHandler != NULL)
			{
				pEventHandler->OnGetRenderData((LPBYTE)lParam, wParam);
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

	return lRet;
}

CClassRegister CAudioRenderImpl::m_sClassRegister(AUDIO_RENDER_CLASS, AudioRenderMessageProc);

static VOID NotifyStatus(HWND hWndMesasge, RENDER_STATUS eStatus, DWORD dwUserData = 0)
{
	::SendMessage(hWndMesasge, WM_RENDER_STATUS, (WPARAM)eStatus, dwUserData);
}

static VOID NotifyWaveFormat(HWND hWndMessage, WAVEFORMATEX* pFormat)
{
	::SendMessage(hWndMessage, WM_WAVE_FORMAT, 0, (LPARAM)(WAVEFORMATEX*)pFormat);
}

static VOID NotifyData(HWND hWndMessage, LPBYTE pData, INT nDataLen)
{
	::SendMessage(hWndMessage, WM_RENDER_DATA, nDataLen, (LPARAM)pData);
}

CAudioRenderImpl::CAudioRenderImpl()
	:m_hWndMessage(NULL), m_bInited(FALSE), m_pDevice(NULL), 
	m_pEventHandler(NULL), m_hEventStarted(NULL), m_hEventStop(NULL)
{

}

CAudioRenderImpl::~CAudioRenderImpl()
{
	if(m_bInited) Destroy();
}

IMMDevice* CAudioRenderImpl::GetDefaultDevice()
{
	IMMDevice* pDevice = NULL;
	IMMDeviceEnumerator *pMMDeviceEnumerator = NULL;
	HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, 
        __uuidof(IMMDeviceEnumerator),
        (void**)&pMMDeviceEnumerator
    );
	if(FAILED(hr)) return NULL;

    hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pMMDeviceEnumerator->Release();
	return pDevice;
}

UINT RenderAudio(HWND hWndMessage, IMMDevice* pDevice, HANDLE hEventStarted, HANDLE hEventStop)
{
	HRESULT hr;
	IAudioClient *pAudioClient = NULL;
	WAVEFORMATEX *pwfx = NULL;
	IAudioRenderClient *pAudioRenderClient = NULL;
	DWORD nTaskIndex = 0;
	HANDLE hTask = NULL;
	BOOL bStarted(FALSE);
	UINT32 nFrameBufferSize(0);
	HANDLE hEventRequestData = NULL;
	do 
	{
		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
		if(FAILED(hr)) break;

		hr = pAudioClient->GetMixFormat(&pwfx);
		if (FAILED(hr)) break;

		SetEvent(hEventStarted);
		 
		NotifyWaveFormat(hWndMessage, pwfx);
		hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, pwfx, 0);
		if(FAILED(hr)) break;

		hr = pAudioClient->GetBufferSize(&nFrameBufferSize);
		if(FAILED(hr)) break;

		hr = pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&pAudioRenderClient);
		if(FAILED(hr)) break;

		hTask = AvSetMmThreadCharacteristics(TEXT("Playback"), &nTaskIndex);
		if (NULL == hTask) break;

		hEventRequestData = CreateEvent(NULL, FALSE, FALSE, NULL);
		if(hEventRequestData == NULL) break;

		hr = pAudioClient->SetEventHandle(hEventRequestData);
		if(FAILED(hr)) break;

		hr = pAudioClient->Start();
		if(FAILED(hr)) break;

		NotifyStatus(hWndMessage, RENDER_START, 0);
		bStarted = TRUE;

		HANDLE waitArray[2] = { hEventStop, hEventRequestData };
		DWORD dwWaitResult;
		BYTE *pData = NULL;
		UINT32 nFramesOfPadding(0);
		UINT nNeedDataLen(0);
		while(TRUE)
		{
			dwWaitResult = WaitForMultipleObjects(sizeof(waitArray)/sizeof(waitArray[0]), waitArray, FALSE, INFINITE);
			if(WAIT_OBJECT_0 == dwWaitResult) break;

			if (WAIT_OBJECT_0 + 1 != dwWaitResult)
			{
				NotifyStatus(hWndMessage, RENDER_ERROR);
				break;
			}

			hr = pAudioClient->GetCurrentPadding(&nFramesOfPadding);
			if(FAILED(hr)) break;

			if(nFrameBufferSize == nFramesOfPadding) continue;

			nNeedDataLen = nFrameBufferSize - nFramesOfPadding;
			hr = pAudioRenderClient->GetBuffer(nNeedDataLen, &pData);
			if(FAILED(hr))break;

			NotifyData(hWndMessage, pData, nNeedDataLen * pwfx->nBlockAlign);

			pAudioRenderClient->ReleaseBuffer(nNeedDataLen, 0);	
		}

	}while(FALSE);

	if(hEventRequestData != NULL)
	{
		CloseHandle(hEventRequestData);
		hEventRequestData = NULL;
	}

	if(hTask != NULL)
	{
		AvRevertMmThreadCharacteristics(hTask);
		hTask = NULL;
	}

	if(pAudioRenderClient != NULL)
	{
		pAudioRenderClient->Release();
		pAudioRenderClient = NULL;
	}

	if(pwfx != NULL)
	{
		CoTaskMemFree(pwfx);
		pwfx = NULL;
	}

	if(pAudioClient != NULL)
	{
		if(bStarted)
		{
			pAudioClient->Stop();
			NotifyStatus(hWndMessage, RENDER_STOP);
		}

		pAudioClient->Release();
		pAudioClient = NULL;
	}

	return 0;
}

UINT __stdcall RenderTheadProc(LPVOID param)
{
	CoInitialize(NULL);

	render_thread_data* pData = (render_thread_data*)param;
	HWND hWndMessage = pData->hWndMessage;
	HANDLE hEventStop = pData->hEventStop;
	IMMDevice* pDevice = pData->pDevice;
	HANDLE hEventStarted = pData->hEventStarted;

	UINT nRet = RenderAudio(hWndMessage, pDevice, hEventStarted, hEventStop);

	CoUninitialize();

	return nRet;
}

BOOL CAudioRenderImpl::Initialize(IAudioRenderEvent* pHandler)
{
	if(m_bInited) return TRUE;

	m_pEventHandler = pHandler;

	do
	{
		if(!m_sClassRegister.IsRegistered())
		{
			m_sClassRegister.Register();
		}

		m_hWndMessage = CreateWindow(AUDIO_RENDER_CLASS, NULL, WS_POPUP, 
		0, 0, 0, 0, HWND_MESSAGE, NULL, g_hInstance, this);
		if(m_hWndMessage == NULL) break;

		m_pDevice = GetDefaultDevice();
		if(m_pDevice == NULL) break;

		m_hEventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
		if(m_hEventStop == NULL) break;

		m_hEventStarted = CreateEvent(NULL, TRUE, FALSE, NULL);
		if(m_hEventStarted == NULL) break;

		m_bInited = TRUE;
	
	}while(FALSE);

	if(!m_bInited)
	{
		Destroy();
	}

	return m_bInited;
}

VOID CAudioRenderImpl::Destroy()
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

BOOL CAudioRenderImpl::Start()
{
	if(!m_bInited) return FALSE;
	if(m_hThreadRender != NULL) return TRUE;

	render_thread_data data;
	data.hEventStop = m_hEventStop;
	data.hWndMessage = m_hWndMessage;
	data.pDevice = m_pDevice;
	data.hEventStarted = m_hEventStarted;

	m_hThreadRender = (HANDLE)_beginthreadex(NULL, 0, &RenderTheadProc, &data, 0, NULL);
	if(m_hThreadRender == NULL) return FALSE;

	HANDLE arWaits[2] = {m_hEventStarted, m_hThreadRender};
	DWORD dwWaitResult = WaitForMultipleObjects(sizeof(arWaits)/sizeof(arWaits[0]), arWaits, FALSE, INFINITE);
	if(dwWaitResult != WAIT_OBJECT_0)
	{
		Stop();
		return FALSE;
	}

	return TRUE;
}

VOID CAudioRenderImpl::OnThreadEnd()
{
	if(m_hThreadRender != NULL)
	{
		CloseHandle(m_hThreadRender);
		m_hThreadRender = NULL;
	}
}

VOID CAudioRenderImpl::Stop()
{
	if(!m_bInited) return;

	if(m_hEventStop != NULL
		&& m_hThreadRender != NULL)
	{
		SetEvent(m_hEventStop);
		OnThreadEnd();
	}
}

BOOL CAudioRenderImpl::IsInited() const
{
	return m_bInited;
}

BOOL CAudioRenderImpl::IsRendering() const
{
	return m_hThreadRender != NULL;
}

CAudioRender::CAudioRender()
{
	m_pImpl = new CAudioRenderImpl;
}

CAudioRender::~CAudioRender()
{
	if(m_pImpl != NULL)
	{
		delete m_pImpl;
	}
}

BOOL CAudioRender::Initialize(IAudioRenderEvent* pHandler)
{
	if(m_pImpl != NULL)
		return m_pImpl->Initialize(pHandler);
	else 
		return FALSE;
}

VOID CAudioRender::Destroy()
{
	if(m_pImpl != NULL)
		m_pImpl->Destroy();
}

BOOL CAudioRender::Start()
{
	if(m_pImpl != NULL)
		return m_pImpl->Start();
	else
		return FALSE;
}

VOID CAudioRender::Stop()
{
	if(m_pImpl != NULL)
		return m_pImpl->Stop();
}

BOOL CAudioRender::IsInited() const
{
	if(m_pImpl != NULL)
		return m_pImpl->IsInited();
	else
		return FALSE;
}

BOOL CAudioRender::IsRendering() const
{
	if(m_pImpl != NULL)
		return m_pImpl->IsRendering();
	else
		return FALSE;
}


