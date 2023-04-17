#pragma comment(lib, "Ws2_32.lib")

#include <cstdio>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SERVER_DOMAIN_NAME L"procademyserver.iptime.org"
#define SERVER_PORT 10010

#define FILE_NAME L"probe.gif"

typedef struct PacketHeader
{
	DWORD	dwPacketCode;		// 0x11223344	우리의 패킷확인 고정값
	WCHAR	szName[32];			// 본인이름, 유니코드 NULL 문자 끝
	WCHAR	szFileName[128];	// 파일이름, 유니코드 NULL 문자 끝
	int	iFileSize;
} PacketHeader_t;

BOOL DomainToIP(const WCHAR* domainName, IN_ADDR* pAddr)
{
	ADDRINFOW* pAddrInfo;
	SOCKADDR_IN* pSockAddr;

	if (GetAddrInfo(domainName, L"0", NULL, &pAddrInfo) != 0)
	{
		return FALSE;
	}

	pSockAddr = (SOCKADDR_IN*)pAddrInfo->ai_addr;
	*pAddr = pSockAddr->sin_addr;
	FreeAddrInfo(pAddrInfo);

	return TRUE;
}

int wmain(void)
{
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}

	SOCKET clientSocket;
	SOCKADDR_IN socketAddr;
	int retVal;

	/*
		Create Socket
	*/
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"error : %d\n", errorCode);
		WSACleanup();
		return 1;
	}
	
	/*
		Address Setting
	*/
	memset(&socketAddr, 0, sizeof(socketAddr));
	socketAddr.sin_family = AF_INET;
	DomainToIP(SERVER_DOMAIN_NAME, &socketAddr.sin_addr);
	socketAddr.sin_port = htons(SERVER_PORT);

	wprintf(L"Destination : %d.%d.%d.%d\n",
		socketAddr.sin_addr.S_un.S_un_b.s_b1,
		socketAddr.sin_addr.S_un.S_un_b.s_b2,
		socketAddr.sin_addr.S_un.S_un_b.s_b3,
		socketAddr.sin_addr.S_un.S_un_b.s_b4
	);
	/*
		Connect To Server
	*/
	retVal = connect(clientSocket, (SOCKADDR*)&socketAddr, sizeof(socketAddr));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"error : %d\n", errorCode);
		
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	/*
		Prepare Data
	*/
	FILE* file;
	int fileSize;
	void* fileDataBuffer;
	PacketHeader_t packetHeader;

	file = _wfopen(FILE_NAME, L"rb");
	if (file == NULL)
	{
		wprintf(L"File Open Failed\n");
		return 1;
	}

	fseek(file, 0, FILE_END);
	fileSize = ftell(file);
	rewind(file);

	fileDataBuffer = malloc(fileSize);
	if (fileDataBuffer == NULL)
	{
		return 1;
	}
	fread(fileDataBuffer, 1, fileSize, file);

	memset(&packetHeader, 0, sizeof(packetHeader));

	packetHeader.dwPacketCode = 0x11223344;
	wcsncpy(packetHeader.szName, L"김경민", wcslen(L"김경민"));
	wcsncpy(packetHeader.szFileName, FILE_NAME, wcslen(FILE_NAME));
	packetHeader.iFileSize = fileSize;

	/*
	Send Data;
	*/
	retVal = send(clientSocket, (const char*)&packetHeader, sizeof(packetHeader), 0);
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"error : %d\n", errorCode);

		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	wprintf(L"Send File %s, %d Byte\n", FILE_NAME, fileSize);
	for (int iCnt = 0; iCnt < fileSize / 1000; iCnt++)
	{
		retVal = send(clientSocket, ((const char*)fileDataBuffer) + (1000 * iCnt), 1000, 0);
		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"error : %d\n", errorCode);

			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

		wprintf(L"[%d] - [Send] %d Bytes\n", iCnt + 1, retVal);
	}

	retVal = send(clientSocket, ((const char*)fileDataBuffer) + (1000 * (fileSize / 1000)), fileSize % 1000, 0);
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"error : %d\n", errorCode);

		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	wprintf(L"[Last] - [Send] %d Bytes\n", retVal);

	/*
		Check Return Code;
	*/

	int returnCode;
	recv(clientSocket, (char*)&returnCode, sizeof(int), 0);

	wprintf(L"returnCode : %#x\n", returnCode);
	

	closesocket(clientSocket);
	WSACleanup();
	
	return 0;
}