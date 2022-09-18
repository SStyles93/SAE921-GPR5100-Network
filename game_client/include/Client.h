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

		sf::Text m_scoreText;
		sf::Text m_connectionText;
		sf::Text m_infoText;
		sf::Text m_actionText;
		sf::Text m_resultText;
		sf::Text m_opponentActionText;

		sf::Event m_event = sf::Event{};

		bool m_playerHasHand = true;
		bool m_shouldSendPacket = false;
		bool m_actionDone = false;

		int m_playerScore = 0;
		int m_opponentScore = 0;
		
		/// <summary>
		/// Initialises the Texts
		/// </summary>
		void InitText();
		/// <summary>
		/// Initialises the Texts fonts
		/// </summary>
		void InitFont();
		/// <summary>
		/// Sets the Texts size
		/// <param name="size">The desired size of texts</param>
		/// </summary>
		void SetTextSize(unsigned int size);
		/// <summary>
		/// Sets the Colours of the Texts
		/// </summary>
		void SetTextColour();
		/// <summary>
		/// Sets the position of Texts
		/// </summary>
		void SetTextPosition();
		/// <summary>
		/// Sets the action to be sent
		/// </summary>
		/// <param name="action">The desired action to send</param>
		void SetAction(sts::PlayerAction action);
		/// <summary>
		/// Updates results
		/// </summary>
		void UpdateResult();
	};
}