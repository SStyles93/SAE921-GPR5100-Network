#include <iostream>
#include "SFML/Network.hpp"

//Echo_Server
int main()
{
    sf::TcpListener listener;
    if (listener.listen(12345) != sf::Socket::Done) 
    {
        //Error...
        return EXIT_FAILURE;
    }

    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Done) 
    {
        //Error...
        return EXIT_FAILURE;
    }

    while (true) 
    {
        char receivedData[100];
        size_t receivedSize;
        if (client.receive(receivedData, sizeof(receivedData), receivedSize) != sf::Socket::Done)
        {
            //Error
            std::cerr << "Error receiving data from: "<<client.getRemoteAddress()<<"." << std::endl;
            break;
        }
        // Print the data
        std::cout << receivedData << std::endl;

        if (client.send(receivedData, sizeof(receivedData)) != sf::Socket::Done) 
        {
            //Error
        }
    }
    
    std::cout << "Server has shut down" << std::endl;
    return EXIT_SUCCESS;
}