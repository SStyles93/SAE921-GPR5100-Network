#include "Client.h"


sts::Client::Client(){}

sts::Client::~Client(){}

void sts::Client::Init() 
{
	//set socket to non blocking
	m_socket.setBlocking(false);

	//Create sfml window once the connection is done
	m_window.create(sf::VideoMode(1280, 1024), "OnlineGame");
	m_window.setVerticalSyncEnabled(true);
	m_window.setFramerateLimit(60.0f);

	//Load font
	if (!m_font.loadFromFile("data/font/RetroGaming.ttf"))
	{
		std::cerr << "Can't find font.\n";
	}

	//Init texts
	m_infoText.setFont(m_font);
	m_infoText.setFillColor(sf::Color::White);
	m_infoText.setCharacterSize(50);
	m_infoText.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.0f));
	m_infoText.setString("You are playing ROCK-PAPER-CISORS-HAND");

	m_connectionText.setFont(m_font);
	m_connectionText.setFillColor(sf::Color::White);
	m_connectionText.setCharacterSize(50);
	m_connectionText.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.0f));

	m_scoreText.setFont(m_font);
	m_scoreText.setFillColor(sf::Color::White);
	m_scoreText.setCharacterSize(50);
	m_scoreText.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.1f));

	m_actionText.setFont(m_font);
	m_actionText.setFillColor(sf::Color::Red);
	m_actionText.setCharacterSize(50);
	m_actionText.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.2f));

	m_opponentActionText.setFont(m_font);
	m_opponentActionText.setFillColor(sf::Color::White);
	m_opponentActionText.setCharacterSize(50);
	m_opponentActionText.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.3f));

	m_resultText.setFont(m_font);
	m_resultText.setCharacterSize(50);
	m_resultText.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.4f));

}

int sts::Client::Update() 
{
	while (m_window.isOpen())
	{
		while (m_window.pollEvent(m_event))
		{
			// Windows events -------------------------------------------------------------------------------
			if (m_event.type == sf::Event::Closed)
			{
				m_window.close();
				return EXIT_SUCCESS;
			}
			if (m_event.type == sf::Event::Resized)
			{
				auto view = m_window.getView();
				view.setSize(m_event.size.width, m_event.size.height);
				m_window.setView(view);
			}
			if (m_event.type == sf::Event::KeyPressed)
			{
				if (m_clientState == sts::ClientState::GAME)
				{
					if (m_event.key.code == sf::Keyboard::Num1)
					{
						m_packet.clear();
						m_gamePacket.action = sts::PlayerAction::ROCK;
						m_packet << m_gamePacket;
						m_shouldSendPacket = true;
						m_actionText.setString("You have played ROCK");
						std::cout << "Key 1 Pressed! " << std::endl;
					}
					if (m_event.key.code == sf::Keyboard::Num2)
					{
						m_packet.clear();
						m_gamePacket.action = sts::PlayerAction::PAPER;
						m_packet << m_gamePacket;
						m_shouldSendPacket = true;
						m_actionText.setString("You have played PAPER");
						std::cout << "Key 2 Pressed! " << std::endl;
					}
					if (m_event.key.code == sf::Keyboard::Num3)
					{
						m_packet.clear();
						m_gamePacket.action = sts::PlayerAction::CISORS;
						m_packet << m_gamePacket;
						m_shouldSendPacket = true;
						m_actionText.setString("You have played CISORS");
						std::cout << "Key 3 Pressed! " << std::endl;
					}
					if (m_event.key.code == sf::Keyboard::Num4)
					{
						if (m_playerHasHand)
						{
							m_packet.clear();
							m_gamePacket.action = sts::PlayerAction::HAND;
							m_packet << m_gamePacket;
							m_shouldSendPacket = true;
							m_actionText.setString("You have played HAND");
							std::cout << "Key 4 Pressed! " << std::endl;
						}
						else
						{
							m_packet.clear();
							m_gamePacket.action = sts::PlayerAction::STUMP;
							m_packet << m_gamePacket;
							m_shouldSendPacket = true;
							m_actionText.setString("You have played STUMP");
							std::cout << "Key 4 Pressed! " << std::endl;
						}
					}
				}
				if (m_clientState == sts::ClientState::END)
				{
					if (m_event.key.code == sf::Keyboard::Enter)
					{
						m_packet.clear();
						m_shouldSendPacket = true;
						m_clientState = sts::ClientState::REPLAY;
						std::cout << "Key ENTER Pressed! " << std::endl;
					}
				}
			}
		}

		switch (m_clientState)
		{
		case sts::ClientState::CONNECTING_TO_SERVER:
		{
			sf::Socket::Status status = m_socket.connect(sf::IpAddress::getLocalAddress(), 12345);

			if (status == sf::Socket::Done || status == sf::Socket::NotReady)
			{
				std::cout << "Connection established with: " << m_socket.getRemoteAddress() << std::endl;
				m_clientState = sts::ClientState::INIT;
			}
			else if (status == sf::Socket::Error)
			{
				std::cerr << "An error has occured\n";
			}
		}
			break;
		case sts::ClientState::INIT:
		{
			if (m_socket.receive(m_packet) == sf::Socket::Done)
			{
				m_packet >> m_receivedPacket;
				if (m_receivedPacket.type == sts::PacketType::INIT)
				{
					m_clientState = sts::ClientState::GAME;
					//RESET receivedPacket
					m_receivedPacket.type = sts::PacketType::NONE;
				}
				std::cout << "Client INIT received\n";
			}

			m_connectionText.setString("Connection established with the server.\nWaiting for a second Player...");
			m_window.clear();
			m_window.draw(m_connectionText);
			m_window.display();
		}
			break;
		case sts::ClientState::GAME:
		{
			// Clear all elements from background
			m_window.clear();

			//Check if packet is sent or not
			if (m_shouldSendPacket && m_socket.send(m_packet) == sf::Socket::Done)
			{
				m_shouldSendPacket = false;
				m_actionDone = true;
				std::cout << "Client GAME sent\n";
			}

			if (m_actionDone)
			{
				m_actionText.setColor(sf::Color::Green);
			}
			else
			{
				//Draw instructions
				m_actionText.setString("Please select your move :\n [1]ROCK\n [2]PAPER\n [3]CISORS\n [4]HAND\n");
			}

			m_scoreText.setString("The current score is: Player: " + std::to_string(m_playerScore) + " | Opponent: " + std::to_string(m_opponentScore));
			
			m_window.draw(m_infoText);
			m_window.draw(m_scoreText);
			m_window.draw(m_actionText);

			// Display all elements
			m_window.display();

			if (m_socket.receive(m_packet) == sf::Socket::Done)
			{
				m_packet >> m_receivedPacket;
				if (m_receivedPacket.type == sts::PacketType::GAME)
				{
					m_clientState = sts::ClientState::END;
					m_shouldSendPacket = true;
				}
				std::cout << "Client GAME received\n";

			}
		}
			break;
		case sts::ClientState::END:
		{
			if (m_shouldSendPacket && m_socket.send(m_packet) == sf::Socket::Done)
			{
				m_shouldSendPacket = false;
				m_packet.clear();
				std::cout << "Client END packet sent\n";
			}

			if (m_socket.receive(m_packet) == sf::Socket::Done)
			{
				m_packet >> m_endPacket;
				m_otherPlayersAction = m_endPacket.opponentAction;
				m_result = m_endPacket.result;
				std::cout << "Client END received\n";

				switch (m_result)
				{
				case sts::Result::WON:
					m_playerScore++;
					m_scoreText.setString("The current score is: Player: " + std::to_string(m_playerScore) + " | Opponent: " + std::to_string(m_opponentScore));
					m_opponentActionText.setString("You played" + ActionToString(m_gamePacket.action) + "\nYour opponent played " + sts::ActionToString(m_otherPlayersAction));
					m_resultText.setFillColor(sf::Color::Green);
					m_resultText.setString("And You Won !");
					break;

				case sts::Result::LOST:
					m_opponentScore++;
					m_scoreText.setString("The current score is: Player: " + std::to_string(m_playerScore) + " | Opponent: " + std::to_string(m_opponentScore));
					m_opponentActionText.setString("You played " + ActionToString(m_gamePacket.action) + "\nYour opponent played " + sts::ActionToString(m_otherPlayersAction));
					m_resultText.setFillColor(sf::Color::Red);
					m_resultText.setString("And You Lost !");
					break;

				case sts::Result::DRAW:
					m_scoreText.setString("The current score is: Player: " + std::to_string(m_playerScore) + " | Opponent: " + std::to_string(m_opponentScore));
					//m_opponentActionText.setString("Your opponent played " + std::to_string(static_cast<int>(m_otherPlayersAction)));
					m_opponentActionText.setString("You played " + ActionToString(m_gamePacket.action) + "\nYour opponent played " + sts::ActionToString(m_otherPlayersAction));
					m_resultText.setFillColor(sf::Color::Color(255, 165, 0, 255));
					m_resultText.setString("So it's and equality");
					break;

				default:
					m_resultText.setString("???");
					break;
				}
			}

			m_actionText.setString("Press ENTER to replay");
			m_actionText.setColor(sf::Color::White);
			m_actionText.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.2f));

			m_window.clear();
			m_window.draw(m_infoText);
			m_window.draw(m_scoreText);
			m_window.draw(m_actionText);
			m_window.draw(m_resultText);
			m_window.draw(m_opponentActionText);
			m_window.display();
			
		}
			break;
		case sts::ClientState::REPLAY:
		{
			if (m_shouldSendPacket && m_socket.send(m_packet) == sf::Socket::Done)
			{
				m_clientState = sts::ClientState::INIT;
				m_gamePacket.action = sts::PlayerAction::NONE;
				m_shouldSendPacket = false;
				m_actionDone = false;
				m_packet.clear();
				std::cout << "Client REPLAY packet sent\n";
			}
		}
			break;
		default:
		{
			m_connectionText.setFillColor(sf::Color::Red);
			m_connectionText.setCharacterSize(50);
			m_connectionText.setString(sf::String("Something went wrong..."));

			m_window.clear();
			m_window.draw(m_connectionText);
			m_window.display();
		}
			break;
		}
	}
	return EXIT_SUCCESS;
}