#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include "Packet.h"

//Game_Client
int main()
{
#pragma region Connection

    sf::TcpSocket socket;
    sf::Socket::Status status = socket.connect(sf::IpAddress::getLocalAddress(), 12345);
    socket.setBlocking(false);

    auto gameState = sts::PacketType::NONE;

    sf::RenderWindow window;
    sf::Font font;
    sf::Text text;


    if (status == sf::Socket::Done)
    {
        std::cout << "Connection established with: " << socket.getRemoteAddress() << std::endl;

        //Create sfml window once the connection is done
        window.create(sf::VideoMode(1280, 1024), "OnlineGame");
        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60.0f);
        //SFML Font loading
        font.loadFromFile("data/font/RetroGaming.ttf");
        //SFML Text Initialisation
        text.setFont(font);
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(50);
        //text.setString(sf::String("Connection established with : " + socket.getRemoteAddress().toString() + "\n" + "Waiting for a second Player..."));
        text.setString(sf::String("Connection established with the server.\nWaiting for a second Player..."));
        text.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.0f));


    }
    else
    {
        //Error...
        std::cerr << "There was a problem connecting with the server." << std::endl;
        window.close();
        return EXIT_FAILURE;

    }

#pragma endregion
#pragma region Game

    while (window.isOpen())
    {
        //Packets
        sf::Packet packet;
        sts::Packet gameStatePacket(sts::PacketType::NONE);
        //Receive
        if (socket.receive(packet) == sf::Socket::Done) 
        {
            packet >> gameStatePacket;
        };
        //Set game state to packet state
        gameState = gameStatePacket.type;
        
        //SFML Event management
        sf::Event event;
        
        //Graphical
        sf::Text initText1;
        sf::Text initText2;
        sf::Text endText;

        bool hasWon = false;

        switch (gameState)
        {
            //----------------------------------------------------------------------------------LOBBY PHASE
        case sts::PacketType::INIT:
#pragma region Event

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
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Space)
                    {
                        packet.clear();
                        gameStatePacket.type = sts::PacketType::GAME;
                        packet << gameStatePacket;
                        socket.send(packet);
                        std::cout << "KeyPressed! " << static_cast<int>(gameStatePacket.type) << std::endl;
                    }
                }
            }

#pragma endregion
#pragma region Graphical

            initText1.setFont(font);
            initText1.setFillColor(sf::Color::White);
            initText1.setCharacterSize(50);
            initText1.setString(sf::String("You are playing ROCK - PAPER - CISORS - STUMP"));
            initText1.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.0f));
            
            
            initText2.setFont(font);
            initText2.setFillColor(sf::Color::Red);
            initText2.setCharacterSize(50);
            initText2.setString(sf::String("Please select your move :\n [1]ROCK\n [2]PAPER\n [3]CISORS\n [4]HAND\n"));
            initText2.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.1f));


            // Clear all elements from background
            window.clear();

            //Draw
            window.draw(text);
            window.draw(initText2);

            // Display all elements
            window.display();

#pragma endregion   
            break;

            //----------------------------------------------------------------------------------GAME PHASE
        case sts::PacketType::GAME:      
#pragma region Network

                /*sf::Packet canPlayPacket;
                if (socket.receive(canPlayPacket) == sf::Socket::Status::Done)
                {
                    canPlayPacket >> canPlay;
                    std::cout << "The client with port " << socket.getLocalPort() << " can play" << std::endl;
                }*/

#pragma endregion
#pragma region Event

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
                    if (event.type == sf::Event::KeyPressed)
                    {
                        if (event.key.code == sf::Keyboard::Space)
                        {
                            packet.clear();
                            gameStatePacket.type = sts::PacketType::END;
                            packet << gameStatePacket;
                            socket.send(packet);
                            std::cout << "KeyPressed! " << static_cast<int>(gameStatePacket.type) << std::endl;
                        }
                    }
                }

#pragma endregion
#pragma region Graphical

                // Clear all elements from background
                window.clear();

                //TODO: Draw

                // Display all elements
                window.display();

#pragma endregion
            break;

            //-----------------------------------------------------------------------------------END PHASE
        case sts::PacketType::END:
#pragma region Network

            packet >> hasWon;

#pragma endregion
#pragma region Event

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
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Space)
                    {
                        packet.clear();
                        gameStatePacket.type = sts::PacketType::NONE;
                        packet << gameStatePacket;
                        socket.send(packet);
                        std::cout << "END OF GAME! " << static_cast<int>(gameStatePacket.type) << std::endl;
                    }
                }
            }

#pragma endregion
#pragma region Graphical

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

#pragma endregion
            break;

        //No packet received, no move
        default:         
#pragma region Event

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

#pragma endregion
#pragma region Graphical

            //SFML Text Initialisation
            text.setFont(font);
            text.setFillColor(sf::Color::White);
            text.setCharacterSize(50);
            //text.setString(sf::String("Connection established with : " + socket.getRemoteAddress().toString() + "\n" + "Waiting for a second Player..."));
            text.setString(sf::String("Connection established with the server.\nWaiting for a second Player..."));
            text.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.0f));

            // Clear all elements from background
            window.clear();

            window.draw(text);

            // Display all elements
            window.display();
            continue;
#pragma endregion
        }
    }
#pragma endregion

    return EXIT_SUCCESS;

}
