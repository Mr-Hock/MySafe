#pragma once
#include "ClistCtrlEx.h"
#include "afxcmn.h"


// MyResourceDlg 对话框

class MyResourceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyResourceDlg)

public:
	MyResourceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyResourceDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESOURCETAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_Resource;
	virtual BOOL OnInitDialog();
};
