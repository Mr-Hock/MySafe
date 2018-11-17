#pragma once
#include "afxwin.h"
#include <iphlpapi.h>


// MyTrafficDlg 对话框

class MyTrafficDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyTrafficDlg)

public:
	MyTrafficDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyTrafficDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAFFIC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CStatic m_Static_Down;
	CStatic m_Static_Up;
	DWORD TIMER_REFRESHTRAFFIC = 0x888;

	BOOL GetNetTraffic();

	typedef DWORD(__stdcall *GIT)(PMIB_IFTABLE, PULONG, BOOL);
	GIT lpGetIfTable;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
