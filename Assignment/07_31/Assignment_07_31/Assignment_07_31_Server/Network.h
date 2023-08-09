#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <unordered_map>

#include "Session.h"
#include "SerializationBuffer.h"

#define SERVER_PORT 30000

using namespace MyDataStruct;

namespace NetworkLibrary
{
	class Network;
	class IContents
	{
	public:
		virtual void OnAccept(size_t sessionId) = 0;
		virtual void OnMessage(size_t sessionId, SPacket* packet) = 0;
		virtual void OnRelease(size_t sessionid) = 0;
	
	protected:
		void BindNetworkLibrary(Network* instance);

	protected:
		Network* _network;
		friend class Network;
	};

	class Network
	{
	public:
		Network(IContents* contents);
		~Network(void);

	private:
		static unsigned int WINAPI AcceptProc(void* arg);
		static unsigned int WINAPI WorkerProc(void* arg);

	private:
		void ReleaseSession(Session* session);

		void SendPost(Session* session);
		void RecvPost(Session* session);

		void RecvHandler(const size_t sessionId, const DWORD byte);
		void SendHandler(const size_t sessionId, const DWORD byte);

		void RecvByteToMsg(Session* session);

	public:
		void SendMsgToByte(const size_t sessionId, SPacket* packet);

	private:
		bool _bShutdown;

		SOCKET _listenSocket;
		SOCKADDR_IN _serverAddr;

		HANDLE _iocp;
		HANDLE _acceptThread;
		HANDLE _workerThreads[16];
		
		SessionManager _sessionManager;
		size_t _sessionId;

		IContents* _contents;
	};
}