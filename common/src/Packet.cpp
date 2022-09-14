#include "Packet.h"


sts::Packet::Packet() {}

sts::Packet::Packet(PacketType type) : type(type){}

sts::Packet::~Packet(){}

sf::Packet& sts::operator <<(sf::Packet& packet, const sts::Packet& p) 
{
	const auto type = static_cast<int>(p.type);
	return packet << type;
}

sf::Packet& sts::operator >>(sf::Packet& packet, sts::Packet& p) 
{
	int type = 0;
	packet >> type;
	p.type = static_cast<sts::PacketType>(type);
	return packet;
}