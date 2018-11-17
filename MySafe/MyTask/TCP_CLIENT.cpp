#include "stdafx.h"
#include "TCP_CLIENT.h"
#include <WS2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")//���ÿ��ļ�

DWORD TCP_CLIENT::MySelect(SOCKET &nSocket)
{
	fd_set nFdread;
	timeval nTv;
	FD_ZERO(&nFdread);//��ʼ��fd_set
	FD_SET(nSocket, &nFdread);//�����׽��־������Ӧ��fd_set
	nTv.tv_sec = 0;//�������Ǵ�����select�ȴ�����󷵻أ����ⱻ����
	nTv.tv_usec = 0;
	select(0, &nFdread, NULL, NULL, &nTv);
	return FD_ISSET(nSocket, &nFdread);
}
TCP_CLIENT::TCP_CLIENT()
{
	IntiClient();
}
TCP_CLIENT::~TCP_CLIENT()
{
	closesocket(m_SockClient);
	WSACleanup();//�ͷų�ʼ��Ws2_32.dll���������Դ��
}
BOOL TCP_CLIENT::IntiClient()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
	{
		MessageBox(NULL,TEXT( "��ʼ��Winsockʧ��"), TEXT("����"), NULL);
		return FALSE;
	}

	m_SockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == m_SockClient)
	{
		MessageBox(NULL, TEXT("�����׽���ʧ��"), TEXT("����"), NULL);
		return FALSE;
	}
	return TRUE;
}
BOOL TCP_CLIENT::LineServer(const CString &nIp, const WORD &nPort)
{
	CStringA nTempIp(nIp);
	SOCKADDR_IN nAddrSrv;
	nAddrSrv.sin_family = AF_INET;
	nAddrSrv.sin_port = htons(nPort);//�˿ں�
	inet_pton(AF_INET, nTempIp, &nAddrSrv.sin_addr.S_un.S_addr);//IP��ַ

	if (connect(m_SockClient, (sockaddr*)&nAddrSrv, sizeof(SOCKADDR_IN)) == INVALID_SOCKET)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL TCP_CLIENT::RecvData(CString &nData)
{
	if (MySelect(m_SockClient))//�Ƿ������ݰ��������ֱ������
	{
		INT nSize;
		CHAR *nRecvBuf = new CHAR[65535]{};//����ѿռ��������
		nSize = recv(m_SockClient, nRecvBuf, 65535, 0);//�����յ����ݿ������ѿռ�

/*
		for (int i = 0; i < nSize; i++)
		{
			nRecvBuf[i] = nRecvBuf[i] ^ 5;
		}*/

		if (nSize>0)//���������Ϊ0
		{
			CHAR *nTempData = new CHAR[nSize + sizeof(TCHAR)]{};//����ָ����С�Ŀռ�
			memcpy(nTempData, nRecvBuf , nSize);//�ӵ�4���ֽں�ʼ����
			nData = nTempData;
			delete[]nRecvBuf;//�ͷŶѿռ�
			delete[]nTempData;//�ͷŶѿռ�
			return TRUE;
		}
		delete[]nRecvBuf;//�ͷŶѿռ�
	}
	return FALSE;
}
BOOL TCP_CLIENT::RecvData(PCHAR &nData, DWORD &nDataSize)
{
	if (MySelect(m_SockClient))//�Ƿ������ݰ��������ֱ������
	{
		CHAR *nRecvBuf = new CHAR[1024]{};//����ѿռ��������
		nDataSize = recv(m_SockClient, nRecvBuf, 1024, 0);//�����յ����ݿ������ѿռ�
		//memcpy(&nDataSize, nRecvBuf, sizeof(DWORD));//����ȡ��ǰ4�ֽڣ����ݰ�������

		if (nDataSize)//���������Ϊ0
		{
			nData = new CHAR[nDataSize+ sizeof(DWORD)]{};//����ָ����С�Ŀռ�
			memcpy(nData, nRecvBuf + sizeof(DWORD), nDataSize);//�ӵ�4���ֽں�ʼ����
			delete[]nRecvBuf;//�ͷŶѿռ�
			return TRUE;
		}
		delete[]nRecvBuf;//�ͷŶѿռ�
	}
	return FALSE;
}
BOOL TCP_CLIENT::SendData(CString &nData)
{
	//����ָ����С�ĶԿռ��Ŵ���������
	INT nSize = sizeof(TCHAR)*nData.GetLength() ;
	CHAR *nSendbuf = new CHAR[nSize]{};

	memcpy(nSendbuf, &*nData, nSize);
	//��ʼ�������ݰ�

	for(int i=0;i<nSize;i++)
	{
		nSendbuf[i] = nSendbuf[i] ^ 5;//������
	}


	DWORD nSend = send(m_SockClient, nSendbuf, nSize, 0);
	delete[]nSendbuf;//��������ͷŶԿռ�

	if (nSend == SOCKET_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL TCP_CLIENT::SendData(const CHAR *nData, const DWORD &nSize)
{
	//����ָ����С�ĶԿռ��Ŵ���������
	CHAR *nSendbuf = new CHAR[nSize + sizeof(DWORD)]{};

	//������д��ѿռ���4�ֽ�
	memcpy(nSendbuf, &nSize, sizeof(DWORD));
	//������4�ֽڿ�ʼ�������д����͵Ľṹ����������
	memcpy(nSendbuf + sizeof(DWORD),nData, nSize);
	//��ʼ�������ݰ�
	DWORD nSend = send(m_SockClient, nSendbuf, nSize + sizeof(DWORD), 0);
	delete[]nSendbuf;//��������ͷŶԿռ�

	if (nSend == SOCKET_ERROR)
	{
		MessageBox(NULL, TEXT("����ʧ��"), TEXT("����"), NULL);
		return FALSE;
	}
	return TRUE;
}
