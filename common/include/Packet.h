#pragma once
#include <SFML/Network.hpp>

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
		~Packet();

		PacketType type = PacketType::NONE;
	};

	sf::Packet& operator <<(sf::Packet& packet, const Packet& p);
	sf::Packet& operator >>(sf::Packet& packet, Packet& p);
}