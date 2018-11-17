// MyTcpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyHead.h"
#include "MyTcpDlg.h"
#include "afxdialogex.h"
#include <WS2tcpip.h>
#pragma comment(lib, "Iphlpapi.lib")

// MyTcpDlg 对话框

IMPLEMENT_DYNAMIC(MyTcpDlg, CDialogEx)

MyTcpDlg::MyTcpDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TCP_DIALOG, pParent)
{
	Create(IDD_TCP_DIALOG);
}

MyTcpDlg::~MyTcpDlg()
{
}

void MyTcpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TCP, m_List_Tcp);
}


BEGIN_MESSAGE_MAP(MyTcpDlg, CDialogEx)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_TCP, &MyTcpDlg::OnKeydownListTcp)
	ON_COMMAND(ID_ENDLINE, &MyTcpDlg::OnEndline)
	ON_COMMAND(ID_REFERSHLINE, &MyTcpDlg::OnRefershline)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TCP, &MyTcpDlg::OnRclickListTcp)
END_MESSAGE_MAP()


// MyTcpDlg 消息处理程序


BOOL MyTcpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Menu.LoadMenu(IDR_MENU7);
	SetMenu(&m_Menu);


	m_List_Tcp.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Tcp.InsertColumn(0, TEXT("进程名字"), 0, 200);
	m_List_Tcp.InsertColumn(1, TEXT("进程ID"), 0, 60);
	m_List_Tcp.InsertColumn(2, TEXT("本地IP"), 0, 100);
	m_List_Tcp.InsertColumn(3, TEXT("本地端口"), 0, 60);
	m_List_Tcp.InsertColumn(4, TEXT("远程IP"), 0, 100);
	m_List_Tcp.InsertColumn(5, TEXT("远程端口"), 0, 60);
	m_List_Tcp.InsertColumn(6, TEXT("连接状态"), 0, 100);
	m_List_Tcp.InsertColumn(7, TEXT("连接类型"), 0, 100);
	m_List_Tcp.InsertColumn(8, TEXT("进程路径"), 0, 100);

	PrintTcpUdp();

	return TRUE;
}

void MyTcpDlg::OnKeydownListTcp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	*pResult = 0;
	if (pLVKeyDow->wVKey == 116)PrintTcpUdp();

}

//遍历网络连接
VOID MyTcpDlg::PrintTcpUdp()
{
	m_List_Tcp.DeleteAllItems();

	static HIMAGELIST nHiml = { 0 };//图标列表
	static SHFILEINFO nPsfi = { 0 };//文件信息
									//ImageList_Destroy(nHiml);//清除图标列表
	static BOOL nOn = FALSE;
	if (!nOn)
	{
		nOn = !nOn;
		nHiml = ImageList_Create(20, 20, ILC_COLOR32, 0, 0);//订制图标框架
		ImageList_SetBkColor(nHiml, m_List_Tcp.GetBkColor());//设置图标列表底色
		m_List_Tcp.SendMessage(LVM_SETIMAGELIST, 1, (LPARAM)nHiml);//设置超级列表显示图标
	}


	// 定义函数指针
	PInternalGetTcpTable2 InternalGetTcpTable2;
	PInternalGetUdpTableWithOwnerPid InternalGetUdpTableWithOwnerPid;

	// 获取函数的入口地址	
	HMODULE hModule = LoadLibrary(L"iphlpapi.dll");
	InternalGetTcpTable2 = (PInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");
	InternalGetUdpTableWithOwnerPid = (PInternalGetUdpTableWithOwnerPid)
		GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");
	// 定义存放TCP和UDP信息的结构体指针
	PMIB_TCPEXTABLE pTcpExTable;
	PMIB_UDPEXTABLE pUdpExTable;



	if (InternalGetTcpTable2 == NULL)
	{
		MessageBox(TEXT("iphlpapi.dll错误！"), TEXT("提示"), MB_ICONERROR);
		return ;
	}

	//调用函数，参数：指向进程堆指针，获取当前进程的一个堆
	if (InternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1) != 0)
	{
		MessageBox(TEXT("InternalGetTcpTable2错误！"), TEXT("提示"), MB_ICONERROR);
		return ;
	}
	if (InternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1) != 0)
	{
		MessageBox(TEXT("InternalGetUdpTableWithOwnerPid错误！"), TEXT("提示"), MB_ICONERROR);
		return ;
	}

	CString szProcessName;
	CString szProcessId;
	CString szLocalAddr;
	CString szLocalPort;
	CString szRemoteAddr;
	CString szRemotePort;
	CString nPath;
	CString nProcessPath;


	in_addr inadLocal, inadRemote;
	DWORD dwRemotePort = 0;
	DWORD nIndex;
	DWORD nStatusNum;
	HANDLE nProcessHandle;
	CHAR *szStatus[13] =
	{ "已关闭","正在监听", "同步发送", "同步接收", "已建立", "FIN_WAIT1", "FIN_WAIT2",
		"关闭等待", "正在关闭", "最后确认", "延时等待", "删除", "未知" };

	// 打印TCP扩展连接表信息
	for (UINT i = 0; i < pTcpExTable->dwNumEntries; ++i)
	{
		nIndex = m_List_Tcp.GetItemCount();

		nStatusNum = pTcpExTable->table[i].dwState;
		if (nStatusNum >= 12)nStatusNum = 12;

		// 本地IP地址
		inadLocal.s_addr = pTcpExTable->table[i].dwLocalAddr;

		// 远程端口
		if (strcmp(szStatus[nStatusNum], "正在监听") != 0)dwRemotePort = pTcpExTable->table[i].dwRemotePort;
		else dwRemotePort = 0;

		// 远程IP地址
		inadRemote.s_addr = pTcpExTable->table[i].dwRemoteAddr;


		szLocalAddr.Format(TEXT("%s"), IntToIp(inadLocal));
		szLocalPort.Format(TEXT("%u"), ntohs((WORD)(0x0000FFFF & pTcpExTable->table[i].dwLocalPort)));

		szRemoteAddr.Format(TEXT("%s"), IntToIp(inadRemote));
		szRemotePort.Format(TEXT("%u"), ntohs((WORD)(0x0000FFFF & dwRemotePort)));

		szProcessId.Format(TEXT("%d"), pTcpExTable->table[i].dwProcessId);

		GetProcessName(szProcessName, pTcpExTable->table[i].dwProcessId);

		nProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, pTcpExTable->table[i].dwProcessId);
		GetProcessFilePath(nProcessHandle, nProcessPath);
		CloseHandle(nProcessHandle);

		m_List_Tcp.InsertItem(nIndex, szProcessName);
		m_List_Tcp.SetItemText(nIndex, 1, szProcessId);
		m_List_Tcp.SetItemText(nIndex, 2, szLocalAddr);
		m_List_Tcp.SetItemText(nIndex, 3, szLocalPort);
		m_List_Tcp.SetItemText(nIndex, 4, szRemoteAddr);
		m_List_Tcp.SetItemText(nIndex, 5, szRemotePort);
		m_List_Tcp.SetItemText(nIndex, 6, szStatus[nStatusNum]);
		m_List_Tcp.SetItemText(nIndex, 7, TEXT("TCP"));
		m_List_Tcp.SetItemText(nIndex, 8, nProcessPath);

	}

	for (DWORD i = 0; i < pUdpExTable->dwNumEntries; ++i)
	{
		nIndex = m_List_Tcp.GetItemCount();
		// 本地IP地址
		inadLocal.s_addr = pUdpExTable->table[i].dwLocalAddr;


		szLocalAddr.Format(TEXT("%s"), IntToIp(inadLocal));
		szLocalPort.Format(TEXT("%u"), ntohs((WORD)(0x0000FFFF & pUdpExTable->table[i].dwLocalPort)));

		szProcessId.Format(TEXT("%d"), pUdpExTable->table[i].dwProcessId);

		GetProcessName(szProcessName, pUdpExTable->table[i].dwProcessId);

		nProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, pUdpExTable->table[i].dwProcessId);
		GetProcessFilePath(nProcessHandle, nProcessPath);
		CloseHandle(nProcessHandle);

		m_List_Tcp.InsertItem(nIndex, szProcessName);
		m_List_Tcp.SetItemText(nIndex, 1, szProcessId);
		m_List_Tcp.SetItemText(nIndex, 2, szLocalAddr);
		m_List_Tcp.SetItemText(nIndex, 3, szLocalPort);
		m_List_Tcp.SetItemText(nIndex, 4, TEXT("0.0.0.0"));
		m_List_Tcp.SetItemText(nIndex, 5, TEXT("0"));
		m_List_Tcp.SetItemText(nIndex, 6, TEXT(""));
		m_List_Tcp.SetItemText(nIndex, 7, TEXT("UDP"));
		m_List_Tcp.SetItemText(nIndex, 8, nProcessPath);

	}


	DWORD nListNum = m_List_Tcp.GetItemCount();

	ImageList_SetImageCount(nHiml, nListNum);

	for (DWORD i = 0; i < nListNum; i++)
	{
		nPath = GetPathEx(m_List_Tcp.GetItemText(i, 8));

		DWORD nFileAttributes = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;//获取文件信息
		if (!SHGetFileInfo(nPath, nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON))
			SHGetFileInfo(nPath, nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
		//DWORD nIco = ImageList_AddIcon(nHiml, nPsfi.hIcon);//置入图标返回图标下标
		ImageList_ReplaceIcon(nHiml, i, nPsfi.hIcon);
		DestroyIcon(nPsfi.hIcon);

		m_List_Tcp.SetItemImageEx(i, 0, m_List_Tcp.GetItemText(i, 0), i);
	}


	LocalFree(pTcpExTable);
	LocalFree(pUdpExTable);
	FreeLibrary(hModule);
}

//断开连接
VOID MyTcpDlg::EndTcpLine(CString lpszLocalAddr, CString lpszRemoteAddr, CString dwLocalPort, CString dwRemotePort)
{
	DWORD nLocalPort = 0;
	DWORD nRemotePort = 0;

	_stscanf_s(dwLocalPort, TEXT("%d"), &nLocalPort);
	_stscanf_s(dwRemotePort, TEXT("%d"), &nRemotePort);

	DWORD dwRetVal = ERROR_NOT_FOUND;
	MIB_TCPROW srtTcpRow;
	srtTcpRow.dwLocalAddr = IpToInt(lpszLocalAddr);
	srtTcpRow.dwRemoteAddr = IpToInt(lpszRemoteAddr);
	srtTcpRow.dwLocalPort = htons((WORD)nLocalPort);
	srtTcpRow.dwRemotePort = htons((WORD)nRemotePort);
	srtTcpRow.dwState = MIB_TCP_STATE_DELETE_TCB; //目前为止，settcpEntry只支持该参数
	dwRetVal = SetTcpEntry(&srtTcpRow);

}

void MyTcpDlg::OnEndline()
{
	CString lpszLocalAddr, lpszRemoteAddr, dwLocalPort, dwRemotePort;
	lpszLocalAddr = m_List_Tcp.GetItemText(m_List_Tcp.GetSelectionEx(), 2);
	dwLocalPort = m_List_Tcp.GetItemText(m_List_Tcp.GetSelectionEx(), 3);
	lpszRemoteAddr = m_List_Tcp.GetItemText(m_List_Tcp.GetSelectionEx(), 4);
	dwRemotePort = m_List_Tcp.GetItemText(m_List_Tcp.GetSelectionEx(), 5);
	EndTcpLine(lpszLocalAddr, lpszRemoteAddr, dwLocalPort, dwRemotePort);

	PrintTcpUdp();

}

void MyTcpDlg::OnRefershline()
{
	PrintTcpUdp();
}

void MyTcpDlg::OnRclickListTcp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu *nMenu = m_Menu.GetSubMenu(0);
	POINT pos;
	GetCursorPos(&pos);

	if (pNMItemActivate->iItem == -1)nMenu->EnableMenuItem(ID_ENDLINE, TRUE);
	else nMenu->EnableMenuItem(ID_ENDLINE, FALSE);


	nMenu->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);

}
