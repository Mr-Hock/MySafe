#pragma once
#include "afxwin.h"


// MyTlsTabDlg �Ի���

class MyTlsTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyTlsTabDlg)

public:
	MyTlsTabDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MyTlsTabDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TLSTAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
