#include "serializer.h"

using namespace std;

std::string serialize(const std::vector<unsigned int> &numbers, unsigned int sum) {
    string reply;
    reply += to_string(numbers.size()) + " "s;
    for (auto number : numbers) {
        reply += to_string(number) + " "s;
    }
    reply += to_string(sum);
    return reply;
}
