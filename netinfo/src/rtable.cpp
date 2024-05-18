#include "rtable.h"

RoutingTable* RoutingTable::_instance = nullptr;

#ifdef _WIN32

void RoutingTable::printRoutingTable() {
    MIB_IPFORWARDTABLE* pIpForwardTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    pIpForwardTable = (MIB_IPFORWARDTABLE*)malloc(sizeof(MIB_IPFORWARDTABLE));
    if (pIpForwardTable == nullptr) {
        std::cerr << "Error allocating memory for IP forward table" << std::endl;
        return;
    }

    if (GetIpForwardTable(pIpForwardTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
        free(pIpForwardTable);
        pIpForwardTable = (MIB_IPFORWARDTABLE*)malloc(dwSize);
        if (pIpForwardTable == nullptr) {
            std::cerr << "Error allocating memory for IP forward table" << std::endl;
            return;
        }
    }

    if ((dwRetVal = GetIpForwardTable(pIpForwardTable, &dwSize, 0)) == NO_ERROR) {
        std::cout << "Destination\tGateway\t\tGenmask\t\tFlags\tMetric\tIface" << std::endl;

        for (int i = 0; i < (int)pIpForwardTable->dwNumEntries; i++) {
            std::cout << convertIpToString(pIpForwardTable->table[i].dwForwardDest) << "\t"
                      << convertIpToString(pIpForwardTable->table[i].dwForwardNextHop) << "\t"
                      << convertIpToString(pIpForwardTable->table[i].dwForwardMask) << "\t"
                      << pIpForwardTable->table[i].dwForwardProto << "\t"
                      << pIpForwardTable->table[i].dwForwardMetric1 << "\t"
                      << pIpForwardTable->table[i].dwForwardIfIndex << std::endl;
        }
    } else {
        std::cerr << "GetIpForwardTable failed with error: " << dwRetVal << std::endl;
    }

    if (pIpForwardTable) {
        free(pIpForwardTable);
        pIpForwardTable = nullptr;
    }
}

std::string RoutingTable::convertIpToString(DWORD ip) {
    struct in_addr ipAddr;
    ipAddr.S_un.S_addr = ip;
    return std::string(inet_ntoa(ipAddr));
}

#else

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

#endif
