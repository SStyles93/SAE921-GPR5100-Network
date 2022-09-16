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
	if (!font.loadFromFile("data/font/RetroGaming.ttf"))
	{
		std::cerr << "Can't find font.\n";
	}


	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(50);
	text.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.0f));

	sf::Text initText1;
	initText1.setFont(font);
	initText1.setFillColor(sf::Color::White);
	initText1.setCharacterSize(50);
	initText1.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.0f));

	sf::Text initText2;
	initText2.setFont(font);
	initText2.setFillColor(sf::Color::Red);
	initText2.setCharacterSize(50);
	initText2.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.1f));
	
	sf::Text endText;
	endText.setFont(font);
	endText.setCharacterSize(50);
	endText.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.5f));

	sf::Packet packet;
	
	sts::Packet receivedPacket;
	sts::GamePacket gamePacket;
	sts::EndPacket endPacket;

	sts::ClientState clientState = sts::ClientState::CONNECTING_TO_SERVER;
	sts::Result result = sts::Result::NONE;
	sts::PlayerAction otherPlayersAction = sts::PlayerAction::NONE;

	bool playerHasHand = true;
	bool shouldSendPacket = false;
	bool actionDone = false;
	
	while (window.isOpen())
	{

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
			if (clientState == sts::ClientState::GAME) 
			{
				if (event.type == sf::Event::KeyPressed) 
				{
					if (event.key.code == sf::Keyboard::Num1) 
					{
						packet.clear();
						gamePacket.action = sts::PlayerAction::ROCK;
						packet << gamePacket;
						shouldSendPacket = true;
						initText1.setString("You have played ROCK");
						std::cout << "KeyPressed! " << std::endl;
					}
					if (event.key.code == sf::Keyboard::Num2)
					{
						packet.clear();
						gamePacket.action = sts::PlayerAction::PAPER;
						packet << gamePacket;
						shouldSendPacket = true;
						initText1.setString("You have played PAPER");
						std::cout << "KeyPressed! " << std::endl;
					}
					if (event.key.code == sf::Keyboard::Num3)
					{
						packet.clear();
						gamePacket.action = sts::PlayerAction::CISORS;
						packet << gamePacket;
						shouldSendPacket = true;
						initText1.setString("You have played CISORS");
						std::cout << "KeyPressed! " << std::endl;
					}
					if (event.key.code == sf::Keyboard::Num4)
					{
						if (playerHasHand)
						{
							packet.clear();
							gamePacket.action = sts::PlayerAction::HAND;
							packet << gamePacket;
							shouldSendPacket = true;
							initText1.setString("You have played HAND");
							std::cout << "KeyPressed! " << std::endl;
						}
						else
						{
							packet.clear();
							gamePacket.action = sts::PlayerAction::STUMP;
							packet << gamePacket;
							shouldSendPacket = true;
							initText1.setString("You have played STUMP");
							std::cout << "KeyPressed! " << std::endl;
						}
					}
				}
			}
		}

		switch (clientState)
		{

		case sts::ClientState::CONNECTING_TO_SERVER:
		{
			sf::Socket::Status status = socket.connect(sf::IpAddress::getLocalAddress(), 12345);

			if (status == sf::Socket::Done || status == sf::Socket::NotReady)
			{
				std::cout << "Connection established with: " << socket.getRemoteAddress() << std::endl;
				clientState = sts::ClientState::INIT;
			}
			else if (status == sf::Socket::Error)
			{
				std::cerr << "An error has occured\n";
			}
			break;
		}
		case sts::ClientState::INIT:
		{
			if (socket.receive(packet) == sf::Socket::Done)
			{
				packet >> receivedPacket;
				if (receivedPacket.type == sts::PacketType::INIT)
				{
					clientState = sts::ClientState::GAME;
				}
				std::cout << "Client INIT received\n";
			}

			text.setString("Connection established with the server.\nWaiting for a second Player...");
			window.clear();
			window.draw(text);
			window.display();
		}
			break;

		case sts::ClientState::GAME:
		{
			// Clear all elements from background
			window.clear();

			//Check if packet is sent or not
			if (shouldSendPacket && socket.send(packet) == sf::Socket::Done)
			{
				shouldSendPacket = false;
				actionDone = true;
				std::cout << "Client GAME sent\n";
			}

			if (actionDone) 
			{
				initText1.setColor(sf::Color::Green);
				window.draw(initText1);
			}
			else 
			{
				//Draw instructions
				initText1.setString(sf::String("You are playing ROCK - PAPER - CISORS - STUMP"));
				initText2.setString(sf::String("Please select your move :\n [1]ROCK\n [2]PAPER\n [3]CISORS\n [4]HAND\n"));
				window.draw(initText1);
				window.draw(initText2);
			}
			// Display all elements
			window.display();

			if (socket.receive(packet) == sf::Socket::Done)
			{
				packet >> receivedPacket;
				if (receivedPacket.type == sts::PacketType::GAME)
				{
					clientState = sts::ClientState::END;
					shouldSendPacket = true;
				}
				std::cout << "Client GAME received\n";
			}

		}
			break;

		case sts::ClientState::END:
		{
			if (shouldSendPacket && socket.send(packet) == sf::Socket::Done) 
			{
				shouldSendPacket = false;
				packet.clear();
				std::cout << "Client END packet sent\n";
			}

			if (socket.receive(packet) == sf::Socket::Done)
			{
				packet >> endPacket;
				result = endPacket.result;
				std::cout << "Client END received\n";

				switch (result)
				{
				case sts::Result::WON:
					endText.setFillColor(sf::Color::Green);
					endText.setString(sf::String("Yay ! You have won !"));
					break;

				case sts::Result::LOST:
					endText.setFillColor(sf::Color::Red);
					endText.setString(sf::String("Too bad, maybe more luck next time ! ;)"));
					break;

				case sts::Result::DRAW:
					endText.setFillColor(sf::Color::Color(255, 165, 0, 255));
					endText.setString(sf::String("DRAW"));
					break;

				default:
					endText.setString("???");
					break;
				}
			}
			window.clear();
			window.draw(endText);
			window.display();
		}
			break;

		default:
		{
			text.setFillColor(sf::Color::Red);
			text.setCharacterSize(50);
			text.setString(sf::String("Something went wrong..."));

			window.clear();
			window.draw(text);
			window.display();
		}
		}
	}
	return EXIT_SUCCESS;
}



