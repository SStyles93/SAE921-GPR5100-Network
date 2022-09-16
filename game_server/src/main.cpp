#include <iostream>
#include <SFML/Network.hpp>
#include <array>
#include "Packet.h"
#include "ServerState.h"

void GameSolver(sts::PlayerAction player1Action, sts::PlayerAction player2Action, sts::Result& player1Result, sts::Result& player2Result) 
{
	switch (player1Action) 
	{
	case sts::PlayerAction::ROCK:
		switch (player2Action)
		{
		case sts::PlayerAction::ROCK:
			player1Result = sts::Result::DRAW;
			player2Result = sts::Result::DRAW;
			break;
		case sts::PlayerAction::PAPER:
			player1Result = sts::Result::LOST;
			player2Result = sts::Result::WON;
			break;
		case sts::PlayerAction::CISORS:
			player1Result = sts::Result::WON;
			player2Result = sts::Result::LOST;
			break;
		case sts::PlayerAction::HAND:
			player1Result = sts::Result::LOST;
			player2Result = sts::Result::WON;
			break;
		case sts::PlayerAction::STUMP:
			player1Result = sts::Result::WON;
			player2Result = sts::Result::LOST;
			break;
		default:
			break;
		}
	case sts::PlayerAction::PAPER:
		switch (player2Action)
		{
		case sts::PlayerAction::ROCK:
			player1Result = sts::Result::WON;
			player2Result = sts::Result::LOST;
			break;
		case sts::PlayerAction::PAPER:
			player1Result = sts::Result::DRAW;
			player2Result = sts::Result::DRAW;
			break;
		case sts::PlayerAction::CISORS:
			player1Result = sts::Result::LOST;
			player2Result = sts::Result::WON;
			break;
		case sts::PlayerAction::HAND:
			player1Result = sts::Result::LOST;
			player2Result = sts::Result::WON;
			break;
		case sts::PlayerAction::STUMP:
			player1Result = sts::Result::WON;
			player2Result = sts::Result::LOST;
			break;
		default:
			break;
		}
	case sts::PlayerAction::CISORS:
		switch (player2Action)
		{
		case sts::PlayerAction::ROCK:
			player1Result = sts::Result::LOST;
			player2Result = sts::Result::WON;
			break;
		case sts::PlayerAction::PAPER:
			player1Result = sts::Result::WON;
			player2Result = sts::Result::LOST;
			break;
		case sts::PlayerAction::CISORS:
			player1Result = sts::Result::DRAW;
			player2Result = sts::Result::DRAW;
			break;
		case sts::PlayerAction::HAND:
			player1Result = sts::Result::WON;
			player2Result = sts::Result::LOST;
			break;
		case sts::PlayerAction::STUMP:
			player1Result = sts::Result::LOST;
			player2Result = sts::Result::WON;
			break;
		default:
			break;
		}
	case sts::PlayerAction::HAND:
		switch (player2Action)
		{
		case sts::PlayerAction::ROCK:
			player1Result = sts::Result::WON;
			player2Result = sts::Result::LOST;
			break;
		case sts::PlayerAction::PAPER:
			player1Result = sts::Result::WON;
			player2Result = sts::Result::LOST;
			break;
		case sts::PlayerAction::CISORS:
			player1Result = sts::Result::LOST;
			player2Result = sts::Result::WON;
			break;
		case sts::PlayerAction::HAND:
			player1Result = sts::Result::DRAW;
			player2Result = sts::Result::DRAW;
			break;
		case sts::PlayerAction::STUMP:
			player1Result = sts::Result::DRAW;
			player2Result = sts::Result::DRAW;
			break;
		default:
			break;
		}
	case sts::PlayerAction::STUMP:
		switch (player2Action)
		{
		case sts::PlayerAction::ROCK:
			player1Result = sts::Result::LOST;
			player2Result = sts::Result::WON;
			break;
		case sts::PlayerAction::PAPER:
			player1Result = sts::Result::LOST;
			player2Result = sts::Result::WON;
			break;
		case sts::PlayerAction::CISORS:
			player1Result = sts::Result::WON;
			player2Result = sts::Result::LOST;
			break;
		case sts::PlayerAction::HAND:
			player1Result = sts::Result::DRAW;
			player2Result = sts::Result::DRAW;
			break;
		case sts::PlayerAction::STUMP:
			player1Result = sts::Result::DRAW;
			player2Result = sts::Result::DRAW;
			break;
		default:
			break;
		}
	}
}

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
	int receptionCount = 0;

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

						if (clients[0].send(statePacket) == sf::Socket::Done)
						{
							std::cout << "Server WAITING_FOR_PLAYERS, sent\n";
						}
						else 
						{
						}

						statePacket.clear();
						statePacket << initStatePacket;
						if (clients[1].send(statePacket) == sf::Socket::Done)
						{
							std::cout << "Server WAITING_FOR_PLAYERS, sent\n";
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
							receptionCount++;
						
							std::cout << "Server PLAYER_MOVE received\n";
						
						}
					}
					else if (i == 1)
					{
						if (clients[i].receive(packet) == sf::Socket::Done)
						{
							packet >> gamePacket;
							p2Action = gamePacket.action;
							receptionCount++;

							std::cout << "Server PLAYER_MOVE received\n";

						}
					}
				}
				if (receptionCount >= 2)
				{
					serverState = sts::ServerState::RESOLVING;
					receptionCount = 0;

					statePacket.clear();
					sts::Packet gameStatePacket(sts::PacketType::GAME);
					statePacket << gameStatePacket;

					if (clients[0].send(statePacket) == sf::Socket::Done)
					{
						std::cout << "Server PLAYER_MOVE sent\n";
					}

					statePacket.clear();
					statePacket << gameStatePacket;
					if (clients[1].send(statePacket) == sf::Socket::Done)
					{
						std::cout << "Server PLAYER_MOVE sent\n";
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
							receptionCount++;
							std::cout << "Server RESOLVING received\n";
						}
					}
					else if (i == 1)
					{
						if (clients[i].receive(packet) == sf::Socket::Done)
						{
							packet.clear();
							receptionCount++;
							std::cout << "Server RESOLVING received\n";

						}
					}
				}
				if (receptionCount >= 2) 
				{
					//RESOLVE THE ACTIONS
					GameSolver(p1Action, p2Action, p1Result, p2Result);

					serverState = sts::ServerState::END;
					receptionCount = 0;
					
					packet.clear();
					endPacket.result = p1Result;
					packet << endPacket;
					if (clients[0].send(packet) == sf::Socket::Done)
					{
						std::cout << "Server RESOLVING sent\n";
					}
						
					packet.clear();
					endPacket.result = p2Result;
					packet << endPacket;
					if (clients[1].send(packet) == sf::Socket::Done)
					{
						std::cout << "Server RESOLVING sent\n";
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