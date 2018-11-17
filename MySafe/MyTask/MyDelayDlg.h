#pragma once
#include "afxcmn.h"
#include "ClistCtrlEx.h"


// MyDelayDlg 对话框

class MyDelayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyDelayDlg)

public:
	MyDelayDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyDelayDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DELAYTAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_Delay_Dll;
	ClistCtrlEx m_List_Delay_Funcition;
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
