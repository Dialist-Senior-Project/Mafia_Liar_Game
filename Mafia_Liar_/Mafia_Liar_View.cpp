
// Mafia_Liar_View.cpp: CMafiaLiarView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Mafia_Liar_.h"
#endif

#include "Mafia_Liar_Doc.h"
#include "Mafia_Liar_View.h"

#include <ctime>
#include <cmath>
#include <cstdlib>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PORT 9999
#define DFT 1000

// CMafiaLiarView

IMPLEMENT_DYNCREATE(CMafiaLiarView, CFormView)

BEGIN_MESSAGE_MAP(CMafiaLiarView, CFormView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
	ON_MESSAGE(WM_ACCEPT_SOCKET, &CMafiaLiarView::OnAcceptSocket)
	ON_MESSAGE(WM_CLIENT_MSG_RECV, &CMafiaLiarView::OnClientMsgRecv)
	ON_MESSAGE(WM_CLIENT_CLOSE, &CMafiaLiarView::OnClientClose)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON1, &CMafiaLiarView::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMafiaLiarView::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON3, &CMafiaLiarView::OnBnClickedButton3)
END_MESSAGE_MAP()

// CMafiaLiarView 생성/소멸

CMafiaLiarView::CMafiaLiarView() noexcept
	: CFormView(IDD_MAFIA_LIAR__FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	for (int i = 0; i < 8; i++) {
		userPort[i] = L"";
		userClient[i] = L"";
		username[i].Format(L"user%d", i + 1);
		item[i] = L"";

		roles[i] = -1;
		voted[i] = 0;
	}

	tutorial[0] = L" 직업 : '시민', '경찰', '의사', '마피아'가 랜덤으로 할당됩니다.";

	/*tutorial[1] = L"이제 '마피아'는 일정 주기마다 대상 1인을 살해할 것입니다.";
	tutorial[2] = L"'시민', '의사', '경찰'은 연합하여 '마피아'를 저지해야 합니다.";
	tutorial[3] = L"'경찰'은 일정 주기마다 대상 1인이 '마피아'인지 화인할 수 있습니다.";
	tutorial[4] = L"'의사'는 일정 주기마다 대상 1인을 '마피아'의 공격에서 보호할 수 있습니다.";*/

	tutorial[1] = L" 또한, '마피아'를 제외한 전원에게 '제시어'가 공개됩니다.";
	tutorial[2] = L" '마피아'로 지목되면 '제시어'에 맞는 그림을 그려 결백을 증명해야 합니다.";
	tutorial[3] = L" '제시어'는 결정적인 단서가 될 수 있으므로 대화 중 발설을 엄금합니다.";  //@@@
	tutorial[4] = L" 곧 서바이벌을 시작합니다. 생존하십시오.";

	cntdown[0] = L" 3,";
	cntdown[1] = L" 2,";
	cntdown[2] = L" 1,";
	cntdown[3] = L"공개; 서바이벌 시작!";
	// 역할, 제시어 공개

	script[0] = L"대화;07; 제한 시간 동안 자유롭게 대화하여 단서를 찾으십시오.";  //@@@@@@   //---시작;대화;20;---
	//20 -- 종료
	script[1] = L"시작;일몰;10; 대화 종료. '마피아'의 시간이 되었습니다.";  //대화를 불허합니다.
	// 역할별 지령
	//15 -- 일몰
	script[2] = L" '마피아'가 희생양을 선택했습니다.";
	script[7] = L" '마피아'가 지목한 희생양을 '의사'가 구명했습니다.";
	script[8] = L" '경찰'이 누군가의 정체를 확인했습니다.";
	// <경찰> 확인

	script[3] = L"대화;07; '마피아'의 시간이 끝났습니다. 제한 시간 동안 대화를 허용합니다.";  //L"일출. 일몰 전까지 대화를 허용합니다._";
	script[4] = L"투표;05; '마피아'로 의심되는 대상을 선택하십시오.";
	//20 -- 종료 
	// 그림
	//15 -- 종료
	//8 -- 투표

	//script[13] = L"투표 완료. [user%d]는 [마피아]로 지목되었습니다.";
	//script[14] = L"투표 완료. [user%d]는 [마피아]로 지목되지 않았습니다.";
	//script[15] = L"[user%d]의 처형 집행. 그는 [마피아]였습니다.";
	//script[16] = L"[user%d]의 처형 집행. 그는 [마피아]가 아니었습니다.";

	script[17] = L" 모든 '마피아'가 처형되었습니다.";
	script[18] = L" '마피아' 패배. '마피아' 패배.";
	script[19] = L" '마피아'를 제외한 모두가 사망했습니다.";
	script[20] = L" '시민' 패배. '시민' 패배.";

	m_time = scp = p_cnt = fvoted = 0;
	tut = spturn = true;
	rop = cdn = tut_5 = cs1 = spt = dth = chk = co2 = vte = rvl = drw = ane = res = exe = end = false;
	saved = hf = false;

	out_role = L"";
	m2_sel = h_sel = p_sel = case1 = exec = ending = -1;
	m_sel = -2;
}

CMafiaLiarView::~CMafiaLiarView()
{
}

void CMafiaLiarView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_client);
	DDX_Control(pDX, IDC_LIST2, m_list_msg);
}

BOOL CMafiaLiarView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
}

void CMafiaLiarView::OnInitialUpdate()//시작 설정
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	//Listen 소켓 초기화
	m_pServerSocket = new CServerSocket;
	m_pServerSocket->SetWnd(this->m_hWnd);

	//소켓 Listen하기
	m_pServerSocket->Create(PORT);
	m_pServerSocket->Listen();
}


// CMafiaLiarView 인쇄

BOOL CMafiaLiarView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMafiaLiarView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMafiaLiarView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CMafiaLiarView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}


// CMafiaLiarView 진단

#ifdef _DEBUG
void CMafiaLiarView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMafiaLiarView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CMafiaLiarDoc* CMafiaLiarView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMafiaLiarDoc)));
	return (CMafiaLiarDoc*)m_pDocument;
}
#endif //_DEBUG


// CMafiaLiarView 메시지 처리기


afx_msg LRESULT CMafiaLiarView::OnAcceptSocket(WPARAM wParam, LPARAM lParam)
{
	CString str, str2, item_all = L"";
	int count = 0;
	m_pClientSocket = (CClientSocket*)lParam;//클라이언트가접속했을 때 SercerSocket에서 받은 정보(클라이언트번호) 가져옴.
	m_ptrClientSocketList.AddTail(m_pClientSocket);//가상 리스트에 저장

	str.Format(_T("Client (%d)"), (int)(m_pClientSocket));
	m_list_client.InsertString(-1, str);//리스트에 입력
	
	CString usercnt;
	int cnt = 0;
	cnt = m_list_client.GetCount();
	usercnt.Format(L"User (%d/8)", cnt);
	GetDlgItem(IDC_STATIC5)->SetWindowTextW(usercnt);

	POSITION pos = m_ptrClientSocketList.GetHeadPosition();//가상 리스트의 첫 노드의 포인터값(클라이언트 번호)
	while (pos != NULL)//클라이언트 번호가 있으면(연결된 클라이언트가 있으면)
	{
		CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);//가상리스트의 다음 노드의 포인터값
		if (pClient != NULL)//이 있으면(메세지를 보낸 클라이언트 외 다른 클라이언트가 열결되어 있으면
		{
			count++;
		}
	}
	int i;
	for (i = 0; i < count; i++) {
		item_all += item[i] + '$';
		if (userClient[i] == L"") {
			userClient[i].Format(L"%d", (int)m_pClientSocket);
			break;
		}
	}


	pos = m_ptrClientSocketList.GetHeadPosition();//가상 리스트의 첫 노드의 포인터값(클라이언트 번호)

	while (pos != NULL)//클라이언트 번호가 있으면(연결된 클라이언트가 있으면)
	{
		CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);//가상리스트의 다음 노드의 포인터값
		if (pClient != NULL)//이 있으면(메세지를 보낸 클라이언트 외 다른 클라이언트가 열결되어 있으면
		{
			//UNICODE 사용하면 기존 메모리크기 *2 해야함//한글은 2바이트 사용해서 그럼
			//Client에 Broadcasting하기
			str2.Format(L"채팅;;%d;%d;;%s", (int)(m_pClientSocket), i + 1, (LPCTSTR)item_all);//port;msg;client;count(;;client;count)
			pClient->Send(str2, lstrlen(str2) * 2);//클라이언트로 데이터 전송
		}
	}

	m_list_msg.InsertString(-1, str2);
	m_list_msg.SetCurSel(m_list_msg.GetCount() - 1);

	m_pClientSocket = NULL;//초기화
	delete m_pClientSocket;//삭제

	return 0;
}


afx_msg LRESULT CMafiaLiarView::OnClientMsgRecv(WPARAM wParam, LPARAM lParam)
{
	//ClientSocket의 OnReceive를 통해 Client에서 입력한 값
	//(strTmp.Format(_T("[%s : %d]: %s"), strIPAddress, uPortNumber, strBuffer))을 가져옴
	LPCTSTR lpszStr = (LPCTSTR)lParam;
	CString str, str2, str3, str4;
	CString userID, talk, userclient, usercount, profileOK, itemRev, pro;
	int count = 0;

	str.Format(L"%s", lpszStr);//port;say;client;
	CString check = str.Left(2);

	
	if (check == "좌표" || check == "마감") {
		POSITION pos = m_ptrClientSocketList.GetHeadPosition(); //가상 리스트의 첫 노드의 포인터값(클라이언트 번호)

		while (pos != NULL) //클라이언트 번호가 있으면(연결된 클라이언트가 있으면)
		{
			CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);//가상리스트의 다음 노드의 포인터값
			if (pClient != NULL) //이 있으면(메세지를 보낸 클라이언트 외 다른 클라이언트가 연결되어 있으면
			{
				//UNICODE 사용하면 기존 메모리크기 *2 해야함//한글은 2바이트 사용해서 그럼
				//Client에 Broadcasting하기
				pClient->Send(lpszStr, lstrlen(lpszStr) * 2); //클라이언트로 데이터 전송
			}
		}
		//m_list_msg.InsertString(-1, str);
		//m_list_msg.SetCurSel(m_list_msg.GetCount() - 1);
	}

	else if (check == "채팅") {//채팅Port;;client;;;item(4개)
		str = str.Right(str.GetLength() - 2);
		AfxExtractSubString(userID, str, 0, ';');
		AfxExtractSubString(talk, str, 1, ';');
		AfxExtractSubString(userclient, str, 2, ';');
		AfxExtractSubString(usercount, str, 3, ';');
		AfxExtractSubString(profileOK, str, 4, ';');
		AfxExtractSubString(itemRev, str, 5, ';');

		//BroadCasting
		//클라이언트에 메세지 전달
		POSITION pos = m_ptrClientSocketList.GetHeadPosition();//가상 리스트의 첫 노드의 포인터값(클라이언트 번호)
		while (pos != NULL)//클라이언트 번호가 있으면(연결된 클라이언트가 있으면)
		{
			CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);//가상리스트의 다음 노드의 포인터값
			if (pClient != NULL)//이 있으면(메세지를 보낸 클라이언트 외 다른 클라이언트가 열결되어 있으면
			{
				count++;
			}
		}

		if (userclient != L"") {
			CString tmp;
			int i;
			for (i = 0; i < 8; i++)
				if (userClient[i] == userclient) {
					userPort[i] = userID;
					item[i].Format(L"%d_%s", i, (LPCTSTR)itemRev);
					break;
				}
			pos = m_ptrClientSocketList.GetHeadPosition();
			while (pos != NULL){
				CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
				if (pClient != NULL){
					str.Format(L"프로;;;;;%s", (LPCTSTR)item[i]);
					pClient->Send(str, lstrlen(str) * 2);//클라이언트로 데이터 전송
				}
			}
			m_list_msg.InsertString(-1, str);
			m_list_msg.SetCurSel(m_list_msg.GetCount() - 1);
		}

		else if (userclient == L"") {
			if (talk != L"") {
				pos = m_ptrClientSocketList.GetHeadPosition();//가상 리스트의 첫 노드의 포인터값(클라이언트 번호)
				while (pos != NULL)//클라이언트 번호가 있으면(연결된 클라이언트가 있으면)
				{
					CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);//가상리스트의 다음 노드의 포인터값
					if (pClient != NULL)//이 있으면(메세지를 보낸 클라이언트 외 다른 클라이언트가 열결되어 있으면
					{
						//UNICODE 사용하면 기존 메모리크기 *2 해야함//한글은 2바이트 사용해서 그럼
						//Client에 Broadcasting하기
						for (int i = 0; i < 8; i++)
							if (userID == userPort[i]) {
								str.Format(L"채팅;[%s] : %s;;", (LPCTSTR)username[i], (LPCTSTR)talk);//port;say;client;count
							}
						pClient->Send(str, lstrlen(str) * 2);//클라이언트로 데이터 전송
					}
				}
				//m_list_msg에 메시지 추가
				m_list_msg.InsertString(-1, str);
				m_list_msg.SetCurSel(m_list_msg.GetCount() - 1);
			}
			/*		else if (profileOK != L"") {
						for (int i = 0; i < 8; i++)
							if (userID == userPort[i])
								if (i + 1 == profileOK)
									profileOK.Format(L"1");
								else
									profileOK.Format(L"0");
						while (pos != NULL)//클라이언트 번호가 있으면(연결된 클라이언트가 있으면)
						{
							CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);//가상리스트의 다음 노드의 포인터값
							for (int i = 0; i < 8; i++)
								if (pClient != NULL && (int)pClient == userClient[i])//이 있으면(메세지를 보낸 클라이언트 외 다른 클라이언트가 열결되어 있으면
								{
									//UNICODE 사용하면 기존 메모리크기 *2 해야함//한글은 2바이트 사용해서 그럼
									//Client에 Broadcasting하기
									str.Format(L";%s:%s;;", username[i], talk);//port;say;client;count
									pClient->Send(str, lstrlen(str) * 2);//클라이언트로 데이터 전송
								}
						}
					}*/
		}
	}


	else if (check == "일몰") {
		CString u, s, tmp;
		str = str.Right(str.GetLength() - 2);  //일몰u;s;
		AfxExtractSubString(u, str, 0, ';');
		AfxExtractSubString(s, str, 1, ';');

		if (spturn) {
			if (roles[_ttoi(u) - 1] == 11)
				h_sel = _ttoi(s) - 1;
			else if (roles[_ttoi(u) - 1] == 22) {
				p_sel = _ttoi(s) - 1;
				POSITION pos = m_ptrClientSocketList.GetHeadPosition();
				while (pos != NULL)
				{
					CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
					if (pClient != NULL)
					{
						if (roles[p_sel] != 33)
							tmp.Format(L"지령22  <경찰>  [user%d]는 '마피아'가 아닙니다.", p_sel + 1);
						else
							tmp.Format(L"지령22  <경찰>  [user%d]는 '마피아'입니다.", p_sel + 1);
						pClient->Send(tmp, lstrlen(tmp) * 2);
					}
				}
			}
			else
				m_sel = _ttoi(s) - 1;

			if (h_sel == m_sel) {
				saved = true;
				//tmp.Format(L"h_sel=%d, m_sel=%d", h_sel, m_sel);	
				//MessageBox(tmp, L"O");
			}
			//else
				//MessageBox(tmp, L"X");
		}

		else if (check == "방장") {
			//OnBnClickedButton2();
			MessageBox(L"방장", L"");
			OnBnClickedButton3();
		}

		else {
			//if (roles[_ttoi(u) - 1] != 33) {
				voted[_ttoi(s) - 1]++;
				if(voted[0] >= 0)  exec = 0;
				for (int i = 0; i < p_cnt - 1; i++)
					if (voted[i] < voted[i + 1])
						exec = i + 1;
			//}
		}
	}

	else if (check == "종료") {
		SetTimer(0, DFT, NULL);
	}

//	else if (check == "마감") {
//		SetTimer(0, DFT, NULL);
//	}
	
	else if (check == "결과") {
		check = str.Right(str.GetLength() - 3);
		if (check == "Y")
			fvoted++;
		if ((float(fvoted) / (p_cnt - 1)) >= 0.5) {
			hf = true;
			//MessageBox(L"true", L"");
		}
	}

/*	else if (check == "프로") {//프로Port;;;;;strTmp
		AfxExtractSubString(userID, str.Right(str.GetLength() - 2), 0, ';');
		AfxExtractSubString(itemRev, str, 5, ';');
		m_list_msg.InsertString(-1, userPort[0]);
		m_list_msg.SetCurSel(m_list_msg.GetCount() - 1);
		int i;
		for (i = 0; i < count; i++) {
			if (userPort[i] == userID) {
				item[i].Format(L"%d_%d", i, itemRev);//strTm=누구_eye_mouth_color_acce
				break;
			}
		}
		POSITION pos = m_ptrClientSocketList.GetHeadPosition();
		while (pos != NULL)
		{
			CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
			if (pClient != NULL)
			{
				str.Format(L"프로;;;;;%s", item[i]);//프로;;;;;누가 보냈나(0~7)프로필정보(eye_mouth_...)
				pClient->Send(str, lstrlen(str) * 2);
			}
		}
	}*/


	else {
		POSITION pos = m_ptrClientSocketList.GetHeadPosition(); //가상 리스트의 첫 노드의 포인터값(클라이언트 번호)

		while (pos != NULL) //클라이언트 번호가 있으면(연결된 클라이언트가 있으면)
		{
			CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);//가상리스트의 다음 노드의 포인터값
			if (pClient != NULL) //이 있으면(메세지를 보낸 클라이언트 외 다른 클라이언트가 연결되어 있으면
			{
				//UNICODE 사용하면 기존 메모리크기 *2 해야함//한글은 2바이트 사용해서 그럼
				//Client에 Broadcasting하기
				pClient->Send(lpszStr, lstrlen(lpszStr) * 2); //클라이언트로 데이터 전송
			}
		}
	}
	
	return 0;
}


afx_msg LRESULT CMafiaLiarView::OnClientClose(WPARAM wParam, LPARAM lParam)
{
	CClientSocket* pClient = (CClientSocket*)lParam;
	CString str;
	UINT idx = 0;
	POSITION pos = m_ptrClientSocketList.Find(pClient);

	if (pos != NULL)
	{
		//m_list_client에서 해당 Client 삭제
		str.Format(_T("Client (%d)"), (int)pClient);
		idx = m_list_client.SelectString(-1, (LPCTSTR)str);
		m_list_client.DeleteString(idx);
		p_cnt--;
		for (int i = 0; i < 8; i++) {
			if ((int)pClient == _ttoi(userClient[i])) {
				userClient[i] = L"";
				userPort[i] = L"";
				item[i] = L"";
				roles[i] = -1;
				voted[i] = 0;
			}
		}

		//CPtrList에서도 해당 Client 삭제
		m_ptrClientSocketList.RemoveAt(pos);//가상 리스트에서 삭제

		CString usercnt;
		int cnt = 0;
		cnt = m_list_client.GetCount();
		usercnt.Format(L"User (%d/8)", cnt);
		GetDlgItem(IDC_STATIC5)->SetWindowTextW(usercnt);
	}

	return 0;
}


HBRUSH CMafiaLiarView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	int nCtlID = pWnd->GetDlgCtrlID();

	switch (nCtlID) {
	case IDC_LIST1:
		pDC->SetBkColor(RGB(20, 20, 20));
		pDC->SetTextColor(RGB(250, 250, 250));
		hbr = ::CreateSolidBrush(RGB(51, 51, 51));
		break;
	case IDC_LIST2:
		pDC->SetBkColor(RGB(20, 20, 20));
		pDC->SetTextColor(RGB(250, 250, 250));
		hbr = ::CreateSolidBrush(RGB(51, 51, 51));
		break;
	case IDC_STATIC2:
		pDC->SetBkColor(RGB(51, 51, 51));
		pDC->SetTextColor(RGB(255, 51, 51));
		hbr = ::CreateSolidBrush(RGB(51, 51, 51));
		break;
	case IDC_STATIC3:
		pDC->SetBkColor(RGB(51, 51, 51));
		pDC->SetTextColor(RGB(250, 250, 250));
		hbr = ::CreateSolidBrush(RGB(51, 51, 51));
		break;
	case IDC_STATIC4:
		pDC->SetBkColor(RGB(51, 51, 51));
		pDC->SetTextColor(RGB(255, 51, 51));
		hbr = ::CreateSolidBrush(RGB(51, 51, 51));
		break;		
	case IDC_STATIC5:
		pDC->SetBkColor(RGB(51, 51, 51));
		pDC->SetTextColor(RGB(250, 250, 250));
		hbr = ::CreateSolidBrush(RGB(51, 51, 51));
		break;
	default:
		break;
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


BOOL CMafiaLiarView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect bg_rc;
	GetClientRect(bg_rc);
	pDC->FillSolidRect(bg_rc, RGB(0, 0, 0));

	return TRUE;
	//return CFormView::OnEraseBkgnd(pDC);
}


void CMafiaLiarView::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
	//::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
}


void CMafiaLiarView::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	srand((unsigned)time(NULL));
	p_cnt = m_list_client.GetCount();

	CString str;
	int nm, rd, n = 0, arr[4] = { -1, -1, -1, -1 };
	(p_cnt <= 6) ? num_m = nm = 1 : nm = 2;
	num_c = p_cnt - (num_m + 2);

	while (arr[2] == -1) { //@@@@
		bool ndp = true;
		rd = rand() % p_cnt;
		for (int i = 0; i < 4; i++)
			if (arr[i] == rd)	ndp = false;
		if (ndp) {
			arr[n] = rd;
			roles[rd] = (n + 1) * 11;
			n++;
		}
	}

	for (int i = 0; i < p_cnt; i++) {
		if (roles[i] == -1)
			roles[i] = 44;
	}

	int rdw = rand() % 9;
	out_role.Format(L"역할%d;%d;%d;" + words[rdw] + L";", arr[0], arr[1], arr[2]);
	/*CString ttt;
	for (int i = 0; i < 4; i++) {
		ttt.Format(L"roles[%d] = %d", i, roles[i]);
		//MessageBox(ttt, L"1");
	}*/

	POSITION pos = m_ptrClientSocketList.GetHeadPosition();//가상 리스트의 첫 노드의 포인터값(클라이언트 번호)
	while (pos != NULL)//클라이언트 번호가 있으면(연결된 클라이언트가 있으면)
	{
		CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);//가상리스트의 다음 노드의 포인터값
		if (pClient != NULL)//이 있으면(메세지를 보낸 클라이언트 외 다른 클라이언트가 열결되어 있으면
		{
			pClient->Send(out_role, lstrlen(out_role) * 2);  // 역할 + 제시
		}
	}

	SetTimer(0, DFT, NULL);
}


void CMafiaLiarView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 0) {
		m_time++;
		gameMain();
	}

	if (nIDEvent == 1) {
		KillTimer(1);
		SetTimer(0, DFT, NULL);
	}

	CFormView::OnTimer(nIDEvent);
}


void CMafiaLiarView::gameMain()
{
	// TODO: 여기에 구현 코드 추가.
	CString tmp;

	POSITION pos = m_ptrClientSocketList.GetHeadPosition();//가상 리스트의 첫 노드의 포인터값(클라이언트 번호)
	while (pos != NULL)//클라이언트 번호가 있으면(연결된 클라이언트가 있으면)
	{
		CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);//가상리스트의 다음 노드의 포인터값
		if (pClient != NULL)//이 있으면(메세지를 보낸 클라이언트 외 다른 클라이언트가 열결되어 있으면
		{
			if (tut) {
				if (scp == 5) {
					tut = false;
					cdn = true;
					scp = 0;
					return;
				}
				pClient->Send(tutorial[scp], lstrlen(tutorial[scp]) * 2);
			}


			if (cdn) {
				//KillTimer(0);
				//SetTimer(0, 1000, NULL);
				if (scp == 4) {
					//KillTimer(0);
					//SetTimer(0, 2000, NULL);
					cdn = false;
					cs1 = true;
					scp = 0;
					return;
				}
				pClient->Send(cntdown[scp], lstrlen(cntdown[scp]) * 2);
			}


			// 대화 ~ 일몰
			if (cs1) {
				if (scp == 1)
					KillTimer(0); //하단 if문에 포함?
				if (scp == 2) {
					cs1 = false;
					dth = true;
					return;
				}
				/*if (scp == 0) {
					tmp2.Format(L"허용");  // 허용일몰 전까지~ (허용 + 스크립트0)
					pClient->Send(tmp2, lstrlen(tmp2) * 2);
					//MessageBox(L"send", L"");
					//cntTime(m_time, 30);  // 채팅 시간 대기
				}*/
				if (scp == 0)
					KillTimer(0);
				pClient->Send(script[scp], lstrlen(script[scp]) * 2);
			}


			if (dth) {//2
				if (scp == 99) {
					scp = 1;
					dth = false;
					end = true;
					return;
				}

				if (scp == 2) {
					pClient->Send(script[scp], lstrlen(script[scp]) * 2);
					//saved = false;  //여기 놓으면 클라이언트1은 cntdown[3] 출력, 나머지는 출력안함
				}

				if ((scp == 3) && (saved)) {
					pClient->Send(script[7], lstrlen(script[7]) * 2);
				}
				else if ((scp == 3) && (!saved)) {
					if (roles[m_sel] != 33) {
						tmp.Format(L"사망총%d [user%d] 사망. 그는 '마피아'가 아닙니다. %d", m_sel, m_sel + 1, num_c);
						num_c--;
					}
					else {
						tmp.Format(L"사망총%d [user%d] 사망. 그는 '마피아'입니다.", m_sel, m_sel + 1);
						num_m--;
					}
					pClient->Send(tmp, lstrlen(tmp) * 2);
					if ((num_m == 0) || (num_c == 0)) {
						scp = 98;
						(num_c) ? ending = 0 : ending = 2;
					}
					//scp = 3;
				}

				if (scp == 5) {//<전체> 경찰
					pClient->Send(script[8], lstrlen(script[8]) * 2);
					
				}
				if (scp == 6) {
					h_sel = p_sel = -1;
					saved = false;
					spturn = dth = false;
					co2 = true;
					scp = 3;
					return;
				}
			}


			// 대화 허용2
			if (co2) {
				if (scp == 4) {
					KillTimer(0);
				}
				if (scp == 5) {
					co2 = false;
					vte = true;
					return;
				}
				pClient->Send(script[scp], lstrlen(script[scp]) * 2);
			}


			// 마피아 투표
			if (vte) {  //scp==5		
				if (scp == 5)
					tmp.Format(L" 투표 완료. 최다 득표자는 [user%d]입니다.", exec + 1);
				if (scp == 6) {
					KillTimer(0);
					tmp.Format(L"그림;%d; 대상자는 제시어에 맞는 그림을 그려 제출하십시오.", exec);
				}
				if (scp == 7) {
					vte = false;
					res = true;
					return;
				}
				pClient->Send(tmp, lstrlen(tmp) * 2);
			}


			if (res) {//7
				if (scp == 7) {
					KillTimer(0);
					tmp.Format(L"최종;10; 최종 투표를 진행합니다. [user%d]는 '마피아'입니까?", exec + 1);
				}
				if (scp == 8) {
					scp = 0;
					res = false;
					ane = true;
					return;
				}
				pClient->Send(tmp, lstrlen(tmp) * 2);
			}


			if (ane) {//0
				if (scp == 11) { //일몰로
					//KillTimer(0);
					scp = 1;
					exec = -1;
					ane = false;
					cs1 = true;
					return;
				}
				if (scp == 99) {
					scp = 1;
					exec = -1;
					ane = false;
					end = true;
					return;
				}

				if (scp == 3) {
					scp = 10;
					if (roles[exec] != 33) {
						tmp.Format(L"사망퉆%d [user%d] 사망. 그는 '마피아'가 아닙니다.", exec, exec + 1);
						if (--num_c == 0) {//(--num_c == 0) ? scp = 98 : scp = 10;
							scp = 98;
							ending = 2; //마피아승리
						}
					}
					else {
						tmp.Format(L"사망퉆%d [user%d] 사망. 그는 '마피아'입니다.", exec, exec + 1);
						if (--num_m == 0) {
							scp = 98;
							ending = 0; //시민승리
						}
					}
				}

				if (scp == 0) {
					if (hf) {
						tmp.Format(L" 최종 투표 결과 : [user%d]는 '마피아'로 지목되었습니다. 처형을 집행합니다.", exec + 1);
						scp = 2;
					}
					else {
						tmp.Format(L" 최종 투표 결과 : [user%d]는 '마피아' 혐의를 벗었습니다.", exec + 1);
						scp = 10;
					}
					hf = false;
				}
				pClient->Send(tmp, lstrlen(tmp) * 2);
			}
			

			if (end) {  //ending = 0 or 2
				if (ending == 0) {
					if (scp == 1)
						tmp.Format(L" 모든 '마피아'가 처형되었습니다.");
					if (scp == 2)
						tmp.Format(L" '마피아' 패배. '마피아' 패배.");
				}
				if (ending == 2) {
					if (scp == 1)
						tmp.Format(L" '마피아'를 제외한 모두가 사망했습니다.");
					if (scp == 2)
						tmp.Format(L" '시민' 패배. '시민' 패배.");
				}
				if (scp == 3) {
					end = false;
					return;
				}
				pClient->Send(tmp, lstrlen(tmp) * 2);
			}

		}
	}

	scp++;
}


void CMafiaLiarView::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	srand((unsigned)time(NULL));
	p_cnt = m_list_client.GetCount();
	roles[0] = 11;
	roles[1] = 22;
	roles[2] = 33;
	roles[p_cnt - 1] = roles[3] = 44;
	num_c = p_cnt - 1;
	//num_c = 1;
	num_m = 1;

	int rdw = rand() % 33;
	out_role.Format(L"역할%d;%d;%d;" + words[rdw] + L";", 0, 1, 2);
	POSITION pos = m_ptrClientSocketList.GetHeadPosition();
	while (pos != NULL)
	{
		CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL)
		{
			pClient->Send(out_role, lstrlen(out_role) * 2);
		}
	}
	SetTimer(0, DFT, NULL);
}
