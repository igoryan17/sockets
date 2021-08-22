#pragma once

#include "../parser.h"
#include <stdexcept>
#include <memory>

class Server {
public:
    Server();

    virtual void start();

    void stop();

    virtual ~Server();

protected:
    static const int MAX_BUFFER_SIZE = 1024;
    bool running_;

    int socket_;

    void bindSocket(const std::string &host, uint16_t port) const;

    std::string handleRequest(const std::string &request) const;
};

class TcpServer : public Server {
public:
    TcpServer(const ParsedData &data);

    void start() override;

    ~TcpServer() override = default;
};

class UdpServer : public Server {
public:
    UdpServer(const ParsedData &data);

    void start() override;

    ~UdpServer() override = default;
};

class ServerFactory {
public:
    static std::unique_ptr<Server> create(const ParsedData &data) {
        if (data.protocol == SupportedProtocols::TCP) {
            return std::make_unique<TcpServer>(data);
        } else if (data.protocol == SupportedProtocols::UDP) {
            return std::make_unique<UdpServer>(data);
        } else {
            throw std::invalid_argument("unknown protocol");
        }
    }
};
