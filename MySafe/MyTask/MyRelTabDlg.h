#pragma once
#include "ClistCtrlEx.h"
#include "afxcmn.h"


// MyRelTabDlg �Ի���

class MyRelTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyRelTabDlg)

public:
	MyRelTabDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MyRelTabDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RELTAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_RelTab;
	ClistCtrlEx m_List_RelRva;
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
