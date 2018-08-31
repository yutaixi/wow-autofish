#include "stdafx.h"
#include "AudioManager.h"
#include "logger.h" 

#ifndef _min
#define _min(a,b)            (((a) < (b)) ? (a) : (b))
#endif //_min



CAudioManager::CAudioManager(IPlaybackEvent* pEvent)
	:m_pPlaybackEventHandler(pEvent)
{
	m_pFormat = NULL;
	m_nOffset = 0;
}

CAudioManager::~CAudioManager()
{
	ClearData();
}

DWORD  CAudioManager::getDeviceFrequency()
{ 
	return m_capture.getDevcieFrequency();
}


BOOL CAudioManager::StartCapture()
{
	BOOL bRet = m_capture.IsCapturing();
	if(!bRet || !m_capture.IsDeviceOK())
	{
		m_capture.Stop(); 
		LOG(INFO) << "StartCapture bRet=" << bRet;
		LOG(INFO) << "m_capture.Stop(); IsDeviceOK="<< m_capture.IsDeviceOK();
		if(m_capture.Initialize(this))
		{
			LOG(INFO) << " m_capture.Start(); ";
			bRet = m_capture.Start();
			
		}
	}

	return bRet;
}

VOID CAudioManager::StopCapture()
{
	if(m_capture.IsCapturing())
	{
		m_capture.Stop();
		m_capture.Destroy();
		printf("ֹͣ");
	}
}

BOOL CAudioManager::IsCapturing() const
{
	return m_capture.IsCapturing();
}

BOOL CAudioManager::StartPlayback()
{
	BOOL bRet = m_render.IsRendering();

	if(!bRet)
	{
		if(m_render.Initialize(this))
		{
			bRet = m_render.Start();
		}
	}

	return bRet;
}

VOID CAudioManager::StopPlayback()
{
	if(m_render.IsRendering())
	{
		m_render.Stop();
		m_render.Destroy();
	}
}

BOOL CAudioManager::IsPlaybacking() const
{
	return m_render.IsRendering();
}

BOOL CAudioManager::CanPlay() const
{
	return !m_dataList.empty();
}

VOID CAudioManager::ClearData()
{
	LPBYTE p = (LPBYTE)m_pFormat;
	delete []p;
	m_pFormat = NULL;

	std::list<Audio_Data>::iterator itr = m_dataList.begin();
	while(itr != m_dataList.end())
	{
		Audio_Data& item = *itr;
		delete []item.pData;
		++itr;
	}
	m_dataList.clear();
}

VOID CAudioManager::OnCatpureStart(DWORD dwInterval)
{
	
}

VOID CAudioManager::OnCaptureStop()
{

}

VOID CAudioManager::OnAdjustCaptureFormat(WAVEFORMATEX* pFormat)
{
	ClearData();	
	/*
	INT nDataLen = sizeof(WAVEFORMATEX) + pFormat->cbSize;
	LPBYTE pData = new BYTE[nDataLen];
	if(pData != NULL)
	{
		memcpy_s(pData, nDataLen, pFormat, nDataLen);
		m_pFormat = (WAVEFORMATEX*)pData;
	}
	*/
}

VOID CAudioManager::OnCatpureData(LPBYTE pData, INT nDataLen)
{
	/*
	Audio_Data item;
	item.nDataLen = nDataLen;
	item.pData = new BYTE[nDataLen];
	if(item.pData != NULL)
	{
		memcpy_s(item.pData, nDataLen, pData, nDataLen);
		m_dataList.push_back(item);
	}
	*/
}


VOID CAudioManager::OnRenderStart()
{
	m_nOffset = 0;
	m_itrCurrent = m_dataList.begin();
}

VOID CAudioManager::OnRenderStop()
{

}

VOID CAudioManager::OnAdjustRenderFormat(WAVEFORMATEX* pFormat)
{
	if(pFormat == NULL) return;

	INT nDataLen1 = sizeof(WAVEFORMATEX) + pFormat->cbSize;
	INT nDataLen2 = sizeof(WAVEFORMATEX) + m_pFormat->cbSize;
	if(nDataLen1 == nDataLen2)
	{
		memcpy_s(pFormat,nDataLen1,  m_pFormat, nDataLen2);
	}
	else
	{
		memcpy_s(pFormat,nDataLen1,  m_pFormat, sizeof(WAVEFORMATEX));
	}
}

VOID CAudioManager::OnGetRenderData(LPBYTE pData, INT nDataLen)
{
	INT nCopyed = 0;
	INT nNeedCopy = nDataLen;
	BOOL bEnd(FALSE);

	if(m_itrCurrent == m_dataList.end()) 
	{
		m_itrCurrent = m_dataList.begin();
		m_nOffset = 0;
	}

	while(nCopyed < nDataLen )
	{
		Audio_Data& item = *m_itrCurrent;
		INT nItemLeftDataLen = item.nDataLen - m_nOffset;
		INT nToCopy = _min(nNeedCopy, nItemLeftDataLen);
		if(nToCopy > 0)
		{
			memcpy_s(pData + nCopyed, nDataLen - nCopyed, item.pData + m_nOffset, nToCopy);
		}
		m_nOffset += nToCopy;
		nCopyed += nToCopy;
		nNeedCopy -= nToCopy;

		if(m_nOffset >= item.nDataLen)
		{
			++m_itrCurrent;
			m_nOffset = 0;

			if(m_itrCurrent == m_dataList.end())
			{
				bEnd = TRUE;
				break;
			}
		}
	}

	if(bEnd)
	{
		if(m_pPlaybackEventHandler != NULL)
		{
			m_pPlaybackEventHandler->OnPlaybackEnd();
		}
	}
}



