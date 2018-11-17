// MyServiceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyHead.h"
#include "MyServiceDlg.h"
#include "afxdialogex.h"


// MyServiceDlg 对话框

IMPLEMENT_DYNAMIC(MyServiceDlg, CDialogEx)

MyServiceDlg::MyServiceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVICE_DIALOG, pParent)
{
	Create(IDD_SERVICE_DIALOG);
}

MyServiceDlg::~MyServiceDlg()
{
}

void MyServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_List_Service);
}


BEGIN_MESSAGE_MAP(MyServiceDlg, CDialogEx)
	ON_COMMAND(ID_REFERSHSERVICE, &MyServiceDlg::OnRefershservice)
	ON_COMMAND(ID_STARTSERVICE, &MyServiceDlg::OnStartservice)
	ON_COMMAND(ID_STOPSERVICE, &MyServiceDlg::OnStopservice)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &MyServiceDlg::OnRclickListFile)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_FILE, &MyServiceDlg::OnKeydownListFile)
END_MESSAGE_MAP()


// MyServiceDlg 消息处理程序


BOOL MyServiceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Menu.LoadMenu(IDR_MENU3);
	SetMenu(&m_Menu);


	m_List_Service.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_List_Service.InsertColumn(0, TEXT("服务名称"), 0, 200);
	m_List_Service.InsertColumn(1, TEXT("显示名称"), 0, 250);
	m_List_Service.InsertColumn(2, TEXT("启动状态"), 0, 70);
	m_List_Service.InsertColumn(3, TEXT("启动类别"), 0, 70);
	m_List_Service.InsertColumn(4, TEXT("程序路径"), 0, 280);



	LoadImageService();


	PrintService();

	return TRUE;
}

void MyServiceDlg::PrintService()
{

	m_List_Service.DeleteAllItems();

	nSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	LPENUM_SERVICE_STATUS lpServices = NULL;
	DWORD    nSize = 0;
	DWORD    n;
	DWORD    nResumeHandle = 0;
	DWORD    nSize2 = 0;

	lpServices = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, 64 * 1024);      //注意分配足够的空间
	EnumServicesStatus(nSCManager, SERVICE_WIN32, SERVICE_STATE_ALL, (LPENUM_SERVICE_STATUS)lpServices, 64 * 1024, &nSize, &n, &nResumeHandle);

	LPQUERY_SERVICE_CONFIG ServicesInfo = NULL;


	for (DWORD i = 0; i < n; i++)
	{
		m_List_Service.InsertItem(i, lpServices[i].lpServiceName);
		m_List_Service.SetItemImageEx(i, 0, lpServices[i].lpServiceName, 0);

		m_List_Service.SetItemText(i, 1, lpServices[i].lpDisplayName);
		if (lpServices[i].ServiceStatus.dwCurrentState != SERVICE_STOPPED)
		{
			m_List_Service.SetItemText(i, 2, TEXT("已启动"));
		}



		SC_HANDLE service = NULL;
		service = OpenService(nSCManager, lpServices[i].lpServiceName, SERVICE_ALL_ACCESS);
		QueryServiceConfig(service, ServicesInfo, 0, &nSize2);
		ServicesInfo = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, nSize2);      //注意分配足够的空间
		QueryServiceConfig(service, ServicesInfo, nSize2, &nSize2);      //枚举各个服务信息


		switch (ServicesInfo->dwStartType)
		{
		case 2:
			m_List_Service.SetItemText(i, 3, TEXT("自动"));
			break;
		case 3:
			m_List_Service.SetItemText(i, 3, TEXT("手动"));
			break;
		case 4:
			m_List_Service.SetItemText(i, 3, TEXT("禁止"));
			break;
		}

		m_List_Service.SetItemText(i, 4, ServicesInfo->lpBinaryPathName);

		CloseServiceHandle(service);
	}

	//CloseServiceHandle(nSCManager);
}


void MyServiceDlg::LoadImageService()
{
	m_ServiceImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2);

	HICON hSmallIco;
	CString nDir = GetProgramDir();

	hSmallIco = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\service.ico"), 0);
	m_ServiceImageList.Add(hSmallIco);
	DestroyIcon(hSmallIco);

	m_List_Service.SetImageList(&m_ServiceImageList, LVSIL_SMALL);
}

void MyServiceDlg::OnRefershservice()
{
	PrintService();
}


void MyServiceDlg::OnStartservice()
{

	CString nServiceName;
	nServiceName = m_List_Service.GetItemText(m_List_Service.GetSelectionEx(), 0);

	SC_HANDLE service = NULL;
	service = OpenService(nSCManager, nServiceName, SERVICE_ALL_ACCESS);

	SERVICE_STATUS status;
	BOOL isSuccess = QueryServiceStatus(service, &status);
	if (!isSuccess)
	{
		MessageBox(TEXT("QueryServiceStatus error！"), TEXT("提示"), MB_ICONWARNING);
		CloseServiceHandle(service);
		return;
	}

	if (status.dwCurrentState == SERVICE_STOPPED)
	{
		isSuccess = StartService(service, NULL, NULL);
		//Sleep(3000);

		if (!isSuccess)
		{
			MessageBox(TEXT("启动服务失败！"), TEXT("提示"), MB_ICONWARNING);
			CloseServiceHandle(service);
			return;
		}
	}

	CloseServiceHandle(service);

	PrintService();
}


void MyServiceDlg::OnStopservice()
{
	CString nServiceName;
	nServiceName = m_List_Service.GetItemText(m_List_Service.GetSelectionEx(), 0);

	SC_HANDLE service = NULL;
	service = OpenService(nSCManager, nServiceName, SERVICE_ALL_ACCESS);

	SERVICE_STATUS status;
	BOOL isSuccess = QueryServiceStatus(service, &status);
	if (!isSuccess)
	{
		MessageBox(TEXT("QueryServiceStatus error！"), TEXT("提示"), MB_ICONWARNING);
		CloseServiceHandle(service);
		return;
	}

	if (status.dwCurrentState == SERVICE_RUNNING)
	{
		isSuccess = ControlService(service, SERVICE_CONTROL_STOP, &status);
		//Sleep(3000);
		if (!isSuccess)
		{
			MessageBox(TEXT("停止服务失败！"), TEXT("提示"), MB_ICONWARNING);
			CloseServiceHandle(service);
			return;
		}
	}

	CloseServiceHandle(service);

	PrintService();
}


void MyServiceDlg::OnRclickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu *nMenu = m_Menu.GetSubMenu(0);

	if (pNMItemActivate->iItem == -1)
	{
		nMenu->EnableMenuItem(ID_STARTSERVICE, TRUE);
		nMenu->EnableMenuItem(ID_STOPSERVICE, TRUE);
	}
	else
	{
		nMenu->EnableMenuItem(ID_STARTSERVICE, FALSE);
		nMenu->EnableMenuItem(ID_STOPSERVICE, FALSE);
	}


	POINT pos;
	GetCursorPos(&pos);
	nMenu->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);
}


void MyServiceDlg::OnKeydownListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	*pResult = 0;
	if (pLVKeyDow->wVKey == 116)PrintService();//F5
}
