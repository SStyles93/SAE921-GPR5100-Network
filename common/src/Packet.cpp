#include "Packet.h"


sts::Packet::Packet() {}

sts::Packet::Packet(PacketType type) : type(type){}

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

sts::GamePacket::GamePacket() {}

sts::GamePacket::GamePacket(sts::PlayerAction action) : action(action) {}


sf::Packet& operator <<(sf::Packet& packet, const sts::GamePacket& p) 
{

}
sf::Packet& operator >>(sf::Packet& packet, sts::GamePacket& p) 
{

}