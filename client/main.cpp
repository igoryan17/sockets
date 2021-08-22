#include <iostream>
#include "client.h"

using namespace std;

void printResult(const pair<std::vector<unsigned int>, unsigned int> &result) {
    bool first = true;
    for (auto number : result.first) {
        if (first) {
            cout << number;
            first = false;
        } else {
            cout << " "s << number;
        }
    }
    cout << "\n" << result.second << endl;
}

int main(int argc, char **argv) {
    Client client(parseProgramArguments(argc, argv));
    string data;
    getline(cin, data);
    printResult(client.sendMessage(data));
    return 0;
}
