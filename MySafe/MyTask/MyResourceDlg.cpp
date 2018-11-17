// MyResourceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTask.h"
#include "PE.h"
#include "MyResourceDlg.h"
#include "afxdialogex.h"

extern PE MyPe;

// MyResourceDlg �Ի���

IMPLEMENT_DYNAMIC(MyResourceDlg, CDialogEx)

MyResourceDlg::MyResourceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RESOURCETAB_DIALOG, pParent)
{

}

MyResourceDlg::~MyResourceDlg()
{
}

void MyResourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List_Resource);
}


BEGIN_MESSAGE_MAP(MyResourceDlg, CDialogEx)
END_MESSAGE_MAP()


// MyResourceDlg ��Ϣ�������


BOOL MyResourceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_List_Resource.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Resource.InsertColumn(0, TEXT("��Դ"), 0, 200);
	m_List_Resource.InsertColumn(1, TEXT("��С"), 0, 120);
	m_List_Resource.InsertColumn(2, TEXT("λ��"), 0, 120);

	PIMAGE_OPTIONAL_HEADER nOptionalHead = MyPe.GetOptionalHead_32();
	IMAGE_RESOURCE_DIRECTORY* pResDir = NULL;
	DWORD dwResTabRva = nOptionalHead->DataDirectory[2].VirtualAddress;
	DWORD dwResTabFoa = MyPe.RvaToFoa_32(dwResTabRva);
	pResDir = (IMAGE_RESOURCE_DIRECTORY*)(dwResTabFoa + (LONG)MyPe.nData);
	m_List_Resource.DeleteAllItems();
	MyPe.PrintResource_32(m_List_Resource, (LPBYTE)pResDir, pResDir);
	return TRUE;
}
