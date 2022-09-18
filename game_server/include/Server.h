#pragma once
#include <iostream>
#include <SFML/Network.hpp>
#include <array>
#include "Packet.h"
#include "ServerState.h"

namespace sts
{
	class Server
	{
	public:
		Server();
		~Server();
		/// <summary>
		/// Initialises the Server
		/// </summary>
		void Init();
		/// <summary>
		/// Runs the Server's main "loop"
		/// </summary>
		/// <returns></returns>
		int Update();

	private:

		//Listener listens to incomming connections
		sf::TcpListener m_listener;
		
		//Clients
		std::array<sf::TcpSocket, 2> m_clients{};
		
		//Selector
		sf::SocketSelector m_selector;

		//Client count to assign IDs to clients
		int m_clientCount = 0;
		
		//State of the server at the begining
		sts::ServerState m_serverState = sts::ServerState::WAITING_FOR_PLAYERS;

		//Packet needed to transmit/receive data
		sf::Packet m_packet;
		sf::Packet m_statePacket;
		sts::GamePacket m_gamePacket;
		sts::EndPacket m_endPacket;

		//The actions of the players 1&2
		sts::PlayerAction m_p1Action = sts::PlayerAction::NONE;
		sts::PlayerAction m_p2Action = sts::PlayerAction::NONE;
		
		//The results of the players 1&2
		sts::Result m_p1Result = sts::Result::NONE;
		sts::Result m_p2Result = sts::Result::NONE;

		bool m_p1HandState = true;
		bool m_p2HandState = true;
		
		//Number of packets received
		int m_receptionCount = 0;
		
		/// <summary>
		/// Gives the results according to actions received
		/// </summary>
		/// <param name="player1Action">The first player's action (sts::PlayerAction)</param>
		/// <param name="player2Action">The second player's action (sts::PlayerAction)</param>
		/// <param name="player1Result">The first player's result (sts::Result)</param>
		/// <param name="player2Result">The first player's result (sts::Result)</param>
		void GameSolver(
			sts::PlayerAction player1Action, sts::PlayerAction player2Action, 
			sts::Result& player1Result, sts::Result& player2Result,
			bool& p1HandState, bool& p2HandState);
	};
}

