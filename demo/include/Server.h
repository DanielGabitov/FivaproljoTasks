#pragma once

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#include <fcntl.h>

#include <vector>
#include <optional>
#include <iostream>
#include <functional>

#include <unistd.h>

#include "Utilities.h"

namespace Inet {

    constexpr int PACKET_SIZE = 256;

    using namespace Utilities;

    using u32 = unsigned int;
    using u16 = unsigned short;
    using u8 = unsigned char;

    struct Address {
        Address() = default;
        Address& operator=(const Address &) = default;
        Address(u8 a, u8 b, u8 c, u8 d, u16 port);
        Address(u32 ip, u16 port);
        u32 a, b, c, d;
        u16 port_;
        u32 ip() const;
        u16 port() const;
    };

    class Socket {
    public:
        Socket(u16 port);
        ~Socket();
        u16 port() const;
        bool send(const Address &dest, const char *data, int dataSize = PACKET_SIZE);
        bool receive(Address &sender, char *data, int maxDataSize = PACKET_SIZE);

    private:
        int sock_;
        u16 port_;
    };

    class InternetConnection {
    public:
        virtual void connect(const Address &) = 0;
        virtual bool receive(int dataMaxSize = PACKET_SIZE) = 0;
        virtual void send(const char *data, int dataSize = PACKET_SIZE) = 0;
        virtual int id() const = 0;
        void setPress(const std::function<void(Utilities::Direction)> &f);
        void setRelease(const std::function<void(Utilities::Direction)> &f);
        void setClick(const std::function<void(int, Utilities::ButtonPurpose)> &f);
        static std::vector<char> buildPacket(bool isPressed, Utilities::Direction dir);
        static std::vector<char> buildPacket(Utilities::ButtonPurpose purpose);
        virtual std::vector<char> buildPacket(PacketType type) = 0;

    protected:
        std::function<void(Utilities::Direction)> press;
        std::function<void(Utilities::Direction)> release;
        std::function<void(int, Utilities::ButtonPurpose)> click;
    };

    class Server : public InternetConnection {
    public:
        explicit Server(u16 port = 5051); // 5051 - порт сервера
        int id() const override;
        void connect(const Address &addr) override;
        bool receive(int dataMaxSize = PACKET_SIZE) override; // хэндлим прям там
        void send(const char *data, int dataSize = PACKET_SIZE) override;
        std::vector<char> buildPacket(PacketType type) override;
        std::vector<char> buildPacket(PacketType type, int id);
        void update_positions(const std::vector<float>& data, int dataSize = PACKET_SIZE);

    private:
        void *controller_ = nullptr;
        Socket socket_;
        std::vector<Address> connections_;
    };

    class Client : public InternetConnection {
    public:
        explicit Client(u16 myPort = 5052);
        int id() const override;
        void setUpdatePositions(const std::function<void(std::vector<float>)> &f);
        void connect(const Address &addr) override;
        bool receive(int dataMaxSize = PACKET_SIZE) override;
        void send(const char *data, int dataSize = PACKET_SIZE) override;
        std::vector<char> buildPacket(PacketType type) override;
    private:
        std::function<void(std::vector<float>)> update_positions;
        int id_ = 0; // zero means not connected
        Socket socket_;
        Address server_;
    };

} // end of namespace Inet