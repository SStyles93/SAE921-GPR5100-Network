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
	m_text1.setFont(m_font);
	m_text1.setFillColor(sf::Color::White);
	m_text1.setCharacterSize(50);
	m_text1.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.0f));

	m_text2.setFont(m_font);
	m_text2.setFillColor(sf::Color::White);
	m_text2.setCharacterSize(50);
	m_text2.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.0f));

	m_text3.setFont(m_font);
	m_text3.setFillColor(sf::Color::Red);
	m_text3.setCharacterSize(50);
	m_text3.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.1f));

	m_text4.setFont(m_font);
	m_text4.setCharacterSize(50);
	m_text4.setPosition(sf::Vector2f(m_window.getSize().x * 0.0f, m_window.getSize().y * 0.5f));

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
			if (m_clientState == sts::ClientState::GAME)
			{
				if (m_event.type == sf::Event::KeyPressed)
				{
					if (m_event.key.code == sf::Keyboard::Num1)
					{
						m_packet.clear();
						m_gamePacket.action = sts::PlayerAction::ROCK;
						m_packet << m_gamePacket;
						m_shouldSendPacket = true;
						m_text2.setString("You have played ROCK");
						std::cout << "Key 1 Pressed! " << std::endl;
					}
					if (m_event.key.code == sf::Keyboard::Num2)
					{
						m_packet.clear();
						m_gamePacket.action = sts::PlayerAction::PAPER;
						m_packet << m_gamePacket;
						m_shouldSendPacket = true;
						m_text2.setString("You have played PAPER");
						std::cout << "Key 2 Pressed! " << std::endl;
					}
					if (m_event.key.code == sf::Keyboard::Num3)
					{
						m_packet.clear();
						m_gamePacket.action = sts::PlayerAction::CISORS;
						m_packet << m_gamePacket;
						m_shouldSendPacket = true;
						m_text2.setString("You have played CISORS");
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
							m_text2.setString("You have played HAND");
							std::cout << "Key 4 Pressed! " << std::endl;
						}
						else
						{
							m_packet.clear();
							m_gamePacket.action = sts::PlayerAction::STUMP;
							m_packet << m_gamePacket;
							m_shouldSendPacket = true;
							m_text2.setString("You have played STUMP");
							std::cout << "Key 4 Pressed! " << std::endl;
						}
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
				}
				std::cout << "Client INIT received\n";
			}

			m_text1.setString("Connection established with the server.\nWaiting for a second Player...");
			m_window.clear();
			m_window.draw(m_text1);
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
				m_text2.setColor(sf::Color::Green);
				m_window.draw(m_text2);
			}
			else
			{
				//Draw instructions
				m_text2.setString(sf::String("You are playing ROCK - PAPER - CISORS - STUMP"));
				m_text3.setString(sf::String("Please select your move :\n [1]ROCK\n [2]PAPER\n [3]CISORS\n [4]HAND\n"));
				m_window.draw(m_text2);
				m_window.draw(m_text3);
			}
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
				m_result = m_endPacket.result;
				std::cout << "Client END received\n";

				switch (m_result)
				{
				case sts::Result::WON:
					m_text4.setFillColor(sf::Color::Green);
					m_text4.setString(sf::String("Yay ! You have won !"));
					break;

				case sts::Result::LOST:
					m_text4.setFillColor(sf::Color::Red);
					m_text4.setString(sf::String("Too bad, maybe more luck next time ! ;)"));
					break;

				case sts::Result::DRAW:
					m_text4.setFillColor(sf::Color::Color(255, 165, 0, 255));
					m_text4.setString(sf::String("DRAW"));
					break;

				default:
					m_text4.setString("???");
					break;
				}
			}
			m_window.clear();
			m_window.draw(m_text4);
			m_window.display();
		}
			break;
		default:
		{
			m_text1.setFillColor(sf::Color::Red);
			m_text1.setCharacterSize(50);
			m_text1.setString(sf::String("Something went wrong..."));

			m_window.clear();
			m_window.draw(m_text1);
			m_window.display();
		}
			break;
		}
	}
	return EXIT_SUCCESS;
}