#include "parser.h"

#include <cassert>
#include <getopt.h>
#include <string>
#include <stdexcept>
#include <regex>
#include <optional>

using namespace std;

ParsedData parseProgramArguments(int argc, char** argv) {
    optional<SupportedProtocols> proto;
    optional<string> host;
    optional<uint16_t> port;
    static struct option long_options[] = {
            {"proto", 1, nullptr, 0},
            {"host",  1, nullptr, 0},
            {"port",  1, nullptr, 0},
            {nullptr,       0, nullptr, 0}
    };
    while (true) {
        int index;
        int c = getopt_long(argc, argv, "", long_options, &index);
        if (c == -1) {
            break;
        }
        switch (index) {
            case 0:
                if (string{optarg} == "TCP"s) {
                    proto = SupportedProtocols::TCP;
                } else if (string{optarg} == "UDP"s) {
                    proto = SupportedProtocols::UDP;
                } else {
                    throw std::invalid_argument("unknown protocol"s);
                }
                break;
            case 1:
                host = string{optarg};
                break;
            case 2:
                port = stoul(string{optarg});
                break;
            default:
                throw invalid_argument("unknown argument"s);
        };
    }
    assert(proto);
    assert(host);
    assert(port);
    return ParsedData{*proto, *host, *port};
}

std::vector<unsigned int> parseNumbers(std::string request) {
    static const regex number_regex("([0-9]+)");
    std::vector<unsigned int> res;
    std::smatch matches;
    while (std::regex_search(request, matches, number_regex)) {
        res.push_back(stoul(matches.str()));
        request = matches.suffix();
    }
    return res;
}

std::ostream &operator<<(ostream &os, const SupportedProtocols proto) {
    if (proto == SupportedProtocols::TCP) {
        os << "TCP"s;
    } else if (proto == SupportedProtocols::UDP) {
        os << "UDP"s;
    } else {
        os << "unknown"s;
    }
    return os;
}

