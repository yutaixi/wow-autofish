#ifndef __CLASS_REGISTER_H__
#define __CLASS_REGISTER_H__

#include <Windows.h>
//extern HINSTANCE g_hInstance;
 static HINSTANCE g_hInstance;
typedef LRESULT (CALLBACK *MessageCallbackProc)(HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam);

class CClassRegister 
{
public:
	CClassRegister(LPCTSTR lpszClassName, MessageCallbackProc pMessageHandler)
		:m_bRegistered(FALSE)
	{
		lstrcpyn(m_szClassName, lpszClassName, 128);
		m_pMesasgeHander = pMessageHandler;
	}

	~CClassRegister()
	{
		if(m_bRegistered)
		{
			UnregisterClass(m_szClassName, g_hInstance);
		}
	}

	BOOL IsRegistered()const
	{
		return m_bRegistered;
	}

	BOOL Register()
	{
		WNDCLASSEX wc;
		memset(&wc,0,sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.hInstance = g_hInstance;
		wc.lpszClassName = m_szClassName;
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpfnWndProc = m_pMesasgeHander;
		wc.hCursor = ::LoadCursor(NULL,IDC_ARROW);
		ATOM atom = RegisterClassEx(&wc);
		return (atom != NULL);
	}

private:
	BOOL m_bRegistered;
	TCHAR m_szClassName[128];
	MessageCallbackProc m_pMesasgeHander;
};

#endif //__CLASS_REGISTER_H__