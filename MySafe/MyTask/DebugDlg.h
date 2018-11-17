#pragma once
#include "ClistCtrlEx.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include <Psapi.h>

using std::vector;

//反汇编引擎头文件，静态库
#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "BeaEngine_4.1/Win32/headers/BeaEngine.h"
#pragma comment (lib , "BeaEngine_4.1/Win32/Win32/Lib/BeaEngine.lib")
#pragma comment(linker, "/NODEFAULTLIB:\"crt.lib\"")

//汇编引擎头文件，静态库
#include "XEDParse/XEDParse.h"
#pragma comment (lib,"XEDParse/x86/XEDParse_x86.lib")


// DebugDlg 对话框

class DebugDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DebugDlg)

public:
	DebugDlg(CString nProName, DWORD nPid);   // 标准构造函数
	virtual ~DebugDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEBUG_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_Num_Asm = 27;
	DWORD m_Num_Heap = 128;
	DWORD m_Num_Memory = 9;


	typedef struct _BREAKPOINT
	{
		DWORD nAddress;
		BYTE nData;
	}BREAKPOINT,*PBREAKPOINT;

	typedef struct _EFLAGS
	{
		BYTE CF : 1;
		BYTE NULL1 : 1;
		BYTE PF : 1;
		BYTE NULL2 : 1;
		BYTE AF : 1;
		BYTE NULL3 : 1;
		BYTE ZF : 1;
		BYTE SF : 1;
		BYTE TF : 1;
		BYTE IF : 1;
		BYTE DF : 1;
		BYTE OF : 1;
		BYTE NULL4 : 4;
	}EFLAGS ,*PEFLAGS;

	typedef struct _DR7
	{
		BYTE Dr0_L : 1;		//1
		BYTE Dr0_G : 1;
		BYTE Dr1_L : 1;
		BYTE Dr1_G : 1;
		BYTE Dr2_L : 1;
		BYTE Dr2_G : 1;		
		BYTE Dr3_L : 1;
		BYTE Dr3_G : 1;

		BYTE NULL1 : 3;		//001
		BYTE GD : 1;		//保留
		BYTE NULL2 : 2;		//00

		BYTE Dr0_RWE : 2;	//00:只执行 01:写入数据断点 10:I/O端口断点 11:读或写数据断点
		BYTE Dr0_LEN : 2;	//00:1字节 01:2字节 10:保留 11:4字节
		BYTE Dr1_RWE : 2;
		BYTE Dr1_LEN : 2;
		BYTE Dr2_RWE : 2;
		BYTE Dr2_LEN : 2;
		BYTE Dr3_RWE : 2;
		BYTE Dr3_LEN : 2;

	}DR7,*PDR7;

	vector<BREAKPOINT>m_BreakPoint;
	vector<BREAKPOINT>m_TempBreakPoint;
	DWORD m_HardBreakPoint[4]{};
	DWORD m_TempHardBreakPoint[4]{};

	CString m_WinText;
	DWORD m_Pid = 0;
	DWORD m_Tid = 0;

	HANDLE m_ProcessHandle = nullptr;
	HANDLE m_ThreadHandle = nullptr;

	DWORD m_Flag_Break = 0;
	DWORD m_Flag_Address = 1;
	DWORD m_Flag_Opcode = 2;
	DWORD m_Flag_Asm = 3;
	DWORD m_Flag_Mark = 4;
	DWORD m_Flag_HeapAddress = 0;
	DWORD m_Flag_HeapData = 1;
	DWORD m_Flag_MemoryAddress = 0;
	DWORD m_Flag_MemoryData = 1;
	DWORD m_Flag_MemoryStr = 2;

	COLORREF m_EaxColor = 0;
	COLORREF m_EbxColor = 0;
	COLORREF m_EcxColor = 0;
	COLORREF m_EdxColor = 0;
	COLORREF m_EsiColor = 0;
	COLORREF m_EdiColor = 0;
	COLORREF m_EbpColor = 0;
	COLORREF m_EspColor = 0;
	COLORREF m_EipColor = 0;
	COLORREF m_Dr0Color = 0;
	COLORREF m_Dr1Color = 0;
	COLORREF m_Dr2Color = 0;
	COLORREF m_Dr3Color = 0;
	COLORREF m_Dr6Color = 0;
	COLORREF m_Dr7Color = 0;
	COLORREF m_EsColor = 0;
	COLORREF m_CsColor = 0;
	COLORREF m_SsColor = 0;
	COLORREF m_DsColor = 0;
	COLORREF m_FsColor = 0;
	COLORREF m_GsColor = 0;
	COLORREF m_CfColor = 0;
	COLORREF m_PfColor = 0;
	COLORREF m_AfColor = 0;
	COLORREF m_ZfColor = 0;
	COLORREF m_SfColor = 0;
	COLORREF m_TfColor = 0;
	COLORREF m_DfColor = 0;
	COLORREF m_OfColor = 0;


	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDblclkListHeap(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawListMemory(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawListAsm(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawListHeap(NMHDR *pNMHDR, LRESULT *pResult);

	ClistCtrlEx m_List_Asm;
	ClistCtrlEx m_List_Heap;
	ClistCtrlEx m_List_Memory;
	CEdit m_Edit_Output;
	//CEdit m_Edit_Address;
	CComboBox m_Combo_Address;
	CEdit m_Edit_Eax;
	CEdit m_Edit_Ebx;
	CEdit m_Edit_Ecx;
	CEdit m_Edit_Edx;
	CEdit m_Edit_Esi;
	CEdit m_Edit_Edi;
	CEdit m_Edit_Ebp;
	CEdit m_Edit_Esp;
	CEdit m_Edit_Eip;
	CEdit m_Edit_Dr0;
	CEdit m_Edit_Dr1;
	CEdit m_Edit_Dr2;
	CEdit m_Edit_Dr3;
	CEdit m_Edit_Dr6;
	CEdit m_Edit_Dr7;
	CEdit m_Edit_Es;
	CEdit m_Edit_Cs;
	CEdit m_Edit_Ss;
	CEdit m_Edit_Ds;
	CEdit m_Edit_Fs;
	CEdit m_Edit_Gs;
	CEdit m_Edit_Cf;
	CEdit m_Edit_Pf;
	CEdit m_Edit_Af;
	CEdit m_Edit_Zf;
	CEdit m_Edit_Sf;
	CEdit m_Edit_Tf;
	CEdit m_Edit_Df;
	CEdit m_Edit_Of;


	VOID InitAsm(DWORD nReadAddress);
	VOID PrintOpcode(BYTE* pOpcode, DWORD nSize, DWORD nItem);
	VOID PrintBreakFlag(DWORD nBreakAddress, DWORD nItem);
	VOID PrintOnceAsm(DWORD nReadAddress);
	DWORD GetBeforeAddress(DWORD nReadAddress);
	DWORD GetNextAddress(DWORD nReadAddress);

	DWORD GetOepAddress();
	DWORD GetDataAddress();
	DWORD GetImageBassAddress();

	VOID SetBreakPoint(DWORD nBreakAddress);
	VOID SetTempBreakPoint(DWORD nBreakAddress);

	VOID OpenAllBreakPoint();
	VOID CloseAllBreakPoint();

	VOID DelBreakPoint(DWORD nBreakAddress);
	VOID DelTempBreakPoint(DWORD nBreakAddress);

	VOID ReadMemoryDword(DWORD nAddress, LPDWORD nValue);
	VOID WriteMemoryDword(DWORD nAddress,DWORD nValue);

	VOID ReadMemoryByte(DWORD nAddress, LPBYTE nValue);
	VOID WriteMemoryByte(DWORD nAddress, BYTE nValue);

	VOID ReadMemoryBytes(DWORD nAddress, LPBYTE nValue,DWORD nLen);
	VOID WriteMemoryBytes(DWORD nAddress, LPBYTE nValue, DWORD nLen);

	VOID ReadMemoryWord(DWORD nAddress, LPWORD nValue);
	VOID WriteMemoryWord(DWORD nAddress, WORD nValue);

	VOID PrintContext(CONTEXT &nCONTEXT);
	VOID PrintHeap(CONTEXT &nCONTEXT);
	VOID ShowSingleAsm(DWORD nShowAddress);
	DWORD FindAddress(DWORD nShowAddress);
	DWORD IsBreakPoint(DWORD nBreakAddress);
	DWORD IsTempBreakPoint(DWORD nBreakAddress);

	DWORD GetApiAddress(CString nApi);

	BOOL IsCallAsm(DWORD nAddress);

	VOID PrintMemory(DWORD nAddress);

	BOOL SetHardBreakPoint(DWORD nBreakAddress);
	DWORD IsHardBreakPoint(DWORD nBreakAddress);
	VOID DelHardBreakPoint(DWORD nBreakAddress);
	VOID OpenAllHardBreakPoint();
	VOID CloseAllHardBreakPoint();
	VOID SetTempHardBreakPoint(DWORD nBreakAddress);
	VOID DelTempHardBreakPoint(DWORD nBreakAddress);
	DWORD IsTempHardBreakPoint(DWORD nBreakAddress);


	typedef struct _LISTCOLOR
	{
		DWORD Item;
		DWORD SubItem;
		COLORREF TextColor;
		COLORREF BkColor;
	}LISTCOLOR,*PLISTCOLOR;

	vector<LISTCOLOR>m_AsmColor;

	COLORREF COLOR_WHITE = RGB(255, 255, 255);
	COLORREF COLOR_BLACK = RGB(0, 0, 0);
	COLORREF COLOR_RED = RGB(255, 0, 0);
	COLORREF COLOR_GREEN = RGB(0, 178, 102);
	COLORREF COLOR_BLUE = RGB(51, 153, 255);
	COLORREF COLOR_ORANGE = RGB(255, 153, 0);
	COLORREF COLOR_VIOLET = RGB(153, 0, 153);
	
	VOID SetItemColor(DWORD Item, DWORD SubItem, COLORREF TextColor, COLORREF BkColor);
	VOID ClearItemColor();
	BOOL IsColorWord(CString nStr, LPDWORD nTextColor, LPDWORD nBkColor);
	BOOL IsJumpWord(CString nStr,LPDWORD nAddress);

	BOOL AsmToOpcaode(DWORD nAddress, CHAR *nAsm, LPBYTE nBytes, DWORD &nByteNum);
};
