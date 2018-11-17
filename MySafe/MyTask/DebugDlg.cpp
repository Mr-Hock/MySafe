// DebugDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyHead.h"
#include "DebugDlg.h"
#include "afxdialogex.h"


#define SETUP_STOP 0
#define SETUP_IN 1
#define SETUP_OUT 2
#define SETUP_RUN 3

#define BREAK_SOFT 0
#define BREAK_HARD 1



LONG g_AsmListProc;
LONG g_MemoryListProc;

DWORD nReadAddressFirst;
DWORD nReadAddressSecond;
DWORD nReadAddressEnd;
DebugDlg* g_pDlg = NULL;


DWORD g_SingDebug = SETUP_RUN;
BOOL g_BreakType = BREAK_SOFT;

// DebugDlg 对话框

IMPLEMENT_DYNAMIC(DebugDlg, CDialogEx)

DebugDlg::DebugDlg(CString nProName, DWORD nPid)
	: CDialogEx(IDD_DEBUG_DIALOG, NULL),
	m_Pid(nPid),
	m_WinText(nProName + TEXT(" - "))
{
	Create(IDD_DEBUG_DIALOG);
	SetWindowText(m_WinText + TEXT("调试进程"));
}

DebugDlg::~DebugDlg()
{
}

void DebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ASM, m_List_Asm);
	DDX_Control(pDX, IDC_LIST_HEAP, m_List_Heap);
	DDX_Control(pDX, IDC_EDIT12, m_Edit_Output);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Eax);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_Ebx);
	DDX_Control(pDX, IDC_EDIT3, m_Edit_Ecx);
	DDX_Control(pDX, IDC_EDIT4, m_Edit_Edx);
	DDX_Control(pDX, IDC_EDIT5, m_Edit_Esi);
	DDX_Control(pDX, IDC_EDIT34, m_Edit_Edi);
	DDX_Control(pDX, IDC_EDIT35, m_Edit_Ebp);
	DDX_Control(pDX, IDC_EDIT6, m_Edit_Esp);
	DDX_Control(pDX, IDC_EDIT11, m_Edit_Eip);
	DDX_Control(pDX, IDC_EDIT15, m_Edit_Dr0);
	DDX_Control(pDX, IDC_EDIT7, m_Edit_Dr1);
	DDX_Control(pDX, IDC_EDIT8, m_Edit_Dr2);
	DDX_Control(pDX, IDC_EDIT9, m_Edit_Dr3);
	DDX_Control(pDX, IDC_EDIT17, m_Edit_Dr6);
	DDX_Control(pDX, IDC_EDIT18, m_Edit_Dr7);
	DDX_Control(pDX, IDC_COMBO3, m_Combo_Address);
	DDX_Control(pDX, IDC_EDIT22, m_Edit_Es);
	DDX_Control(pDX, IDC_EDIT19, m_Edit_Cs);
	DDX_Control(pDX, IDC_EDIT20, m_Edit_Ss);
	DDX_Control(pDX, IDC_EDIT21, m_Edit_Ds);
	DDX_Control(pDX, IDC_EDIT23, m_Edit_Fs);
	DDX_Control(pDX, IDC_EDIT24, m_Edit_Gs);
	DDX_Control(pDX, IDC_LIST_MEMORY, m_List_Memory);
	DDX_Control(pDX, IDC_EDIT28, m_Edit_Cf);
	DDX_Control(pDX, IDC_EDIT25, m_Edit_Pf);
	DDX_Control(pDX, IDC_EDIT26, m_Edit_Af);
	DDX_Control(pDX, IDC_EDIT27, m_Edit_Zf);
	DDX_Control(pDX, IDC_EDIT29, m_Edit_Sf);
	DDX_Control(pDX, IDC_EDIT30, m_Edit_Tf);
	DDX_Control(pDX, IDC_EDIT31, m_Edit_Df);
	DDX_Control(pDX, IDC_EDIT32, m_Edit_Of);
}


BEGIN_MESSAGE_MAP(DebugDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_HEAP, &DebugDlg::OnDblclkListHeap)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_MEMORY, &DebugDlg::OnCustomdrawListMemory)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_ASM, &DebugDlg::OnCustomdrawListAsm)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_HEAP, &DebugDlg::OnCustomdrawListHeap)
END_MESSAGE_MAP()



LRESULT CALLBACK ListAsmProc(HWND hwnd,UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_VSCROLL)
	{
		if (wParam == 0)
		{
			g_pDlg->PrintOnceAsm(g_pDlg->GetBeforeAddress(nReadAddressFirst));

			if (g_pDlg->m_List_Asm.GetSelectionEx() <= 25)
			{
				int nIndex = g_pDlg->m_List_Asm.GetSelectionEx();
				g_pDlg->m_List_Asm.SetSelectionEx(nIndex + 1);
			}
		}
		else 
		{ 
			g_pDlg->PrintOnceAsm(nReadAddressSecond); 
			if (g_pDlg->m_List_Asm.GetSelectionEx() != 0)
			{
				int nIndex = g_pDlg->m_List_Asm.GetSelectionEx();
				g_pDlg->m_List_Asm.SetSelectionEx(nIndex - 1);
			}

		}
	}
	else if (uMsg == WM_LBUTTONDBLCLK)
	{

	}
	else if(uMsg == WM_KEYDOWN)
	{
		DWORD nBreakAddress;
		DWORD nReadAddress;
		CString nTempAddress;
		DWORD nIndex;
		DWORD nSelect;
		
		if (wParam == VK_SPACE)
		{
			nTempAddress = g_pDlg->m_List_Asm.GetItemText(g_pDlg->m_List_Asm.GetSelectionEx(), g_pDlg->m_Flag_Asm);

			if (g_pDlg->IsJumpWord(nTempAddress,&nReadAddress))
			{
				g_pDlg->InitAsm(nReadAddress);
				g_pDlg->m_List_Asm.SetSelectionEx(0);

			}
		}
		else if (wParam == VK_F2)
		{
			nSelect = g_pDlg->m_List_Asm.GetSelectionEx();
			nTempAddress = g_pDlg->m_List_Asm.GetItemText(nSelect, g_pDlg->m_Flag_Address);
			_stscanf_s(nTempAddress, TEXT("%x"), &nBreakAddress);

			nIndex = g_pDlg->IsBreakPoint(nBreakAddress);
			if (nIndex == -1)
			{
				g_pDlg->m_List_Asm.SetItemText(nSelect, g_pDlg->m_Flag_Break, TEXT("●"));
				g_pDlg->SetBreakPoint(nBreakAddress);
				g_pDlg->SetItemColor(nSelect, g_pDlg->m_Flag_Break, g_pDlg->COLOR_WHITE, g_pDlg->COLOR_RED);
				g_pDlg->SetItemColor(nSelect, g_pDlg->m_Flag_Address, g_pDlg->COLOR_WHITE, g_pDlg->COLOR_RED);
			}
			else
			{
				g_pDlg->m_List_Asm.SetItemText(nSelect, g_pDlg->m_Flag_Break, TEXT(""));
				g_pDlg->DelBreakPoint(nBreakAddress);
				g_pDlg->SetItemColor(nSelect, g_pDlg->m_Flag_Break, g_pDlg->COLOR_BLACK, g_pDlg->COLOR_WHITE);
				g_pDlg->SetItemColor(nSelect, g_pDlg->m_Flag_Address, g_pDlg->COLOR_BLACK, g_pDlg->COLOR_WHITE);
			}

			OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("%s软件断点：%08X"), nIndex == -1 ? TEXT("设置") : TEXT("删除"), nBreakAddress);
		}
		else if (wParam == VK_F3)
		{
			nSelect = g_pDlg->m_List_Asm.GetSelectionEx();
			nTempAddress = g_pDlg->m_List_Asm.GetItemText(nSelect, g_pDlg->m_Flag_Address);
			_stscanf_s(nTempAddress, TEXT("%x"), &nBreakAddress);

			nIndex = g_pDlg->IsHardBreakPoint(nBreakAddress);
			if (nIndex == -1)
			{
				g_pDlg->m_List_Asm.SetItemText(nSelect, g_pDlg->m_Flag_Break, TEXT("■"));
				g_pDlg->SetHardBreakPoint(nBreakAddress);
				g_pDlg->SetItemColor(nSelect, g_pDlg->m_Flag_Break, g_pDlg->COLOR_WHITE, g_pDlg->COLOR_BLACK);
				g_pDlg->SetItemColor(nSelect, g_pDlg->m_Flag_Address, g_pDlg->COLOR_WHITE, g_pDlg->COLOR_BLACK);
			}
			else
			{
				g_pDlg->m_List_Asm.SetItemText(nSelect, g_pDlg->m_Flag_Break, TEXT(""));
				g_pDlg->DelHardBreakPoint(nBreakAddress);
				g_pDlg->SetItemColor(nSelect, g_pDlg->m_Flag_Break, g_pDlg->COLOR_BLACK, g_pDlg->COLOR_WHITE);
				g_pDlg->SetItemColor(nSelect, g_pDlg->m_Flag_Address, g_pDlg->COLOR_BLACK, g_pDlg->COLOR_WHITE);
			}

			OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("%s硬件断点：%08X"), nIndex == -1 ? TEXT("设置") : TEXT("删除"), nBreakAddress);
		}
		else if (wParam == VK_F4)
		{
		}
		else if (wParam == VK_F7)
		{
			g_SingDebug = SETUP_IN;
			ResumeThread(g_pDlg->m_ThreadHandle);
			//OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("F7单步步入"));
		}
		else if (wParam == VK_F8)
		{
			g_SingDebug = SETUP_OUT;
			ResumeThread(g_pDlg->m_ThreadHandle);
			//OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("F8单步步过"));
		}
		else if (wParam == VK_F9)
		{
			g_SingDebug = SETUP_RUN;
			ResumeThread(g_pDlg->m_ThreadHandle);
			//OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("F9运行"));

		}
	}

	return CallWindowProc((WNDPROC)g_AsmListProc, hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK ListMemoryProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_VSCROLL)
	{
		CString nTemp;
		DWORD nAddressFirst;
		DWORD nAddressSecond;
		DWORD nSeg;
		if (wParam == 0)
		{
			nTemp = g_pDlg->m_List_Memory.GetItemText(0, g_pDlg->m_Flag_MemoryAddress);
			_stscanf_s(nTemp, TEXT("%x"), &nAddressFirst);
			nTemp = g_pDlg->m_List_Memory.GetItemText(1, g_pDlg->m_Flag_MemoryAddress);
			_stscanf_s(nTemp, TEXT("%x"), &nAddressSecond);

			nSeg = nAddressSecond - nAddressFirst;
			g_pDlg->PrintMemory(nAddressFirst - nSeg);
		}
		else
		{
			nTemp = g_pDlg->m_List_Memory.GetItemText(0, g_pDlg->m_Flag_MemoryAddress);
			_stscanf_s(nTemp, TEXT("%x"), &nAddressFirst);
			nTemp = g_pDlg->m_List_Memory.GetItemText(1, g_pDlg->m_Flag_MemoryAddress);
			_stscanf_s(nTemp, TEXT("%x"), &nAddressSecond);

			nSeg = nAddressSecond - nAddressFirst;
			g_pDlg->PrintMemory(nAddressFirst + nSeg);
		}
	}

	return CallWindowProc((WNDPROC)g_MemoryListProc, hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI DebugThread(LPVOID lpThreadParameter)
{
	DebugDlg *nDlg = (DebugDlg*)lpThreadParameter;

	DEBUG_EVENT nDebugEvent{};

	if (!DebugActiveProcess(nDlg->m_Pid))MessageBox(NULL,TEXT("调试进程失败！"), TEXT("提示"), MB_ICONERROR);


	while (WaitForDebugEvent(&nDebugEvent, INFINITE))
	{
		if (nDebugEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
		{
			if (nDebugEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)
			{
			
				CONTEXT nContext{};
				nContext.ContextFlags = CONTEXT_FULL;
				GetThreadContext(nDlg->m_ThreadHandle, &nContext);
				nContext.Eip = nContext.Eip - 1;


				if (nDlg->IsBreakPoint(nContext.Eip) != -1 || nDlg->IsTempBreakPoint(nContext.Eip) != -1)
				{
					if (nDlg->IsTempBreakPoint(nContext.Eip) != -1)nDlg->DelTempBreakPoint(nContext.Eip);

					nDlg->PrintContext(nContext);
					OutputDebugEdit(nDlg->m_Edit_Output, TEXT("中断：%08X"), nContext.Eip);


					g_pDlg->SetWindowText(g_pDlg->m_WinText + TEXT("已暂停线程..."));
					SwitchToThisWindow(g_pDlg->m_hWnd, TRUE);//窗口最前
					g_pDlg->m_List_Asm.SetFocus();//列表框获取焦点

					g_pDlg->CloseAllBreakPoint();

					nDlg->ShowSingleAsm(nContext.Eip);
					nDlg->PrintHeap(nContext);
					//g_pDlg->m_List_Asm.SetSelectionEx(0);


					nContext.EFlags = nContext.EFlags + 256;
					SetThreadContext(nDlg->m_ThreadHandle, &nContext);
					SuspendThread(nDlg->m_ThreadHandle);

					OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("断点触发 - 单步"));
					g_BreakType = BREAK_SOFT;
				}

			}
			else if (nDebugEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP)
			{
				DWORD nHardBreak;
				DWORD nTempHardBreak;
				CONTEXT nContext{};

				nContext.ContextFlags = CONTEXT_FULL;
				GetThreadContext(nDlg->m_ThreadHandle, &nContext);

				nHardBreak = nDlg->IsHardBreakPoint(nContext.Eip);
				nTempHardBreak = nDlg->IsTempHardBreakPoint(nContext.Eip);

				nDlg->PrintContext(nContext);
				nDlg->PrintHeap(nContext);
				nDlg->ShowSingleAsm(nContext.Eip);


				if (nHardBreak != -1 || nTempHardBreak != -1 || g_BreakType == BREAK_HARD)
				{
					if (nHardBreak != -1 || nTempHardBreak != -1)
					{
						if (nHardBreak != -1)nDlg->CloseAllHardBreakPoint();
						else if(nTempHardBreak != -1)nDlg->DelTempHardBreakPoint(nContext.Eip);

						g_pDlg->SetWindowText(g_pDlg->m_WinText + TEXT("已暂停线程..."));
						SwitchToThisWindow(g_pDlg->m_hWnd, TRUE);//窗口最前
						g_pDlg->m_List_Asm.SetFocus();//列表框获取焦点

						nContext.EFlags = nContext.EFlags + 256;
						SetThreadContext(nDlg->m_ThreadHandle, &nContext);
						SuspendThread(nDlg->m_ThreadHandle);

						OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("硬件断点单步触发 - 单步步入"));
						g_BreakType = BREAK_HARD;
					}
					else if (g_SingDebug == SETUP_IN)
					{
						nContext.EFlags = nContext.EFlags + 256;
						SetThreadContext(nDlg->m_ThreadHandle, &nContext);
						SuspendThread(nDlg->m_ThreadHandle);

						OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("硬件断点单步触发 - 单步步入"));
					}
					else if (g_SingDebug == SETUP_OUT)
					{
						DWORD nNextAddress;

						if (nDlg->IsCallAsm(nContext.Eip))
						{
							nNextAddress = nDlg->GetNextAddress(nContext.Eip);

							nDlg->SetTempHardBreakPoint(nNextAddress);

							SuspendThread(nDlg->m_ThreadHandle);

							OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("硬件断点满足步过条件，当前地址：%08X"), nContext.Eip);
							OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("硬件断点下一步地址：%08X"), nNextAddress);

						}
						else
						{
							nContext.EFlags = nContext.EFlags + 256;
							SetThreadContext(nDlg->m_ThreadHandle, &nContext);
							SuspendThread(nDlg->m_ThreadHandle);
							OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("硬件断点不满足步过条件，当前地址：%08X"), nContext.Eip);
						}

						//OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("单步触发 - 单步步过"));

					}
					else if (g_SingDebug == SETUP_RUN)
					{
						g_pDlg->SetWindowText(g_pDlg->m_WinText + TEXT("调试进程"));
						g_pDlg->OpenAllHardBreakPoint();

						OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("硬件断点单步触发 - 执行"));

					}

				}
				else if(g_BreakType == BREAK_SOFT)
				{
					if (g_SingDebug == SETUP_IN)
					{
						nContext.EFlags = nContext.EFlags + 256;
						SetThreadContext(nDlg->m_ThreadHandle, &nContext);
						SuspendThread(nDlg->m_ThreadHandle);

						OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("软件断点单步触发 - 单步步入"));

					}
					else if (g_SingDebug == SETUP_OUT)
					{
						DWORD nNextAddress;

						if (nDlg->IsCallAsm(nContext.Eip))
						{
							nNextAddress = nDlg->GetNextAddress(nContext.Eip);

							nDlg->SetTempBreakPoint(nNextAddress);

							SuspendThread(nDlg->m_ThreadHandle);

							OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("软件断点满足步过条件，当前地址：%08X"), nContext.Eip);
							OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("软件断点下一步地址：%08X"), nNextAddress);

						}
						else
						{
							nContext.EFlags = nContext.EFlags + 256;
							SetThreadContext(nDlg->m_ThreadHandle, &nContext);
							SuspendThread(nDlg->m_ThreadHandle);
							OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("软件断点不满足步过条件，当前地址：%08X"), nContext.Eip);
						}

						//OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("单步触发 - 单步步过"));

					}
					else if (g_SingDebug == SETUP_RUN)
					{
						g_pDlg->SetWindowText(g_pDlg->m_WinText + TEXT("调试进程"));
						g_pDlg->OpenAllBreakPoint();

						OutputDebugEdit(g_pDlg->m_Edit_Output, TEXT("软件断点单步触发 - 执行"));

					}
				}

			}
		}


		ContinueDebugEvent(nDebugEvent.dwProcessId, nDebugEvent.dwThreadId, DBG_CONTINUE);
	}
	return 0;
}

BOOL DebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	g_pDlg = this;



	m_List_Asm.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Asm.InsertColumn(m_Flag_Break, TEXT(""), 0, 18);
	m_List_Asm.InsertColumn(m_Flag_Address, TEXT("地址"), 0, 70);
	m_List_Asm.InsertColumn(m_Flag_Opcode, TEXT("字节"), 0, 150);
	m_List_Asm.InsertColumn(m_Flag_Asm, TEXT("汇编"), 0, 250);
	m_List_Asm.InsertColumn(m_Flag_Mark, TEXT("备注"), 0, 200);
	m_List_Heap.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Heap.InsertColumn(m_Flag_HeapAddress, TEXT("地址"), 0, 100);
	m_List_Heap.InsertColumn(m_Flag_HeapData, TEXT("数据"), 0, 100);
	m_List_Memory.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Memory.InsertColumn(m_Flag_MemoryAddress, TEXT("地址"), 0, 100);
	m_List_Memory.InsertColumn(m_Flag_MemoryData, TEXT("字节"), 0, 420);
	m_List_Memory.InsertColumn(m_Flag_MemoryStr, TEXT("文本"), 0, 200);

	m_Combo_Address.InsertString(m_Combo_Address.GetCount(), TEXT("MessageBoxA"));
	m_Combo_Address.InsertString(m_Combo_Address.GetCount(), TEXT("MessageBoxW"));
	m_Combo_Address.InsertString(m_Combo_Address.GetCount(), TEXT("OpenProcess"));
	m_Combo_Address.InsertString(m_Combo_Address.GetCount(), TEXT("TerminateProcess"));
	m_Combo_Address.InsertString(m_Combo_Address.GetCount(), TEXT("ReadProcessMemory"));
	m_Combo_Address.InsertString(m_Combo_Address.GetCount(), TEXT("WriteProcessMemory"));

	m_Combo_Address.SetWindowText(TEXT("MessageBoxA"));
	for (DWORD i = 0; i < m_Num_Asm; i++) m_List_Asm.InsertItem(0, TEXT(""));
	for (DWORD i = 0; i < m_Num_Heap; i++) m_List_Heap.InsertItem(0, TEXT(""));
	for (DWORD i = 0; i < m_Num_Memory; i++) m_List_Memory.InsertItem(0, TEXT(""));


	m_ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_Pid);
	if (m_ProcessHandle == nullptr)MessageBox(TEXT("打开进程失败！"), TEXT("提示"), MB_ICONERROR);

	m_Tid = GetProcessThreadId(m_Pid);
	m_ThreadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, m_Tid);
	if (m_ProcessHandle == nullptr)MessageBox(TEXT("打开线程失败！"), TEXT("提示"), MB_ICONERROR);


	InitAsm(GetOepAddress());
	PrintMemory(GetImageBassAddress());
	m_List_Asm.SetSelectionEx(0);

	g_AsmListProc = SetWindowLong(m_List_Asm.m_hWnd, GWL_WNDPROC, (LONG)ListAsmProc);
	g_MemoryListProc = SetWindowLong(m_List_Memory.m_hWnd, GWL_WNDPROC, (LONG)ListMemoryProc);

	CreateThread(NULL, NULL, DebugThread, (LPVOID)this, NULL, NULL);

	return TRUE;
}
void DebugDlg::OnClose()
{


	DebugActiveProcessStop(m_Pid);
	CloseHandle(m_ProcessHandle);
	CloseHandle(m_ThreadHandle);

	CDialogEx::OnClose();
	DestroyWindow();
}
BOOL DebugDlg::PreTranslateMessage(MSG* pMsg)
{

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		COMBOBOXINFO info = { 0 };
		info.cbSize = sizeof(COMBOBOXINFO);
		m_Combo_Address.GetComboBoxInfo(&info);

		if (pMsg->hwnd == info.hwndItem)
		{
			DWORD nReadAddress;
			CString nTempAddress;
			m_Combo_Address.GetWindowText(nTempAddress);

			nReadAddress = GetApiAddress(nTempAddress);
			if (nReadAddress)InitAsm(nReadAddress);
			else
			{
				_stscanf_s(nTempAddress, TEXT("%x"), &nReadAddress);
				InitAsm(nReadAddress);
			}
			m_List_Asm.SetFocus();
			m_List_Asm.SetSelectionEx(0);
		}
		return FALSE;
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}
HBRUSH DebugDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (nCtlColor)
	{
	case CTLCOLOR_EDIT://假设控件是文本框或者消息框，则进入下一个switch  
		switch (pWnd->GetDlgCtrlID())//对某一个特定控件进行判断  
		{
		case IDC_EDIT1:
			pDC->SetTextColor(m_EaxColor);
			break;
		case IDC_EDIT2:
			pDC->SetTextColor(m_EbxColor);
			break;
		case IDC_EDIT3:
			pDC->SetTextColor(m_EcxColor);
			break;
		case IDC_EDIT4:
			pDC->SetTextColor(m_EdxColor);
			break;
		case IDC_EDIT5:
			pDC->SetTextColor(m_EsiColor);
			break;
		case IDC_EDIT34:
			pDC->SetTextColor(m_EdiColor);
			break;
		case IDC_EDIT35:
			pDC->SetTextColor(m_EbpColor);
			break;
		case IDC_EDIT6:
			pDC->SetTextColor(m_EspColor);
			break;
		case IDC_EDIT11:
			pDC->SetTextColor(m_EipColor);
			break;
		case IDC_EDIT15:
			pDC->SetTextColor(m_Dr0Color);
			break;
		case IDC_EDIT7:
			pDC->SetTextColor(m_Dr1Color);
			break;
		case IDC_EDIT8:
			pDC->SetTextColor(m_Dr2Color);
			break;
		case IDC_EDIT9:
			pDC->SetTextColor(m_Dr3Color);
			break;
		case IDC_EDIT17:
			pDC->SetTextColor(m_Dr6Color);
			break;
		case IDC_EDIT18:
			pDC->SetTextColor(m_Dr7Color);
			break;
		case IDC_EDIT22:
			pDC->SetTextColor(m_EsColor);
			break;
		case IDC_EDIT19:
			pDC->SetTextColor(m_CsColor);
			break;
		case IDC_EDIT20:
			pDC->SetTextColor(m_SsColor);
			break;
		case IDC_EDIT21:
			pDC->SetTextColor(m_DsColor);
			break;
		case IDC_EDIT23:
			pDC->SetTextColor(m_FsColor);
			break;
		case IDC_EDIT24:
			pDC->SetTextColor(m_GsColor);
			break;
		case IDC_EDIT28:
			pDC->SetTextColor(m_CfColor);
			break;
		case IDC_EDIT25:
			pDC->SetTextColor(m_PfColor);
			break;
		case IDC_EDIT26:
			pDC->SetTextColor(m_AfColor);
			break;
		case IDC_EDIT27:
			pDC->SetTextColor(m_AfColor);
			break;
		case IDC_EDIT29:
			pDC->SetTextColor(m_SfColor);
			break;
		case IDC_EDIT30:
			pDC->SetTextColor(m_TfColor);
			break;
		case IDC_EDIT31:
			pDC->SetTextColor(m_TfColor);
			break;
		case IDC_EDIT32:
			pDC->SetTextColor(m_OfColor);
			break;
		}
		break;
	}
	return hbr;
}


VOID DebugDlg::InitAsm(DWORD nReadAddress)
{

	BYTE nReadBuff[1024]{};
	CString nAddr;
	DISASM disAsm = { 0 };
	COLORREF nTextColor;
	COLORREF nBkColor;

	ReadMemoryBytes(nReadAddress, nReadBuff, 1024);

	disAsm.EIP = (UIntPtr)nReadBuff;
	disAsm.VirtualAddr = nReadAddress;
	disAsm.Archi = 0;
	disAsm.Options = 0x000;
	DWORD nOpcodeSize = 1024;
	INT nLen = 0;

	ClearItemColor();
	for (DWORD i = 0; i < m_Num_Asm; i++)
	{
		nLen = Disasm(&disAsm);
		if (nLen == -1)
		{
			disAsm.EIP += 1;
			disAsm.VirtualAddr += 1;
			continue;
		}

		PrintBreakFlag((DWORD)disAsm.VirtualAddr,i);
		nAddr.Format(TEXT("%08X"), disAsm.VirtualAddr);
		m_List_Asm.SetItemText(i, m_Flag_Address, nAddr);
		PrintOpcode((BYTE*)disAsm.EIP, nLen, i);
		m_List_Asm.SetItemText(i, m_Flag_Asm, disAsm.CompleteInstr);

		disAsm.EIP += nLen;
		disAsm.VirtualAddr += nLen;

		
		if (IsColorWord(CString(disAsm.CompleteInstr), &nTextColor, &nBkColor))
			SetItemColor(i, m_Flag_Asm, nTextColor, nBkColor);

	}
	
	_stscanf_s(m_List_Asm.GetItemText(0, m_Flag_Address), TEXT("%x"), &nReadAddressFirst);
	_stscanf_s(m_List_Asm.GetItemText(1, m_Flag_Address), TEXT("%x"), &nReadAddressSecond);
	_stscanf_s(m_List_Asm.GetItemText(m_Num_Asm - 1, m_Flag_Address), TEXT("%x"), &nReadAddressEnd);

}
VOID DebugDlg::PrintOnceAsm(DWORD nReadAddress)
{
	BYTE nReadBuff[1024]{};
	CString nAddr;
	DISASM disAsm = { 0 };
	COLORREF nTextColor;
	COLORREF nBkColor;

	ReadMemoryBytes(nReadAddress, nReadBuff, 1024);

	disAsm.EIP = (UIntPtr)nReadBuff;
	disAsm.VirtualAddr = nReadAddress;
	disAsm.Archi = 0;
	disAsm.Options = 0x000;
	DWORD nOpcodeSize = 1024;

	INT nLen = 0;

	ClearItemColor();
	for (DWORD i = 0; i < m_Num_Asm; i++)
	{
		nLen = Disasm(&disAsm);
		if (nLen == -1)
		{
			disAsm.EIP += 1;
			disAsm.VirtualAddr += 1;
			continue;
		}
		
		PrintBreakFlag((DWORD)disAsm.VirtualAddr, i);
		nAddr.Format(TEXT("%08X"), disAsm.VirtualAddr);
		m_List_Asm.SetItemText(i, m_Flag_Address, nAddr);
		PrintOpcode((BYTE*)disAsm.EIP, nLen, i);
		m_List_Asm.SetItemText(i, m_Flag_Asm, disAsm.CompleteInstr);

		disAsm.EIP += nLen;
		disAsm.VirtualAddr += nLen;

		if (IsColorWord(CString(disAsm.CompleteInstr), &nTextColor, &nBkColor))
			SetItemColor(i, m_Flag_Asm, nTextColor, nBkColor);

	}

	_stscanf_s(m_List_Asm.GetItemText(0, m_Flag_Address), TEXT("%x"), &nReadAddressFirst);
	_stscanf_s(m_List_Asm.GetItemText(1, m_Flag_Address), TEXT("%x"), &nReadAddressSecond);
	_stscanf_s(m_List_Asm.GetItemText(m_Num_Asm - 1, m_Flag_Address), TEXT("%x"), &nReadAddressEnd);

}
VOID DebugDlg::ShowSingleAsm(DWORD nShowAddress)
{

	if (m_List_Asm.GetSelectionEx() == m_List_Asm.GetItemCount() - 2)
	{
		g_pDlg->PrintOnceAsm(nReadAddressSecond);

		INT nIndex = FindAddress(nShowAddress);
		if (nIndex != -1)m_List_Asm.SetSelectionEx(nIndex);
		else
		{
			PrintOnceAsm(nShowAddress);
			m_List_Asm.SetSelectionEx(0);
		}
	}
	else
	{

		INT nIndex = FindAddress(nShowAddress);
		if (nIndex != -1)m_List_Asm.SetSelectionEx(nIndex);
		else
		{
			PrintOnceAsm(nShowAddress);
			m_List_Asm.SetSelectionEx(0);
		}
	}
}
VOID DebugDlg::PrintOpcode(BYTE* pOpcode, DWORD nSize, DWORD nItem)
{
	CString nTemp;
	CString nOpcode;
	for (DWORD i = 0; i < nSize; ++i)
	{
		nTemp.Format(TEXT("%02X "), pOpcode[i]);
		nOpcode += nTemp;
	}

	m_List_Asm.SetItemText(nItem, g_pDlg->m_Flag_Opcode, nOpcode);

}
VOID DebugDlg::PrintBreakFlag(DWORD nBreakAddress, DWORD nItem)
{
	CString nAddr;
	if (IsBreakPoint(nBreakAddress) != -1)
	{
		nAddr = TEXT("●");
		SetItemColor(nItem, m_Flag_Break, COLOR_WHITE, COLOR_RED);
		SetItemColor(nItem, m_Flag_Address, COLOR_WHITE, COLOR_RED);
	}
	else if (IsHardBreakPoint(nBreakAddress) != -1) 
	{ 
		nAddr = TEXT("■"); 
		SetItemColor(nItem, m_Flag_Break, COLOR_WHITE, COLOR_BLACK);
		SetItemColor(nItem, m_Flag_Address, COLOR_WHITE, COLOR_BLACK);
	}
	else nAddr = TEXT("");
	m_List_Asm.SetItemText(nItem, m_Flag_Break, nAddr);
}
VOID DebugDlg::PrintContext(CONTEXT &nCONTEXT)
{
	CString nTemp;
	CString nOldData;
	PEFLAGS m_EFlags = nullptr;
	m_EFlags = (PEFLAGS)&nCONTEXT.EFlags;

	nTemp.Format(TEXT("%08X"), nCONTEXT.Eax);
	m_Edit_Eax.GetWindowText(nOldData);
	m_Edit_Eax.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EaxColor = RGB(255, 0, 0);
	}
	else m_EaxColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Ebx);
	m_Edit_Ebx.GetWindowText(nOldData);
	m_Edit_Ebx.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EbxColor = RGB(255, 0, 0);
	}
	else m_EbxColor = RGB(0, 0, 0);



	nTemp.Format(TEXT("%08X"), nCONTEXT.Ecx);
	m_Edit_Ecx.GetWindowText(nOldData);
	m_Edit_Ecx.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EcxColor = RGB(255, 0, 0);
	}
	else m_EcxColor = RGB(0, 0, 0);



	nTemp.Format(TEXT("%08X"), nCONTEXT.Edx);
	m_Edit_Edx.GetWindowText(nOldData);
	m_Edit_Edx.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EdxColor = RGB(255, 0, 0);
	}
	else m_EdxColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Esi);
	m_Edit_Esi.GetWindowText(nOldData);
	m_Edit_Esi.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EsiColor = RGB(255, 0, 0);
	}
	else m_EsiColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Edi);
	m_Edit_Edi.GetWindowText(nOldData);
	m_Edit_Edi.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EdiColor = RGB(255, 0, 0);
	}
	else m_EdiColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Ebp);
	m_Edit_Ebp.GetWindowText(nOldData);
	m_Edit_Ebp.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EbpColor = RGB(255, 0, 0);
	}
	else m_EbpColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Esp);
	m_Edit_Esp.GetWindowText(nOldData);
	m_Edit_Esp.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EspColor = RGB(255, 0, 0);
	}
	else m_EspColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Eip);
	m_Edit_Eip.GetWindowText(nOldData);
	m_Edit_Eip.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EipColor = RGB(255, 0, 0);
	}
	else m_EipColor = RGB(0, 0, 0);



	nTemp.Format(TEXT("%08X"), nCONTEXT.Dr0);
	m_Edit_Dr0.GetWindowText(nOldData);
	m_Edit_Dr0.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_Dr0Color = RGB(255, 0, 0);
	}
	else m_Dr0Color = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Dr1);
	m_Edit_Dr1.GetWindowText(nOldData);
	m_Edit_Dr1.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_Dr1Color = RGB(255, 0, 0);
	}
	else m_Dr1Color = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Dr2);
	m_Edit_Dr2.GetWindowText(nOldData);
	m_Edit_Dr2.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_Dr2Color = RGB(255, 0, 0);
	}
	else m_Dr2Color = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Dr3);
	m_Edit_Dr3.GetWindowText(nOldData);
	m_Edit_Dr3.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_Dr3Color = RGB(255, 0, 0);
	}
	else m_Dr3Color = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Dr6);
	m_Edit_Dr6.GetWindowText(nOldData);
	m_Edit_Dr6.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_Dr6Color = RGB(255, 0, 0);
	}
	else m_Dr6Color = RGB(0, 0, 0);


	nTemp.Format(TEXT("%08X"), nCONTEXT.Dr7);
	m_Edit_Dr7.GetWindowText(nOldData);
	m_Edit_Dr7.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_Dr7Color = RGB(255, 0, 0);
	}
	else m_Dr7Color = RGB(0, 0, 0);


	nTemp.Format(TEXT("%04X"), nCONTEXT.SegEs);
	m_Edit_Es.GetWindowText(nOldData);
	m_Edit_Es.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_EsColor = RGB(255, 0, 0);
	}
	else m_EsColor = RGB(0, 0, 0);

	nTemp.Format(TEXT("%04X"), nCONTEXT.SegCs);
	m_Edit_Cs.GetWindowText(nOldData);
	m_Edit_Cs.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_CsColor = RGB(255, 0, 0);
	}
	else m_CsColor = RGB(0, 0, 0);

	nTemp.Format(TEXT("%04X"), nCONTEXT.SegSs);
	m_Edit_Ss.GetWindowText(nOldData);
	m_Edit_Ss.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_SsColor = RGB(255, 0, 0);
	}
	else m_SsColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%04X"), nCONTEXT.SegDs);
	m_Edit_Ds.GetWindowText(nOldData);
	m_Edit_Ds.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_DsColor = RGB(255, 0, 0);
	}
	else m_DsColor = RGB(0, 0, 0);

	nTemp.Format(TEXT("%04X"), nCONTEXT.SegFs);
	m_Edit_Fs.GetWindowText(nOldData);
	m_Edit_Fs.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_FsColor = RGB(255, 0, 0);
	}
	else m_FsColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%04X"), nCONTEXT.SegGs);
	m_Edit_Gs.GetWindowText(nOldData);
	m_Edit_Gs.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_GsColor = RGB(255, 0, 0);
	}
	else m_GsColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), m_EFlags->CF);
	m_Edit_Cf.GetWindowText(nOldData);
	m_Edit_Cf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_CfColor = RGB(255, 0, 0);
	}
	else m_CfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), m_EFlags->PF);
	m_Edit_Pf.GetWindowText(nOldData);
	m_Edit_Pf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_PfColor = RGB(255, 0, 0);
	}
	else m_PfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), m_EFlags->AF);
	m_Edit_Af.GetWindowText(nOldData);
	m_Edit_Af.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_AfColor = RGB(255, 0, 0);
	}
	else m_AfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), m_EFlags->ZF);
	m_Edit_Zf.GetWindowText(nOldData);
	m_Edit_Zf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_ZfColor = RGB(255, 0, 0);
	}
	else m_ZfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), m_EFlags->SF);
	m_Edit_Sf.GetWindowText(nOldData);
	m_Edit_Sf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_SfColor = RGB(255, 0, 0);
	}
	else m_SfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), m_EFlags->TF);
	m_Edit_Tf.GetWindowText(nOldData);
	m_Edit_Tf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_TfColor = RGB(255, 0, 0);
	}
	else m_TfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), m_EFlags->DF);
	m_Edit_Df.GetWindowText(nOldData);
	m_Edit_Df.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_DfColor = RGB(255, 0, 0);
	}
	else m_DfColor = RGB(0, 0, 0);

	nTemp.Format(TEXT("%X"), m_EFlags->OF);
	m_Edit_Of.GetWindowText(nOldData);
	m_Edit_Of.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
		m_OfColor = RGB(255, 0, 0);
	}
	else m_OfColor = RGB(0, 0, 0);




	/*
	nTemp.Format(TEXT("%X"), nCONTEXT.EFlags & 0x1);
	m_Edit_Cf.GetWindowText(nOldData);
	m_Edit_Cf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
	m_CfColor = RGB(255, 0, 0);
	}
	else m_CfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), (nCONTEXT.EFlags & 0x4) >> 2);
	m_Edit_Pf.GetWindowText(nOldData);
	m_Edit_Pf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
	m_PfColor = RGB(255, 0, 0);
	}
	else m_PfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), (nCONTEXT.EFlags & 0x10) >> 4);
	m_Edit_Af.GetWindowText(nOldData);
	m_Edit_Af.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
	m_AfColor = RGB(255, 0, 0);
	}
	else m_AfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), (nCONTEXT.EFlags & 0x40) >> 6);
	m_Edit_Zf.GetWindowText(nOldData);
	m_Edit_Zf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
	m_ZfColor = RGB(255, 0, 0);
	}
	else m_ZfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), (nCONTEXT.EFlags & 0x80) >> 7);
	m_Edit_Sf.GetWindowText(nOldData);
	m_Edit_Sf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
	m_SfColor = RGB(255, 0, 0);
	}
	else m_SfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), (nCONTEXT.EFlags & 0x100) >> 8);
	m_Edit_Tf.GetWindowText(nOldData);
	m_Edit_Tf.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
	m_TfColor = RGB(255, 0, 0);
	}
	else m_TfColor = RGB(0, 0, 0);


	nTemp.Format(TEXT("%X"), (nCONTEXT.EFlags & 0x400) >> 10);
	m_Edit_Df.GetWindowText(nOldData);
	m_Edit_Df.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
	m_DfColor = RGB(255, 0, 0);
	}
	else m_DfColor = RGB(0, 0, 0);

	nTemp.Format(TEXT("%X"), (nCONTEXT.EFlags & 0x800) >> 11);
	m_Edit_Of.GetWindowText(nOldData);
	m_Edit_Of.SetWindowText(nTemp);
	if (nTemp != nOldData)
	{
	m_OfColor = RGB(255, 0, 0);
	}
	else m_OfColor = RGB(0, 0, 0);
	*/




}
VOID DebugDlg::PrintHeap(CONTEXT &nCONTEXT)
{

	DWORD nReadData;
	CString nShowData;
	for (DWORD i = 0; i < m_Num_Heap; i++)
	{
		nShowData.Format(TEXT("%08X+%X"), nCONTEXT.Esp, (i * 4));
		m_List_Heap.SetItemText(i, m_Flag_HeapAddress, nShowData);
		ReadMemoryDword(nCONTEXT.Esp + (i * 4), &nReadData);
		nShowData.Format(TEXT("%08X"), nReadData);
		m_List_Heap.SetItemText(i, m_Flag_HeapData, nShowData);
	}

}
VOID DebugDlg::PrintMemory(DWORD nAddress)
{
	BYTE nValue[24]{};
	CString nTemp;
	CString nByte;

	for (DWORD i = 0; i < m_Num_Memory; i++)
	{
		nTemp.Format(TEXT("%08X"), nAddress + (i * _countof(nValue)));
		m_List_Memory.SetItemText(i, m_Flag_MemoryAddress, nTemp);

		ReadMemoryBytes(nAddress + (i * _countof(nValue)), nValue, _countof(nValue));
		for (DWORD x = 0; x < _countof(nValue); x++)
		{
			nTemp.Format(TEXT("%02X "), nValue[x]);
			nByte = nByte + nTemp;
		}
		m_List_Memory.SetItemText(i, m_Flag_MemoryData, nByte);
		nByte.Empty();

		for (DWORD x = 0; x < _countof(nValue); x++)
		{
			if (!nValue[x])nValue[x] = '\n';
		}
		m_List_Memory.SetItemText(i, m_Flag_MemoryStr, CString(nValue));
	}
}

BOOL DebugDlg::SetHardBreakPoint(DWORD nBreakAddress)
{
	CONTEXT nContext{};
	PDR7 nDr7 = nullptr;
	nContext.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	GetThreadContext(m_ThreadHandle, &nContext);
	nDr7 = (PDR7)&nContext.Dr7;

	if (nContext.Dr0 == 0)
	{
		m_HardBreakPoint[0] = nBreakAddress;
		nContext.Dr0 = nBreakAddress;
		nDr7->Dr0_L = 1;
		nDr7->Dr0_LEN = 0;
		nDr7->Dr0_RWE = 0;
		SetThreadContext(m_ThreadHandle, &nContext);
		return TRUE;
	}
	else if (nContext.Dr1 == 0)
	{
		m_HardBreakPoint[1] = nBreakAddress;
		nContext.Dr1 = nBreakAddress;
		nDr7->Dr1_L = 1;
		nDr7->Dr1_LEN = 0;
		nDr7->Dr1_RWE = 0;
		SetThreadContext(m_ThreadHandle, &nContext);
		return TRUE;
	}
	else if (nContext.Dr2 == 0)
	{
		m_HardBreakPoint[2] = nBreakAddress;
		nContext.Dr2 = nBreakAddress;
		nDr7->Dr2_L = 1;
		nDr7->Dr2_LEN = 0;
		nDr7->Dr2_RWE = 0;
		SetThreadContext(m_ThreadHandle, &nContext);
		return TRUE;
	}
	else if (nContext.Dr3 == 0)
	{
		m_HardBreakPoint[3] = nBreakAddress;
		nContext.Dr3 = nBreakAddress;
		nDr7->Dr3_L = 1;
		nDr7->Dr3_LEN = 0;
		nDr7->Dr3_RWE = 0;
		SetThreadContext(m_ThreadHandle, &nContext);
		return TRUE;
	}
	return FALSE;
}
VOID DebugDlg::SetTempHardBreakPoint(DWORD nBreakAddress)
{
	CONTEXT nContext{};
	PDR7 nDr7 = nullptr;
	nContext.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	GetThreadContext(m_ThreadHandle, &nContext);
	nDr7 = (PDR7)&nContext.Dr7;

	nContext.Dr0 = nBreakAddress;
	nDr7->Dr0_L = 1;
	nDr7->Dr0_LEN = 0;
	nDr7->Dr0_RWE = 0;
	SetThreadContext(m_ThreadHandle, &nContext);
	m_TempHardBreakPoint[0] = nBreakAddress;

}
VOID DebugDlg::DelHardBreakPoint(DWORD nBreakAddress)
{
	DWORD nIndex;
	CONTEXT nContext{};
	PDR7 nDr7 = nullptr;
	nContext.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	GetThreadContext(m_ThreadHandle, &nContext);
	nDr7 = (PDR7)&nContext.Dr7;

	nIndex = IsHardBreakPoint(nBreakAddress);
	if (nIndex == -1)return;

	if (nIndex == 0)
	{
		nContext.Dr0 = 0;
		nDr7->Dr0_L = 0;
		nDr7->Dr0_LEN = 0;
		nDr7->Dr0_RWE = 0;
		m_HardBreakPoint[0] = 0;
	}
	else if (nIndex == 1)
	{
		nContext.Dr1 = 0;
		nDr7->Dr1_L = 0;
		nDr7->Dr1_LEN = 0;
		nDr7->Dr1_RWE = 0;
		m_HardBreakPoint[1] = 0;
	}
	else if (nIndex == 2)
	{
		nContext.Dr2 = 0;
		nDr7->Dr2_L = 0;
		nDr7->Dr2_LEN = 0;
		nDr7->Dr2_RWE = 0;
		m_HardBreakPoint[2] = 0;
	}
	else if (nIndex == 3)
	{
		nContext.Dr3 = 0;
		nDr7->Dr3_L = 0;
		nDr7->Dr3_LEN = 0;
		nDr7->Dr3_RWE = 0;
		m_HardBreakPoint[3] = 0;
	}
	SetThreadContext(m_ThreadHandle, &nContext);
}
VOID DebugDlg::DelTempHardBreakPoint(DWORD nBreakAddress)
{
	DWORD nIndex;

	nIndex = IsTempHardBreakPoint(nBreakAddress);
	if (nIndex == -1)return;

	CONTEXT nContext{};
	PDR7 nDr7 = nullptr;
	nContext.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	GetThreadContext(m_ThreadHandle, &nContext);
	nDr7 = (PDR7)&nContext.Dr7;

	nContext.Dr0 = 0;
	nDr7->Dr0_L = 0;
	nDr7->Dr0_LEN = 0;
	nDr7->Dr0_RWE = 0;
	SetThreadContext(m_ThreadHandle, &nContext);

	m_TempHardBreakPoint[nIndex] = 0;
}
VOID DebugDlg::OpenAllHardBreakPoint()
{
	CONTEXT nContext{};
	PDR7 nDr7 = nullptr;
	nContext.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	GetThreadContext(m_ThreadHandle, &nContext);
	nDr7 = (PDR7)&nContext.Dr7;

	if (m_HardBreakPoint[0] != 0)
	{
		nContext.Dr0 = m_HardBreakPoint[0];
		nDr7->Dr0_L = 1;
		nDr7->Dr0_LEN = 0;
		nDr7->Dr0_RWE = 0;
	}
	if (m_HardBreakPoint[1] != 0)
	{
		nContext.Dr1 = m_HardBreakPoint[1];
		nDr7->Dr1_L = 1;
		nDr7->Dr1_LEN = 0;
		nDr7->Dr1_RWE = 0;
	}
	if (m_HardBreakPoint[2] != 0)
	{
		nContext.Dr2 = m_HardBreakPoint[2];
		nDr7->Dr2_L = 1;
		nDr7->Dr2_LEN = 0;
		nDr7->Dr2_RWE = 0;
	}
	if (m_HardBreakPoint[3] != 0)
	{
		nContext.Dr3 = m_HardBreakPoint[3];
		nDr7->Dr3_L = 1;
		nDr7->Dr3_LEN = 0;
		nDr7->Dr3_RWE = 0;
	}
	SetThreadContext(m_ThreadHandle, &nContext);
}
VOID DebugDlg::CloseAllHardBreakPoint()
{
	CONTEXT nContext{};
	PDR7 nDr7 = nullptr;
	nContext.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	GetThreadContext(m_ThreadHandle, &nContext);
	nDr7 = (PDR7)&nContext.Dr7;

	nContext.Dr0 = 0;
	nContext.Dr1 = 0;
	nContext.Dr2 = 0;
	nContext.Dr3 = 0;

	nDr7->Dr0_L = 0;
	nDr7->Dr0_LEN = 0;
	nDr7->Dr0_RWE = 0;
	nDr7->Dr1_L = 0;
	nDr7->Dr1_LEN = 0;
	nDr7->Dr1_RWE = 0;
	nDr7->Dr2_L = 0;
	nDr7->Dr2_LEN = 0;
	nDr7->Dr2_RWE = 0;
	nDr7->Dr3_L = 0;
	nDr7->Dr3_LEN = 0;
	nDr7->Dr3_RWE = 0;
	SetThreadContext(m_ThreadHandle, &nContext);
}
DWORD DebugDlg::IsHardBreakPoint(DWORD nBreakAddress)
{
	if (m_HardBreakPoint[0] == nBreakAddress)return 0;
	else if (m_HardBreakPoint[1] == nBreakAddress)return 1;
	else if (m_HardBreakPoint[2] == nBreakAddress)return 2;
	else if (m_HardBreakPoint[3] == nBreakAddress)return 3;
	return -1;
}
DWORD DebugDlg::IsTempHardBreakPoint(DWORD nBreakAddress)
{
	for (DWORD i = 0; i < 4; i++)
	{
		if (m_TempHardBreakPoint[i] == nBreakAddress)
		{
			return i;
		}
	}
	return -1;
}



DWORD DebugDlg::GetBeforeAddress(DWORD nReadAddress)
{

	BYTE nReadBuff[128]{};
	CString nAddr;
	DISASM disAsm = { 0 };

	ReadMemoryBytes((nReadAddress - 30), nReadBuff, 128);

	disAsm.EIP = (UIntPtr)nReadBuff;
	disAsm.VirtualAddr = nReadAddress - 30;
	disAsm.Archi = 0;
	disAsm.Options = 0x000;
	DWORD nOpcodeSize = 128;

	INT nLen = 0;

	for (DWORD i = 0; i < 128; i++)
	{
		nLen = Disasm(&disAsm);
		if (nLen == -1)
		{
			disAsm.EIP += 1;
			disAsm.VirtualAddr += 1;
			continue;
		}
		disAsm.EIP += nLen;
		disAsm.VirtualAddr += nLen;

		if (disAsm.VirtualAddr == nReadAddress)
		{
			return nReadAddress -= nLen;
		}
	}

	return nReadAddress-1;
}
DWORD DebugDlg::GetNextAddress(DWORD nReadAddress)
{
	BYTE nReadBuff[128]{};
	CString nAddr;
	DISASM disAsm = { 0 };

	ReadMemoryBytes(nReadAddress, nReadBuff, 128);

	disAsm.EIP = (UIntPtr)nReadBuff;
	disAsm.VirtualAddr = nReadAddress;
	disAsm.Archi = 0;
	disAsm.Options = 0x000;
	DWORD nOpcodeSize = 128;

	INT nLen = 0;

	for (DWORD i = 0; i < 128; i++)
	{
		nLen = Disasm(&disAsm);
		if (nLen == -1)
		{
			disAsm.EIP += 1;
			disAsm.VirtualAddr += 1;
			continue;
		}

		disAsm.EIP += nLen;
		disAsm.VirtualAddr += nLen;

		return (DWORD)disAsm.VirtualAddr;
	}
	return nReadAddress + 1;
}

DWORD DebugDlg::GetOepAddress()
{
	HMODULE nHmodule[255]{};
	DWORD nNeedSize;
	BYTE nProcessBuff[1024]{};

	EnumProcessModules(m_ProcessHandle, nHmodule, 255, &nNeedSize);

	ReadMemoryBytes((DWORD)nHmodule[0], nProcessBuff, 1024);

	PIMAGE_DOS_HEADER pDosHead = (PIMAGE_DOS_HEADER)nProcessBuff;
	PIMAGE_NT_HEADERS pNtHead = (PIMAGE_NT_HEADERS)(pDosHead->e_lfanew + nProcessBuff);

	return (DWORD)nHmodule[0] + pNtHead->OptionalHeader.AddressOfEntryPoint;
}
DWORD DebugDlg::GetDataAddress()
{
	HMODULE nHmodule[255]{};
	DWORD nNeedSize;
	BYTE nProcessBuff[1024]{};

	EnumProcessModules(m_ProcessHandle, nHmodule, 255, &nNeedSize);

	ReadMemoryBytes((DWORD)nHmodule[0], nProcessBuff, 1024);

	PIMAGE_DOS_HEADER pDosHead = (PIMAGE_DOS_HEADER)nProcessBuff;
	PIMAGE_NT_HEADERS pNtHead = (PIMAGE_NT_HEADERS)(pDosHead->e_lfanew + nProcessBuff);

	return (DWORD)nHmodule[0] + pNtHead->OptionalHeader.BaseOfData;
}
DWORD DebugDlg::GetImageBassAddress()
{
	HMODULE nHmodule[255]{};
	DWORD nNeedSize;
	BYTE nProcessBuff[1024]{};

	EnumProcessModules(m_ProcessHandle, nHmodule, 255, &nNeedSize);

	return (DWORD)nHmodule[0];
}
DWORD DebugDlg::GetApiAddress(CString nApi)
{
	HMODULE nHandle = nullptr;
	DWORD nAddress = 0;

	nHandle = GetModuleHandle(TEXT("kernel32.dll"));
	nAddress = (DWORD)GetProcAddress(nHandle, CStringA(nApi));
	if (nAddress)return nAddress;
	FreeLibrary(nHandle);

	nHandle = GetModuleHandle(TEXT("user32.dll"));
	nAddress = (DWORD)GetProcAddress(nHandle, CStringA(nApi));
	if (nAddress)return nAddress;
	FreeLibrary(nHandle);

	nHandle = GetModuleHandle(TEXT("gdi32.dll"));
	nAddress = (DWORD)GetProcAddress(nHandle, CStringA(nApi));
	if (nAddress)return nAddress;
	FreeLibrary(nHandle);

	nHandle = GetModuleHandle(TEXT("ntdll.dll"));
	nAddress = (DWORD)GetProcAddress(nHandle, CStringA(nApi));
	if (nAddress)return nAddress;
	FreeLibrary(nHandle);

	return 0;
}

VOID DebugDlg::SetBreakPoint(DWORD nBreakAddress)
{
	BYTE nReadByte;
	BYTE nWriteByte = 0xcc;

	ReadMemoryByte(nBreakAddress, &nReadByte);
	WriteMemoryByte(nBreakAddress, nWriteByte);

	m_BreakPoint.push_back({ nBreakAddress ,nReadByte});

}
VOID DebugDlg::SetTempBreakPoint(DWORD nBreakAddress)
{
	BYTE nReadByte;
	BYTE nWriteByte = 0xcc;

	ReadMemoryByte(nBreakAddress, &nReadByte);
	WriteMemoryByte(nBreakAddress, nWriteByte);

	m_TempBreakPoint.push_back({ nBreakAddress ,nReadByte });
}
VOID DebugDlg::DelBreakPoint(DWORD nBreakAddress)
{
	BYTE nWriteByte;
	DWORD nIndex;

	nIndex = IsBreakPoint(nBreakAddress);
	if (nIndex == -1)return;

	nWriteByte = m_BreakPoint[nIndex].nData;
	WriteMemoryByte(nBreakAddress, nWriteByte);
	m_BreakPoint.erase(m_BreakPoint.begin() + nIndex);
}
VOID DebugDlg::DelTempBreakPoint(DWORD nBreakAddress)
{
	BYTE nWriteByte;
	DWORD nIndex;

	nIndex = IsTempBreakPoint(nBreakAddress);
	if (nIndex == -1)return;

	nWriteByte = m_TempBreakPoint[nIndex].nData;
	WriteMemoryByte(nBreakAddress, nWriteByte);
	m_TempBreakPoint.erase(m_TempBreakPoint.begin() + nIndex);
}
VOID DebugDlg::OpenAllBreakPoint()
{
	BYTE nWriteByte = 0xcc;

	for (DWORD i = 0; i < m_BreakPoint.size(); i++)
	{
		WriteMemoryByte(m_BreakPoint[i].nAddress, nWriteByte);
	}
}
VOID DebugDlg::CloseAllBreakPoint()
{
	BYTE nWriteByte;

	for (DWORD i = 0; i < m_BreakPoint.size(); i++)
	{
		nWriteByte = m_BreakPoint[i].nData;
		WriteMemoryByte(m_BreakPoint[i].nAddress, nWriteByte);
	}
}
DWORD DebugDlg::IsBreakPoint(DWORD nBreakAddress)
{
	for (DWORD i = 0; i < m_BreakPoint.size(); i++)
	{
		if (m_BreakPoint[i].nAddress == nBreakAddress)
		{
			return i;
		}
	}
	return -1;
}
DWORD DebugDlg::IsTempBreakPoint(DWORD nBreakAddress)
{
	for (DWORD i = 0; i < m_TempBreakPoint.size(); i++)
	{
		if (m_TempBreakPoint[i].nAddress == nBreakAddress)
		{
			return i;
		}
	}
	return -1;
}

BOOL DebugDlg::IsCallAsm(DWORD nAddress)
{
	BYTE nReadBuff[64]{};
	CString nAddr;
	DISASM disAsm = { 0 };

	ReadMemoryBytes(nAddress, nReadBuff, 64);

	disAsm.EIP = (UIntPtr)nReadBuff;
	disAsm.VirtualAddr = nAddress;
	disAsm.Archi = 0;
	disAsm.Options = 0x000;
	DWORD nOpcodeSize = 64;

	INT nLen = 0;

	for (DWORD i = 0; i < 64; i++)
	{
		nLen = Disasm(&disAsm);
		if (nLen == -1)
		{
			disAsm.EIP += 1;
			disAsm.VirtualAddr += 1;
			continue;
		}
		break;
	}

	CString nTempAsm(disAsm.CompleteInstr);
	if (nTempAsm.Left(4) == TEXT("call"))return TRUE;
	return FALSE;
}
DWORD DebugDlg::FindAddress(DWORD nShowAddress)
{
	DWORD nShow;
	CString nFind;
	for (INT i = 0; i < m_List_Asm.GetItemCount(); i++)
	{
		nFind = m_List_Asm.GetItemText(i, m_Flag_Address);
		_stscanf_s(nFind, TEXT("%x"), &nShow);
		if (nShowAddress == nShow)return i;
	}
	return -1;
}

VOID DebugDlg::ReadMemoryDword(DWORD nAddress, LPDWORD nValue)
{
	DWORD nReadSize;
	ReadProcessMemory(m_ProcessHandle, (LPCVOID)nAddress, nValue, 4, &nReadSize);
}
VOID DebugDlg::WriteMemoryDword(DWORD nAddress, DWORD nValue)
{
	DWORD nOldProtect;
	DWORD nWriteSize;
	VirtualProtectEx(m_ProcessHandle, (LPVOID)nAddress, 1, PAGE_EXECUTE_READWRITE, &nOldProtect);
	WriteProcessMemory(m_ProcessHandle, (LPVOID)nAddress, &nValue, 4, &nWriteSize);
	VirtualProtectEx(m_ProcessHandle, (LPVOID)nAddress, 1, nOldProtect, &nOldProtect);
}
VOID DebugDlg::ReadMemoryByte(DWORD nAddress, LPBYTE nValue)
{
	DWORD nReadSize;
	ReadProcessMemory(m_ProcessHandle, (LPCVOID)nAddress, nValue, 1, &nReadSize);
}
VOID DebugDlg::WriteMemoryByte(DWORD nAddress, BYTE nValue)
{
	DWORD nOldProtect;
	DWORD nWriteSize;
	VirtualProtectEx(m_ProcessHandle, (LPVOID)nAddress, 1, PAGE_EXECUTE_READWRITE, &nOldProtect);
	WriteProcessMemory(m_ProcessHandle, (LPVOID)nAddress, &nValue, 1, &nWriteSize);
	VirtualProtectEx(m_ProcessHandle, (LPVOID)nAddress, 1, nOldProtect, &nOldProtect);
}
VOID DebugDlg::ReadMemoryBytes(DWORD nAddress, LPBYTE nValue, DWORD nLen)
{
	DWORD nReadSize;
	ReadProcessMemory(m_ProcessHandle, (LPCVOID)nAddress, nValue, nLen, &nReadSize);
}
VOID DebugDlg::WriteMemoryBytes(DWORD nAddress, LPBYTE nValue, DWORD nLen)
{
	DWORD nOldProtect;
	DWORD nWriteSize;
	VirtualProtectEx(m_ProcessHandle, (LPVOID)nAddress, nLen, PAGE_EXECUTE_READWRITE, &nOldProtect);
	WriteProcessMemory(m_ProcessHandle, (LPVOID)nAddress, nValue, nLen, &nWriteSize);
	VirtualProtectEx(m_ProcessHandle, (LPVOID)nAddress, nLen, nOldProtect, &nOldProtect);
}
VOID DebugDlg::ReadMemoryWord(DWORD nAddress, LPWORD nValue)
{
	DWORD nReadSize;
	ReadProcessMemory(m_ProcessHandle, (LPCVOID)nAddress, nValue, 2, &nReadSize);
}
VOID DebugDlg::WriteMemoryWord(DWORD nAddress, WORD nValue)
{
	DWORD nOldProtect;
	DWORD nWriteSize;
	VirtualProtectEx(m_ProcessHandle, (LPVOID)nAddress, 1, PAGE_EXECUTE_READWRITE, &nOldProtect);
	WriteProcessMemory(m_ProcessHandle, (LPVOID)nAddress, &nValue, 2, &nWriteSize);
	VirtualProtectEx(m_ProcessHandle, (LPVOID)nAddress, 1, nOldProtect, &nOldProtect);
}



void DebugDlg::OnDblclkListHeap(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (pNMItemActivate->iItem == -1)return;

	CString nTemp;
	DWORD nReadAddress;

	if (pNMItemActivate->iSubItem == 1)
	{
		nTemp = m_List_Heap.GetItemText(pNMItemActivate->iItem, 1);
		if (nTemp.IsEmpty())return;

		_stscanf_s(nTemp, TEXT("%x"), &nReadAddress);
		if (nReadAddress < 65536)return;

		InitAsm(nReadAddress);
		m_List_Asm.SetFocus();
		m_List_Asm.SetSelectionEx(0);


	}

}

void DebugDlg::OnCustomdrawListMemory(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	LPNMCUSTOMDRAW nmCustomDraw = &pNMCD->nmcd;

	switch (nmCustomDraw->dwDrawStage)
	{
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
	{

		if (m_List_Memory.IsSelectionItem(nmCustomDraw->dwItemSpec))
		{
			pNMCD->clrText = COLOR_WHITE;
			pNMCD->clrTextBk = COLOR_BLUE;
			nmCustomDraw->uItemState = 0;
			*pResult = CDRF_NEWFONT;
			return;
		}
		break;
	}
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;		//必须有，不然就没有效果
	*pResult |= CDRF_NOTIFYITEMDRAW;		//必须有，不然就没有效果}

}
void DebugDlg::OnCustomdrawListHeap(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	LPNMCUSTOMDRAW nmCustomDraw = &pNMCD->nmcd;

	switch (nmCustomDraw->dwDrawStage)
	{
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
	{

		if (m_List_Heap.IsSelectionItem(nmCustomDraw->dwItemSpec))
		{
			pNMCD->clrText = COLOR_WHITE;
			pNMCD->clrTextBk = COLOR_BLUE;
			nmCustomDraw->uItemState = 0;
			*pResult = CDRF_NEWFONT;
			return;
		}
		break;
	}
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;		//必须有，不然就没有效果
	*pResult |= CDRF_NOTIFYITEMDRAW;		//必须有，不然就没有效果}}
}
void DebugDlg::OnCustomdrawListAsm(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	LPNMCUSTOMDRAW nmCustomDraw = &pNMCD->nmcd;

	switch (nmCustomDraw->dwDrawStage)
	{
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
	{


		if (m_List_Asm.IsSelectionItem(nmCustomDraw->dwItemSpec))
		{
			pNMCD->clrText = COLOR_WHITE;
			pNMCD->clrTextBk = COLOR_BLUE;
			nmCustomDraw->uItemState = 0;
			*pResult = CDRF_NEWFONT;
			return;
		}


		for(auto &value: m_AsmColor)
		{
			if (value.Item == nmCustomDraw->dwItemSpec && value.SubItem == pNMCD->iSubItem)
			{
				pNMCD->clrText = value.TextColor;
				pNMCD->clrTextBk = value.BkColor;
				break;
			}
			else
			{
				pNMCD->clrText = COLOR_BLACK;
				pNMCD->clrTextBk = COLOR_WHITE;
			}
		}
		break;
	}
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;		//必须有，不然就没有效果
	*pResult |= CDRF_NOTIFYITEMDRAW;		//必须有，不然就没有效果}
}

VOID DebugDlg::SetItemColor(DWORD Item, DWORD SubItem, COLORREF TextColor, COLORREF BkColor)
{
	BOOL nHava = FALSE;
	for (auto &value : m_AsmColor)
	{
		if (value.Item == Item && value.SubItem== SubItem)
		{
			value.TextColor = TextColor;
			value.BkColor = BkColor;
			nHava = TRUE;
			break;
		}
	}

	if (!nHava)m_AsmColor.push_back({ Item ,SubItem ,TextColor ,BkColor });
	
}
VOID DebugDlg::ClearItemColor()
{
	m_AsmColor.clear();
	m_AsmColor.swap(vector<LISTCOLOR>());
}

BOOL DebugDlg::IsColorWord(CString nStr, LPDWORD nTextColor, LPDWORD nBkColor)
{
	if (nStr.Left(4) == "call")
	{
		*nTextColor = COLOR_RED;
		*nBkColor = COLOR_WHITE;
		return TRUE;
	}
	else if (nStr.Left(1) == "j")
	{
		*nTextColor = COLOR_BLUE;
		*nBkColor = COLOR_WHITE;
		return TRUE;
	}
	else if (nStr.Left(3) == "ret")
	{
		*nTextColor = COLOR_GREEN;
		*nBkColor = COLOR_WHITE;
		return TRUE;
	}
	return FALSE;
}
BOOL DebugDlg::IsJumpWord(CString nStr, LPDWORD nAddress)
{
	if (nStr.Left(4) == CString("call") || nStr.Left(1) == CString("j"))
	{
		nStr.Replace(TEXT("["), TEXT(""));
		nStr.Replace(TEXT("]"), TEXT(""));
		nStr.Replace(TEXT("h"), TEXT(""));
		nStr = nStr.Right(8);
		_stscanf_s(nStr, TEXT("%x"), nAddress);

		return TRUE;
	}

	return FALSE;
}

BOOL DebugDlg::AsmToOpcaode(DWORD nAddress, CHAR *nAsm, LPBYTE nBytes, DWORD &nByteNum)
{
	XEDPARSE nXedparse = { 0 };

	nXedparse.cip = (ULONGLONG)nAddress;
	strcpy_s(nXedparse.instr, XEDPARSE_MAXBUFSIZE, nAsm);

	if (XEDParseAssemble(&nXedparse) != XEDPARSE_OK)return FALSE;

	nByteNum = nXedparse.dest_size;

	memcpy_s(nBytes, nXedparse.dest_size, nXedparse.dest, nXedparse.dest_size);

	return TRUE;
}

