#pragma once
#include <SFML/Network.hpp>

namespace sts 
{
	enum class PacketType : int
	{
		INIT = 0,
		GAME,
		END
	};

	class Packet
	{
	public:
		Packet(PacketType type);
		~Packet();

		PacketType type = PacketType::INIT;
	};

	sf::Packet& operator <<(sf::Packet& packet, const Packet& p);
	sf::Packet& operator >>(sf::Packet& packet, Packet& p);
}