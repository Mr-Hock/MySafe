#pragma once
#include "MyHead.h"
#include "TCP_CLIENT.h"


class LoadVirusLib
{
public:
	TCP_CLIENT m_Client;

	LoadVirusLib();
	~LoadVirusLib();

	PVIRUSINFO m_VirusLib = nullptr;
	DWORD m_VirusCount = 0;
	PVIRUSINFO m_ProcessLib = nullptr;
	DWORD m_ProcessCount = 0;


	CString m_Ip = TEXT("127.0.0.1");
	WORD m_Port = 8888;
	CString SEG = TEXT("|#|");
	BOOL m_IsLine = FALSE;

	DWORD LoadVirusLibData();
	VOID GetVirusLib(PVIRUSINFO &nVIRUSINFO);
	VOID ClearVirusLibData();
	VOID SetVirusLib(PVIRUSINFO &nVIRUSINFO, DWORD nCount);

	DWORD LoadProcessLibData();
	VOID GetProcessLib(PVIRUSINFO &nVIRUSINFO);
	VOID ClearProcessLibData();
	VOID SetProcessLib(PVIRUSINFO &nVIRUSINFO, DWORD nCount);



	BOOL LineServer();

	BOOL UpDataVirus(VIRUSINFO &nVIRUSINFO);
	BOOL DownDataVirus();

	DWORD RecvDataVirus(PVIRUSINFO &nVIRUSINFO);
};

