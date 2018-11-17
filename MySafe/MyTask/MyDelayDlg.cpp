// MyDelayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyDelayDlg.h"
#include "afxdialogex.h"
#include "PE.h"

extern PE MyPe;


// MyDelayDlg 对话框

IMPLEMENT_DYNAMIC(MyDelayDlg, CDialogEx)

MyDelayDlg::MyDelayDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DELAYTAB_DIALOG, pParent)
{

}

MyDelayDlg::~MyDelayDlg()
{
}

void MyDelayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List_Delay_Dll);
	DDX_Control(pDX, IDC_LIST2, m_List_Delay_Funcition);
}


BEGIN_MESSAGE_MAP(MyDelayDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &MyDelayDlg::OnClickList1)
END_MESSAGE_MAP()


// MyDelayDlg 消息处理程序


BOOL MyDelayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_List_Delay_Dll.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Delay_Funcition.SetExtendedStyle(LVS_EX_FULLROWSELECT);


	m_List_Delay_Dll.InsertColumn(0, TEXT("DLL名字"), 0, 200);
	m_List_Delay_Dll.InsertColumn(1, TEXT("INT(RVA)"), 0, 120);
	m_List_Delay_Dll.InsertColumn(2, TEXT("IAT(RVA)"), 0, 120);

	m_List_Delay_Funcition.InsertColumn(0, TEXT("函数名字"), 0, 200);
	m_List_Delay_Funcition.InsertColumn(1, TEXT("Thunk Value"), 0, 120);
	m_List_Delay_Funcition.InsertColumn(2, TEXT("HINT"), 0, 120);
	//m_List_Import_Function.InsertColumn(3, TEXT("RVA"), 0, 120);



	if (MyPe.Is32Pe())
	{
		MyPe.PrintDelayDll_32(m_List_Delay_Dll);
	}
	else
	{
		MyPe.PrintDelayDll_64(m_List_Delay_Dll);
	}


	return TRUE; 
}


void MyDelayDlg::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	if (pNMItemActivate->iItem == -1)return;

	if (MyPe.Is32Pe())
	{
		MyPe.PrintDelayFunction_32(m_List_Delay_Funcition, pNMItemActivate->iItem);
	}
	else
	{
		MyPe.PrintDelayFunction_64(m_List_Delay_Funcition, pNMItemActivate->iItem);
	}

}
