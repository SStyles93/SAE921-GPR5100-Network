#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

//Game_Client
int main()
{
#pragma region Connection (INIT)

    sf::TcpSocket socket;
    sf::Socket::Status status = socket.connect(sf::IpAddress::getLocalAddress(), 12345);
    socket.setBlocking(false);

    bool gameStarted = false;

    sf::RenderWindow window;
    sf::Font font;
    sf::Text initText;


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
        initText.setFont(font);
        initText.setFillColor(sf::Color::White);
        initText.setCharacterSize(50);
        //text.setString(sf::String("Connection established with : " + socket.getRemoteAddress().toString() + "\n" + "Waiting for a second Player..."));
        initText.setString(sf::String("Connection established with the server.\nWaiting for a second Player..."));
        initText.setPosition(sf::Vector2f(window.getSize().x * 0.0f, window.getSize().y * 0.0f));


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
        //----------------------------------------------------------------------------------LOBBY PHASE
        while (!gameStarted)
        {
#pragma region Network

            //TODO: Receive a INIT(type) packet 
            //and set "gameStarted" to true

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
#pragma region Graphical

            // Clear all elements from background
            window.clear();

            //Draw
            window.draw(initText);

            // Display all elements
            window.display();

#pragma endregion   
        }

        //----------------------------------------------------------------------------------GAME PHASE
        bool canPlay = false;
        
        //-------------------------------------------------Other Client
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
#pragma region Graphical

            // Clear all elements from background
            window.clear();

            //TODO: Draw

            // Display all elements
            window.display();

#pragma endregion
        }

        //-------------------------------------------------This Client
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
                // Keyboard events (TODO)
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1))
                {
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2))
                {
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3))
                {
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4))
                {
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
                {

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
        }
    }

#pragma endregion

    //---------------------------------------------------------------------------------------END PHASE

    return EXIT_SUCCESS;
}