// CServerSocket.cpp: 구현 파일
//

#include "pch.h"
#include "Mafia_Liar_.h"
#include "CServerSocket.h"


// CServerSocket

CServerSocket::CServerSocket()
{
}

CServerSocket::~CServerSocket()
{
}

// CServerSocket 멤버 함수

void CServerSocket::SetWnd(HWND hWnd)//핸들 받는 함수
{
    m_hWnd = hWnd;
}

void CServerSocket::OnAccept(int nErrorCode)//클라이언트가 서버와 연결 할 때 작동
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    CClientSocket* pClient = new CClientSocket; //Client 소켓 포인터 추가//클라이언트의 번호?가 들어간다.

    if (Accept(*pClient))  //Listen에서 클라이언트 접속을 확인하면
    {
        pClient->SetWnd(m_hWnd); //Client소켓에 메인핸들 연결
    }
    else    //클라이언트 접속 문제시
    {
        delete pClient;
        AfxMessageBox(_T("ERROR : Failed can't accept new Client!"));
    }

    SendMessage(m_hWnd, WM_ACCEPT_SOCKET, 0, (LPARAM)pClient);//view의 OnAcceptSocket로 pClient 보내기
    CSocket::OnAccept(nErrorCode);
}
