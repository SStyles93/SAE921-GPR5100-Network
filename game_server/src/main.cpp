#include <iostream>
#include "SFML/Network.hpp"

//Game_Server
int main() 
{
    sf::TcpListener listener;
    if (listener.listen(12345) != sf::Socket::Done)
    {
        //Error...
        return EXIT_FAILURE;
    }

    std::vector<sf::TcpListener> clients;
    for (int i = 0; i < clients.size(); i++)
    {
        
    }

    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Done)
    {
        //Error...
        return EXIT_FAILURE;
    }
}