#include <cassert>
#include <cstring>
#include "Server.h"

namespace Inet {

    Address::Address(u8 a, u8 b, u8 c, u8 d, u16 port)
            : a(a)
            , b(b)
            , c(c)
            , d(d)
            , port_(port)
    {}

    Address::Address(u32 ip, u16 port)
            : port_(port) {
        u32 mask = ~static_cast<u8>(0);
        d = ip & mask;
        mask <<= 8;
        c = ip & mask;
        mask <<= 8;
        b = ip & mask;
        mask <<= 8;
        a = ip & mask;
    }

    u32 Address::ip() const {
        return (a << 24) | (b << 16) | (c << 8) | d;
    }

    u16 Address::port() const {
        return port_;
    }

    Socket::Socket(u16 port)
            : sock_(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
            , port_(port) {
        assert(sock_ > 0);

        sockaddr_in sockAddress;
        sockAddress.sin_family = AF_INET;
        sockAddress.sin_addr.s_addr = INADDR_ANY;
        sockAddress.sin_port = htons(static_cast<u16>(port_));

        int res = bind(sock_, reinterpret_cast<sockaddr *>(&sockAddress), sizeof(sockAddress));
        assert(res >= 0);

        res = fcntl(sock_, F_SETFL, O_NONBLOCK, 1);
        assert(res != -1);
    }

    Socket::~Socket() {
        close(sock_);
    }

    u16 Socket::port() const {
        return port_;
    }

    bool Socket::send(const Address &dest, const char *data, int dataSize) {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(dest.ip());
        addr.sin_port = htons(dest.port());
        int sentBytes = sendto(sock_, data, dataSize, 0, (sockaddr *)&addr, sizeof(addr));
        return sentBytes == dataSize;
    }

    bool Socket::receive(Address& sender, char *data, int maxDataSize) {
        sockaddr_in from;
        socklen_t fromLen = sizeof(from);
        int receivedBytes = recvfrom(sock_, data, maxDataSize, 0, (sockaddr *)&from, &fromLen);
        sender = Address(ntohl(from.sin_addr.s_addr), ntohl(from.sin_port));
        return receivedBytes == maxDataSize;
    }

    void InternetConnection::setPress(const std::function<void(Utilities::Direction)> &f) {
        press = f; // to pay respect
    }

    void InternetConnection::setRelease(const std::function<void(Utilities::Direction)> &f) {
        release = f;
    }

    void InternetConnection::setClick(const std::function<void(int, Utilities::ButtonPurpose)> &f) {
        click = f;
    }

    std::vector<char> InternetConnection::buildPacket(bool isPressed, Utilities::Direction dir) { //!pressed=rlsed TODO enum
        std::vector<char> packet(PACKET_SIZE, 0);
        if (isPressed) {
            packet[0] = 1;
        } else {
            packet[0] = 2;
        }
        if (dir == Utilities::Direction::UP) {
            packet[1] = 1;
        } else
        if (dir == Utilities::Direction::LEFT) {
            packet[1] = 2;
        } else
        if (dir == Utilities::Direction::DOWN) {
            packet[1] = 3;
        } else
        if (dir == Utilities::Direction::RIGHT) {
            packet[1] = 4;
        }
        return packet;
    }

    std::vector<char> InternetConnection::buildPacket(Utilities::ButtonPurpose purpose) {
        std::vector<char> packet(PACKET_SIZE, 0);
        packet[0] = 3;
        if (purpose == Utilities::ButtonPurpose::CUSTOMIZE) {
            packet[1] = 1;
        } else if (purpose == Utilities::ButtonPurpose::READY) {
            packet[1] = 2;
        } else if (purpose == Utilities::ButtonPurpose::BACK) {
            packet[1] = 3;
        }
        return packet;
    }

Server::Server(u16 port) : socket_(port) {}

    int Server::id() const {
        return 0;
    }

    void Server::connect(const Address& addr) {
        connections_.push_back(addr);
        socket_.send(addr, buildPacket(PacketType::INIT, connections_.size()).data());
    }

/*
 * Тут kinda документация к тому, какие пакеты перекидываются, и что они делают и вообещпывлиыволрмифвылодмв
 * В будущем планируется, что тут будет проверка того, что все пакеты дошли, но это потом
 * По глубинам байт:
 *   0 | 1 | 2...
 * | 0 | - начальный коммит
 * * * | 1 + 2 | - порт, little endian беззнаковое 16-битное число
 * | 1 | - кнопка нажата
 * * * | 1 | - нажата кнопка "Вверх"
 * * * | 2 | - нажата кнопка "Влево"
 * * * | 3 | - нажата кнопка "Вниз"
 * * * | 4 | - нажата кнопка "Вправо
 * | 2 | - кнопка отпущена
 * * * | 1 | - отпущена кнопка "Вверх"
 * * * | 2 | - отпущена кнопка "Влево"
 * * * | 3 | - отпущена кнопка "Вниз"
 * * * | 4 | - отпущена кнопка "Вправо"
 * | 3 | - клик в меню
 * * * | 1 | - кликнута кнопка "customize"
 * * * | 2 | - кликнута кнопка "ready"
 * * * | 3 | - кликнута кнопка "back"
 * | X | - id игрока, пока все id считаются равными 1, ибо пока пилю под 2 игрока // хотя лучше определять это у себя по порту
 * Впилить сюда меню
 */

    bool Server::receive(int dataMaxSize) { // TODO получив, разослать всем, но пока 2 игрока - не нужно
        char packet[dataMaxSize];
        Address sender;
        if (socket_.receive(sender, packet)) {
            if (packet[0] == 0) { // init
                u16 port{};
                port += static_cast<u8>(packet[1]);
                port += static_cast<u16>(packet[2]) << 8;
                connect({ 127, 0, 0, 1, port });
            } else if (packet[0] == 1) { // pressed
                if (InternetConnection::press) {
                    if (packet[1] == 1) { // UP
                        InternetConnection::press(Utilities::Direction::UP);
                    } else if (packet[1] == 2) { // LEFT
                        InternetConnection::press(Utilities::Direction::LEFT);
                    } else if (packet[1] == 3) { // DOWN
                        InternetConnection::press(Utilities::Direction::DOWN);
                    } else if (packet[1] == 4) { // RIGHT
                        InternetConnection::press(Utilities::Direction::RIGHT);
                    }
                }
            } else if (packet[0] == 2) { // released
                if (InternetConnection::release) {
                    if (packet[1] == 1) { // UP
                        InternetConnection::release(Utilities::Direction::UP);
                    } else if (packet[1] == 2) { // LEFT
                        InternetConnection::release(Utilities::Direction::LEFT);
                    } else if (packet[1] == 3) { // DOWN
                        InternetConnection::release(Utilities::Direction::DOWN);
                    } else if (packet[1] == 4) { // RIGHT
                        InternetConnection::release(Utilities::Direction::RIGHT);
                    }
                }
            } else if (packet[0] == 3) {
                if (InternetConnection::click) {
                    if (packet[1] == 1) {
                        InternetConnection::click(0, Utilities::ButtonPurpose::CUSTOMIZE);
                    } else if (packet[1] == 2) {
                        InternetConnection::click(0, Utilities::ButtonPurpose::READY);
                    } else if (packet[1] == 3) {
                        InternetConnection::click(0, Utilities::ButtonPurpose::BACK);
                    }
                }
            }
            return true;
        } else {
            return false;
        }
    }

    std::vector<char> Server::buildPacket(PacketType type, int id) {
        std::vector<char> packet(PACKET_SIZE, 0);
        if (type == PacketType::INIT) {
            packet[0] = 0;
            packet[1] = id;
        } else {
            assert(false);
        }
        return packet;
    }

    void Server::send(const char *data, int dataSize) {
        for (auto& addr : connections_) {
            socket_.send(addr, data, dataSize);
            // войти в стиль соколова и поставить тонну ассертов
        }
    }

    std::vector<char> Server::buildPacket(PacketType type) {
        return {};
    }

void Server::update_positions(const std::vector<float>& data, int dataSize) {
        char packet[dataSize];
        packet[0] = 4;
        for (int i = 0; i < data.size(); i++){
            memcpy(&packet[1 + i * sizeof(float)], (char *) &data[i], sizeof(float));
        }
        send(packet);
}

Client::Client(u16 myPort) : socket_(myPort) {};


    int Client::id() const {
        return id_;
    }

    void Client::connect(const Address &addr) {
        server_ = addr;
        send(buildPacket(PacketType::INIT).data());
    }

    bool Client::receive(int dataMaxSize) {
        char packet[dataMaxSize];
        Address sender;
        if (socket_.receive(sender, packet)) {
            if (packet[0] == 0) { // init
                id_ = packet[1];
            } else if (packet[0] == 1) { // pressed
                if (InternetConnection::press) {
                    if (packet[1] == 1) { // UP
                        InternetConnection::press(Utilities::Direction::UP);
                    } else if (packet[1] == 2) { // LEFT
                        InternetConnection::press(Utilities::Direction::LEFT);
                    } else if (packet[1] == 3) { // DOWN
                        InternetConnection::press(Utilities::Direction::DOWN);
                    } else if (packet[1] == 4) { // RIGHT
                        InternetConnection::press(Utilities::Direction::RIGHT);
                    }
                }
            } else if (packet[0] == 2) { // released
                if (InternetConnection::release) {
                    if (packet[1] == 1) { // UP
                        InternetConnection::release(Utilities::Direction::UP);
                    } else if (packet[1] == 2) { // LEFT
                        InternetConnection::release(Utilities::Direction::LEFT);
                    } else if (packet[1] == 3) { // DOWN
                        InternetConnection::release(Utilities::Direction::DOWN);
                    } else if (packet[1] == 4) { // RIGHT
                        InternetConnection::release(Utilities::Direction::RIGHT);
                    }
                }
            } else if (packet[0] == 3) {
                if (InternetConnection::click) {
                    if (packet[1] == 1) {
                        InternetConnection::click(id_, Utilities::ButtonPurpose::CUSTOMIZE);
                    } else if (packet[1] == 2) {
                        InternetConnection::click(id_, Utilities::ButtonPurpose::READY);
                    } else if (packet[1] == 3) {
                        InternetConnection::click(id_, Utilities::ButtonPurpose::BACK);
                    }
                }
            } else if (packet[0] == 4){
                std::vector<float> positions;
                for (int i = 0; i < 4 /*players amount multiplied by cord amount*/; i++){
                    float coordinate;
                    memcpy(&packet[1 + i * sizeof(float)], (char *)&coordinate, sizeof(float));
                    positions.push_back(coordinate);
                }
                update_positions(positions);
            }
            return true;
        } else {
            return false;
        }
    }

    void Client::send(const char *data, int dataSize) {
        socket_.send(server_, data, dataSize);
    }

    std::vector<char> Client::buildPacket(PacketType type) {
        std::vector<char> packet(PACKET_SIZE, 0);
        if (type == PacketType::INIT) {
            packet[0] = 0;
            packet[1] = socket_.port() % 256;
            packet[2] = (socket_.port() >> 8);
        }
        return packet;
    }

void Client::setUpdatePositions(const std::function<void(std::vector<float>)> &f) {
    update_positions = f;
}

} // end of namespace Inet