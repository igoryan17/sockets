#pragma once

#include <string>
#include <vector>
#include <ostream>

enum class SupportedProtocols {
    TCP, UDP
};

std::ostream &operator<<(std::ostream &os, const SupportedProtocols proto);

struct ParsedData {
    SupportedProtocols protocol;
    std::string host;
    uint16_t port;
};

ParsedData parseProgramArguments(int argc, char** argv);

std::vector<unsigned int> parseNumbers(std::string request);