#include "client.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>
#include <string_view>
#include "../deserializer.h"
#include <glog/logging.h>

using namespace std;

Client::Client(const ParsedData &data) : sock_(data.protocol == SupportedProtocols::TCP ? createTcpSocket()
                                                                                        : createUdpSocket()) {
    assert(sock_ != -1);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(data.port);
    inet_pton(AF_INET, data.host.data(), &addr.sin_addr);
    // may be connect here if TCP is chosen
    // but if we spent much time between constructor and sendMessage call
    // it will be not effective to maintain TCP session
    // but UDP will not be connected, only gives info about destination
    LOG(INFO) << "connect client socket to server; proto: "s << data.protocol << ", " << data.host << ':' << data.port;
    int con_res = connect(sock_, (struct sockaddr *) &addr, sizeof(addr));
    assert(con_res == 0);
    net_interface_ = NetInterfaceFactory::create(sock_);
}

Client::~Client() {
    close(sock_);
}

pair<vector<unsigned int>, unsigned int> Client::sendMessage(const string &msg) const {
    bool res = net_interface_->send(msg);
    LOG(INFO) << "message is sent: "s << res;
    return deserialize(net_interface_->receive());
}

int Client::createTcpSocket() {
    return socket(PF_INET, SOCK_STREAM, 0);
}

int Client::createUdpSocket() {
    return socket(PF_INET, SOCK_DGRAM, 0);
}
