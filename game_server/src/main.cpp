#include <iostream>
#include <SFML/Network.hpp>
#include "Packet.h"

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

    auto serverState = sts::PacketType::NONE;

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
    serverState = sts::PacketType::INIT;

#pragma endregion
#pragma region GAME

    client1.setBlocking(false);
    client2.setBlocking(false);

    sf::Packet packet;
    bool hasWon = false;

    sts::Packet initPacket(sts::PacketType::INIT);
    sts::Packet gamePacket(sts::PacketType::GAME);
    sts::Packet endPacket(sts::PacketType::END);

    while (true) 
    {
        if (client1.receive(packet) == sf::Socket::Done) 
        {
            sts::Packet serverPacket(sts::PacketType::NONE);
            packet >> serverPacket;
            serverState = serverPacket.type;
        };
        if (client2.receive(packet) == sf::Socket::Done)
        {
            sts::Packet serverPacket(sts::PacketType::NONE);
            packet >> serverPacket;
            serverState = serverPacket.type;
        };

        switch (serverState)
        {
        case sts::PacketType::INIT:
            
            packet.clear();
            packet << initPacket;
            client1.send(packet);
            client2.send(packet);

            break;

        case sts::PacketType::GAME:

            packet.clear();
            packet << gamePacket;
            client1.send(packet);
            client2.send(packet);

            break;

        case sts::PacketType::END:

            packet.clear();
            packet << endPacket;
            client1.send(packet);
            client2.send(packet);

            break;

        default:
            break;
        }

        ////If the client is disconnected, send winning state to the other one
        //if (client1.send(packet) != sf::Socket::Status::Done)
        //{
        //    clientCount = 1;
        //    selector.remove(client1);
        //    std::cerr<< "Client with port: " << client1.getRemotePort() << " has disconnected" << std::endl;
        //    packet.clear();
        //    sts::Packet endPacket(sts::PacketType::END);
        //    hasWon = true;
        //    packet << endPacket;
        //    client2.send(packet);
        //};
        //if (client2.send(packet) != sf::Socket::Status::Done)
        //{
        //    clientCount = 1;
        //    selector.remove(client2);
        //    std::cerr << "Client with port: " << client1.getRemotePort() << " has disconnected" << std::endl;
        //    packet.clear();
        //    sts::Packet endPacket(sts::PacketType::END);
        //    hasWon = true;
        //    packet << endPacket;
        //    client1.send(packet);
        //};
    }
   

    

    //while (true) 
    //{
    //    //TODO: Has to be a GAME(type) Packet
    //    sf::Packet canPlayPacket;
    //    bool player1Turn = true;
    //    bool canPlay = true;
    //    
    //    while (player1Turn) 
    //    {
    //        canPlayPacket << canPlay;
    //        client1.send(canPlayPacket);

    //        // The client has sent some data, we can receive it
    //        sf::Packet packet;
    //        if (client1.receive(packet) == sf::Socket::Done)
    //        {
    //            //Data to send
    //            std::string data;
    //            packet >> data;
    //            std::cout << data;
    //            player1Turn = false;
    //        }
    //    }
    //    while (!player1Turn)
    //    {
    //        canPlayPacket.clear();
    //        canPlayPacket << canPlay;
    //        client2.send(canPlayPacket);

    //        sf::Packet packet;
    //        if (client2.receive(packet) == sf::Socket::Done)
    //        {
    //            std::string data;
    //            packet >> data;
    //            std::cout << data;
    //            player1Turn = true;
    //        }
    //    }
    //}

#pragma endregion
    
    //End
    std::cout << "Server has stopped working" << std::endl;
    return EXIT_SUCCESS;
}