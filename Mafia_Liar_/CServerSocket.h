#pragma once
#include "CClientSocket.h"

#define WM_ACCEPT_SOCKET WM_USER+1
// CServerSocket 명령 대상

class CServerSocket : public CSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();
	void SetWnd(HWND hWnd);
	HWND m_hWnd;//윈도우 접근을 위한 핸들 설정
	virtual void OnAccept(int nErrorCode);
};


