#pragma once
#include "ClistCtrlEx.h"
#include "afxcmn.h"


// MyResourceDlg �Ի���

class MyResourceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyResourceDlg)

public:
	MyResourceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MyResourceDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESOURCETAB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_Resource;
	virtual BOOL OnInitDialog();
};
