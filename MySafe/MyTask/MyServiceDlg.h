#pragma once
#include "ClistCtrlEx.h"
#include "afxcmn.h"
#include <winsvc.h>


// MyServiceDlg 对话框

class MyServiceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyServiceDlg)

public:
	MyServiceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyServiceDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVICE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMenu m_Menu;
	ClistCtrlEx m_List_Service;
	SC_HANDLE nSCManager = nullptr;
	CImageList  m_ServiceImageList;

	virtual BOOL OnInitDialog();

	void PrintService();
	void LoadImageService();
	afx_msg void OnRefershservice();
	afx_msg void OnStartservice();
	afx_msg void OnStopservice();
	afx_msg void OnRclickListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeydownListFile(NMHDR *pNMHDR, LRESULT *pResult);
};
