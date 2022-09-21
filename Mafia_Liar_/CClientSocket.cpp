// CClientSocket.cpp: 구현 파일
//

#include "pch.h"
#include "Mafia_Liar_.h"
#include "CClientSocket.h"


// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 멤버 함수

void CClientSocket::SetWnd(HWND hWnd)
{
    m_hWnd = hWnd;
}

void CClientSocket::OnReceive(int nErrorCode)//Client 정보 받아오기
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    CString strTmp = _T(""), strIPAddress = _T(""), str, str2;
    UINT uPortNumber = 0;
    TCHAR strBuffer[1024];
    ZeroMemory(strBuffer, sizeof(strBuffer));//메모리 블럭을 0으로 채움

    GetPeerName(strIPAddress, uPortNumber);//연결된 클라이언트 정보 가져오기
    //Receive는 문자열 갯수를 반환. 이때 strBuffer에client에서 보낸 메세지가 저장됨.
    if (Receive(strBuffer, sizeof(strBuffer)) > 0) { // 전달된 데이터(문자열)가 있을 경우
        strTmp.Format(L"%s", strBuffer);
        CString check = strTmp.Left(2);

        if (check == "좌표" || check == "마감") {}
        else if (check == "채팅") {//채팅;%s;;;%d_%d_%d_%d
            strTmp.Format(_T("채팅%d;%s"), uPortNumber, (LPCTSTR)strTmp.Right(strTmp.GetLength() - 2));//채팅Port;;client;;;item
        }
        else if (check == "프로") {//프로;;;;;%s가 온다
            strTmp.Format(L"프로%d%s", uPortNumber, (LPCTSTR)strTmp.Right(strTmp.GetLength() - 2));
            //프로Port;;;;;strTmp
            //0;1;2;3;4;5
        }
    }
    //LPCTSTR = long pointer constant string = const char *==>포인터 형식.
    //실제 정보를 받아들이는건 WPARAM, 포인터를 받아들이는건 LPARAM==>그래서 LPARAM사용
    SendMessage(m_hWnd, WM_CLIENT_MSG_RECV, 0, (LPARAM)((LPCTSTR)strTmp));
    //          윈도우 핸들을 통해 RECV로, ,strTmp전달
    CSocket::OnReceive(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode)//Client닫을 때
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    SendMessage(m_hWnd, WM_CLIENT_CLOSE, 0, (LPARAM)this);//view의 OnClientClose로 보냄
    CSocket::OnClose(nErrorCode);
}
