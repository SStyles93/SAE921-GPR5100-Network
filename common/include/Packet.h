#pragma once
#include <SFML/Network.hpp>
#include "PlayerAction.h"

namespace sts 
{
	enum class PacketType : int
	{
		NONE = 0,
		INIT,
		GAME,
		END
	};

	class Packet
	{
	public:
		Packet();
		Packet(PacketType type);

		PacketType type = PacketType::NONE;
	};
	sf::Packet& operator <<(sf::Packet& packet, const Packet& p);
	sf::Packet& operator >>(sf::Packet& packet, Packet& p);

	class GamePacket : public Packet 
	{
	public:
		GamePacket();
		GamePacket(sts::PlayerAction);

		sts::PlayerAction action = sts::PlayerAction::HAND;
	};
	sf::Packet& operator <<(sf::Packet& packet, const GamePacket& p);
	sf::Packet& operator >>(sf::Packet& packet, GamePacket& p);

}