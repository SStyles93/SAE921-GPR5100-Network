#include "Server.h"

namespace sts
{
	Server::Server() {}
	sts::Server::~Server() {}

	void sts::Server::GameSolver(
		sts::PlayerAction player1Action, sts::PlayerAction player2Action,
		sts::Result& player1Result, sts::Result& player2Result,
		bool& p1HandState, bool& p2HandState)
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
			case sts::PlayerAction::SCISSORS:
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
			break;
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
			case sts::PlayerAction::SCISSORS:
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
			break;
		case sts::PlayerAction::SCISSORS:
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
			case sts::PlayerAction::SCISSORS:
				player1Result = sts::Result::DRAW;
				player2Result = sts::Result::DRAW;
				m_p1HandState = false;
				m_p2HandState = false;
				break;
			case sts::PlayerAction::HAND:
				player1Result = sts::Result::WON;
				player2Result = sts::Result::LOST;
				p2HandState = false;
				break;
			case sts::PlayerAction::STUMP:
				player1Result = sts::Result::LOST;
				player2Result = sts::Result::WON;
				break;
			default:
				break;
			}
			break;
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
			case sts::PlayerAction::SCISSORS:
				player1Result = sts::Result::LOST;
				player2Result = sts::Result::WON;
				p1HandState = false;
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
			break;
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
			case sts::PlayerAction::SCISSORS:
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
			break;
		}
	}
	void sts::Server::SendStatePacket(sts::PacketType type)
	{
		m_gameStatePacket.type = type;

		m_statePacket.clear();
		m_statePacket << m_gameStatePacket;
		if (m_clients[0].send(m_statePacket) == sf::Socket::Done)
		{
			std::cout << "Server" << StateToString(m_serverState) << " package sent\n";
		}

		m_statePacket.clear();
		m_statePacket << m_gameStatePacket;
		if (m_clients[1].send(m_statePacket) == sf::Socket::Done)
		{
			std::cout << "Server " << StateToString(m_serverState) << " package sent\n";
		}
	}
	void sts::Server::ResetGame()
	{
		m_p1Action = sts::PlayerAction::NONE;
		m_p2Action = sts::PlayerAction::NONE;
		m_p1Result = sts::Result::NONE;
		m_p2Result = sts::Result::NONE;
	}

	void sts::Server::Init()
	{
		//The port to listen
		m_listener.listen(12345);
		//Add the listener to the selector
		m_selector.add(m_listener);
	}
	void sts::Server::Run()
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

						if (m_clientCount >= 2)
						{
							SendStatePacket(sts::PacketType::INIT);
							m_serverState = sts::ServerState::PLAYERS_MOVE;
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
						SendStatePacket(sts::PacketType::GAME);

						m_serverState = sts::ServerState::RESOLVING;
						m_receptionCount = 0;
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
						GameSolver(m_p1Action, m_p2Action, m_p1Result, m_p2Result, m_p1HandState, m_p2HandState);

						m_packet.clear();
						m_endPacket.result = m_p1Result;
						m_endPacket.opponentAction = m_p2Action;
						m_endPacket.hasHand = m_p1HandState;
						m_packet << m_endPacket;
						if (m_clients[0].send(m_packet) == sf::Socket::Done)
						{
							std::cout << "Server RESOLVING sent\n";
						}

						m_packet.clear();
						m_endPacket.result = m_p2Result;
						m_endPacket.opponentAction = m_p1Action;
						m_endPacket.hasHand = m_p2HandState;
						m_packet << m_endPacket;
						if (m_clients[1].send(m_packet) == sf::Socket::Done)
						{
							std::cout << "Server RESOLVING sent\n";
						}

						m_serverState = sts::ServerState::END;
						m_receptionCount = 0;
					}
					break;

				case sts::ServerState::END:

					for (std::size_t i = 0; i < m_clients.size(); i++)
					{
						if (!m_selector.isReady(m_clients[i])) continue;
						if (i == 0)
						{
							if (m_clients[i].receive(m_packet) == sf::Socket::Done)
							{
								m_packet.clear();
								m_receptionCount++;
								std::cout << "Server END received\n";
							}
						}
						else if (i == 1)
						{
							if (m_clients[i].receive(m_packet) == sf::Socket::Done)
							{
								m_packet.clear();
								m_receptionCount++;
								std::cout << "Server END received\n";

							}
						}
					}
					if (m_receptionCount >= 2)
					{
						SendStatePacket(sts::PacketType::INIT);
						ResetGame();

						m_serverState = sts::ServerState::PLAYERS_MOVE;
						m_receptionCount = 0;
					}

					break;
				}
			}

		}
		std::cout << "Server has stopped working" << std::endl;
	}
}