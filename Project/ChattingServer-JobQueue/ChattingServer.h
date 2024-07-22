#pragma once

#include "INetServer.h"

#include "ObjectPool.h"
#include "LockFreeQueue.h"
#include "Job.h"
#include "Region.h"
#include "Player.h"

#include <synchapi.h>
#include <unordered_map>

#pragma comment(lib, "synchronization.lib")

using namespace std;

class ChattingServer : public INetServer
{
public:
	ChattingServer();
	virtual ~ChattingServer();

public:
	bool Initialize(void);
	void Terminate(void);
	inline bool IsAlive(void) const
	{
		return _isAlive;
	}

private:
	virtual void OnInitialize(void) override;
	virtual void OnTerminate(void) override;

	virtual bool OnConnectRequest(const wchar_t* ip, const short port) override;
	virtual void OnAccept(const SessionID sessionId) override;
	virtual void OnRelease(const SessionID sessionId) override;

	virtual void OnRecv(const SessionID sessionId, SPacket* packet) override;
	//virtual void OnSend(const SessionID sessionId, const int sentByte) override;

private:
	bool AcceptHandler(const SessionID sessionId);
	void ReleaseHandler(const SessionID sessionId);
	void TimeoutHandler(void);
	void RecvPacketHandler(const SessionID sessionId, SPacket* packet);
	
private:
	static unsigned int WINAPI LogicProc(void* arg);
	static unsigned int WINAPI TimeoutAlertProc(void* arg);
	static unsigned int WINAPI MonitorProc(void* arg);
	
private:
	void HandlePacketLogin(SPacket* packet, Player* player);
	void HandlePacketMove(SPacket* packet, Player* player);
	void HandlePacketChat(SPacket* packet, Player* player);
	
	void MashallPacketLogin(SPacket* packet, const unsigned char status, const __int64 accountNumber);
	void MashallPacketMove(SPacket* packet, const __int64 accountNumber, const short x, const short y);
	void MashallPacketChat(SPacket* packet, const __int64 accountNumber, wchar_t* id, wchar_t* nickname, const short textLen, wchar_t* text);

	void DemashallPacketLogin(SPacket* packet, __int64* outAccountNumber, wchar_t* outId, wchar_t* outNickname, char* outSessionKey);
	void DemashallPacketMove(SPacket* packet, __int64* outAccountNumber, short* outX, short* outY);
	void DemashallPacketChat(SPacket* packet, __int64* outAccountNumber, short* outMsgLen, wchar_t** outMessage);

private:
	void SendUnicast(SPacket* packet, const SessionID sessionId);
	void SendMulticastAroundRegion(SPacket* packet, const Region& centerRegion, int excludePlayerNum = 0, Player* excludePlayer = nullptr);

private:
	bool _isAlive = true;
	HANDLE _logicThread;
	HANDLE _timeoutThread;
	HANDLE _monitorThread;

	Region _regions[REGION_Y_MAX][REGION_X_MAX];
	unsigned __int64 _playerIdProvider = 0;
	unordered_map<unsigned __int64, Player*> _playerMap;
	ObjectPool<Player>* _playerPool;

	long _jobQueueSignal = 0;
	LockFreeQueue<Job*>* _jobQueue;
	ObjectPool<Job>* _jobPool;

	long _updateTPS = 0;
	long _packetHandleTPS = 0;
};