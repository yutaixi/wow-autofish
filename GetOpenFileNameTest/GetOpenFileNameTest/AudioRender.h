#ifndef __AUDIO_RENDER_H__
#define __AUDIO_RENDER_H__
#include <Windows.h>
#include <mmreg.h> 
class IAudioRenderEvent
{
public:
	virtual ~IAudioRenderEvent() {NULL;}	

	virtual VOID OnRenderStart() = 0;
	virtual VOID OnRenderStop() = 0;

	virtual VOID OnAdjustRenderFormat(WAVEFORMATEX* pFormat) = 0;
	virtual VOID OnGetRenderData(LPBYTE pData, INT nDataLen) = 0;
};


class CAudioRenderImpl;

class CAudioRender
{
public:
	CAudioRender();
	~CAudioRender();

	BOOL Initialize(IAudioRenderEvent* pHandler);
	VOID Destroy();

	BOOL Start();
	VOID Stop();

	BOOL IsInited() const;
	BOOL IsRendering() const;

private:
	CAudioRender(const CAudioRender&);
	CAudioRender& operator = (const CAudioRender&);

private:
	CAudioRenderImpl* m_pImpl=NULL;
};

#endif //__AUDIO_RENDER_H__