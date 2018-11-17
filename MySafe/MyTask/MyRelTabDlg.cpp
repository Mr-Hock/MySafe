// MyRelTabDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTask.h"
#include "PE.h"
#include "MyRelTabDlg.h"
#include "afxdialogex.h"

extern PE MyPe;

// MyRelTabDlg 对话框

IMPLEMENT_DYNAMIC(MyRelTabDlg, CDialogEx)

MyRelTabDlg::MyRelTabDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RELTAB_DIALOG, pParent)
{

}

MyRelTabDlg::~MyRelTabDlg()
{
}

void MyRelTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List_RelTab);
	DDX_Control(pDX, IDC_LIST2, m_List_RelRva);
}


BEGIN_MESSAGE_MAP(MyRelTabDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &MyRelTabDlg::OnClickList1)
END_MESSAGE_MAP()


// MyRelTabDlg 消息处理程序


BOOL MyRelTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_List_RelTab.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_RelRva.SetExtendedStyle(LVS_EX_FULLROWSELECT);


	m_List_RelTab.InsertColumn(0, TEXT("索引"), 0, 80);
	m_List_RelTab.InsertColumn(1, TEXT("区段"), 0, 60);
	m_List_RelTab.InsertColumn(2, TEXT("RVA"), 0, 120);
	m_List_RelTab.InsertColumn(3, TEXT("块大小"), 0, 120);
	m_List_RelTab.InsertColumn(4, TEXT("数量"), 0, 60);

	m_List_RelRva.InsertColumn(0, TEXT("索引"), 0, 80);
	m_List_RelRva.InsertColumn(1, TEXT("RVA"), 0, 120);
	m_List_RelRva.InsertColumn(2, TEXT("属性"), 0, 80);

	if (MyPe.Is32Pe())
	{
		MyPe.PrintRelPage_32(m_List_RelTab);
	}
	else
	{
		MyPe.PrintRelPage_64(m_List_RelTab);
	}

	return TRUE;
}


void MyRelTabDlg::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (pNMItemActivate->iItem == -1)return;
	if (MyPe.Is32Pe())
	{
		MyPe.PrintRelRva_32(m_List_RelRva, pNMItemActivate->iItem);
	}
	else
	{
		MyPe.PrintRelRva_64(m_List_RelRva, pNMItemActivate->iItem);
	}
}
