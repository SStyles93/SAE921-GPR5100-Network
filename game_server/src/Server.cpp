#include "Server.h"

sts::Server::Server() {}
sts::Server::~Server() {}

void sts::Server::GameSolver(sts::PlayerAction player1Action, sts::PlayerAction player2Action, sts::Result& player1Result, sts::Result& player2Result)
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

void sts::Server::Init() 
{
	//The port to listen
	m_listener.listen(12345);
	//Add the listener to the selector
	m_selector.add(m_listener);
}

int sts::Server::Update() 
{
	while (true)
	{
		// Make the selector wait for data on any socket
		if (m_selector.wait(sf::seconds(10.0f)))
		{
			switch (m_serverState)
			{
			case sts::ServerState::WAITING_FOR_PLAYERS:
				// Test the listener
				if (m_selector.isReady(m_listener))
				{
					// The listener is ready: there is a pending connection
					if (m_listener.accept(m_clients[0]) == sf::Socket::Done)
					{
						// Add the new client to the selector so that we will
						// be notified when he sends something
						m_selector.add(m_clients[0]);
						m_clientCount++;
						std::cout << "Connection established with: " << m_clients[0].getRemoteAddress() << " as clients[0]." << std::endl;
					}
					if (m_listener.accept(m_clients[1]) == sf::Socket::Done)
					{
						m_selector.add(m_clients[1]);
						m_clientCount++;
						std::cout << "Connection established with: " << m_clients[1].getRemoteAddress() << " as clients[1]." << std::endl;
					}

					if (m_clientCount >= 2) {
						m_serverState = sts::ServerState::PLAYERS_MOVE;

						sts::Packet initStatePacket(sts::PacketType::INIT);
						m_statePacket << initStatePacket;

						if (m_clients[0].send(m_statePacket) == sf::Socket::Done)
						{
							std::cout << "Server WAITING_FOR_PLAYERS, sent\n";
						}
						else
						{
						}

						m_statePacket.clear();
						m_statePacket << initStatePacket;
						if (m_clients[1].send(m_statePacket) == sf::Socket::Done)
						{
							std::cout << "Server WAITING_FOR_PLAYERS, sent\n";
						}
					}
				}
				break;

			case sts::ServerState::PLAYERS_MOVE:
				for (std::size_t i = 0; i < m_clients.size(); i++)
				{
					if (!m_selector.isReady(m_clients[i])) continue;

					if (i == 0)
					{
						if (m_clients[i].receive(m_packet) == sf::Socket::Done)
						{
							m_packet >> m_gamePacket;
							m_p1Action = m_gamePacket.action;
							m_receptionCount++;

							std::cout << "Server PLAYER_MOVE received\n";

						}
					}
					else if (i == 1)
					{
						if (m_clients[i].receive(m_packet) == sf::Socket::Done)
						{
							m_packet >> m_gamePacket;
							m_p2Action = m_gamePacket.action;
							m_receptionCount++;

							std::cout << "Server PLAYER_MOVE received\n";

						}
					}
				}
				if (m_receptionCount >= 2)
				{
					m_serverState = sts::ServerState::RESOLVING;
					m_receptionCount = 0;

					m_statePacket.clear();
					sts::Packet gameStatePacket(sts::PacketType::GAME);
					m_statePacket << gameStatePacket;

					if (m_clients[0].send(m_statePacket) == sf::Socket::Done)
					{
						std::cout << "Server PLAYER_MOVE sent\n";
					}

					m_statePacket.clear();
					m_statePacket << gameStatePacket;
					if (m_clients[1].send(m_statePacket) == sf::Socket::Done)
					{
						std::cout << "Server PLAYER_MOVE sent\n";
					}
				}
				break;

			case sts::ServerState::RESOLVING:


				for (std::size_t i = 0; i < m_clients.size(); i++)
				{
					if (!m_selector.isReady(m_clients[i])) continue;
					if (i == 0)
					{
						if (m_clients[i].receive(m_packet) == sf::Socket::Done)
						{
							m_packet.clear();
							m_receptionCount++;
							std::cout << "Server RESOLVING received\n";
						}
					}
					else if (i == 1)
					{
						if (m_clients[i].receive(m_packet) == sf::Socket::Done)
						{
							m_packet.clear();
							m_receptionCount++;
							std::cout << "Server RESOLVING received\n";

						}
					}
				}
				if (m_receptionCount >= 2)
				{
					//RESOLVE THE ACTIONS
					GameSolver(m_p1Action, m_p2Action, m_p1Result, m_p2Result);

					m_serverState = sts::ServerState::END;
					m_receptionCount = 0;

					m_packet.clear();
					m_endPacket.result = m_p1Result;
					m_packet << m_endPacket;
					if (m_clients[0].send(m_packet) == sf::Socket::Done)
					{
						std::cout << "Server RESOLVING sent\n";
					}

					m_packet.clear();
					m_endPacket.result = m_p2Result;
					m_packet << m_endPacket;
					if (m_clients[1].send(m_packet) == sf::Socket::Done)
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