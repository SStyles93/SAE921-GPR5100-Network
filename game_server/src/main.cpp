#include <iostream>
#include "SFML/Network.hpp"

//Game_Server
int main() 
{
    //Listener listens to incomming connections
    sf::TcpListener listener;
    //Vector of Sockets (client)
    std::vector<sf::TcpSocket> clients;

    if (listener.listen(12345) != sf::Socket::Done)
    {
        //Error...
        return EXIT_FAILURE;
    }

    for (int i = 0; i < clients.size(); i++)
    {
        if (listener.accept(clients[i]) != sf::Socket::Done) 
        {
            //Error
            return EXIT_FAILURE;
        };
    }

    std::cout << "Server has stopped working" << std::endl;
    return EXIT_SUCCESS;
}