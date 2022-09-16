#pragma once
#include <SFML/Network.hpp>
#include "PlayerAction.h"
#include "Result.h"

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
		GamePacket(PlayerAction action);

		sts::PlayerAction action = sts::PlayerAction::NONE;
	};
	sf::Packet& operator <<(sf::Packet& packet, const GamePacket& p);
	sf::Packet& operator >>(sf::Packet& packet, GamePacket& p);

	class EndPacket : public Packet
	{
	public:
		EndPacket();
		EndPacket(sts::Result result);

		sts::Result result = sts::Result::NONE;
	};
	sf::Packet& operator <<(sf::Packet& packet, const EndPacket& p);
	sf::Packet& operator >>(sf::Packet& packet, EndPacket& p);
}