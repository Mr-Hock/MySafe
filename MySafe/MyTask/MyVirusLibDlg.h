#pragma once
#include "ClistCtrlEx.h"
#include "afxcmn.h"


// MyVirusLibDlg 对话框

class MyVirusLibDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyVirusLibDlg)

public:
	MyVirusLibDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyVirusLibDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIRUSLIB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMenu m_Menu;
	CImageList  m_VirusImageList;

	virtual BOOL OnInitDialog();
	ClistCtrlEx m_List_VirusLib;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnUpdatavirus();
	afx_msg void OnDownvirus();
	afx_msg void OnDeleteallvirus();
	afx_msg void OnDeleteonece();
	afx_msg void OnRclickListFile(NMHDR *pNMHDR, LRESULT *pResult);
	void UpDataVirusLib();
	void LoadImageVirus();
	void PrintVirusList();
};
