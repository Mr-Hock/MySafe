// MyTrafficDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyHead.h"
#include "MyTrafficDlg.h"
#include "afxdialogex.h"

extern BOOL g_OpenTraffic;
// MyTrafficDlg �Ի���

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


// MyTrafficDlg ��Ϣ�������

BOOL MyTrafficDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer(TIMER_REFRESHTRAFFIC,1000, NULL);//���ö�ʱˢ��

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 1);//���ڲ�����������ʾ
	
	DWORD nX = GetSystemMetrics(SM_CXFULLSCREEN);//ȡ����Ļ���
	DWORD nY = GetSystemMetrics(SM_CYFULLSCREEN);//ȡ����Ļ�߶�
	SetWindowPos(NULL, nX - 200, nY - 50, 0, 0, SWP_NOSIZE);//���õ����½�����

	SetTransparentWin(m_hWnd, 180);//����Ϊ��͸��

	SetCircularWin(m_hWnd,12);//���������Բ��

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
	DWORD dwSize = 0;//�����Ҫ�Ļ�������С

	HINSTANCE hIphlpDll = LoadLibrary(L"iphlpapi.dll");//����ģ��
	if (hIphlpDll == NULL) { return FALSE; }//���ʧ�ܷ���
	lpGetIfTable = (GIT)GetProcAddress(hIphlpDll, "GetIfTable");//ȡ��������ַ
	if (lpGetIfTable == NULL) { return FALSE; }//���ʧ�ܷ���

	MIB_IFTABLE *pMIT = new MIB_IFTABLE[sizeof(MIB_IFTABLE)];//����MIB_IFTABLE�ѿռ�

	if (lpGetIfTable(pMIT, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)//����������ִ��ʧ��
	{
		delete[]pMIT;//ɾ��ԭ���Ķ�
		pMIT = new MIB_IFTABLE[dwSize];//��������һ����ʴ�С�Ķѿռ�
	}

	if (lpGetIfTable(pMIT, &dwSize, 0) == NO_ERROR)//ִ�гɹ���û�д���
	{
		for (DWORD i = 0; i < (pMIT->dwNumEntries); i++)//����dwNumEntries��Ա ѭ������
		{
			if (pMIT->table[i].dwInOctets)
			{
				static DWORD nSendBack = 0;
				static DWORD nRecvBack = 0;

				if (!nSendBack)
				{
					nSendBack = pMIT->table[i].dwOutOctets;//�ϴ�
					nRecvBack = pMIT->table[i].dwInOctets;//����
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




