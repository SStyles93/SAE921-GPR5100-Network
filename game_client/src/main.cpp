#include <iostream>
#include "SFML/Network.hpp"

//Game_Client
int main()
{
    sf::TcpSocket socket;
    sf::Socket::Status status = socket.connect(sf::IpAddress::getLocalAddress(), 12345);
    if (status != sf::Socket::Done)
    {
        //Error...
        return EXIT_FAILURE;
    }
}