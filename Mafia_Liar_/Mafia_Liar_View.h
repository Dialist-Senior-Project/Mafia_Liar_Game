
// Mafia_Liar_View.h: CMafiaLiarView 클래스의 인터페이스
//

#pragma once
#include "afxwin.h"
#include "CServerSocket.h"

class CMafiaLiarView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CMafiaLiarView() noexcept;
	DECLARE_DYNCREATE(CMafiaLiarView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_MAFIA_LIAR__FORM };
#endif

// 특성입니다.
public:
	CMafiaLiarDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CMafiaLiarView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
private:
	CClientSocket* m_pClientSocket;
	CServerSocket* m_pServerSocket;
public:
	CListBox m_list_client;
	CListBox m_list_msg;
	CPtrList m_ptrClientSocketList;
protected:
	afx_msg LRESULT OnAcceptSocket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientMsgRecv(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientClose(WPARAM wParam, LPARAM lParam);
	CString userPort[8];
	CString userClient[8];
	CString username[8];
	CString item[8];
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButton1();

	CString tutorial[6], cntdown[4], script[21];
	bool tut, rop, cdn, tut_5, cs1, spt, dth, chk, co2, vte, rvl, drw, ane, res, exe, end;
	int m_time, scp, exec;
	int p_cnt, roles[8], voted[8], fvoted;
	CString out_role, cword;
	int m_sel, m2_sel, h_sel, p_sel, num_m, num_c, case1, spturn, ending;
	CString words[100] = { L"독침", L"전기충격기", L"화염방사기", L"펜싱 검", L"당구 큐대", L"접이식테이블", L"벽돌", L"저격총", L"빈대", L"장수하늘소", L"땃쥐", L"승냥이", L"랫서팬더", L"딱다구리", L"현무", L"스핑크스", L"비버", L"아르마딜로", L"이무기", L"수상스키", L"김말이", L"부대찌개", L"깐쇼새우", L"대패삼겹살", L"육회", L"손톱깎이", L"커피필터", L"사원증", L"헤어롤", L"식탁보", L"중문", L"건전지", L"그림액자" };
	bool saved, hf;

	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void gameMain();
	afx_msg void OnBnClickedButton3();
};

#ifndef _DEBUG  // Mafia_Liar_View.cpp의 디버그 버전
inline CMafiaLiarDoc* CMafiaLiarView::GetDocument() const
   { return reinterpret_cast<CMafiaLiarDoc*>(m_pDocument); }
#endif

