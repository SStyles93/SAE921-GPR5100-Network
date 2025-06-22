#pragma once
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpListener.hpp>

#include "network_client.h"
#include "server.h"
#include "game/game_globals.h"

namespace game
{
/**
 * \brief ClientInfo is a struct used by a network server to store all needed infos about a client
 */
struct ClientInfo
{
    ClientId clientId = INVALID_CLIENT_ID;
    unsigned long long timeDifference = 0;
    sf::IpAddress udpRemoteAddress;
    unsigned short udpRemotePort = 0;
};

/**
 * \brief NetworkServer is a network server using SFML sockets.
 */
class NetworkServer final : public Server
{
public:
    enum class PacketSocketSource
    {
        TCP,
        UDP
    };

    void SendReliablePacket(std::unique_ptr<Packet> packet) override;

    void SendUnreliablePacket(std::unique_ptr<Packet> packet) override;

    void Begin() override;

    void Update(sf::Time dt) override;

    void End() override;

    void SetTcpPort(unsigned short i);

    [[nodiscard]] bool IsOpen() const;
    
protected:
    void SpawnNewPlayer(ClientId clientId, PlayerNumber playerNumber) override;

private:
    void ProcessReceivePacket(std::unique_ptr<Packet> packet,
        PacketSocketSource packetSource,
        sf::IpAddress address = "localhost",
        unsigned short port = 0);
    void ReceiveNetPacket(sf::Packet& packet, PacketSocketSource packetSource,
                          sf::IpAddress address = "localhost",
                          unsigned short port = 0);

    enum ServerStatus
    {
        OPEN = 1u << 0u,
        STARTED = 1u << 1u,
        FIRST_PLAYER_CONNECT = 1u << 2u,
    };
    sf::UdpSocket udpSocket_;
    sf::TcpListener tcpListener_;
    std::array<sf::TcpSocket, MAX_PLAYER_NMB> tcpSockets_;

    std::array<ClientInfo, MAX_PLAYER_NMB> clientInfoMap_{};


    unsigned short tcpPort_ = 12345;
    unsigned short udpPort_ = 12345;
    std::uint32_t lastSocketIndex_ = 0;
    std::uint8_t status_ = 0;

#ifdef ENABLE_SQLITE
    DebugDatabase db_;
#endif
};
}
