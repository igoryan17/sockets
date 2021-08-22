#include "deserializer.h"

#include <regex>

using namespace std;

pair<vector<unsigned int>, unsigned int> deserialize(string raw) {
    stringstream is(raw);
    size_t count;
    is >> count;
    std::vector<unsigned int> numbers;
    numbers.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        unsigned int number;
        is >> number;
        numbers.push_back(number);
    }
    unsigned int sum;
    is >> sum;
    return make_pair(std::move(numbers), sum);
}
