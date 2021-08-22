#include "read_write.h"

#include <sys/socket.h>
#include <cassert>
#include <glog/logging.h>

using namespace std;

class SenderReceiver : public NetInterface {
public:
    SenderReceiver(int sd) : socket_(sd) {}

    bool send(const string &msg) const override {
        ssize_t count = ::send(socket_, msg.data(), msg.length(), 0);
        return count == msg.length();
    }

    string receive() const override {
        char buf[MAX_BUF_SIZE];
        size_t remain_size = MAX_BUF_SIZE;
        LOG(INFO) << "wait to receive message"s;
        ssize_t count = recv(socket_, buf, MAX_BUF_SIZE, 0);
        assert(count >= 0);
        remain_size -= count;
        return string{buf, MAX_BUF_SIZE - remain_size};
    }

    ~SenderReceiver() = default;

protected:
    static const int MAX_BUF_SIZE = 1024;
    int socket_;
};

unique_ptr<NetInterface> NetInterfaceFactory::create(int socket) {
    return make_unique<SenderReceiver>(socket);
}
