#pragma once

#include <string>
#include <memory>

class NetInterface {
public:
    virtual bool send(const std::string &msg) const = 0;

    virtual std::string receive() const = 0;

    virtual ~NetInterface() = default;
};

class NetInterfaceFactory {
public:
    static std::unique_ptr<NetInterface> create(int socket);
};