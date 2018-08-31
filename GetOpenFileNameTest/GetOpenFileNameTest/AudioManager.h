#ifndef __AUDIO_MANAGER_H__
#define __AUDIO_MANAGER_H__ 

#include "AudioRender.h"
#include "PlaybackAudioCapture.h"
#include <list>
#include <Windows.h>
typedef struct _Audio_Data 
{
	INT nDataLen;
	LPBYTE pData;
} Audio_Data;

class IPlaybackEvent
{
public:
	virtual VOID OnPlaybackEnd() = 0;
};

class CAudioManager: public IPlaybackCaptureEvent,
					 public IAudioRenderEvent
{
public:
	CAudioManager(IPlaybackEvent* pEvent);
	~CAudioManager();

	
	DWORD  CAudioManager::getDeviceFrequency();

	BOOL StartCapture();
	VOID StopCapture();
	BOOL IsCapturing() const;

	BOOL StartPlayback();
	VOID StopPlayback();
	BOOL IsPlaybacking() const;

	BOOL CanPlay() const;

public:
	//IPlaybackCaptureEvent
	virtual VOID OnCatpureStart(DWORD dwInterval);
	virtual VOID OnCaptureStop();
	virtual VOID OnAdjustCaptureFormat(WAVEFORMATEX* pFormat);
	virtual VOID OnCatpureData(LPBYTE pData, INT nDataLen);

	//IAudioRenderEvent
	virtual VOID OnRenderStart();
	virtual VOID OnRenderStop();
	virtual VOID OnAdjustRenderFormat(WAVEFORMATEX* pFormat);
	virtual VOID OnGetRenderData(LPBYTE pData, INT nDataLen);

protected:
	VOID ClearData();

private:
	CPlaybackCapture m_capture;
	CAudioRender m_render;
	WAVEFORMATEX* m_pFormat=NULL;

	std::list<Audio_Data> m_dataList;
	INT m_nOffset;
	std::list<Audio_Data>::iterator m_itrCurrent;

	IPlaybackEvent* m_pPlaybackEventHandler;
};







#endif //__AUDIO_MANAGER_H__