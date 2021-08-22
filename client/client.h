#pragma once

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include "../parser.h"
#include "../read_write.h"

class ClientInterface {
public:
    virtual std::pair<std::vector<unsigned int>, unsigned int> sendMessage(const std::string &msg) const = 0;

    virtual ~ClientInterface() = default;
};

class Client : public ClientInterface {
public:
    Client(const ParsedData &data);

    [[nodiscard]] std::pair<std::vector<unsigned int>, unsigned int> sendMessage(const std::string &msg) const override;

    ~Client() override;

private:
    int sock_;
    std::unique_ptr<NetInterface> net_interface_;

    static int createTcpSocket();

    static int createUdpSocket();
};

