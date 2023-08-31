#include "Server.h"
#include "SerializationBuffer.h"
#include "Protocol.h"
#include "main.h"

#include <cstdio>

namespace TCPFighter_Select_Server
{
	Server::Server()
	{
		WSADATA wsa;

		srand(0);

		g_bRunning = false;
		
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			wprintf(L"# WSAStartup Failed\n");
			return;
		}

		{
			_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (_listenSocket == INVALID_SOCKET)
			{
				int errorCode;
				errorCode = WSAGetLastError();
				wprintf(L"# Create Listen Socket Failed : %d\n", errorCode);
				return;
			}
		}

		{
			int bindRet;

			SOCKADDR_IN serverAddr;
			ZeroMemory(&serverAddr, sizeof(serverAddr));
			serverAddr.sin_family = AF_INET;
			InetPtonW(AF_INET, SERVER_IP, &serverAddr.sin_addr);
			serverAddr.sin_port = htons(SERVER_PORT);

			bindRet = bind(_listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
			if (bindRet == SOCKET_ERROR)
			{
				int errorCode;
				errorCode = WSAGetLastError();
				wprintf(L"# Bind Listen Socket Failed : %d\n", errorCode);
				return;
			}
		}

		{
			int ioctlRet;
			u_long ioctlOption = 1;
			
			ioctlRet = ioctlsocket(_listenSocket, FIONBIO, &ioctlOption);
			if (ioctlRet == SOCKET_ERROR)
			{
				int errorCode;
				errorCode = WSAGetLastError();
				wprintf(L"# Set Non-block Socket Failed : %d\n", errorCode);
				return;
			}
		}

		{
			int setSockOptRet;
			linger l;
			l.l_onoff = 1;
			l.l_linger = 0;

			setSockOptRet = setsockopt(_listenSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));

			if (setSockOptRet == SOCKET_ERROR)
			{
				int errorCode;
				errorCode = WSAGetLastError();
				wprintf(L"# Set Linger Option Failed : %d\n", errorCode);
				return;
			}

			BOOL nagleOption = TRUE;
			setSockOptRet = setsockopt(_listenSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&nagleOption, sizeof(nagleOption));

			if (setSockOptRet == SOCKET_ERROR)
			{
				int errorCode;
				errorCode = WSAGetLastError();
				wprintf(L"# Set Nagle Option Failed : %d\n", errorCode);
				return;
			}
		}

		{
			int listenRet;
			listenRet = listen(_listenSocket, SOMAXCONN_HINT(SOMAXCONN));
			if (listenRet == SOCKET_ERROR)
			{
				int errorCode = WSAGetLastError();
				wprintf(L"# Listen Failed : %d\n", errorCode);
				return;
			}
		}
		
		wprintf(L"# Network Setup Success\n");
		
		// Contents Initialization
		g_bRunning = true;

		for (int yCnt = 0; yCnt < REGION_MAX_Y; ++yCnt)
		{
			for (int xCnt = 0; xCnt < REGION_MAX_X; ++xCnt)
			{
				_regionMap[yCnt][xCnt].Initialize(xCnt, yCnt);
			}
		}

		RegionMapInitialize();

		_sessionPool = new ObjectPool<Session>(SESSION_MAX);
		_playerPool = new ObjectPool<Player>(SESSION_MAX);

		_time.tv_sec = 0;
		_time.tv_usec = 0;

		_lastTick = GetTickCount64();

		wprintf(L"# Server Setup Success - time : %lld\n", _lastTick);
	}

	Server::~Server()
	{
		closesocket(_listenSocket);

		for (int iCnt = 0; iCnt < SESSION_MAX; ++iCnt)
		{
			if (_sessions[iCnt] != nullptr)
			{
				closesocket(_sessions[iCnt]->_sock);
				_sessionPool->Free(_sessions[iCnt]);
			}

			if(_players[iCnt] != nullptr)
			{
				_playerPool->Free(_players[iCnt]);
			}
		}

		WSACleanup();

		delete _sessionPool;
		delete _playerPool;
	}

	void Server::IOController(void)
	{
		int rIdx;
		int rCnt;
		int sIdx;
		int sCnt;

		rIdx = 0;
		rCnt = 0;
		sIdx = 0;
		sCnt = 0;

		for (int sessionIdx = 0; sessionIdx < SESSION_MAX; ++sessionIdx)
		{
			if (_sessions[sessionIdx] == nullptr)
			{
				continue;
			}

			_recvSessions[rCnt++] = _sessions[sessionIdx];

			if (_sessions[sessionIdx]->_sendBuf.Size() > 0)
			{
				_sendSessions[sCnt++] = _sessions[sessionIdx];
			}
		}

		while ((rCnt - rIdx) >= (FD_SETSIZE - 1) && (sCnt - sIdx) >= FD_SETSIZE)
		{
			SelectSockets(rIdx, (FD_SETSIZE - 1), sIdx, FD_SETSIZE);
			rIdx += (FD_SETSIZE - 1);
			sIdx += FD_SETSIZE;
		}

		while ((rCnt - rIdx) >= (FD_SETSIZE - 1))
		{
			SelectSockets(rIdx, (FD_SETSIZE - 1), 0, 0);
			rIdx += (FD_SETSIZE - 1);
		}

		while ((sCnt - sIdx) >= FD_SETSIZE)
		{
			SelectSockets(0, 0, sIdx, FD_SETSIZE);
			sIdx += FD_SETSIZE;
		}

		SelectSockets(rIdx, (rCnt - rIdx), sIdx, (sCnt - sIdx));

		ReleaseDeadSession();
	}

	void Server::Update(void)
	{
		ULONGLONG now = GetTickCount64();

		if ((now - _lastTick) < MILLISEC_PER_FRAME)
		{
			return;
		}

		_lastTick += MILLISEC_PER_FRAME;

		for (int playerIdx = 0; playerIdx <= SESSION_MAX; ++playerIdx)
		{
			if (_players[playerIdx] == nullptr)
			{
				continue;
			}

			if ((GetTickCount64() - _players[playerIdx]->_session->_lastRecvTime) > NETWORK_PACKET_RECV_TIMEOUT)
			{
				_timeoutTPS++;
				DeactivateSession(_players[playerIdx]->_session, false);
				continue;
			}

			if (_players[playerIdx]->_status == ePlayerStatus::MOVE)
			{
				UpdatePlayer(_players[playerIdx]);
			}
		}
	}

	void Server::Monitor(void)
	{
		static ULONGLONG lastTick = GetTickCount64();

		if (GetTickCount64() - lastTick < 1000)
		{
			return;
		}
		lastTick += 1000;

		_acceptTotal += _acceptTPS;
		_disconnectTotal += _disconnectTPS;
		_deadTotal += _deadTPS;
		_timeoutTotal += _timeoutTPS;
		_gracefulTotal += _gracefulTPS;

		wprintf(L"=========================================================\n");
		wprintf(L" - Accept Total : %zu\n", _acceptTotal);
		wprintf(L" - Disconnect Total : %zu\n", _disconnectTotal);
		wprintf(L"   - Disconnect By Dead : %zu\n", _deadTotal);
		wprintf(L"   - Disconnect By Timeout : %zu\n", _timeoutTotal);
		wprintf(L"   - Disconnect By Server : %zu\n\n", _gracefulTotal);
		
		wprintf(L" - Accept/s : %zu\n", _acceptTPS);
		wprintf(L" - Recv/s : %zu\n", _recvTPS);
		wprintf(L" - Send/s : %zu\n", _sendTPS);
		wprintf(L" - Sync/s : %zu\n", _syncTPS);
		wprintf(L" - Connected Sessions : %d\n", _sessionIdProvider - _returnedIdCnt);
		wprintf(L"=========================================================\n\n");

		_acceptTPS = 0;
		_disconnectTPS = 0;
		_deadTPS = 0;
		_timeoutTPS = 0;
		_gracefulTPS = 0;
		_recvTPS = 0;
		_sendTPS = 0;
		_syncTPS = 0;
	}

	void Server::SelectSockets(const int recvSocketIdx, const int recvSocketCnt, const int sendSocketIdx, const int sendSocketCnt)
	{
		int selectRet;

		FD_ZERO(&_readSet);
		FD_ZERO(&_writeSet);

		FD_SET(_listenSocket, &_readSet);

		for (int offset = 0; offset < recvSocketCnt; ++offset)
		{
			FD_SET(_recvSessions[recvSocketIdx + offset]->_sock, &_readSet);
		}

		for (int offset = 0; offset < sendSocketCnt; ++offset)
		{
			FD_SET(_sendSessions[sendSocketIdx + offset]->_sock, &_writeSet);
		}

		selectRet = select(0, &_readSet, &_writeSet, NULL, &_time);

		if (selectRet == SOCKET_ERROR)
		{
			int errorCode;
			errorCode = WSAGetLastError();
			wprintf(L"# Select Error : %d\n", errorCode);
			return;
		}
		else if (selectRet > 0)
		{
			if (FD_ISSET(_listenSocket, &_readSet))
			{
				AcceptProc();
				--selectRet;
			}

			for (int offset = 0; offset < sendSocketCnt; ++offset)
			{
				if (selectRet <= 0)
				{
					break;
				}

				if (FD_ISSET(_sendSessions[sendSocketIdx + offset]->_sock, &_writeSet))
				{
					SendProc(_sendSessions[sendSocketIdx + offset]);
					--selectRet;
				}
			}

			for (int offset = 0; offset < recvSocketCnt; ++offset)
			{
				if (selectRet <= 0)
				{
					break;
				}

				if (FD_ISSET(_recvSessions[recvSocketIdx + offset]->_sock, &_readSet))
				{
					RecvProc(_recvSessions[recvSocketIdx + offset]);
					--selectRet;
				}
			}
		}
	}

	void Server::AcceptProc(void)
	{
		if (_sessionIdProvider >= SESSION_MAX && _returnedIdCnt == 0)
		{
			return;
		}

		_acceptTPS++;

		SessionID newId;

		if (_returnedIdCnt > 0)
		{
			newId = _returnedIds[--_returnedIdCnt];
		}
		else
		{
			newId = _sessionIdProvider++;
		}

		Session* newSession = _sessionPool->Alloc();
		newSession->Initialize(newId);	// i don't care whether newSession is nullptr.

		int addrlen = sizeof(SOCKADDR_IN);
		newSession->_sock = accept(_listenSocket, (SOCKADDR*)&newSession->_addr, &addrlen);

		if (newSession->_sock == INVALID_SOCKET)
		{
			int errorCode;
			errorCode = WSAGetLastError();
			wprintf(L"# Accept Failed : %d\n", errorCode);
			return;
		}

		newSession->_lastRecvTime = GetTickCount64();
		_sessions[newId] = newSession;
		
		CreatePlayer(newSession);
	}

	void Server::RecvProc(Session* session)
	{
		session->_lastRecvTime = GetTickCount64();
		_recvTPS++;

		int recvRet;
		int size;
		int directEnqueueSize;

		directEnqueueSize = static_cast<int>(session->_recvBuf.DirectEnqueueSize());
		
		recvRet = recv(session->_sock, session->_recvBuf.GetRearBufferPtr(), directEnqueueSize, 0);

		if (recvRet == SOCKET_ERROR)
		{
			int errorCode;
			errorCode = WSAGetLastError();

			switch (errorCode)
			{
			case WSAEWOULDBLOCK:
				break;
			case WSAECONNABORTED:
			case WSAECONNRESET:
				DeactivateSession(session, true);
				return;
			default:
				wprintf(L"# Session %d - Recv Error : %d\n", session->_id, errorCode);
				return;
			}
		}
		else if (recvRet == 0)
		{
			DeactivateSession(session, true);
			return;
		}

		if (recvRet > 0)
		{
			session->_recvBuf.MoveRear(recvRet);
		}
		size = static_cast<int>(session->_recvBuf.Size());

		Player* player = _players[session->_id];

		while (true)
		{
			if (size <= sizeof(SPacketHeader))
			{
				break;
			}

			SPacketHeader header;
			session->_recvBuf.Peek((char*)&header, sizeof(SPacketHeader));

			if (header.code != PACKET_CODE)
			{
				DeactivateSession(session, false);
				return;
			}

			if (size < sizeof(SPacketHeader) + header.size)
			{
				break;
			}

			session->_recvPacket.Clear();

			session->_recvBuf.Dequeue(sizeof(SPacketHeader));
			session->_recvBuf.Peek(session->_recvPacket.GetPayloadPtr(), header.size);
			session->_recvBuf.Dequeue(header.size);
			session->_recvPacket.MoveWritePos(header.size);

			HandleRecvPacket(player, header.type);

			size -= sizeof(SPacketHeader) + header.size;
		}

		return;
	}

	void Server::SendProc(Session* session)
	{
		_sendTPS++;

		int sendRet;
		int size;
		int directDequeueSize;
		int moveFrontRet;

		size = static_cast<int>(session->_sendBuf.Size());
		directDequeueSize = static_cast<int>(session->_sendBuf.DirectDequeueSize());

		sendRet = send(session->_sock, session->_sendBuf.GetFrontBufferPtr(), directDequeueSize, 0);

		if (sendRet == SOCKET_ERROR)
		{
			int errorCode;
			errorCode = WSAGetLastError();

			switch (errorCode)
			{
			case WSAEWOULDBLOCK:
				return;
			case WSAECONNABORTED:
			case WSAECONNRESET:
				DeactivateSession(session, true);
				return;
			default:
				wprintf(L"# Session %d - Send Error : %d\n", session->_id, errorCode);
				return;
			}
		}
		
		session->_sendBuf.MoveFront(sendRet);

		return;
	}

	void Server::HandleRecvPacket(Player* player, const BYTE packetType)
	{
		ePacketType type = static_cast<ePacketType>(packetType);

		switch (type)
		{
		case ePacketType::PACKET_CS_MOVE_START:
			HandlePacketMoveStart(player);
			break;
		case ePacketType::PACKET_CS_MOVE_STOP:
			HandlePacketMoveStop(player);
			break;
		case ePacketType::PACKET_CS_ATTACK_1:
			HandlePacketAttack1(player);
			break;
		case ePacketType::PACKET_CS_ATTACK_2:
			HandlePacketAttack2(player);
			break;
		case ePacketType::PACKET_CS_ATTACK_3:
			HandlePacketAttack3(player);
			break;
		case ePacketType::PACKET_CS_ECHO:
			HandlePacketEcho(player);
			break;
		default:
			wprintf(L"# Session %d - Wrong Packet Type Received : type = %d\n", player->_session->_id, packetType);
			break;
		}
	}

	void Server::HandlePacketMoveStart(Player* player)
	{
		BYTE dir;
		short x;
		short y;

		Session* session = player->_session;

		(session->_recvPacket) >> dir >> x >> y;

		if (abs(player->_x - x) > ERROR_RANGE || abs(player->_y - y) > ERROR_RANGE)
		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketSync(&session->_sendPacket, player->_id, player->_x, player->_y);
			SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);

			x = player->_x;
			y = player->_y;
		}

		player->_x = x;
		player->_y = y;

		player->_moveDir = static_cast<eMoveDir>(dir);

		switch (player->_moveDir)
		{
		case eMoveDir::LL:
		case eMoveDir::LU:
		case eMoveDir::LD:
			player->_dir = eMoveDir::LL;
			break;
		case eMoveDir::RU:
		case eMoveDir::RR:
		case eMoveDir::RD:
			player->_dir = eMoveDir::RR;
			break;
		default:
			break;
		}

		player->_status = ePlayerStatus::MOVE;

		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketMoveStart(&session->_sendPacket, player->_id, dir, x, y);
			SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, player->_region, session);
		}
		return;
	}

	void Server::HandlePacketMoveStop(Player* player)
	{
		BYTE dir;
		short x;
		short y;

		Session* session = player->_session;

		(session->_recvPacket) >> dir >> x >> y;

		if (abs(player->_x - x) > ERROR_RANGE || abs(player->_y - y) > ERROR_RANGE)
		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketSync(&session->_sendPacket, player->_id, player->_x, player->_y);
			SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);

			x = player->_x;
			y = player->_y;
		}

		player->_x = x;
		player->_y = y;
		player->_dir = static_cast<eMoveDir>(dir);
		player->_status = ePlayerStatus::IDLE;

		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketMoveStop(&session->_sendPacket, player->_id, dir, x, y);
			SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, player->_region, session);
		}

		return;
	}

	void Server::HandlePacketAttack1(Player* player)
	{
		BYTE dir;
		short x;
		short y;

		Session* session = player->_session;

		(session->_recvPacket) >> dir >> x >> y;

		if (abs(player->_x - x) > ERROR_RANGE || abs(player->_y - y) > ERROR_RANGE)
		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketSync(&session->_sendPacket, player->_id, player->_x, player->_y);
			SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);

			x = player->_x;
			y = player->_y;
		}

		player->_x = x;
		player->_y = y;
		player->_dir = static_cast<eMoveDir>(dir);
		player->_status = ePlayerStatus::ATTACK;

		Player* damagedPlayer = nullptr;
		HandlePlayerAttack(player, &damagedPlayer, dir, 1, x, y);

		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketAttack1(&session->_sendPacket, player->_id, dir, x, y);
			SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, player->_region, session);
		}

		if (damagedPlayer != nullptr)
		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketDamage(&session->_sendPacket, player->_id, damagedPlayer->_id, damagedPlayer->_hp);
			SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, damagedPlayer->_region);
		}

		return;
	}

	void Server::HandlePacketAttack2(Player* player)
	{
		BYTE dir;
		short x;
		short y;

		Session* session = player->_session;

		(session->_recvPacket) >> dir >> x >> y;

		if (abs(player->_x - x) > ERROR_RANGE || abs(player->_y - y) > ERROR_RANGE)
		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketSync(&session->_sendPacket, player->_id, player->_x, player->_y);
			SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);

			x = player->_x;
			y = player->_y;
		}

		player->_x = x;
		player->_y = y;
		player->_dir = static_cast<eMoveDir>(dir);
		player->_status = ePlayerStatus::ATTACK;

		Player* damagedPlayer = nullptr;
		HandlePlayerAttack(player, &damagedPlayer, dir, 2, x, y);

		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketAttack2(&session->_sendPacket, player->_id, dir, x, y);
			SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, player->_region, session);
		}

		if (damagedPlayer != nullptr)
		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketDamage(&session->_sendPacket, player->_id, damagedPlayer->_id, damagedPlayer->_hp);
			SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, damagedPlayer->_region);
		}

		return;
	}

	void Server::HandlePacketAttack3(Player* player)
	{
		BYTE dir;
		short x;
		short y;

		Session* session = player->_session;

		(session->_recvPacket) >> dir >> x >> y;

		if (abs(player->_x - x) > ERROR_RANGE || abs(player->_y - y) > ERROR_RANGE)
		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketSync(&session->_sendPacket, player->_id, player->_x, player->_y);
			SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);

			x = player->_x;
			y = player->_y;
		}

		player->_x = x;
		player->_y = y;
		player->_dir = static_cast<eMoveDir>(dir);
		player->_status = ePlayerStatus::ATTACK;

		Player* damagedPlayer = nullptr;
		HandlePlayerAttack(player, &damagedPlayer, dir, 3, x, y);

		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketAttack3(&session->_sendPacket, player->_id, dir, x, y);
			SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, player->_region, session);
		}

		if (damagedPlayer != nullptr)
		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketDamage(&session->_sendPacket, player->_id, damagedPlayer->_id, damagedPlayer->_hp);
			SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, damagedPlayer->_region);
		}

		return;
	}

	void Server::HandlePacketEcho(Player* player)
	{
		int time;

		Session* session = player->_session;

		(session->_recvPacket) >> time;

		{
			int packetSize;
			
			session->_sendPacket.Clear();
			packetSize = SetPacketEcho(&session->_sendPacket, time);

			SPacketHeader header;
			session->_sendPacket.GetHeaderData(&header);
			
			SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);
		}
	}


	int Server::SetPacketCreateMyCharacter(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y, const BYTE hp)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_CREATE_MY_CHARACTER);

		(*outPacket) << playerId << dir << x << y << hp;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}

	int Server::SetPacketCreateOtherCharacter(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y, const BYTE hp)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_CREATE_OTHER_CHARACTER);

		(*outPacket) << playerId << dir << x << y << hp;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}
	
	int Server::SetPacketDeleteCharacter(SPacket* outPacket, const int playerId)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_DELETE_CHARACTER);

		(*outPacket) << playerId;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}

	int Server::SetPacketMoveStart(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_MOVE_START);

		(*outPacket) << playerId << dir << x << y;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}
	
	int Server::SetPacketMoveStop(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_MOVE_STOP);

		(*outPacket) << playerId << dir << x << y;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}
	
	int Server::SetPacketAttack1(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_ATTACK_1);

		(*outPacket) << playerId << dir << x << y;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}
	
	int Server::SetPacketAttack2(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_ATTACK_2);

		(*outPacket) << playerId << dir << x << y;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}
	
	int Server::SetPacketAttack3(SPacket* outPacket, const int playerId, const BYTE dir, const short x, const short y)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_ATTACK_3);

		(*outPacket) << playerId << dir << x << y;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}
	
	int Server::SetPacketDamage(SPacket* outPacket, const int attackId, const int damagedId, const BYTE damagedHp)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_DAMAGE);

		(*outPacket) << attackId << damagedId << damagedHp;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}
	
	int Server::SetPacketSync(SPacket* outPacket, const int id, const short x, const short y)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_SYNC);

		(*outPacket) << id << x << y;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		_syncTPS++;

		return header.size + sizeof(SPacketHeader);
	}
	
	int Server::SetPacketEcho(SPacket* outPacket, const int time)
	{
		SPacketHeader header;
		header.code = PACKET_CODE;
		header.type = static_cast<BYTE>(ePacketType::PACKET_SC_ECHO);

		(*outPacket) << time;

		header.size = (BYTE)outPacket->Size();
		outPacket->SetHeaderData(&header);

		return header.size + sizeof(SPacketHeader);
	}

	void Server::HandlePlayerAttack(Player* player, Player** outDamagedPlayer, const BYTE dir, const BYTE attackType, const short x, const short y)
	{
		int rangeX;
		int rangeY;
		int damage;
		
		int xDirCoef = 0;	//Left(1) or right(-1)
		short xLimit = 0;

		switch (attackType)
		{
		case 1:
			rangeX = ATTACK1_RANGE_X;
			rangeY = ATTACK1_RANGE_Y;
			damage = ATTACK1_DAMAGE;
			break;
		case 2:
			rangeX = ATTACK2_RANGE_X;
			rangeY = ATTACK2_RANGE_Y;
			damage = ATTACK2_DAMAGE;
			break;
		case 3:
			rangeX = ATTACK3_RANGE_X;
			rangeY = ATTACK3_RANGE_Y;
			damage = ATTACK3_DAMAGE;
			break;
		default:
			wprintf(L"# Player %d - Wrong Attack Type : %d", player->_id, attackType);
			break;
		}

		Region* curRegion;
		curRegion = player->_region;

		if (static_cast<eMoveDir>(dir) == eMoveDir::LL)
		{
			xDirCoef = 1;
			xLimit = curRegion->_xMin;
		}
		else if (static_cast<eMoveDir>(dir) == eMoveDir::RR)
		{
			xDirCoef = -1;
			xLimit = curRegion->_xMax;
		}

		// Check Own Region First.
		for (auto playerIter = curRegion->_playerList.begin(); playerIter != curRegion->_playerList.end(); ++playerIter)
		{
			if (*playerIter == player)
			{
				continue;
			}

			int xDist = (x - (*playerIter)->_x) * xDirCoef;
			if (xDist >= 0 && xDist <= rangeX && abs(y - (*playerIter)->_y) <= rangeY)	// Damaged Player Found
			{
				*outDamagedPlayer = *playerIter;
				(*outDamagedPlayer)->_hp -= damage;

				if ((*outDamagedPlayer)->_hp <= 0)
				{
					(*outDamagedPlayer)->_status = ePlayerStatus::DEAD;
					_deadTPS++;

					DeactivateSession((*outDamagedPlayer)->_session, false);
				}

				return;
			}
		}
		
		// Check Left & Right Regions and LU, RU, LD, RD Regions
		// Generalization coord calculation by Attacking Direction
		if (((player->_x - (rangeX * xDirCoef)) - xLimit) * xDirCoef < 0)
		{
			int dirIdx = 0;
			if (xDirCoef == 1)
			{
				dirIdx = static_cast<int>(eMoveDir::LL);
			}
			else if (xDirCoef == -1)
			{
				dirIdx = static_cast<int>(eMoveDir::RR);
			}

			for (auto playerIter = curRegion->_aroundRegions[dirIdx]->_playerList.begin(); playerIter != curRegion->_aroundRegions[dirIdx]->_playerList.end(); ++playerIter)
			{
				int xDist = (x - (*playerIter)->_x) * xDirCoef;
				if (xDist >= 0 && xDist <= rangeX && abs(y - (*playerIter)->_y) <= rangeY)	// Damaged Player Found
				{
					*outDamagedPlayer = *playerIter;
					(*outDamagedPlayer)->_hp -= damage;

					if ((*outDamagedPlayer)->_hp <= 0)
					{
						(*outDamagedPlayer)->_status = ePlayerStatus::DEAD;
						_deadTPS++;

						DeactivateSession((*outDamagedPlayer)->_session, false);
					}

					return;
				}
			}

			int diagnalDirIdx = 0;
			if (player->_y - rangeY <= curRegion->_yMin)
			{
				if (xDirCoef == 1)
				{
					diagnalDirIdx = static_cast<int>(eMoveDir::LU);
				}
				else if (xDirCoef == -1)
				{
					diagnalDirIdx = static_cast<int>(eMoveDir::RU);
				}

			}
			else if (player->_y + rangeY >= curRegion->_yMax)
			{
				if (xDirCoef == 1)
				{
					diagnalDirIdx = static_cast<int>(eMoveDir::LD);
				}
				else if (xDirCoef == -1)
				{
					diagnalDirIdx = static_cast<int>(eMoveDir::RD);
				}
			}

			if (diagnalDirIdx != 0)
			{
				for (auto playerIter = curRegion->_aroundRegions[diagnalDirIdx]->_playerList.begin(); playerIter != curRegion->_aroundRegions[diagnalDirIdx]->_playerList.end(); ++playerIter)
				{
					int xDist = (x - (*playerIter)->_x) * xDirCoef;
					if (xDist >= 0 && xDist <= rangeX && abs(y - (*playerIter)->_y) <= rangeY)	// Damaged Player Found
					{
						*outDamagedPlayer = *playerIter;
						(*outDamagedPlayer)->_hp -= damage;

						if ((*outDamagedPlayer)->_hp <= 0)
						{
							(*outDamagedPlayer)->_status = ePlayerStatus::DEAD;
							_deadTPS++;

							DeactivateSession((*outDamagedPlayer)->_session, false);
						}

						return;
					}
				}
			}
		}

		{
			int dirIdx = 0;
			if (player->_y - rangeY <= curRegion->_yMin)
			{
				dirIdx = static_cast<int>(eMoveDir::UU);
			}
			else if (player->_y + rangeY >= curRegion->_yMax)
			{
				dirIdx = static_cast<int>(eMoveDir::DD);
			}

			for (auto playerIter = curRegion->_aroundRegions[dirIdx]->_playerList.begin(); playerIter != curRegion->_aroundRegions[dirIdx]->_playerList.end(); ++playerIter)
			{
				int xDist = (x - (*playerIter)->_x) * xDirCoef;
				if (xDist >= 0 && xDist <= rangeX && abs(y - (*playerIter)->_y) <= rangeY)	// Damaged Player Found
				{
					*outDamagedPlayer = *playerIter;
					(*outDamagedPlayer)->_hp -= damage;

					if ((*outDamagedPlayer)->_hp <= 0)
					{
						(*outDamagedPlayer)->_status = ePlayerStatus::DEAD;
						_deadTPS++;

						DeactivateSession((*outDamagedPlayer)->_session, false);
					}
					return;
				}
			}
		}
	}


	void Server::SendToPlayer(char* msg, const int size, Session* session)
	{
		//wprintf(L"# Send To %d Session %d Byte\n", session->_id, size);
		session->_sendBuf.Enqueue(msg, size);
	}
	
	void Server::SendToOneRegion(char* msg, const int size, Region* region, Session* excludedSession)
	{
		if (excludedSession == nullptr)
		{
			for (auto playerIter = region->_playerList.begin(); playerIter != region->_playerList.end(); ++playerIter)
			{
				(*playerIter)->_session->_sendBuf.Enqueue(msg, size);
			}
		}
		else
		{
			for (auto playerIter = region->_playerList.begin(); playerIter != region->_playerList.end(); ++playerIter)
			{
				if ((*playerIter)->_session != excludedSession)
				{
					(*playerIter)->_session->_sendBuf.Enqueue(msg, size);
				}
			}
		}
	}
	
	void Server::SendToAroundRegion(char* msg, const int size, Region* region, Session* excludedSession)
	{
		if (excludedSession == nullptr)
		{
			for (int regionIdx = 0; regionIdx < 9; ++regionIdx)
			{
				Region* curRegion = region->_aroundRegions[regionIdx];

				for (auto playerIter = curRegion->_playerList.begin(); playerIter != curRegion->_playerList.end(); ++playerIter)
				{
					(*playerIter)->_session->_sendBuf.Enqueue(msg, size);
				}
			}
		}
		else
		{
			for (int regionIdx = 0; regionIdx < 9; ++regionIdx)
			{
				Region* curRegion = region->_aroundRegions[regionIdx];

				for (auto playerIter = curRegion->_playerList.begin(); playerIter != curRegion->_playerList.end(); ++playerIter)
				{
					if ((*playerIter)->_session != excludedSession)
					{
						(*playerIter)->_session->_sendBuf.Enqueue(msg, size);
					}
				}
			}
		}
	}

	void Server::DeactivateSession(Session* session, const bool bGracefulShutdown)
	{
		if (session->_bRunning)
		{
			session->_bRunning = false;
			_deactivatedSessions[_deactivatedCnt++] = session;
			
			if (bGracefulShutdown)
			{
				_gracefulTPS++;
			}

			_disconnectTPS++;

			//wprintf(L"# Session %d is Deactivated\n", session->_id);
		}
	}
	
	void Server::ReleaseDeadSession(void)
	{
		for (int iCnt = 0; iCnt < _deactivatedCnt; ++iCnt)
		{
			Session* session = _deactivatedSessions[iCnt];
			SessionID id = session->_id;

			Player* player = _players[id];
			_players[id] = nullptr;

			Region* region = player->_region;

			for (auto iter = region->_playerList.begin(); iter != region->_playerList.end(); ++iter)
			{
				if ((*iter) == player)
				{
					region->_playerList.erase(iter);
					break;
				}
			}

			{
				int packetSize;

				session->_sendPacket.Clear();
				packetSize = SetPacketDeleteCharacter(&session->_sendPacket, player->_id);
				SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, region);
				_playerPool->Free(player);
			}

			{
				_sessions[id] = nullptr;
				closesocket(session->_sock);
				_sessionPool->Free(session);
				_returnedIds[_returnedIdCnt++] = id;
			}
		}

		_deactivatedCnt = 0;
	}

	void Server::CreatePlayer(Session* session)
	{
		Player* newPlayer = _playerPool->Alloc();
		newPlayer->Initialize(_playerIdProvider++, session);

		_players[session->_id] = newPlayer;
		SetRegion(newPlayer);

		//wprintf(L"# New Player is Created - x:%d, y:%d\n", newPlayer->_x, newPlayer->_y);

		{
			int packetSize;
			
			session->_sendPacket.Clear();
			packetSize = SetPacketCreateMyCharacter(&session->_sendPacket, newPlayer->_id, (BYTE)newPlayer->_dir, newPlayer->_x, newPlayer->_y, newPlayer->_hp);
			SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);
		}

		{
			int packetSize;

			session->_sendPacket.Clear();
			packetSize = SetPacketCreateOtherCharacter(&session->_sendPacket, newPlayer->_id, (BYTE)newPlayer->_dir, newPlayer->_x, newPlayer->_y, newPlayer->_hp);
			SendToAroundRegion(session->_sendPacket.GetBufferPtr(), packetSize, newPlayer->_region, session);
		}

		{
			for (int regionCnt = 0; regionCnt < 8; ++regionCnt)
			{
				Region* curRegion = newPlayer->_region->_aroundRegions[regionCnt];
				for (auto playerIter = curRegion->_playerList.begin(); playerIter != curRegion->_playerList.end(); ++playerIter)
				{
					Player* otherPlayer = (*playerIter);
					int packetSize;

					session->_sendPacket.Clear();
					packetSize = SetPacketCreateOtherCharacter(&session->_sendPacket, otherPlayer->_id, (BYTE)otherPlayer->_dir, otherPlayer->_x, otherPlayer->_y, otherPlayer->_hp);
					SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);

					if (otherPlayer->_status == ePlayerStatus::MOVE)
					{
						int packetSize;

						session->_sendPacket.Clear();
						packetSize = SetPacketMoveStart(&session->_sendPacket, otherPlayer->_id, (BYTE)otherPlayer->_moveDir, otherPlayer->_x, otherPlayer->_y);
						SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);
					}
				}
			}

			for (auto playerIter = newPlayer->_region->_playerList.begin(); playerIter != newPlayer->_region->_playerList.end(); ++playerIter)
			{
				Player* otherPlayer = (*playerIter);
				if (otherPlayer == newPlayer)
				{
					continue;
				}

				int packetSize;

				session->_sendPacket.Clear();
				packetSize = SetPacketCreateOtherCharacter(&session->_sendPacket, otherPlayer->_id, (BYTE)otherPlayer->_dir, otherPlayer->_x, otherPlayer->_y, otherPlayer->_hp);
				SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);

				if (otherPlayer->_status == ePlayerStatus::MOVE)
				{
					int packetSize;

					session->_sendPacket.Clear();
					packetSize = SetPacketMoveStart(&session->_sendPacket, otherPlayer->_id, (BYTE)otherPlayer->_moveDir, otherPlayer->_x, otherPlayer->_y);
					SendToPlayer(session->_sendPacket.GetBufferPtr(), packetSize, session);
				}
			}
		}
	}
	
	bool Server::IsMovable(short x, short y, eMoveDir dir)
	{
		short delta[8][2] = {
			{-1, 0},
			{-1, -1},
			{0, -1},
			{1, -1},
			{1, 0},
			{1, 1},
			{0, 1},
			{-1, 1}
		};
		
		int dirIdx = static_cast<int>(dir);
		
		short xPos = x + SPEED_PLAYER_X * delta[dirIdx][0];
		short yPos = y + SPEED_PLAYER_Y * delta[dirIdx][1];

		if (xPos < RANGE_MOVE_LEFT || xPos > RANGE_MOVE_RIGHT || yPos < RANGE_MOVE_TOP || yPos > RANGE_MOVE_BOTTOM)
		{
			return false;
		}

		return true;
	}

	void Server::UpdatePlayer(Player* player)
	{
		eMoveDir dir = static_cast<eMoveDir>(player->_moveDir);
		
		switch (dir)
		{
		case TCPFighter_Select_Server::eMoveDir::LL:
			if (IsMovable(player->_x, player->_y, dir))
			{
				player->_x -= SPEED_PLAYER_X;

				if (player->_x < player->_region->_xMin)
				{
					UpdateRegion(player, eMoveDir::LL);
				}
			}
			break;
		case TCPFighter_Select_Server::eMoveDir::LU:
			if (IsMovable(player->_x, player->_y, dir))
			{
				player->_x -= SPEED_PLAYER_X;
				player->_y -= SPEED_PLAYER_Y;

				if (player->_x < player->_region->_xMin && player->_y < player->_region->_yMin)
				{
					UpdateRegion(player, eMoveDir::LU);
				}
				else if (player->_x < player->_region->_xMin)
				{
					UpdateRegion(player, eMoveDir::LL);
				}
				else if (player->_y < player->_region->_yMin)
				{
					UpdateRegion(player, eMoveDir::UU);
				}
			}
			break;
		case TCPFighter_Select_Server::eMoveDir::UU:
			if (IsMovable(player->_x, player->_y, dir))
			{
				player->_y -= SPEED_PLAYER_Y;

				if (player->_y < player->_region->_yMin)
				{
					UpdateRegion(player, eMoveDir::UU);
				}
			}
			break;
		case TCPFighter_Select_Server::eMoveDir::RU:
			if (IsMovable(player->_x, player->_y, dir))
			{
				player->_x += SPEED_PLAYER_X;
				player->_y -= SPEED_PLAYER_Y;

				if (player->_x > player->_region->_xMax && player->_y < player->_region->_yMin)
				{
					UpdateRegion(player, eMoveDir::RU);
				}
				else if (player->_x > player->_region->_xMax)
				{
					UpdateRegion(player, eMoveDir::RR);
				}
				else if (player->_y < player->_region->_yMin)
				{
					UpdateRegion(player, eMoveDir::UU);
				}
			}
			break;
		case TCPFighter_Select_Server::eMoveDir::RR:
			if (IsMovable(player->_x, player->_y, dir))
			{
				player->_x += SPEED_PLAYER_X;

				if (player->_x > player->_region->_xMax)
				{
					UpdateRegion(player, eMoveDir::RR);
				}
			}
			break;
		case TCPFighter_Select_Server::eMoveDir::RD:
			if (IsMovable(player->_x, player->_y, dir))
			{
				player->_x += SPEED_PLAYER_X;
				player->_y += SPEED_PLAYER_Y;

				if (player->_x > player->_region->_xMax && player->_y > player->_region->_yMax)
				{
					UpdateRegion(player, eMoveDir::RD);
				}
				else if (player->_x > player->_region->_xMax)
				{
					UpdateRegion(player, eMoveDir::RR);
				}
				else if (player->_y > player->_region->_yMax)
				{
					UpdateRegion(player, eMoveDir::DD);
				}
			}
			break;
		case TCPFighter_Select_Server::eMoveDir::DD:
			if (IsMovable(player->_x, player->_y, dir))
			{
				player->_y += SPEED_PLAYER_Y;

				if (player->_y > player->_region->_yMax)
				{
					UpdateRegion(player, eMoveDir::DD);
				}
			}
			break;
		case TCPFighter_Select_Server::eMoveDir::LD:
			if (IsMovable(player->_x, player->_y, dir))
			{
				player->_x -= SPEED_PLAYER_X;
				player->_y += SPEED_PLAYER_Y;

				if (player->_x < player->_region->_xMin && player->_y > player->_region->_yMax)
				{
					UpdateRegion(player, eMoveDir::LD);
				}
				else if (player->_x < player->_region->_xMin)
				{
					UpdateRegion(player, eMoveDir::LL);
				}
				else if (player->_y > player->_region->_yMax)
				{
					UpdateRegion(player, eMoveDir::DD);
				}
			}
			break;
		default:
			break;
		}
		
		//wprintf(L"# Move Player - x:%d, y:%d, region(%d,%d)\n", player->_x, player->_y, player->_region->_x, player->_region->_y);
	}

	void Server::UpdateRegion(Player* player, eMoveDir dir)
	{
		Region* prevRegion = player->_region;

		for (auto playerIter = prevRegion->_playerList.begin(); playerIter != prevRegion->_playerList.end(); ++playerIter)
		{
			if ((*playerIter) == player)
			{
				prevRegion->_playerList.erase(playerIter);
				break;
			}
		}

		int dirIdx = static_cast<int>(dir);
		int regionCnt = _regionCnt[dirIdx];
		Region* curRegion = prevRegion->_aroundRegions[dirIdx];
		Region** newRegions = prevRegion->_newRegions[dirIdx];
		Region** oldRegions = prevRegion->_oldRegions[dirIdx];

		{
			int packetSize;
			
			player->_session->_sendPacket.Clear();
			packetSize = SetPacketCreateOtherCharacter(&player->_session->_sendPacket, player->_id, (BYTE)dir, player->_x, player->_y, player->_hp);
			
			for (int regionIdx = 0; regionIdx < regionCnt; ++regionIdx)
			{
				SendToOneRegion(player->_session->_sendPacket.GetBufferPtr(), packetSize, newRegions[regionIdx]);
			}
		}

		{
			int packetSize;

			player->_session->_sendPacket.Clear();
			packetSize = SetPacketMoveStart(&player->_session->_sendPacket, player->_id, (BYTE)dir, player->_x, player->_y);

			for (int regionIdx = 0; regionIdx < regionCnt; ++regionIdx)
			{
				SendToOneRegion(player->_session->_sendPacket.GetBufferPtr(), packetSize, newRegions[regionIdx]);
			}
		}

		{
			int packetSize;

			player->_session->_sendPacket.Clear();
			packetSize = SetPacketDeleteCharacter(&player->_session->_sendPacket, player->_id);

			for (int regionIdx = 0; regionIdx < regionCnt; ++regionIdx)
			{
				SendToOneRegion(player->_session->_sendPacket.GetBufferPtr(), packetSize, oldRegions[regionIdx]);
			}
		}

		for (int regionIdx = 0; regionIdx < regionCnt; ++regionIdx)
		{
			for (auto playerIter = newRegions[regionIdx]->_playerList.begin(); playerIter != newRegions[regionIdx]->_playerList.end(); ++playerIter)
			{
				Player* otherPlayer = (*playerIter);
				int packetSize;

				player->_session->_sendPacket.Clear();
				packetSize = SetPacketCreateOtherCharacter(&player->_session->_sendPacket, otherPlayer->_id, (BYTE)otherPlayer->_dir, otherPlayer->_x, otherPlayer->_y, otherPlayer->_hp);
				SendToPlayer(player->_session->_sendPacket.GetBufferPtr(), packetSize, player->_session);

				if (otherPlayer->_status == ePlayerStatus::MOVE)
				{
					int packetSize;

					player->_session->_sendPacket.Clear();
					packetSize = SetPacketMoveStart(&player->_session->_sendPacket, otherPlayer->_id, (BYTE)otherPlayer->_dir, otherPlayer->_x, otherPlayer->_y);
					SendToPlayer(player->_session->_sendPacket.GetBufferPtr(), packetSize, player->_session);
				}
			}
		}

		for (int regionIdx = 0; regionIdx < regionCnt; ++regionIdx)
		{
			for (auto playerIter = oldRegions[regionIdx]->_playerList.begin(); playerIter != oldRegions[regionIdx]->_playerList.end(); ++playerIter)
			{
				Player* otherPlayer = (*playerIter);
				int packetSize;

				player->_session->_sendPacket.Clear();
				packetSize = SetPacketDeleteCharacter(&player->_session->_sendPacket, otherPlayer->_id);
				SendToPlayer(player->_session->_sendPacket.GetBufferPtr(), packetSize, player->_session);
			}
		}

		player->_region = curRegion;
		curRegion->_playerList.push_back(player);

		return;
	}

	void Server::SetRegion(Player* player)
	{
		int x = (player->_x / REGION_SIZE_X) + 2;
		int y = (player->_y / REGION_SIZE_Y) + 2;
		_regionMap[y][x]._playerList.push_back(player);
		player->_region = &_regionMap[y][x];
	}

	void Server::RegionMapInitialize(void)
	{
		for (int yCnt = 2; yCnt < REGION_MAX_Y - 2; ++yCnt)
		{
			for (int xCnt = 2; xCnt < REGION_MAX_X - 2; ++xCnt)
			{
				Region* region = &_regionMap[yCnt][xCnt];

				region->_aroundRegions[0] = &_regionMap[yCnt][xCnt - 1];
				region->_aroundRegions[1] = &_regionMap[yCnt - 1][xCnt - 1];
				region->_aroundRegions[2] = &_regionMap[yCnt - 1][xCnt];
				region->_aroundRegions[3] = &_regionMap[yCnt - 1][xCnt + 1];
				region->_aroundRegions[4] = &_regionMap[yCnt][xCnt + 1];
				region->_aroundRegions[5] = &_regionMap[yCnt + 1][xCnt + 1];
				region->_aroundRegions[6] = &_regionMap[yCnt + 1][xCnt];
				region->_aroundRegions[7] = &_regionMap[yCnt + 1][xCnt - 1];
				region->_aroundRegions[8] = &_regionMap[yCnt][xCnt];

				region->_llNew[0] = &_regionMap[yCnt - 1][xCnt - 2];
				region->_llNew[1] = &_regionMap[yCnt][xCnt - 2];
				region->_llNew[2] = &_regionMap[yCnt + 1][xCnt - 2];

				region->_llOld[0] = &_regionMap[yCnt - 1][xCnt + 1];
				region->_llOld[1] = &_regionMap[yCnt][xCnt + 1];
				region->_llOld[2] = &_regionMap[yCnt + 1][xCnt + 1];

				region->_luNew[0] = &_regionMap[yCnt - 2][xCnt - 2];
				region->_luNew[1] = &_regionMap[yCnt - 2][xCnt - 1];
				region->_luNew[2] = &_regionMap[yCnt - 2][xCnt];
				region->_luNew[3] = &_regionMap[yCnt - 1][xCnt - 2];
				region->_luNew[4] = &_regionMap[yCnt][xCnt - 2];

				region->_luOld[0] = &_regionMap[yCnt - 1][xCnt + 1];
				region->_luOld[1] = &_regionMap[yCnt][xCnt + 1];
				region->_luOld[2] = &_regionMap[yCnt + 1][xCnt - 1];
				region->_luOld[3] = &_regionMap[yCnt + 1][xCnt];
				region->_luOld[4] = &_regionMap[yCnt + 1][xCnt + 1];

				region->_uuNew[0] = &_regionMap[yCnt - 2][xCnt - 1];
				region->_uuNew[1] = &_regionMap[yCnt - 2][xCnt];
				region->_uuNew[2] = &_regionMap[yCnt - 2][xCnt + 1];

				region->_uuOld[0] = &_regionMap[yCnt + 1][xCnt - 1];
				region->_uuOld[1] = &_regionMap[yCnt + 1][xCnt];
				region->_uuOld[2] = &_regionMap[yCnt + 1][xCnt + 1];

				region->_ruNew[0] = &_regionMap[yCnt - 2][xCnt];
				region->_ruNew[1] = &_regionMap[yCnt - 2][xCnt + 1];
				region->_ruNew[2] = &_regionMap[yCnt - 2][xCnt + 2];
				region->_ruNew[3] = &_regionMap[yCnt - 1][xCnt + 2];
				region->_ruNew[4] = &_regionMap[yCnt][xCnt + 2];

				region->_ruOld[0] = &_regionMap[yCnt - 1][xCnt - 1];
				region->_ruOld[1] = &_regionMap[yCnt][xCnt - 1];
				region->_ruOld[2] = &_regionMap[yCnt + 1][xCnt - 1];
				region->_ruOld[3] = &_regionMap[yCnt + 1][xCnt];
				region->_ruOld[4] = &_regionMap[yCnt + 1][xCnt + 1];

				region->_rrNew[0] = &_regionMap[yCnt - 1][xCnt + 2];
				region->_rrNew[1] = &_regionMap[yCnt][xCnt + 2];
				region->_rrNew[2] = &_regionMap[yCnt + 1][xCnt + 2];

				region->_rrOld[0] = &_regionMap[yCnt - 1][xCnt - 1];
				region->_rrOld[1] = &_regionMap[yCnt][xCnt - 1];
				region->_rrOld[2] = &_regionMap[yCnt + 1][xCnt - 1];

				region->_rdNew[0] = &_regionMap[yCnt + 2][xCnt + 2];
				region->_rdNew[1] = &_regionMap[yCnt + 2][xCnt + 1];
				region->_rdNew[2] = &_regionMap[yCnt + 2][xCnt];
				region->_rdNew[3] = &_regionMap[yCnt + 1][xCnt + 2];
				region->_rdNew[4] = &_regionMap[yCnt][xCnt + 2];

				region->_rdOld[0] = &_regionMap[yCnt - 1][xCnt - 1];
				region->_rdOld[1] = &_regionMap[yCnt - 1][xCnt];
				region->_rdOld[2] = &_regionMap[yCnt - 1][xCnt + 1];
				region->_rdOld[3] = &_regionMap[yCnt][xCnt - 1];
				region->_rdOld[4] = &_regionMap[yCnt + 1][xCnt - 1];

				region->_ddNew[0] = &_regionMap[yCnt + 2][xCnt - 1];
				region->_ddNew[1] = &_regionMap[yCnt + 2][xCnt];
				region->_ddNew[2] = &_regionMap[yCnt + 2][xCnt + 1];

				region->_ddOld[0] = &_regionMap[yCnt - 1][xCnt - 1];
				region->_ddOld[1] = &_regionMap[yCnt - 1][xCnt];
				region->_ddOld[2] = &_regionMap[yCnt - 1][xCnt + 1];

				region->_ldNew[0] = &_regionMap[yCnt][xCnt - 2];
				region->_ldNew[1] = &_regionMap[yCnt + 1][xCnt - 2];
				region->_ldNew[2] = &_regionMap[yCnt + 2][xCnt - 2];
				region->_ldNew[3] = &_regionMap[yCnt + 2][xCnt - 1];
				region->_ldNew[4] = &_regionMap[yCnt + 2][xCnt];

				region->_ldOld[0] = &_regionMap[yCnt - 1][xCnt - 1];
				region->_ldOld[1] = &_regionMap[yCnt - 1][xCnt];
				region->_ldOld[2] = &_regionMap[yCnt - 1][xCnt + 1];
				region->_ldOld[3] = &_regionMap[yCnt][xCnt + 1];
				region->_ldOld[4] = &_regionMap[yCnt + 1][xCnt + 1];
			}
		}
	}
}