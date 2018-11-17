#include "stdafx.h"
#include "LoadVirusLib.h"


LoadVirusLib::LoadVirusLib()
{

}


LoadVirusLib::~LoadVirusLib()
{
	ClearVirusLibData();
}
//读取本地病毒库
DWORD LoadVirusLib::LoadVirusLibData()
{

	HANDLE  pFile = nullptr;
	pFile = CreateFile(GetProgramDir()+ TEXT("VirusLib.dat"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (pFile == (HANDLE)-1)return 0;

	DWORD nSize = GetFileSize((PHANDLE)pFile, 0);
	DWORD nReadSize = 0;

	m_VirusCount = nSize / sizeof(VIRUSINFO);
	m_VirusLib = new VIRUSINFO[m_VirusCount];
	ReadFile(pFile, (LPVOID)m_VirusLib, nSize, &nReadSize,NULL);
	CloseHandle(pFile);
	return m_VirusCount;
}
//取出病毒库信息
VOID LoadVirusLib::GetVirusLib(PVIRUSINFO &nVIRUSINFO)
{
	memcpy_s(nVIRUSINFO, sizeof(VIRUSINFO)*m_VirusCount, m_VirusLib, sizeof(VIRUSINFO)*m_VirusCount);
}
//写出病毒库
VOID LoadVirusLib::SetVirusLib(PVIRUSINFO &nVIRUSINFO, DWORD nCount)
{
	FILE *pFile = nullptr;
	fopen_s(&pFile, GetProgramDir() + "VirusLib.dat", "wb");
	m_VirusCount = fwrite(nVIRUSINFO, sizeof(VIRUSINFO), nCount, pFile);
	fclose(pFile);
}
//关闭病毒库
VOID LoadVirusLib::ClearVirusLibData()
{
	if (m_VirusLib !=nullptr)
	{
		delete[] m_VirusLib;
	}
	m_VirusLib = nullptr;
	m_VirusCount = 0;
}


//读取白名单
DWORD LoadVirusLib::LoadProcessLibData()
{

	HANDLE  pFile = nullptr;
	pFile = CreateFile(GetProgramDir() + TEXT("ProcessLib.dat"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (pFile == (HANDLE)-1)return 0;

	DWORD nSize = GetFileSize((PHANDLE)pFile, 0);
	DWORD nReadSize = 0;

	m_ProcessCount = nSize / sizeof(VIRUSINFO);
	m_ProcessLib = new VIRUSINFO[m_ProcessCount];
	ReadFile(pFile, (LPVOID)m_ProcessLib, nSize, &nReadSize, NULL);
	CloseHandle(pFile);
	return m_ProcessCount;
}
//取出白名单信息
VOID LoadVirusLib::GetProcessLib(PVIRUSINFO &nVIRUSINFO)
{
	memcpy_s(nVIRUSINFO, sizeof(VIRUSINFO)*m_ProcessCount, m_ProcessLib, sizeof(VIRUSINFO)*m_ProcessCount);
}
//写出白名单
VOID LoadVirusLib::SetProcessLib(PVIRUSINFO &nVIRUSINFO, DWORD nCount)
{
	FILE *pFile = nullptr;
	fopen_s(&pFile, GetProgramDir() + "ProcessLib.dat", "wb");
	m_ProcessCount = fwrite(nVIRUSINFO, sizeof(VIRUSINFO), nCount, pFile);
	fclose(pFile);
}
//关闭白名单
VOID LoadVirusLib::ClearProcessLibData()
{
	if (m_ProcessLib != nullptr)
	{
		delete[] m_ProcessLib;
	}
	m_ProcessLib = nullptr;
	m_ProcessCount = 0;
}


//连接服务器
BOOL LoadVirusLib::LineServer()
{
	m_IsLine = m_Client.LineServer(m_Ip, m_Port);
	return m_IsLine;
}
//上传病毒库
BOOL LoadVirusLib::UpDataVirus(VIRUSINFO &nVIRUSINFO)
{
	if (!m_IsLine)
	{
		m_IsLine = m_Client.LineServer(m_Ip, m_Port);
		if (!m_IsLine)
		{
			MessageBox(NULL, TEXT("连接云端服务器失败！"), TEXT("错误"), MB_ICONERROR);
			return FALSE;
		}
	}

	CString nTemp;
	nTemp.Format(TEXT("上传病毒%s%s%s%s"), SEG, CString(nVIRUSINFO.FileName), SEG, CString(nVIRUSINFO.FileMd5));

	return m_Client.SendData(nTemp);
	//		MessageBox(NULL, TEXT("发送失败"), TEXT("错误"), NULL);

}
//下载病毒库
BOOL LoadVirusLib::DownDataVirus()
{
	if (!m_IsLine)
	{
		m_IsLine = m_Client.LineServer(m_Ip, m_Port);
		if (!m_IsLine) 
		{ 
			MessageBox(NULL, TEXT("连接云端服务器失败！"), TEXT("错误"), MB_ICONERROR);
			return FALSE;
		}
	}
	
	CString nTemp;
	nTemp.Format(TEXT("下载病毒%s0"), SEG);
	return m_Client.SendData(nTemp);
	//		MessageBox(NULL, TEXT("发送失败"), TEXT("错误"), NULL);
}
//接受服务器返回病毒库
DWORD LoadVirusLib::RecvDataVirus(PVIRUSINFO &nVIRUSINFO)
{
	BOOL nRet = FALSE;
	CString nRecvData;
	CString nState;
	CString nData;

	vector<CString>nRecvText;
	vector<CString>nVirusStruct;

	while (true)
	{
		nRet = m_Client.RecvData(nRecvData);

		if (nRet)
		{
			SegmentText(nRecvText, nRecvData, SEG);
			if (nRecvText.size() >= 2)
			{
				nState = nRecvText[0];
				nData = nRecvText[1];
				break;
			}

		}
		Sleep(5);
	}

	if (nState == TEXT("下载病毒"))
	{
		SegmentText(nRecvText, nData, TEXT("|*|"));

		DWORD nVirusNum = nRecvText.size();
		if (nVirusNum == 0)	return 0;


		nVIRUSINFO = new VIRUSINFO[nVirusNum];

		for (DWORD i = 0; i < nVirusNum; i++)
		{
			SegmentText(nVirusStruct, nRecvText[i], TEXT("|"));

			strcpy_s(nVIRUSINFO[i].FileName, MAX_PATH, CStringA(nVirusStruct[0]));
			strcpy_s(nVIRUSINFO[i].FileMd5, MAX_PATH, CStringA(nVirusStruct[1]));
		}

		return nVirusNum;
	}

	return 0;
}
