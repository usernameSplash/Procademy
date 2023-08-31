#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "Session.h"
#include "Player.h"
#include "Region.h"
#include "ObjectPool.h"

using namespace std;

namespace TCPFighter_Select_Server
{
	class Server
	{
	public:
		Server();
		~Server();

	public:
		void IOController(void);
		void Update(void);
		void Monitor(void);

	private:
		void SelectSockets(const int recvSocketIdx, const int recvSocketCnt, const int sendSocketIdx, const int sendSocketCnt);
		void AcceptProc(void);
		void RecvProc(Session* session);
		void SendProc(Session* session);

	private:
		void HandleRecvPacket(Player* player, const BYTE packetType);
		void HandlePacketMoveStart(Player* player);
		void HandlePacketMoveStop(Player* player);
		void HandlePacketAttack1(Player* player);
		void HandlePacketAttack2(Player* player);
		void HandlePacketAttack3(Player* player);
		void HandlePacketEcho(Player* player);

		int SetPacketCreateMyCharacter(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y, const BYTE hp);
		int SetPacketCreateOtherCharacter(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y, const BYTE hp);
		int SetPacketDeleteCharacter(SPacket* outPacket, const int playerId);
		int SetPacketMoveStart(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y);
		int SetPacketMoveStop(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y);
		int SetPacketAttack1(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y);
		int SetPacketAttack2(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y);
		int SetPacketAttack3(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y);
		int SetPacketDamage(SPacket* outPacket, const int attackId, const int damagedId, const BYTE damagedHp);
		int SetPacketSync(SPacket* outPacket, const int id, const short x, const short y);
		int SetPacketEcho(SPacket* outPacket, const int time);

		void HandlePlayerAttack(Player* player, Player** outDamagedPlayer, const BYTE dir, const BYTE attackType, const short x, const short y);

	private:
		void SendToPlayer(char* msg, const int size, Session* session);
		void SendToOneRegion(char* msg, const int size, Region* region, Session* excludedSession = nullptr);
		void SendToAroundRegion(char* msg, const int size, Region* region, Session* excludedSession = nullptr);

		void DeactivateSession(Session* session, const bool bGracefulShutdown);
		void ReleaseDeadSession(void);

	private:
		void CreatePlayer(Session* session);
		bool IsMovable(short x, short y, eMoveDir dir);
		void UpdatePlayer(Player* player);
		
	private:
		void UpdateRegion(Player* player, eMoveDir dir);
		void SetRegion(Player* player);
		void RegionMapInitialize(void);

	private:
		ObjectPool<Session>* _sessionPool;
		ObjectPool<Player>* _playerPool;

	// Variables For Session
	private:
		FD_SET _readSet;
		FD_SET _writeSet;
		timeval _time;

		SOCKET _listenSocket;

		Session* _sessions[SESSION_MAX];	// active sessions
		Session* _recvSessions[SESSION_MAX];	// sockets to register FD_SET (select)
		Session* _sendSessions[SESSION_MAX];

		SessionID _sessionIdProvider = 0;
		int _returnedIdCnt = 0;
		SessionID _returnedIds[SESSION_MAX];

		int _deactivatedCnt = 0;
		Session* _deactivatedSessions[SESSION_MAX];

		ULONGLONG _lastTick;

	// Variables For Player
	private:
		int _playerIdProvider = 0;
		Player* _players[SESSION_MAX];

	// Variables For Region
	private:
		Region _regionMap[REGION_MAX_Y][REGION_MAX_X];
		int _regionCnt[8] = {
			REGION_NUM_ORTHO, REGION_NUM_DIAG, REGION_NUM_ORTHO, REGION_NUM_DIAG, REGION_NUM_ORTHO, REGION_NUM_DIAG, REGION_NUM_ORTHO, REGION_NUM_DIAG
		};

	//Variables For Monitoring
	private:
		size_t _acceptTotal = 0;
		size_t _disconnectTotal = 0;
		size_t _deadTotal = 0;
		size_t _timeoutTotal = 0;
		size_t _gracefulTotal = 0;
		size_t _syncTotal;

		size_t _acceptTPS = 0;
		size_t _disconnectTPS = 0;
		size_t _deadTPS = 0;
		size_t _timeoutTPS = 0;
		size_t _gracefulTPS = 0;

		size_t _recvTPS = 0;
		size_t _sendTPS = 0;
		size_t _syncTPS = 0;
	};
}