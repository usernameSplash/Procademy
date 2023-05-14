#include "Packet.h"

void CreatePacketCreateMyCharacter(PacketHeader* pHeader, PacketSCCreateMyCharacter* pPacket, PlayerID id, BYTE dir, SHORT x, SHORT y, BYTE hp)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCCreateMyCharacter);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_CREATE_MY_CHARACTER;

	*pPacket = { id, dir, x, y, hp };
}

void CreatePacketCreateOtherCharacter(PacketHeader* pHeader, PacketSCCreateOtherCharacter* pPacket, PlayerID id, BYTE dir, SHORT x, SHORT y, BYTE hp)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCCreateOtherCharacter);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_CREATE_OTHER_CHARACTER;

	*pPacket = { id, dir, x, y, hp };
}

void CreatePacketDeleteCharacter(PacketHeader* pHeader, PacketSCDeleteCharacter* pPacket, PlayerID id)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCDeleteCharacter);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_DELETE_CHARACTER;

	*pPacket = { id };
}

void CreatePacketMoveStart(PacketHeader* pHeader, PacketSCMoveStart* pPacket, PlayerID id, BYTE dir, SHORT x, SHORT y)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCMoveStart);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_MOVE_START;

	*pPacket = { id, dir, x, y };
}

void CreatePacketMoveStop(PacketHeader* pHeader, PacketSCMoveStop* pPacket, PlayerID id, BYTE dir, SHORT x, SHORT y)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCMoveStop);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_MOVE_STOP;

	*pPacket = { id, dir, x, y };
}

void CreatePacketAttack1(PacketHeader* pHeader, PacketSCAttack1* pPacket, PlayerID id, BYTE dir, SHORT x, SHORT y)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCAttack1);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_ATTACK_1;

	*pPacket = { id, dir, x, y };
}

void CreatePacketAttack2(PacketHeader* pHeader, PacketSCAttack2* pPacket, PlayerID id, BYTE dir, SHORT x, SHORT y)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCAttack2);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_ATTACK_2;

	*pPacket = { id, dir, x, y };
}

void CreatePacketAttack3(PacketHeader* pHeader, PacketSCAttack3* pPacket, PlayerID id, BYTE dir, SHORT x, SHORT y)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCAttack3);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_ATTACK_3;

	*pPacket = { id, dir, x, y };
}

void CreatePacketDamage(PacketHeader* pHeader, PacketSCDamage* pPacket, PlayerID attackId, PlayerID damagedId, BYTE hp)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCDamage);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_DAMAGE;

	*pPacket = { attackId, damagedId, hp };
}

void CreatePacketSync(PacketHeader* pHeader, PacketSCSync* pPacket, PlayerID id, SHORT x, SHORT y)
{
	pHeader->code = 0x89;
	pHeader->size = sizeof(PacketSCSync);
	pHeader->type = (BYTE)ePacketType::PACKET_SC_SYNC;

	*pPacket = { id, x, y };
}
