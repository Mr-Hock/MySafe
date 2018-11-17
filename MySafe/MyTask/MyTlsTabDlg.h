#pragma once
#include "afxwin.h"


// MyTlsTabDlg 对话框

class MyTlsTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyTlsTabDlg)

public:
	MyTlsTabDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyTlsTabDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TLSTAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_Edit_StartAddress;
	CEdit m_Edit_EndAddress;
	CEdit m_Edit_AddressIndex;
	CEdit m_Edit_AddressCallBack;
	CEdit m_Edit_SizeOfZero;
	CEdit m_Edit_Characteristics;



};
