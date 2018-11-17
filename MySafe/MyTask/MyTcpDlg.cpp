// MyTcpDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyHead.h"
#include "MyTcpDlg.h"
#include "afxdialogex.h"
#include <WS2tcpip.h>
#pragma comment(lib, "Iphlpapi.lib")

// MyTcpDlg �Ի���

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


// MyTcpDlg ��Ϣ�������


BOOL MyTcpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Menu.LoadMenu(IDR_MENU7);
	SetMenu(&m_Menu);


	m_List_Tcp.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Tcp.InsertColumn(0, TEXT("��������"), 0, 200);
	m_List_Tcp.InsertColumn(1, TEXT("����ID"), 0, 60);
	m_List_Tcp.InsertColumn(2, TEXT("����IP"), 0, 100);
	m_List_Tcp.InsertColumn(3, TEXT("���ض˿�"), 0, 60);
	m_List_Tcp.InsertColumn(4, TEXT("Զ��IP"), 0, 100);
	m_List_Tcp.InsertColumn(5, TEXT("Զ�̶˿�"), 0, 60);
	m_List_Tcp.InsertColumn(6, TEXT("����״̬"), 0, 100);
	m_List_Tcp.InsertColumn(7, TEXT("��������"), 0, 100);
	m_List_Tcp.InsertColumn(8, TEXT("����·��"), 0, 100);

	PrintTcpUdp();

	return TRUE;
}

void MyTcpDlg::OnKeydownListTcp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	*pResult = 0;
	if (pLVKeyDow->wVKey == 116)PrintTcpUdp();

}

//������������
VOID MyTcpDlg::PrintTcpUdp()
{
	m_List_Tcp.DeleteAllItems();

	static HIMAGELIST nHiml = { 0 };//ͼ���б�
	static SHFILEINFO nPsfi = { 0 };//�ļ���Ϣ
									//ImageList_Destroy(nHiml);//���ͼ���б�
	static BOOL nOn = FALSE;
	if (!nOn)
	{
		nOn = !nOn;
		nHiml = ImageList_Create(20, 20, ILC_COLOR32, 0, 0);//����ͼ����
		ImageList_SetBkColor(nHiml, m_List_Tcp.GetBkColor());//����ͼ���б��ɫ
		m_List_Tcp.SendMessage(LVM_SETIMAGELIST, 1, (LPARAM)nHiml);//���ó����б���ʾͼ��
	}


	// ���庯��ָ��
	PInternalGetTcpTable2 InternalGetTcpTable2;
	PInternalGetUdpTableWithOwnerPid InternalGetUdpTableWithOwnerPid;

	// ��ȡ��������ڵ�ַ	
	HMODULE hModule = LoadLibrary(L"iphlpapi.dll");
	InternalGetTcpTable2 = (PInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");
	InternalGetUdpTableWithOwnerPid = (PInternalGetUdpTableWithOwnerPid)
		GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");
	// ������TCP��UDP��Ϣ�Ľṹ��ָ��
	PMIB_TCPEXTABLE pTcpExTable;
	PMIB_UDPEXTABLE pUdpExTable;



	if (InternalGetTcpTable2 == NULL)
	{
		MessageBox(TEXT("iphlpapi.dll����"), TEXT("��ʾ"), MB_ICONERROR);
		return ;
	}

	//���ú�����������ָ����̶�ָ�룬��ȡ��ǰ���̵�һ����
	if (InternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1) != 0)
	{
		MessageBox(TEXT("InternalGetTcpTable2����"), TEXT("��ʾ"), MB_ICONERROR);
		return ;
	}
	if (InternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1) != 0)
	{
		MessageBox(TEXT("InternalGetUdpTableWithOwnerPid����"), TEXT("��ʾ"), MB_ICONERROR);
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
	{ "�ѹر�","���ڼ���", "ͬ������", "ͬ������", "�ѽ���", "FIN_WAIT1", "FIN_WAIT2",
		"�رյȴ�", "���ڹر�", "���ȷ��", "��ʱ�ȴ�", "ɾ��", "δ֪" };

	// ��ӡTCP��չ���ӱ���Ϣ
	for (UINT i = 0; i < pTcpExTable->dwNumEntries; ++i)
	{
		nIndex = m_List_Tcp.GetItemCount();

		nStatusNum = pTcpExTable->table[i].dwState;
		if (nStatusNum >= 12)nStatusNum = 12;

		// ����IP��ַ
		inadLocal.s_addr = pTcpExTable->table[i].dwLocalAddr;

		// Զ�̶˿�
		if (strcmp(szStatus[nStatusNum], "���ڼ���") != 0)dwRemotePort = pTcpExTable->table[i].dwRemotePort;
		else dwRemotePort = 0;

		// Զ��IP��ַ
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
		// ����IP��ַ
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

		DWORD nFileAttributes = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;//��ȡ�ļ���Ϣ
		if (!SHGetFileInfo(nPath, nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON))
			SHGetFileInfo(nPath, nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
		//DWORD nIco = ImageList_AddIcon(nHiml, nPsfi.hIcon);//����ͼ�귵��ͼ���±�
		ImageList_ReplaceIcon(nHiml, i, nPsfi.hIcon);
		DestroyIcon(nPsfi.hIcon);

		m_List_Tcp.SetItemImageEx(i, 0, m_List_Tcp.GetItemText(i, 0), i);
	}


	LocalFree(pTcpExTable);
	LocalFree(pUdpExTable);
	FreeLibrary(hModule);
}

//�Ͽ�����
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
	srtTcpRow.dwState = MIB_TCP_STATE_DELETE_TCB; //ĿǰΪֹ��settcpEntryֻ֧�ָò���
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
