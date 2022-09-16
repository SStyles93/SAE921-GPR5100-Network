#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include "ClientState.h"
#include "Packet.h"
#include "PlayerAction.h"

namespace sts
{

	class Client
	{
	public:
		Client();
		~Client();

		/// <summary>
		/// Initialises the Client
		/// </summary>
		void Init();
		/// <summary>
		/// Runs the Client 
		/// </summary>
		int Update();

	private:

		sf::TcpSocket m_socket;

		sf::Packet m_packet;
		sts::Packet m_receivedPacket;
		sts::GamePacket m_gamePacket;
		sts::EndPacket m_endPacket;

		sts::ClientState m_clientState = sts::ClientState::CONNECTING_TO_SERVER;
		sts::Result m_result = sts::Result::NONE;
		sts::PlayerAction m_otherPlayersAction = sts::PlayerAction::NONE;

		sf::RenderWindow m_window;

		sf::Font m_font;

		sf::Text m_text1;
		sf::Text m_text2;
		sf::Text m_text3;
		sf::Text m_text4;

		sf::Event m_event;

		bool m_playerHasHand = true;
		bool m_shouldSendPacket = false;
		bool m_actionDone = false;

	};
}