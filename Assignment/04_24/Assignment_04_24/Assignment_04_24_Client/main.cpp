#pragma comment(lib, "Ws2_32.lib")

#include <cstdio>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define GAME_ROOM_NAME_LEN 64

int wmain(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}

	SOCKET clientSocket;
	SOCKADDR_IN destAddress;
	int retVal;

	const UINT8 protocolHeader[10] = { 0xff, 0xee, 0xdd, 0xaa, 0x00, 0x99, 0x77, 0x55, 0x33, 0x11 };

	/* 
		Make Socket
	*/
	clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clientSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d\n", errorCode);
		WSACleanup();
		return 1;
	}

	/*
		Set Socket Address
	*/
	destAddress.sin_family = AF_INET;
	destAddress.sin_addr.s_addr = INADDR_BROADCAST;

	/*
		Set Socket Option
	*/
	BOOL bBroadcastOption = true;
	retVal = setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&bBroadcastOption, sizeof(bBroadcastOption));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d\n", errorCode);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	int timeOut = 200;
	retVal = setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeOut, sizeof(timeOut));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d\n", errorCode);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	/*
		Broadcast via port 10000 ~ 10099
	*/
	for (USHORT portNum = 10000; portNum <= 10099; portNum++)
	{
		WCHAR gameRoomNameBuf[GAME_ROOM_NAME_LEN + 1];
		WCHAR responseAddressBuf[20];
		SOCKADDR_IN responseAddress;
		int responseAddressSize = sizeof(responseAddress);

		destAddress.sin_port = htons(portNum);

		retVal = sendto(clientSocket, (const char*)protocolHeader, sizeof(protocolHeader), 0, (SOCKADDR*)&destAddress, sizeof(destAddress));
		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d\n", errorCode);
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

		retVal = recvfrom(clientSocket, (char*)gameRoomNameBuf, sizeof(gameRoomNameBuf), 0, (SOCKADDR*)&responseAddress, &responseAddressSize);
		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == 10060)
			{
				continue;
			}

			wprintf(L"Error : %d\n", errorCode);
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
		else if (retVal == 0)
		{
			break;
		}

		gameRoomNameBuf[retVal / 2] = '\0';
		InetNtopW(AF_INET, (void*)&(responseAddress.sin_addr), responseAddressBuf, sizeof(responseAddressBuf));
		wprintf(L"Host : %s:%d | Room Name : %s\n", responseAddressBuf, ntohs(responseAddress.sin_port), gameRoomNameBuf);
	}
	
	return 0;
}