#pragma once
#include "afxcmn.h"
#include "ClistCtrlEx.h"


// MyTcpDlg 对话框

class MyTcpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyTcpDlg)

public:
	MyTcpDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyTcpDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TCP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	ClistCtrlEx m_List_Tcp;
	CMenu m_Menu;

	VOID PrintTcpUdp();
	VOID EndTcpLine(CString lpszLocalAddr, CString lpszRemoteAddr, CString dwLocalPort, CString dwRemotePort);

	typedef struct
	{
		DWORD   dwState;        	// 连接状态
		DWORD   dwLocalAddr;    	// 本地地址
		DWORD   dwLocalPort;    	// 本地端口
		DWORD   dwRemoteAddr;   	// 远程地址
		DWORD   dwRemotePort;   	// 远程端口
		DWORD	dwProcessId;		// 进程ID号
		DWORD	dwFlag;				// 保留
	} MIB_TCPEXROW, *PMIB_TCPEXROW;

	typedef struct
	{
		DWORD			dwNumEntries;	//记录MIB_TCPEXROW数量
		MIB_TCPEXROW	table[ANY_SIZE];//不等数量的MIB_TCPEXROW
	} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;

	typedef struct
	{
		DWORD   dwLocalAddr;    	// 本地地址
		DWORD   dwLocalPort;    	// 本地端口
		DWORD	  dwProcessId;		// 进程ID号
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
