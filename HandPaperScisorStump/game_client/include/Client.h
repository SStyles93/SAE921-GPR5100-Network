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

		//Server's ip adress
		sf::IpAddress m_serverAdress = sf::IpAddress::getLocalAddress();

		//The client's socket
		sf::TcpSocket m_socket;
		
		//Packet used to send/receive data
		sf::Packet m_packet;

		//Packet used to store states
		sts::Packet m_statePacket;
		//Packet containing actions
		sts::GamePacket m_gamePacket;
		//Packet containing end game informations
		sts::EndPacket m_endPacket;

		//The state of the client
		sts::ClientState m_clientState = sts::ClientState::CONNECTING_TO_SERVER;
		//The result of the client
		sts::Result m_result = sts::Result::NONE;
		//The opponent's action
		sts::PlayerAction m_otherPlayersAction = sts::PlayerAction::NONE;

		//The render window
		sf::RenderWindow m_window;
		//Font used for texts
		sf::Font m_font;

		//The text used for the score
		sf::Text m_scoreText;
		//The text used to display connection informations
		sf::Text m_connectionText;
		//The text used to display informations
		sf::Text m_infoText;
		//The text used to display actions (to do / done)
		sf::Text m_actionText;
		//The text used to display results
		sf::Text m_resultText;
		//The text used to display the opponent's action
		sf::Text m_opponentActionText;

		//Event used for window & input
		sf::Event m_event = sf::Event{};

		//bool used to define if the player has HAND/STUMP
		bool m_playerHasHand = true;
		//bool used to send Packets once
		bool m_shouldSendPacket = false;
		//bool used to check if action was done or not
		bool m_actionDone = false;

		//The player's score
		int m_playerScore = 0;
		//The opponent's score
		int m_opponentScore = 0;
	};
}