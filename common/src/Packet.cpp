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

sts::GamePacket::GamePacket(PlayerAction action) : action(action) {}


sf::Packet& sts::operator <<(sf::Packet& packet, const sts::GamePacket& p) 
{
	const auto action = static_cast<int>(p.action);
	return packet << action;
}
sf::Packet& sts::operator >>(sf::Packet& packet, sts::GamePacket& p) 
{
	int action = 0;
	packet >> action;
	p.action = static_cast<sts::PlayerAction>(action);
	return packet;
}

sts::EndPacket::EndPacket() {}

sts::EndPacket::EndPacket(sts::Result state) : result(state){}

sf::Packet& sts::operator <<(sf::Packet& packet, const sts::EndPacket& p) 
{
	const auto state = static_cast<int>(p.result);
	return packet << state;

}
sf::Packet& sts::operator >>(sf::Packet& packet, sts::EndPacket& p) 
{
	int state = 0;
	packet >> state;
	p.result = static_cast<sts::Result>(state);
	return packet;
}
