// MyVirusLibDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyHead.h"
#include "MyVirusLibDlg.h"
#include "afxdialogex.h"
#include "MD5_FILE.h"
#include "LoadVirusLib.h"


// MyVirusLibDlg 对话框

IMPLEMENT_DYNAMIC(MyVirusLibDlg, CDialogEx)

MyVirusLibDlg::MyVirusLibDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VIRUSLIB_DIALOG, pParent)
{

}

MyVirusLibDlg::~MyVirusLibDlg()
{
}

void MyVirusLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_List_VirusLib);
}


BEGIN_MESSAGE_MAP(MyVirusLibDlg, CDialogEx)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_UPDATAVIRUS, &MyVirusLibDlg::OnUpdatavirus)
	ON_COMMAND(ID_DOWNVIRUS, &MyVirusLibDlg::OnDownvirus)
	ON_COMMAND(ID_DELETEALLVIRUS, &MyVirusLibDlg::OnDeleteallvirus)
	ON_COMMAND(ID_DELETEONECE, &MyVirusLibDlg::OnDeleteonece)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &MyVirusLibDlg::OnRclickListFile)
END_MESSAGE_MAP()


// MyVirusLibDlg 消息处理程序


BOOL MyVirusLibDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Menu.LoadMenu(IDR_MENU5);
	SetMenu(&m_Menu);

	LoadImageVirus();

	m_List_VirusLib.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_VirusLib.InsertColumn(0, TEXT("病毒文件名"), 0, 260);
	m_List_VirusLib.InsertColumn(1, TEXT("病毒文件MD5"), 0, 250);


	PrintVirusList();


	return TRUE;
}

void MyVirusLibDlg::LoadImageVirus()
{
	m_VirusImageList.Create(20, 20, ILC_COLOR32 | ILC_MASK, 2, 2);

	HICON hSmallIco;
	CString nDir = GetProgramDir();

	hSmallIco = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\virus.ico"), 0);
	m_VirusImageList.Add(hSmallIco);
	DestroyIcon(hSmallIco);

	m_List_VirusLib.SetImageList(&m_VirusImageList, LVSIL_SMALL);
}

void MyVirusLibDlg::PrintVirusList()
{
	m_List_VirusLib.DeleteAllItems();

	LoadVirusLib nLoadVirusLib;
	DWORD nCount = nLoadVirusLib.LoadVirusLibData();
	PVIRUSINFO m_VirusInfo = new VIRUSINFO[nCount];
	nLoadVirusLib.GetVirusLib(m_VirusInfo);
	nLoadVirusLib.ClearVirusLibData();


	for (DWORD i = 0; i < nCount; i++)
	{
		m_List_VirusLib.InsertItem(i, CString(m_VirusInfo[i].FileName), 0);
		m_List_VirusLib.SetItemText(i, 1, CString(m_VirusInfo[i].FileMd5));
	}

	delete[]m_VirusInfo;
}

void MyVirusLibDlg::OnDropFiles(HDROP hDropInfo)
{
	CDialogEx::OnDropFiles(hDropInfo);

	CString buff;
	buff.GetBufferSetLength(MAX_PATH);

	DragQueryFile(hDropInfo, 0, (TCHAR*)(const TCHAR*)buff, MAX_PATH);
	DragFinish(hDropInfo);

	CStringA nFileMd5;
	nFileMd5 = GetFileMd5(buff);

	if (nFileMd5 != "")
	{
		CString nTemp;
		nTemp.Format(TEXT("是否将文件：%s\n文件MD5：%s\n加入病毒库？"), GetPathShortFileName(buff),CString(nFileMd5));

		DWORD nRet = MessageBox(nTemp, TEXT("提示"), MB_YESNO + MB_ICONASTERISK);
		if (nRet == IDNO)return;

		DWORD nIndex = m_List_VirusLib.GetItemCount();
		m_List_VirusLib.InsertItem(nIndex, GetPathShortFileName(buff),0);
		m_List_VirusLib.SetItemText(nIndex, 1, CString(nFileMd5));

		UpDataVirusLib();
	}

}


void MyVirusLibDlg::OnUpdatavirus()
{
	CString nFileName = m_List_VirusLib.GetItemText(m_List_VirusLib.GetSelectionEx(), 0);
	CString nFileMd5 = m_List_VirusLib.GetItemText(m_List_VirusLib.GetSelectionEx(), 1);

	VIRUSINFO nVIRUSINFO{};
	strcpy_s(nVIRUSINFO.FileName, MAX_PATH, CStringA(nFileName));
	strcpy_s(nVIRUSINFO.FileMd5, MAX_PATH, CStringA(nFileMd5));

	LoadVirusLib nLoadVirusLib;
	BOOL nRet = nLoadVirusLib.UpDataVirus(nVIRUSINFO);
	if (nRet)MessageBox(TEXT("上传成功！"), TEXT("提示"), MB_ICONINFORMATION);
	else MessageBox(TEXT("上传失败！"), TEXT("提示"), MB_ICONERROR);

}


void MyVirusLibDlg::OnDownvirus()
{
	LoadVirusLib nLoadVirusLib;
	BOOL nRet = nLoadVirusLib.DownDataVirus();
	if (nRet)
	{
		PVIRUSINFO nVIRUSINFO = nullptr;
		DWORD nVirusNum = nLoadVirusLib.RecvDataVirus(nVIRUSINFO);


		if (nVIRUSINFO == nullptr) 
		{ 
			MessageBox(TEXT("同步成功！"), TEXT("提示"), MB_ICONINFORMATION);
			return;
		};

		nLoadVirusLib.SetVirusLib(nVIRUSINFO, nVirusNum);
		delete[]nVIRUSINFO;

		PrintVirusList();

		MessageBox(TEXT("同步成功！"), TEXT("提示"), MB_ICONINFORMATION);
	}
	else MessageBox(TEXT("同步失败！"), TEXT("提示"), MB_ICONERROR);


}


void MyVirusLibDlg::OnDeleteallvirus()
{
	DWORD nRet = MessageBox(TEXT("是否要清空病毒库所有信息？"), TEXT("提示"), MB_ICONASTERISK | MB_YESNO);
	if (nRet == IDNO)return;
	m_List_VirusLib.DeleteAllItems();

	UpDataVirusLib();


}


void MyVirusLibDlg::OnDeleteonece()
{
	CString nFileName;
	nFileName.Format(TEXT("是否将：%s\n移除病毒库？"), m_List_VirusLib.GetItemText(m_List_VirusLib.GetSelectionEx(), 0));
	DWORD nRet = MessageBox(nFileName, TEXT("提示"), MB_ICONASTERISK | MB_YESNO);
	if (nRet == IDNO)return;

	m_List_VirusLib.DeleteItem(m_List_VirusLib.GetSelectionEx());

	UpDataVirusLib();
}

void MyVirusLibDlg::UpDataVirusLib()
{
	DWORD nCount = m_List_VirusLib.GetItemCount();
	PVIRUSINFO nVirusInfo = new VIRUSINFO[nCount];
	for (DWORD i = 0; i < nCount; i++)
	{
		strcpy_s(nVirusInfo[i].FileName, MAX_PATH, CStringA(m_List_VirusLib.GetItemText(i, 0)));
		strcpy_s(nVirusInfo[i].FileMd5, MAX_PATH, CStringA(m_List_VirusLib.GetItemText(i, 1)));
	}

	LoadVirusLib nLoadVirusLib;
	nLoadVirusLib.SetVirusLib(nVirusInfo, nCount);
}


void MyVirusLibDlg::OnRclickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu *nMenu = m_Menu.GetSubMenu(0);
	POINT pos;
	GetCursorPos(&pos);

	if (pNMItemActivate->iItem == -1)
	{
		nMenu->EnableMenuItem(ID_UPDATAVIRUS, TRUE);
		nMenu->EnableMenuItem(ID_DELETEONECE, TRUE);
	}
	else
	{
		nMenu->EnableMenuItem(ID_UPDATAVIRUS, FALSE);
		nMenu->EnableMenuItem(ID_DELETEONECE, FALSE);
	}
	nMenu->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);

}

