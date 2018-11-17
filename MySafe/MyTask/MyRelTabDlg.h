#pragma once
#include "ClistCtrlEx.h"
#include "afxcmn.h"


// MyRelTabDlg 对话框

class MyRelTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyRelTabDlg)

public:
	MyRelTabDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyRelTabDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RELTAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_RelTab;
	ClistCtrlEx m_List_RelRva;
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
