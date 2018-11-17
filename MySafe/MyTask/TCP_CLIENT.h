#pragma once
class TCP_CLIENT
{
private:
	SOCKET m_SockClient;
	WSADATA m_WsaData;
	DWORD MySelect(SOCKET &nSocket);
public:

	TCP_CLIENT();
	~TCP_CLIENT();

	BOOL IntiClient();
	BOOL LineServer(const CString &nIp, const WORD &nPort);
	BOOL RecvData(CString &nData);
	BOOL RecvData(PCHAR &nData, DWORD &nSize);
	BOOL SendData(CString &nData);
	BOOL SendData(const CHAR *nData, const DWORD &nSize);
};

