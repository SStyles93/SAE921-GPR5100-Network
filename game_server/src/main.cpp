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
	sf::Packet statePacket;
	sts::GamePacket gamePacket;
	sts::EndPacket endPacket;
	sts::PlayerAction p1Action = sts::PlayerAction::NONE;
	sts::PlayerAction p2Action = sts::PlayerAction::NONE;
	sts::Result p1Result = sts::Result::NONE;
	sts::Result p2Result = sts::Result::NONE;
	int actionCount = 0;
	int returnCount = 0;
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

						sts::Packet initStatePacket(sts::PacketType::INIT);
						statePacket << initStatePacket;

						if (clients[0].send(statePacket) != sf::Socket::Done)
						{
							std::cout << "Packet not received from" << clients[0].getRemotePort() << std::endl;
						}
						else 
						{
							std::cout << "Server PLAYER_MOVE sent\n";
						}

						statePacket.clear();
						statePacket << initStatePacket;
						if (clients[1].send(statePacket) != sf::Socket::Done)
						{
							std::cout << "Packet not received from" << clients[1].getRemotePort() << std::endl;
						}
						else
						{
							std::cout << "Server PLAYER_MOVE sent\n";
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
						if (clients[i].receive(packet) == sf::Socket::Done)
						{
							packet >> gamePacket;
							p1Action = gamePacket.action;
							actionCount++;
						
							std::cout << "Server PLAYER_MOVE received\n";
						
						}
					}
					else if (i == 1)
					{
						if (clients[i].receive(packet) == sf::Socket::Done)
						{
							packet >> gamePacket;
							p2Action = gamePacket.action;
							actionCount++;

							std::cout << "Server PLAYER_MOVE received\n";

						}
					}
				}
				if (actionCount >= 2)
				{
					serverState = sts::ServerState::RESOLVING;
					actionCount = 0;

					statePacket.clear();
					sts::Packet gameStatePacket(sts::PacketType::GAME);
					statePacket << gameStatePacket;

					if (clients[0].send(statePacket) != sf::Socket::Done)
					{
						std::cout << "Packet not received from" << clients[0].getRemotePort() << std::endl;
					}
					else
					{
						std::cout << "Server RESOLVING sent\n";
					}

					statePacket.clear();
					statePacket << gameStatePacket;
					if (clients[1].send(statePacket) != sf::Socket::Done)
					{
						std::cout << "Packet not received from" << clients[1].getRemotePort() << std::endl;
					}
					else
					{
						std::cout << "Server RESOLVING sent\n";
					}
				}
				
				
				break;

			case sts::ServerState::RESOLVING:


				for (std::size_t i = 0; i < clients.size(); i++)
				{
					if (!selector.isReady(clients[i])) continue;

					if (i == 0)
					{
						if (clients[i].receive(packet) == sf::Socket::Done)
						{
							packet.clear();
							actionCount++;
							std::cout << "Server RESOLVING received\n";
						}
					}
					else if (i == 1)
					{
						if (clients[i].receive(packet) == sf::Socket::Done)
						{
							packet.clear();
							actionCount++;
							std::cout << "Server RESOLVING received\n";

						}
					}
				}
				if (actionCount >= 2) 
				{
					//RESOLVE THE ACTIONS
					p1Result = sts::Result::DRAW;
					p2Result = sts::Result::DRAW;
					std::cout << "Server RESOLUTION\n";
					serverState = sts::ServerState::END;

					
					packet.clear();
					endPacket.result = p1Result;
					packet << endPacket;
					if (clients[0].send(packet) == sf::Socket::Done)
					{
						std::cout << "Server END sent\n";
					}
						
					packet.clear();
					endPacket.result = p2Result;
					packet << endPacket;
					if (clients[1].send(packet) == sf::Socket::Done)
					{
						std::cout << "Server END sent\n";
					}
				}
				break;

			case sts::ServerState::END:

				break;
			}
		}

	}
	std::cout << "Server has stopped working" << std::endl;
	return EXIT_SUCCESS;
}