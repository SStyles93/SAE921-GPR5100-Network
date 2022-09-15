#include <iostream>
#include <SFML/Network.hpp>
#include <array>
#include "Packet.h"
#include "ServerState.h"

//Game_Server
int main()
{
	//Listener listens to incomming connections
	sf::TcpListener listener;
	//The port to listen
	listener.listen(12345);
	//Clients
	std::array<sf::TcpSocket, 2> clients{};
	//Selector
	sf::SocketSelector selector;
	//Add the listener to the selector
	selector.add(listener);
	//Client count to assign IDs to clients
	int clientCount = 0;
	//State of the server at the begining
	auto serverState = sts::ServerState::WAITING_FOR_PLAYERS;

	sf::Packet packet;
	sts::Packet initPacket(sts::PacketType::INIT);
	sts::Packet gamePacket(sts::PacketType::GAME);
	sts::Packet endPacket(sts::PacketType::END);
	sts::PlayerAction p1Action;
	sts::PlayerAction p2Action;
	int actionCount = 0;
	bool hasWon = false;

	while (true)
	{
		// Make the selector wait for data on any socket
		if (selector.wait(sf::seconds(10.0f)))
		{
			switch (serverState)
			{
			case sts::ServerState::WAITING_FOR_PLAYERS:
				// Test the listener
				if (selector.isReady(listener))
				{
					// The listener is ready: there is a pending connection
					if (listener.accept(clients[0]) == sf::Socket::Done)
					{
						// Add the new client to the selector so that we will
						// be notified when he sends something
						selector.add(clients[0]);
						clientCount++;
						std::cout << "Connection established with: " << clients[0].getRemoteAddress() << " as clients[0]." << std::endl;
					}
					if (listener.accept(clients[1]) == sf::Socket::Done)
					{
						selector.add(clients[1]);
						clientCount++;
						std::cout << "Connection established with: " << clients[1].getRemoteAddress() << " as clients[1]." << std::endl;
					}

					if (clientCount >= 2) {
						serverState = sts::ServerState::PLAYERS_MOVE;
						sf::Packet startGamePacket;
						sts::Packet initPacket(sts::PacketType::INIT);
						
						startGamePacket << initPacket;
						if (clients[0].send(startGamePacket) != sf::Socket::Done)
						{
							std::cout << "Packet not received from" << clients[0].getRemotePort() << std::endl;
						}
						
						startGamePacket.clear();
						startGamePacket << initPacket;
						if (clients[1].send(startGamePacket) != sf::Socket::Done)
						{
							std::cout << "Packet not received from" << clients[1].getRemotePort() << std::endl;
						}
					}
				}
				break;

			case sts::ServerState::PLAYERS_MOVE:
				for (std::size_t i = 0; i < clients.size(); i++)
				{
					if (!selector.isReady(clients[i])) continue;

					if (i == 0)
					{
						//clients[0].receive() == sf::Socket::Done;// ...
						//clients[1].send();
						//p1Action = action;
						actionCount++;
					}
					else if (i == 1)
					{
						//clients[1].receive();// ...
						//clients[0].send();
						//p2Action = action;
						actionCount++;
					}
				}
				if (actionCount >= 2) {
					serverState = sts::ServerState::RESOLVING;
				}
				break;
			case sts::ServerState::RESOLVING:
				//SOLVER
				break;
			case sts::ServerState::END:
				//END
				break;
			}
		}

		std::cout << "Server has stopped working" << std::endl;
		return EXIT_SUCCESS;
	}