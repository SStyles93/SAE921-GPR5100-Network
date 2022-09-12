#include <iostream>
#include "SFML/Network.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window//Event.hpp"

//Game_Client
int main()
{
#pragma region Connection (INIT)

    sf::TcpSocket socket;
    sf::Socket::Status status = socket.connect(sf::IpAddress::getLocalAddress(), 12345);

    sf::RenderWindow window;

    if (status == sf::Socket::Done)
    {
        std::cout << "Connection established with: " << socket.getRemoteAddress() << std::endl;
        
        //Create sfml window once the connection is done
        window.create(sf::VideoMode(1280, 1024), "OnlineGame");
        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60.0f);
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

#pragma region Game

        bool canPlay = false;
        while (!canPlay) 
        {
#pragma region Network

            sf::Packet canPlayPacket;
            if (socket.receive(canPlayPacket) == sf::Socket::Status::Done) 
            {
                canPlayPacket >> canPlay;
                std::cout << "The client with port " << socket.getLocalPort() << " can play" << std::endl;
            }

#pragma endregion
#pragma region Event

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
#pragma endregion
#pragma region Draw

            // Clear all elements from background
            window.clear();

            //TODO: Draw

            // Display all elements
            window.display();
            //TODO: Draw

#pragma endregion
        }


        while (canPlay)
        {
#pragma region Network

            sf::Packet dataToSend;
            std::string stringToSend;
            std::cin >> stringToSend;
            dataToSend << stringToSend;
            if (socket.send(dataToSend) == sf::Socket::Done)
            {
                //Error
                canPlay = false;
            }

#pragma endregion
#pragma region Event

            sf::Event event;
            window.setKeyRepeatEnabled(false);
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
                //TODO: if the game is running
                if (true)
                {
                    // Keyboard events (TODO)
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
                    {
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
                    {
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
                    {
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
                    {
                    }
                }
                //TODO: if the game is over
                else if (!true)
                {
                    if (event.type == sf::Event::KeyReleased)
                    {
                        if (event.key.code == sf::Keyboard::Space)
                        {
                            //reset Game States
                        }
                    }

                }
            }

#pragma endregion
#pragma region Draw

            // Clear all elements from background
            window.clear();

            //TODO: Draw

            // Display all elements
            window.display();
            //TODO: Draw

#pragma endregion
        }
    }

#pragma endregion

    return EXIT_SUCCESS;
}