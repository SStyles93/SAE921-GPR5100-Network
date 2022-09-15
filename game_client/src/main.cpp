#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include "ClientState.h"
#include "Packet.h"
#include "PlayerAction.h"

//Game_Client
int main()
{
	sf::TcpSocket socket;
	socket.setBlocking(false);

	sf::RenderWindow window;
	//Create sfml window once the connection is done
	window.create(sf::VideoMode(1280, 1024), "OnlineGame");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60.0f);

	sf::Font font;
	if (font.loadFromFile("data/font/RetroGaming.ttf"))
	{
		std::cerr << "Can't find font.\n";
	}

	sf::Text text;
	sf::Text initText1;
	sf::Text initText2;
	sf::Text endText;

	text.setFont(font);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(50);
	text.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.0f));

	initText1.setFont(font);
	initText1.setFillColor(sf::Color::White);
	initText1.setCharacterSize(50);
	initText1.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.0f));

	initText2.setFont(font);
	initText2.setFillColor(sf::Color::Red);
	initText2.setCharacterSize(50);
	initText2.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.1f));


#pragma region Game

	sf::Packet packet;
	sts::Packet receivedPacket;
	sts::ClientState clientState = sts::ClientState::CONNECTING_TO_SERVER;

	while (window.isOpen())
	{
		bool shouldSendPacket = false;
		bool hasWon = false;

		sf::Event event;
		while (window.pollEvent(event))
		{
			// Windows events -------------------------------------------------------------------------------
			if (event.type == sf::Event::Closed)
			{
				window.close();
				return EXIT_SUCCESS;
			}
			if (event.type == sf::Event::Resized)
			{
				auto view = window.getView();
				view.setSize(event.size.width, event.size.height);
				window.setView(view);
			}
		}

		switch (clientState)
		{

		case sts::ClientState::CONNECTING_TO_SERVER:

			sf::Socket::Status status = socket.connect("localhost", 12345);

			if (status == sf::Socket::Done)
			{
				std::cout << "Connection established with: " << socket.getRemoteAddress() << std::endl;
				clientState = sts::ClientState::INIT;
			}
			else if (status == sf::Socket::Error)
			{
				std::cerr << "A thing\n";
			}
			break;

		case sts::ClientState::INIT:

			if (socket.receive(packet) == sf::Socket::Done)
			{
				packet >> receivedPacket;
				if (receivedPacket.type == sts::PacketType::INIT)
				{
					clientState == sts::ClientState::GAME;
				}
			}

			//SFML Text Initialisation
			text.setString("Connection established with the server.\nWaiting for a second Player...");

			break;

		case sts::ClientState::GAME:

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
			{
				packet.clear();
				//TODO: send action 1
				shouldSendPacket = true;
				std::cout << "KeyPressed! " << std::endl;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
			{
				packet.clear();
				//TODO: send action 2
				shouldSendPacket = true;
				std::cout << "KeyPressed! " << std::endl;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
			{
				packet.clear();
				//TODO: send action 3
				shouldSendPacket = true;
				std::cout << "KeyPressed! " << std::endl;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
			{
				packet.clear();
				//TODO: send action 4
				shouldSendPacket = true;
				std::cout << "KeyPressed! " << std::endl;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
			{
				packet.clear();
				//TODO: send action 5
				shouldSendPacket = true;
				std::cout << "KeyPressed! " << std::endl;
			}

			initText1.setString(sf::String("You are playing ROCK - PAPER - CISORS - STUMP"));
			initText2.setString(sf::String("Please select your move :\n [1]ROCK\n [2]PAPER\n [3]CISORS\n [4]HAND\n"));

			// Clear all elements from background
			window.clear();
			//Draw
			window.draw(initText1);
			window.draw(initText2);
			// Display all elements
			window.display();

			if (shouldSendPacket && socket.send(packet) == sf::Socket::Done)
			{
				shouldSendPacket = false;
			};

			break;

		case sts::ClientState::END:

			if (hasWon)
			{
				endText.setFont(font);
				endText.setFillColor(sf::Color::Green);
				endText.setCharacterSize(50);
				endText.setString(sf::String("Yay ! You have won !"));
				endText.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.5f));

			}
			else
			{
				endText.setFont(font);
				endText.setFillColor(sf::Color::Red);
				endText.setCharacterSize(50);
				endText.setString(sf::String("Too bad, maybe more luck next time ! ;)"));
				endText.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.5f));
			}

			// Clear all elements from background
			window.clear();

			window.draw(endText);

			// Display all elements
			window.display();

			break;

		default:

			text.setFillColor(sf::Color::Red);
			text.setCharacterSize(50);
			text.setString(sf::String("Something went wrong..."));

			window.clear();
			window.draw(text);
			window.display();

			break;
		}
	}
	return EXIT_SUCCESS;
}



