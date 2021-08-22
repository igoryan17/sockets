#include "server.h"
#include <thread>

using namespace std;

int main(int argc, char** argv) {
    std::unique_ptr<Server> server_ptr = ServerFactory::create(parseProgramArguments(argc, argv));
    thread worker([&server_ptr] {
        server_ptr->start();
    });
    std::this_thread::sleep_for(std::chrono::seconds (30));
    server_ptr->stop();
    worker.join();
    return 0;
}
