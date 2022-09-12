#include <iostream>
#include "SFML/Network.hpp"
#include <list>

//Game_Server
int main() 
{
    //Listener listens to incomming connections
    sf::TcpListener listener;
    //The port to listen
    listener.listen(12345);

    //list of Sockets
    sf::TcpSocket client1;
    sf::TcpSocket client2;

    //Selector
    sf::SocketSelector selector;
    //Add the listener to the selector
    selector.add(listener);
    //Client count to assign IDs to clients
    int clientCount = 0;

#pragma region INIT

    while (clientCount < 2)
    {
        // Make the selector wait for data on any socket
        if (selector.wait())
        {
            // Test the listener
            if (selector.isReady(listener))
            {
                // The listener is ready: there is a pending connection
                if (listener.accept(client1) == sf::Socket::Done)
                {
                    // Add the new client to the selector so that we will
                    // be notified when he sends something
                    selector.add(client1);
                    clientCount++;
                    std::cout << "Connection established with: " << client1.getRemoteAddress() << " as client1." << std::endl;
                }
                if (listener.accept(client2) == sf::Socket::Done) 
                {
                    selector.add(client2);
                    clientCount++;
                    std::cout << "Connection established with: " << client1.getRemoteAddress() << " as client2." << std::endl;
                }
            }
        }
    }
    //Send validation to clients for GAME PHASE
    //TODO: Packet for validation (Ex: Packet>>INIT(type)>>Validation)

#pragma endregion
#pragma region GAME

    while (true) 
    {
        //TODO: Has to be a GAME(type) Packet
        sf::Packet canPlayPacket;
        bool player1Turn = true;
        bool canPlay = true;
        
        while (player1Turn) 
        {
            canPlayPacket << canPlay;
            client1.send(canPlayPacket);

            // The client has sent some data, we can receive it
            sf::Packet packet;
            if (client1.receive(packet) == sf::Socket::Done)
            {
                //Data to send
                std::string data;
                packet >> data;
                std::cout << data;
                player1Turn = false;
            }
        }
        while (!player1Turn)
        {
            canPlayPacket.clear();
            canPlayPacket << canPlay;
            client2.send(canPlayPacket);

            sf::Packet packet;
            if (client2.receive(packet) == sf::Socket::Done)
            {
                std::string data;
                packet >> data;
                std::cout << data;
                player1Turn = true;
            }
        }
    }

#pragma endregion
    
    //End
    std::cout << "Server has stopped working" << std::endl;
    return EXIT_SUCCESS;
}