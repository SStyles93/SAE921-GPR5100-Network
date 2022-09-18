#include "Server.h"

sts::Server::Server() {}
sts::Server::~Server() {}

void sts::Server::ConnectClients() 
{
	if (m_listener.accept(m_clients[0]) == sf::Socket::Done)
	{
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
}
void sts::Server::SendStatePackets(sts::PacketType type) 
{
	m_gameStatePacket.type = type;
	
	m_packet.clear();
	m_packet << m_gameStatePacket;
	if (m_clients[0].send(m_packet) == sf::Socket::Done)
	{
		std::cout << "Server " << StateToString(m_serverState) << " package sent\n";
	}
	m_packet.clear();
	m_packet << m_gameStatePacket;
	if (m_clients[1].send(m_packet) == sf::Socket::Done)
	{
		std::cout << "Server" << StateToString(m_serverState) << " package sent\n";
	}
}
void sts::Server::ReceiveAction(int clientId, sts::PlayerAction action) 
{
	if (m_clients[clientId].receive(m_packet) == sf::Socket::Done)
	{
		m_packet >> m_gamePacket;
		action = m_gamePacket.action;

		std::cout << "Server "<< StateToString(m_serverState)<< " received\n";
	}
}
void sts::Server::ReceiveValidation(int clientIndex)
{
	if (m_clients[clientIndex].receive(m_packet) == sf::Socket::Done)
	{
		m_packet.clear();
		std::cout << "Server "<< StateToString(m_serverState) << " received\n";
	}
}
void sts::Server::GameSolver(sts::PlayerAction player1Action, sts::PlayerAction player2Action, sts::Result& player1Result, sts::Result& player2Result,bool& p1HandState, bool& p2HandState)
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
		break;
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
		case sts::PlayerAction::CISORS:
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
		break;
	}
}
void sts::Server::SendResolution() 
{
	m_packet.clear();
	m_endPacket.result = m_p1Result;
	m_endPacket.opponentAction = m_p2Action;
	m_endPacket.hasHand = m_p1HandState;
	m_packet << m_endPacket;
	if (m_clients[0].send(m_packet) == sf::Socket::Done)
	{
		std::cout << "Server " << StateToString(m_serverState)<< " sent\n";
	}

	m_packet.clear();
	m_endPacket.result = m_p2Result;
	m_endPacket.opponentAction = m_p1Action;
	m_endPacket.hasHand = m_p2HandState;
	m_packet << m_endPacket;
	if (m_clients[1].send(m_packet) == sf::Socket::Done)
	{
		std::cout << "Server " << StateToString(m_serverState) << " sent\n";
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
					ConnectClients();

					if (m_clientCount >= 2) 
					{
						SendStatePackets(sts::PacketType::INIT);
						m_serverState = sts::ServerState::PLAYERS_MOVE;
					}
				}
				break;

			case sts::ServerState::PLAYERS_MOVE:
				for (std::size_t client = 0; client < m_clients.size(); client++)
				{
					if (!m_selector.isReady(m_clients[client])) continue;

					if (client == 0)
					{
						ReceiveAction(client, m_p1Action);
						m_receptionCount++;
					}
					else if (client == 1)
					{
						ReceiveAction(client, m_p2Action);
						m_receptionCount++;
					}
				}
				if (m_receptionCount >= 2)
				{
					m_receptionCount = 0;
					SendStatePackets(sts::PacketType::GAME);
					m_serverState = sts::ServerState::RESOLVING;
				}
				break;

			case sts::ServerState::RESOLVING:

				for (std::size_t client = 0; client < m_clients.size(); client++)
				{
					if (!m_selector.isReady(m_clients[client])) continue;

					if (client == 0)
					{
						ReceiveValidation(client);
						m_receptionCount++;
					}
					else if (client == 1)
					{
						ReceiveValidation(client);
						m_receptionCount++;
					}
				}
				if (m_receptionCount >= 2)
				{
					m_receptionCount = 0;
					GameSolver(m_p1Action, m_p2Action, m_p1Result, m_p2Result, m_p1HandState, m_p2HandState);
					SendResolution();
					m_serverState = sts::ServerState::END;
				}
				break;

			case sts::ServerState::END:

				for (std::size_t client = 0; client < m_clients.size(); client++)
				{
					if (!m_selector.isReady(m_clients[client])) continue;
					if (client == 0)
					{
						ReceiveValidation(client);
						m_receptionCount++;
					}
					else if (client == 1)
					{
						ReceiveValidation(client);
						m_receptionCount++;
					}
				}
				if (m_receptionCount >= 2) 
				{
					m_receptionCount = 0;
					SendStatePackets(sts::PacketType::INIT);
					ResetGame();
					m_serverState = sts::ServerState::PLAYERS_MOVE;
				}

				break;
			}
		}

	}
	std::cout << "Server has stopped working" << std::endl;
	return EXIT_SUCCESS;
}