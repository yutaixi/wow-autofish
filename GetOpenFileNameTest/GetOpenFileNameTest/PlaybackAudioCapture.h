#ifndef __PLAYBACK_AUDIO_CAPTURE_H__
#define __PLAYBACK_AUDIO_CAPTURE_H__

#include <mmreg.h>

class IPlaybackCaptureEvent
{
public:
	virtual ~IPlaybackCaptureEvent() {NULL;}	

	virtual VOID OnCatpureStart(DWORD dwInterval) = 0;
	virtual VOID OnCaptureStop() = 0;

	virtual VOID OnAdjustCaptureFormat(WAVEFORMATEX* pFormat) = 0;
	virtual VOID OnCatpureData(LPBYTE pData, INT nDataLen) = 0;
};

class CPlaybackCaptureImpl;

class CPlaybackCapture
{
public:
	CPlaybackCapture();
	~CPlaybackCapture();

	BOOL Initialize(IPlaybackCaptureEvent* pHandler);
	VOID Destroy();

	BOOL Start();
	VOID Stop();

	BOOL IsInited() const;
	BOOL IsCapturing() const;
	BOOL IsDeviceOK() const;
	DWORD getDevcieFrequency();
private:
	CPlaybackCapture(const CPlaybackCapture&);
	CPlaybackCapture& operator = (const CPlaybackCapture&);

private:
	CPlaybackCaptureImpl* m_pImpl;
};

#endif //__PLAYBACK_AUDIO_CAPTURE_H__