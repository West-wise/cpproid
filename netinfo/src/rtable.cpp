#include "rtable.h"

RoutingTable* RoutingTable::_instance = nullptr;
void RoutingTable::printRoutingTable() {
    std::ifstream file("/proc/net/route");
    if (!file.is_open()) {
        std::cerr << "Failed to open /proc/net/route" << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // Skip the first line (header)

    std::cout << "IfName\tDestination\tGateway\t\tGenmask" << std::endl;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string iface, destination, gateway, flags, refcnt, use, metric, mask, mtu, window, irtt;

        iss >> iface >> destination >> gateway >> mask;

        std::cout << iface << "\t"
                    << convertHexToIp(destination) << "\t"
                  << convertHexToIp(gateway) << "\t\t"
                  << convertHexToIp(mask) << std::endl;
    }
}

std::string RoutingTable::convertHexToIp(const std::string& hex) {
    unsigned int ip;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> ip;

    return std::to_string((ip >> 0) & 0xFF) + "." +
           std::to_string((ip >> 8) & 0xFF) + "." +
           std::to_string((ip >> 16) & 0xFF) + "." +
           std::to_string((ip >> 24) & 0xFF);
}

