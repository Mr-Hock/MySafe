#pragma once
#include "afxcmn.h"
#include "ClistCtrlEx.h"


// MyDelayDlg �Ի���

class MyDelayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyDelayDlg)

public:
	MyDelayDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MyDelayDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DELAYTAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_Delay_Dll;
	ClistCtrlEx m_List_Delay_Funcition;
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
