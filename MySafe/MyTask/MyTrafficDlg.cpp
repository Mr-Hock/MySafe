// MyTrafficDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyHead.h"
#include "MyTrafficDlg.h"
#include "afxdialogex.h"

extern BOOL g_OpenTraffic;
// MyTrafficDlg 对话框

IMPLEMENT_DYNAMIC(MyTrafficDlg, CDialogEx)

MyTrafficDlg::MyTrafficDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRAFFIC_DIALOG, pParent)
{
	Create(IDD_TRAFFIC_DIALOG, GetDesktopWindow());
}

MyTrafficDlg::~MyTrafficDlg()
{
}

void MyTrafficDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DOWN, m_Static_Down);
	DDX_Control(pDX, IDC_STATIC_UP, m_Static_Up);
}


BEGIN_MESSAGE_MAP(MyTrafficDlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// MyTrafficDlg 消息处理程序

BOOL MyTrafficDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer(TIMER_REFRESHTRAFFIC,1000, NULL);//设置定时刷新

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 1);//窗口不在任务栏显示
	
	DWORD nX = GetSystemMetrics(SM_CXFULLSCREEN);//取出屏幕宽度
	DWORD nY = GetSystemMetrics(SM_CYFULLSCREEN);//取出屏幕高度
	SetWindowPos(NULL, nX - 200, nY - 50, 0, 0, SWP_NOSIZE);//设置到右下角坐标

	SetTransparentWin(m_hWnd, 180);//设置为半透明

	SetCircularWin(m_hWnd,12);//将窗口菱角圆滑

	return TRUE;
}
void MyTrafficDlg::OnClose()
{
	CDialogEx::OnClose();
	g_OpenTraffic = FALSE;

	DestroyWindow();
}

void MyTrafficDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);

	if (nIDEvent == TIMER_REFRESHTRAFFIC)GetNetTraffic();

}

void MyTrafficDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDown(nFlags, point);
	SendMessage(161, 2, 0);
}
void MyTrafficDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDialogEx::OnRButtonUp(nFlags, point);
	OnClose();
}

BOOL MyTrafficDlg::GetNetTraffic()
{
	DWORD dwSize = 0;//输出需要的缓冲区大小

	HINSTANCE hIphlpDll = LoadLibrary(L"iphlpapi.dll");//加载模块
	if (hIphlpDll == NULL) { return FALSE; }//如果失败返回
	lpGetIfTable = (GIT)GetProcAddress(hIphlpDll, "GetIfTable");//取出函数地址
	if (lpGetIfTable == NULL) { return FALSE; }//如果失败返回

	MIB_IFTABLE *pMIT = new MIB_IFTABLE[sizeof(MIB_IFTABLE)];//申请MIB_IFTABLE堆空间

	if (lpGetIfTable(pMIT, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)//缓冲区不足执行失败
	{
		delete[]pMIT;//删除原来的堆
		pMIT = new MIB_IFTABLE[dwSize];//重新申请一块合适大小的堆空间
	}

	if (lpGetIfTable(pMIT, &dwSize, 0) == NO_ERROR)//执行成功，没有错误
	{
		for (DWORD i = 0; i < (pMIT->dwNumEntries); i++)//根据dwNumEntries成员 循环遍历
		{
			if (pMIT->table[i].dwInOctets)
			{
				static DWORD nSendBack = 0;
				static DWORD nRecvBack = 0;

				if (!nSendBack)
				{
					nSendBack = pMIT->table[i].dwOutOctets;//上传
					nRecvBack = pMIT->table[i].dwInOctets;//下载
				}

				DWORD nSending = pMIT->table[i].dwOutOctets - nSendBack;
				DWORD nRevcing = pMIT->table[i].dwInOctets - nRecvBack;

				nSendBack = pMIT->table[i].dwOutOctets;
				nRecvBack = pMIT->table[i].dwInOctets;

				CString nTemp;
				nTemp.Format(TEXT("%.2lf KB/S"), (DOUBLE)nRevcing / 1024.0);
				m_Static_Down.SetWindowText(nTemp);

				nTemp.Format(TEXT("%.2lf KB/S"), (DOUBLE)nSending / 1024.0);
				m_Static_Up.SetWindowText(nTemp);
				break;

			}

		}
	}
	delete[]pMIT;
	FreeLibrary(hIphlpDll);
	return TRUE;
}



BOOL MyTrafficDlg::PreTranslateMessage(MSG* pMsg)
{

	if (pMsg->message == WM_KEYDOWN)
	{
		int a = pMsg->wParam;
		if (pMsg->wParam == 27 || pMsg->wParam == 13)return 0;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}




