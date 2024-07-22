#include "ChattingServer.h"

//#include <wchar.h>
#include <tchar.h>

ChattingServer::ChattingServer()
{

}

ChattingServer::~ChattingServer()
{
	Terminate();
}

bool ChattingServer::Initialize(void)
{
	_playerMap.reserve(PLAYER_MAX);
	_playerPool = new ObjectPool<Player>(PLAYER_MAX + 1);

	_jobQueue = new LockFreeQueue<Job*>;
	_jobPool = new ObjectPool<Job>(5000);

	for (int y = 0; y < REGION_Y_MAX; ++y)
	{
		for (int x = 0; x < REGION_X_MAX; ++x)
		{
			int deltaX[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
			int deltaY[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

			_regions[y][x]._x = x;
			_regions[y][x]._y = y;

			for (int dir = 0; dir < 8; ++dir)
			{
				if (x + deltaX[dir] < 0 || x + deltaX[dir] >= REGION_X_MAX)
				{
					continue;
				}
				if (y + deltaY[dir] < 0 || y + deltaY[dir] >= REGION_Y_MAX)
				{
					continue;
				}

				_regions[y][x]._aroundRegion.push_back(&_regions[y + deltaY[dir]][x + deltaX[dir]]);
			}
		}
	}

	_logicThread = (HANDLE)_beginthreadex(NULL, 0, LogicProc, this, 0, NULL);
	if (_logicThread == NULL)
	{
		return false;
	}


	bool initRet = ServerInitialize(SERVER_IP, SERVER_PORT, 10, true);
	if (initRet == false)
	{
		Terminate();
		return false;
	}

	_timeoutThread = (HANDLE)_beginthreadex(NULL, 0, TimeoutAlertProc, this, 0, NULL);
	if (_timeoutThread == NULL)
	{
		return false;
	}

	_monitorThread = (HANDLE)_beginthreadex(NULL, 0, MonitorProc, this, 0, NULL);
	if (_monitorThread == NULL)
	{
		return false;
	}

	wprintf(L"Chatting Server Initialize\n");

	return true;
}

void ChattingServer::Terminate(void)
{
	ServerTerminate();
	_isAlive = false;

	Job* terminateJob = _jobPool->Alloc();
	terminateJob->Initialize(eJobType::SYSTEM, eSystemJobType::TERMINATE, 0, NULL);
	_jobQueue->Enqueue(terminateJob);

	InterlockedIncrement(&_jobQueueSignal);
	WakeByAddressSingle(&_jobQueueSignal);

	WaitForSingleObject(_logicThread, INFINITE);
	WaitForSingleObject(_timeoutThread, INFINITE);
	WaitForSingleObject(_monitorThread, INFINITE);

	wprintf(L"Chatting Server Terminate\n");
}

void ChattingServer::OnInitialize(void)
{
	wprintf(L"Network Core Ready\n");
}

void ChattingServer::OnTerminate(void)
{
	wprintf(L"Network Core is Terminated\n");
}

bool ChattingServer::OnConnectRequest(const wchar_t* ip, const short port)
{
	// Verify Connect Request
	return true;
}

void ChattingServer::OnAccept(const SessionID sessionId)
{
	Job* acceptJob = _jobPool->Alloc();
	acceptJob->Initialize(eJobType::SYSTEM, eSystemJobType::ACCEPT, sessionId, nullptr);
	_jobQueue->Enqueue(acceptJob);

	InterlockedIncrement(&_jobQueueSignal);
	WakeByAddressSingle(&_jobQueueSignal);
}

void ChattingServer::OnRelease(const SessionID sessionId)
{
	Job* releaseJob = _jobPool->Alloc();
	releaseJob->Initialize(eJobType::SYSTEM, eSystemJobType::RELEASE, sessionId, nullptr);
	_jobQueue->Enqueue(releaseJob);

	InterlockedIncrement(&_jobQueueSignal);
	WakeByAddressSingle(&_jobQueueSignal);
}

void ChattingServer::OnRecv(const SessionID sessionId, SPacket* packet)
{
	packet->AddUseCnt(1);
	
	PacketHeader header;
	packet->GetHeaderData(&header);

	Job* recvJob = _jobPool->Alloc();
	recvJob->Initialize(eJobType::CONTENTS, eSystemJobType::INVALID, sessionId, packet);
	_jobQueue->Enqueue(recvJob);

	short* ptr = (short*)recvJob->_packet->GetPayloadPtr();
	wprintf(L"[TYPE] OnRecv sessionID : %lld, type : %d, packet ptr : %p, packet capacity, size : %d, %d\n", GET_PTR(sessionId), *ptr, recvJob->_packet, recvJob->_packet->Capacity(), recvJob->_packet->Size());

	InterlockedIncrement(&_jobQueueSignal);
	WakeByAddressSingle(&_jobQueueSignal);
}

//void ChattingServer::OnSend(const SessionID sessionId, const int sentByte)
//{
//
//}

bool ChattingServer::AcceptHandler(const SessionID sessionId)
{
	if (_playerMap.size() >= PLAYER_MAX)
	{
		return false;
	}

	Player* newPlayer = _playerPool->Alloc();
	newPlayer->_sessionId = sessionId;
	newPlayer->_playerId = _playerIdProvider++;

	auto insertRet = _playerMap.insert(make_pair(sessionId, newPlayer));
	if (insertRet.second == false)
	{
		// Duplicated Accept
		return false;
	}

	return true;
}

void ChattingServer::ReleaseHandler(const SessionID sessionId)
{
	// Remove Player in PlayerMap
	auto iter = _playerMap.find(sessionId);
	if (iter == _playerMap.end())
	{
		return;
	}
	
	Player* player = iter->second;
	_playerMap.erase(iter);

	if (player->_regionX >= 0 && player->_regionX < REGION_X_MAX && player->_regionY >= 0 && player->_regionY < REGION_Y_MAX)
	{
		Region region = _regions[player->_regionY][player->_regionX];
		for (auto it = region._players.begin(); it != region._players.end(); ++it)
		{
			if (*it == player)
			{
				region._players.erase(it);
				break;
			}
		}
	}

	_playerPool->Free(player);
}

void ChattingServer::TimeoutHandler(void)
{
	// Check Players' Last Recv Time
	for (auto it = _playerMap.begin(); it != _playerMap.end();)
	{
		Player* player = it->second;

		if (GetTickCount64() - player->_lastRecvTime >= TIMEOUT)
		{
			Disconnect(player->_sessionId);
			it = _playerMap.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ChattingServer::RecvPacketHandler(const SessionID sessionId, SPacket* packet)
{
	auto iter = _playerMap.find(sessionId);
	if (iter == _playerMap.end())
	{
		Disconnect(iter->second->_sessionId);
		return;
	}

	Player* player = iter->second;
	player->_lastRecvTime = GetTickCount64();

	short type;
	(*packet) >> type;
	
	switch (type)
	{
	case ePacketType::PACKET_CS_CHAT_REQ_LOGIN:
		HandlePacketLogin(packet, player);
		break;
	case ePacketType::PACKET_CS_CHAT_REQ_REGION_MOVE:
		HandlePacketMove(packet, player);
		break;
	case ePacketType::PACKET_CS_CHAT_REQ_MESSAGE:
		HandlePacketChat(packet, player);
		break;
	case ePacketType::PACKET_CS_CHAT_REQ_HEARTBEAT:
		break;
	default:
		Disconnect(player->_sessionId);
		break;
	}

	_packetHandleTPS++;
}

unsigned int WINAPI ChattingServer::LogicProc(void* arg)
{
	ChattingServer* instance = (ChattingServer*)arg;

	long compareValue = 0;

	wprintf(L"Logic Proc Start\n");

	while (instance->_isAlive)
	{
		WaitOnAddress(&instance->_jobQueueSignal, &compareValue, sizeof(long), INFINITE);

		while (instance->_jobQueue->Size() > 0)
		{
			Job* job = instance->_jobQueue->Dequeue();

			if (job->_jobType == eJobType::SYSTEM)
			{
				if (job->_systemJobType == eSystemJobType::ACCEPT)
				{
					instance->AcceptHandler(job->_sessionId);
				}
				else if (job->_systemJobType == eSystemJobType::RELEASE)
				{
					instance->ReleaseHandler(job->_sessionId);
				}
				else if (job->_systemJobType == eSystemJobType::TIMEOUT)
				{
					//instance->TimeoutHandler();
				}
				else if (job->_systemJobType == eSystemJobType::TERMINATE)
				{
					break;
				}
			}
			else if (job->_jobType == eJobType::CONTENTS)
			{
				short* ptr = (short*)job->_packet->GetPayloadPtr();
				wprintf(L"[TYPE] LogicProc sessionID : %lld, type : %d, packet ptr : %p\n", GET_PTR(job->_sessionId), *ptr, job->_packet);

				instance->RecvPacketHandler(job->_sessionId, job->_packet);
				SPacket::Free(job->_packet);
			}

			instance->_jobPool->Free(job);
			InterlockedDecrement(&instance->_jobQueueSignal);
		}

		instance->_updateTPS++;
	}

	return 0;
}

unsigned int WINAPI ChattingServer::TimeoutAlertProc(void* arg)
{
	ChattingServer* instance = (ChattingServer*)arg;

	while (instance->_isAlive)
	{
		Sleep(TIMEOUT);

		Job* timeoutAlertJob = instance->_jobPool->Alloc();
		timeoutAlertJob->Initialize(eJobType::SYSTEM, eSystemJobType::TIMEOUT, 0, nullptr);
		instance->_jobQueue->Enqueue(timeoutAlertJob);

		InterlockedIncrement(&instance->_jobQueueSignal);
		WakeByAddressSingle(&instance->_jobQueueSignal);

	}

	return 0;
}

unsigned int WINAPI ChattingServer::MonitorProc(void* arg)
{
	ChattingServer* instance = (ChattingServer*)arg;

	while (instance->_isAlive)
	{
		instance->UpdateMonitorData();

		SYSTEMTIME time;
		GetLocalTime(&time);

		wchar_t text[1200];
		__int64 jobQueueSize = instance->_jobQueue->Size();

		//swprintf(text, L"[%s %02d:%02d:%02d]\n\nConnected Session : %d\n\nJob Queue Size : %lld\nPacket Pool Size : %lld\nPacket Node Count : %lld\nJob Pool Size : %lld\nJob Node Count : %lld\nPlayer Map Size : %zu\nPlayer Pool Size : %lld\nPlayer Node Count : %lld\n\nAccept Total : %d\nDisconnect Total : %d\n\nAccept TPS : %d\nDisconnect TPS : %d\n Receive TPS : %d\n SendTPS : %d\n\nUpdate TPS : %d\nPacket Handle TPS : %d\n\n",
		//	_T(__DATE__), time.wHour, time.wMinute, time.wSecond,
		//	instance->GetSessionCnt(), jobQueueSize,
		//	SPacket::PoolBlockSize(), SPacket::PoolNodeSize(),
		//	instance->_jobPool->Size(), instance->_jobPool->NodeCount(),
		//	instance->_playerMap.size(), instance->_playerPool->Size(), instance->_playerPool->NodeCount(),
		//	instance->GetAcceptTotal(), instance->GetDisconnectTotal(),
		//	instance->GetAcceptTPS(), instance->GetDisconnectTPS(), instance->GetRecvTPS(), instance->GetSendTPS(),
		//	instance->_updateTPS, instance->_packetHandleTPS
		//);

		//fwprintf(stdout, text);
		
		instance->_updateTPS = 0;
		instance->_packetHandleTPS = 0;

		Sleep(1000);
	}

	return 0;
}

void ChattingServer::HandlePacketLogin(SPacket* packet, Player* player)
{
	__int64 accountNumber;
	wchar_t id[ID_LEN_MAX];
	wchar_t nickName[NICKNAME_LEN_MAX];
	char sessionKey[SESSION_KEY_LEN_MAX];

	DemashallPacketLogin(packet, &accountNumber, id, nickName, sessionKey);

	player->_accountNumber = accountNumber;
	memcpy(player->_id, id, sizeof(wchar_t) * ID_LEN_MAX);
	memcpy(player->_nickname, nickName, sizeof(wchar_t) * NICKNAME_LEN_MAX);
	memcpy(player->_sessionKey, sessionKey, sizeof(char) * SESSION_KEY_LEN_MAX);

	BYTE status = 1;
	SPacket* resPacket = SPacket::Alloc();

	MashallPacketLogin(resPacket, status, accountNumber);

	SendUnicast(resPacket, player->_sessionId);
}

void ChattingServer::HandlePacketMove(SPacket* packet, Player* player)
{
	__int64 accountNumber;
	short x;
	short y;

	DemashallPacketMove(packet, &accountNumber, &x, &y);

	if (player->_accountNumber != accountNumber)
	{
		Disconnect(player->_sessionId);
		return;
	}

	if (x < 0 || x >= REGION_X_MAX || y < 0 || y >= REGION_Y_MAX)
	{
		Disconnect(player->_sessionId);

		Region* region = &_regions[player->_regionY][player->_regionX];
		for (auto it = region->_players.begin(); it != region->_players.end(); ++it)
		{
			if (*it == player)
			{
				region->_players.erase(it);
				break;
			}
		}
		return;
	}

	if (player->_regionX >= 0 && player->_regionX < REGION_X_MAX && player->_regionY >= 0 && player->_regionY < REGION_Y_MAX)
	{
		Region* oldRegion = &_regions[player->_regionY][player->_regionX];
		for (auto it = oldRegion->_players.begin(); it != oldRegion->_players.end(); ++it)
		{
			if (*it == player)
			{
				oldRegion->_players.erase(it);
				break;
			}
		}
	}

	player->_regionX = x;
	player->_regionY = y;

	_regions[y][x]._players.push_back(player);

	SPacket* resPacket = SPacket::Alloc();
	MashallPacketMove(resPacket, accountNumber, x, y);

	SendUnicast(resPacket, player->_sessionId);
}

void ChattingServer::HandlePacketChat(SPacket* packet, Player* player)
{
	__int64 accountNumber;
	short msgLen;
	wchar_t* msg;

	DemashallPacketChat(packet, &accountNumber, &msgLen, &msg);

	if (player->_accountNumber != accountNumber)
	{
		Disconnect(player->_sessionId);
		return;
	}
	
	SPacket* resPacket = SPacket::Alloc();
	MashallPacketChat(resPacket, accountNumber, player->_id, player->_nickname, msgLen, msg);

	SendMulticastAroundRegion(resPacket, _regions[player->_regionY][player->_regionX]);
	
	delete[] msg;

	return;
}

void ChattingServer::MashallPacketLogin(SPacket* packet, const unsigned char status, const __int64 accountNumber)
{
	short type = ePacketType::PACKET_CS_CHAT_RES_LOGIN;

	(*packet) << type << status << accountNumber;

	return;
}

void ChattingServer::MashallPacketMove(SPacket* packet, const __int64 accountNumber, const short x, const short y)
{
	short type = ePacketType::PACKET_CS_CHAT_RES_REGION_MOVE;

	(*packet) << type << accountNumber << x << y;

	return;
}

void ChattingServer::MashallPacketChat(SPacket* packet, const __int64 accountNumber, wchar_t* id, wchar_t* nickname, const short textLen, wchar_t* text)
{
	short type = ePacketType::PACKET_CS_CHAT_RES_MESSAGE;

	(*packet) << type << accountNumber;

	packet->SetPayloadData(id, sizeof(wchar_t) * ID_LEN_MAX);
	packet->SetPayloadData(nickname, sizeof(wchar_t) * NICKNAME_LEN_MAX);
	(*packet) << textLen;
	packet->SetPayloadData(text, textLen);

	return;
}

void ChattingServer::DemashallPacketLogin(SPacket* packet, __int64* outAccountNumber, wchar_t* outId, wchar_t* outNickname, char* outSessionKey)
{
	(*packet) >> *outAccountNumber;
	packet->GetPayloadData(outId, sizeof(wchar_t) * ID_LEN_MAX);
	packet->GetPayloadData(outNickname, sizeof(wchar_t) * NICKNAME_LEN_MAX);
	packet->GetPayloadData(outSessionKey, sizeof(char) * SESSION_KEY_LEN_MAX);

	return;
}

void ChattingServer::DemashallPacketMove(SPacket* packet, __int64* outAccountNumber, short* outX, short* outY)
{
	(*packet) >> *outAccountNumber >> *outX >> *outY;

	return;
}

void ChattingServer::DemashallPacketChat(SPacket* packet, __int64* outAccountNumber, short* outMsgLen, wchar_t** outMessage)
{
	(*packet) >> *outAccountNumber >> *outMsgLen;

	(*outMessage) = new wchar_t[*outMsgLen / 2];
	packet->GetPayloadData(*outMessage, *outMsgLen);

	return;
}

void ChattingServer::SendUnicast(SPacket* packet, const SessionID sessionId)
{
	packet->AddUseCnt(1);
	bool sendRet = SendPacket(sessionId, packet);

	if (sendRet == false)
	{
		SPacket::Free(packet);
	}

	return;
}

void ChattingServer::SendMulticastAroundRegion(SPacket* packet, const Region& centerRegion, int excludePlayerNum, Player* excludePlayer)
{
	SessionID sendSessions[SESSION_MAX];
	int sendCnt = 0;

	if (excludePlayer == nullptr)
	{
		auto aroundIter = centerRegion._aroundRegion.begin();

		for (; aroundIter != centerRegion._aroundRegion.end(); ++aroundIter)
		{
			auto playerIter = (*aroundIter)->_players.begin();
			for (; playerIter != (*aroundIter)->_players.end(); ++playerIter)
			{
				sendSessions[sendCnt] = (*playerIter)->_sessionId;
				sendCnt++;
			}
		}

		for (auto playerIter = centerRegion._players.begin(); playerIter != centerRegion._players.end(); ++playerIter)
		{
			sendSessions[sendCnt] = (*playerIter)->_sessionId;
			sendCnt++;
		}
	}
	else
	{
		auto aroundIter = centerRegion._aroundRegion.begin();

		for (; aroundIter != centerRegion._aroundRegion.end(); ++aroundIter)
		{
			auto playerIter = (*aroundIter)->_players.begin();
			for (; playerIter != (*aroundIter)->_players.end(); ++playerIter)
			{
				sendSessions[sendCnt] = (*playerIter)->_sessionId;
				sendCnt++;
			}
		}

		for (auto playerIter = centerRegion._players.begin(); playerIter != centerRegion._players.end(); ++playerIter)
		{
			if (*playerIter == excludePlayer)
			{
				// Incomplete Function (excluding players)
				continue;
			}
			sendSessions[sendCnt] = (*playerIter)->_sessionId;
			sendCnt++;
		}
	}

	packet->AddUseCnt(sendCnt);

	for (int iCnt = 0; iCnt < sendCnt; ++iCnt)
	{
		bool sendRet = SendPacket(sendSessions[iCnt], packet);
		if (sendRet == false)
		{
			SPacket::Free(packet);
		}
	}

	return;
}