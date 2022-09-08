#include <iostream>
#include "SFML/Network.hpp"

//Client
int main()
{
    sf::TcpSocket socket;
    sf::Socket::Status status = socket.connect(sf::IpAddress::getLocalAddress(), 12345);
    if (status != sf::Socket::Done)
    {
        //Error...
    }

    do
    {
        char dataToSend[100];
        std::cin >> dataToSend;
        // Push to the server
        if (socket.send(dataToSend, sizeof(dataToSend)) != sf::Socket::Done)
        {
            // Error
        }

        char dataToRecieve[100];
        size_t recievedSize;
        if (socket.receive(dataToRecieve, sizeof(dataToRecieve), recievedSize) != sf::Socket::Done)
        {
            //Error
        }
        //Print ReceivedData
        std::cout << dataToRecieve;

    } while (true);

    return EXIT_SUCCESS;
}