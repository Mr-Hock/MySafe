#include "stdafx.h"
#include "TCP_CLIENT.h"
#include <WS2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")//引用库文件

DWORD TCP_CLIENT::MySelect(SOCKET &nSocket)
{
	fd_set nFdread;
	timeval nTv;
	FD_ZERO(&nFdread);//初始化fd_set
	FD_SET(nSocket, &nFdread);//分配套接字句柄到相应的fd_set
	nTv.tv_sec = 0;//这里我们打算让select等待两秒后返回，避免被锁死
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
	WSACleanup();//释放初始化Ws2_32.dll所分配的资源。
}
BOOL TCP_CLIENT::IntiClient()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_WsaData) != 0)
	{
		MessageBox(NULL,TEXT( "初始化Winsock失败"), TEXT("错误"), NULL);
		return FALSE;
	}

	m_SockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == m_SockClient)
	{
		MessageBox(NULL, TEXT("创建套接字失败"), TEXT("错误"), NULL);
		return FALSE;
	}
	return TRUE;
}
BOOL TCP_CLIENT::LineServer(const CString &nIp, const WORD &nPort)
{
	CStringA nTempIp(nIp);
	SOCKADDR_IN nAddrSrv;
	nAddrSrv.sin_family = AF_INET;
	nAddrSrv.sin_port = htons(nPort);//端口号
	inet_pton(AF_INET, nTempIp, &nAddrSrv.sin_addr.S_un.S_addr);//IP地址

	if (connect(m_SockClient, (sockaddr*)&nAddrSrv, sizeof(SOCKADDR_IN)) == INVALID_SOCKET)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL TCP_CLIENT::RecvData(CString &nData)
{
	if (MySelect(m_SockClient))//是否有数据包到达，否则直接跳过
	{
		INT nSize;
		CHAR *nRecvBuf = new CHAR[65535]{};//申请堆空间接受数据
		nSize = recv(m_SockClient, nRecvBuf, 65535, 0);//将接收到数据拷贝到堆空间

/*
		for (int i = 0; i < nSize; i++)
		{
			nRecvBuf[i] = nRecvBuf[i] ^ 5;
		}*/

		if (nSize>0)//如果数量不为0
		{
			CHAR *nTempData = new CHAR[nSize + sizeof(TCHAR)]{};//申请指定大小的空间
			memcpy(nTempData, nRecvBuf , nSize);//从第4个字节后开始拷贝
			nData = nTempData;
			delete[]nRecvBuf;//释放堆空间
			delete[]nTempData;//释放堆空间
			return TRUE;
		}
		delete[]nRecvBuf;//释放堆空间
	}
	return FALSE;
}
BOOL TCP_CLIENT::RecvData(PCHAR &nData, DWORD &nDataSize)
{
	if (MySelect(m_SockClient))//是否有数据包到达，否则直接跳过
	{
		CHAR *nRecvBuf = new CHAR[1024]{};//申请堆空间接受数据
		nDataSize = recv(m_SockClient, nRecvBuf, 1024, 0);//将接收到数据拷贝到堆空间
		//memcpy(&nDataSize, nRecvBuf, sizeof(DWORD));//首先取出前4字节，数据包的数量

		if (nDataSize)//如果数量不为0
		{
			nData = new CHAR[nDataSize+ sizeof(DWORD)]{};//申请指定大小的空间
			memcpy(nData, nRecvBuf + sizeof(DWORD), nDataSize);//从第4个字节后开始拷贝
			delete[]nRecvBuf;//释放堆空间
			return TRUE;
		}
		delete[]nRecvBuf;//释放堆空间
	}
	return FALSE;
}
BOOL TCP_CLIENT::SendData(CString &nData)
{
	//申请指定大小的对空间存放待发送数据
	INT nSize = sizeof(TCHAR)*nData.GetLength() ;
	CHAR *nSendbuf = new CHAR[nSize]{};

	memcpy(nSendbuf, &*nData, nSize);
	//开始发送数据包

	for(int i=0;i<nSize;i++)
	{
		nSendbuf[i] = nSendbuf[i] ^ 5;//异或加密
	}


	DWORD nSend = send(m_SockClient, nSendbuf, nSize, 0);
	delete[]nSendbuf;//发送完毕释放对空间

	if (nSend == SOCKET_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL TCP_CLIENT::SendData(const CHAR *nData, const DWORD &nSize)
{
	//申请指定大小的对空间存放待发送数据
	CHAR *nSendbuf = new CHAR[nSize + sizeof(DWORD)]{};

	//把数量写入堆空间首4字节
	memcpy(nSendbuf, &nSize, sizeof(DWORD));
	//再往后4字节开始拷贝所有待发送的结构体数组数据
	memcpy(nSendbuf + sizeof(DWORD),nData, nSize);
	//开始发送数据包
	DWORD nSend = send(m_SockClient, nSendbuf, nSize + sizeof(DWORD), 0);
	delete[]nSendbuf;//发送完毕释放对空间

	if (nSend == SOCKET_ERROR)
	{
		MessageBox(NULL, TEXT("发送失败"), TEXT("错误"), NULL);
		return FALSE;
	}
	return TRUE;
}
