#include "server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <set>
#include <functional>
#include <fcntl.h>
#include <numeric>
#include <unistd.h>
#include "../read_write.h"
#include "../serializer.h"
#include <glog/logging.h>

using namespace std;

Server::Server() : running_(false) {
}

void Server::start() {
    running_ = true;
}

void Server::stop() {
    running_ = false;
}

Server::~Server() {
    close(socket_);
}

void Server::bindSocket(const string &host, uint16_t port) const {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.data(), &addr.sin_addr);
    int bind_res = bind(socket_, (struct sockaddr *) &addr, sizeof(addr));
    assert(bind_res >= 0);
}

string Server::handleRequest(const string &request) const {
    vector<unsigned int> parsed_numbers = parseNumbers(request);
    sort(parsed_numbers.begin(), parsed_numbers.end());
    unsigned int sum = accumulate(parsed_numbers.begin(), parsed_numbers.end(), (unsigned int) 0);
    return serialize(parsed_numbers, sum);
}

TcpServer::TcpServer(const ParsedData &data) : Server() {
    socket_ = socket(PF_INET, SOCK_STREAM, 0);
    assert(socket_ != -1);
    fcntl(socket_, F_SETFL, O_NONBLOCK);
    bindSocket(data.host, data.port);
    int listen_res = listen(socket_, 3);
    assert(listen_res >= 0);
    LOG(INFO) << "server listen on TCP "s << data.host << ':' << data.port;
}

void TcpServer::start() {
    Server::start();
    set<int, std::greater<>> clients;
    fd_set read_set;
    while (running_) {
        FD_ZERO(&read_set);
        FD_SET(socket_, &read_set);
        for (int client_socket : clients) {
            FD_SET(client_socket, &read_set);
        }
        int max_socket = clients.empty() ? socket_ : max(socket_, *clients.begin());
        struct timeval timeout{1, 0};
        VLOG(2) << "wait for clients"s;
        int select_res = select(max_socket + 1, &read_set, nullptr, nullptr, &timeout);
        assert(select_res >= 0);
        if (FD_ISSET(socket_, &read_set)) {
            VLOG(1) << "server socket ready for read, accept new connection"s;
            int sock = accept(socket_, nullptr, nullptr);
            assert(sock >= 0);
            fcntl(sock, F_SETFL, O_NONBLOCK);
            clients.insert(sock);
        }
        auto it = clients.begin();
        while (it != clients.end()) {
            if (FD_ISSET(*it, &read_set)) {
                VLOG(2) << "receive request from client; client_id: "s << *it;
                unique_ptr<NetInterface> net_ptr = NetInterfaceFactory::create(*it);
                const string request = net_ptr->receive();
                string reply = handleRequest(request);
                VLOG(2) << "send reply to client; client_id: "s << *it;
                net_ptr->send(reply);
                close(*it);
                it = clients.erase(it);
            } else {
                ++it;
            }
        }
    }
}

UdpServer::UdpServer(const ParsedData &data) : Server() {
    socket_ = socket(PF_INET, SOCK_DGRAM, 0);
    assert(socket_ != -1);
    fcntl(socket_, F_SETFL, O_NONBLOCK);
    bindSocket(data.host, data.port);
    LOG(INFO) << "server listen on UDP "s << data.host << ':' << data.port;
}

void UdpServer::start() {
    Server::start();
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    fd_set read_set;
    while (running_) {
        FD_ZERO(&read_set);
        FD_SET(socket_, &read_set);
        struct timeval timeout{1, 0};

        int res = select(socket_ + 1, &read_set, nullptr, nullptr, &timeout);
        assert(res >= 0);
        if (FD_ISSET(socket_, &read_set)) {
            VLOG(1) << "server is ready to receive messages"s;
            char buf[MAX_BUFFER_SIZE];
            ssize_t received_count = recvfrom(socket_, buf, MAX_BUFFER_SIZE, 0, (struct sockaddr *) &addr, &len);
            assert(received_count >= 0);
            const string reply = handleRequest(string{buf, static_cast<size_t>(received_count)});
            ssize_t send_count = sendto(socket_, reply.data(), reply.length(), 0, (struct sockaddr *) &addr,
                                        sizeof(addr));
            assert(send_count >= 0);
        } else {
            VLOG(2) << "server is not ready yet to receive message";
        }
    }
}
