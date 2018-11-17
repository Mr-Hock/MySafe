#pragma once
#include "afxcmn.h"
#include "ClistCtrlEx.h"


// MyTcpDlg �Ի���

class MyTcpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyTcpDlg)

public:
	MyTcpDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MyTcpDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TCP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	ClistCtrlEx m_List_Tcp;
	CMenu m_Menu;

	VOID PrintTcpUdp();
	VOID EndTcpLine(CString lpszLocalAddr, CString lpszRemoteAddr, CString dwLocalPort, CString dwRemotePort);

	typedef struct
	{
		DWORD   dwState;        	// ����״̬
		DWORD   dwLocalAddr;    	// ���ص�ַ
		DWORD   dwLocalPort;    	// ���ض˿�
		DWORD   dwRemoteAddr;   	// Զ�̵�ַ
		DWORD   dwRemotePort;   	// Զ�̶˿�
		DWORD	dwProcessId;		// ����ID��
		DWORD	dwFlag;				// ����
	} MIB_TCPEXROW, *PMIB_TCPEXROW;

	typedef struct
	{
		DWORD			dwNumEntries;	//��¼MIB_TCPEXROW����
		MIB_TCPEXROW	table[ANY_SIZE];//����������MIB_TCPEXROW
	} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;

	typedef struct
	{
		DWORD   dwLocalAddr;    	// ���ص�ַ
		DWORD   dwLocalPort;    	// ���ض˿�
		DWORD	  dwProcessId;		// ����ID��
	} MIB_UDPEXROW, *PMIB_UDPEXROW;
	typedef struct
	{
		DWORD			dwNumEntries;
		MIB_UDPEXROW	table[ANY_SIZE];
	} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;

	typedef DWORD(WINAPI *PInternalGetTcpTable2)(PMIB_TCPEXTABLE *pTcpTable,HANDLE heap,DWORD flags);
	typedef DWORD(WINAPI *PInternalGetUdpTableWithOwnerPid)(PMIB_UDPEXTABLE *pUdpTable,HANDLE heap,DWORD flags);
	afx_msg void OnKeydownListTcp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEndline();
	afx_msg void OnRefershline();
	afx_msg void OnRclickListTcp(NMHDR *pNMHDR, LRESULT *pResult);
};
