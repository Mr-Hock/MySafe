// MyTlsTabDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyTlsTabDlg.h"
#include "afxdialogex.h"
#include "PE.h"

extern PE MyPe;

// MyTlsTabDlg 对话框

IMPLEMENT_DYNAMIC(MyTlsTabDlg, CDialogEx)

MyTlsTabDlg::MyTlsTabDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TLSTAB_DIALOG, pParent)
{

}

MyTlsTabDlg::~MyTlsTabDlg()
{
}

void MyTlsTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_StartAddress);
	DDX_Control(pDX, IDC_EDIT4, m_Edit_EndAddress);
	DDX_Control(pDX, IDC_EDIT6, m_Edit_AddressIndex);
	DDX_Control(pDX, IDC_EDIT8, m_Edit_AddressCallBack);
	DDX_Control(pDX, IDC_EDIT10, m_Edit_SizeOfZero);
	DDX_Control(pDX, IDC_EDIT12, m_Edit_Characteristics);
}


BEGIN_MESSAGE_MAP(MyTlsTabDlg, CDialogEx)
END_MESSAGE_MAP()


// MyTlsTabDlg 消息处理程序


BOOL MyTlsTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (MyPe.Is32Pe())
	{
		MyPe.GetTlsInfo_32(m_Edit_StartAddress, m_Edit_EndAddress, m_Edit_AddressIndex, m_Edit_AddressCallBack, m_Edit_SizeOfZero, m_Edit_Characteristics);
	}
	else
	{
		MyPe.GetTlsInfo_64(m_Edit_StartAddress, m_Edit_EndAddress, m_Edit_AddressIndex, m_Edit_AddressCallBack, m_Edit_SizeOfZero, m_Edit_Characteristics);
	}


	return TRUE;
}


