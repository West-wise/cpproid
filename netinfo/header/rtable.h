#pragma once
#include <iostream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#include <iphlpapi.h>
#include <vector>
#pragma comment(lib, "iphlpapi.lib")
#else
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#endif

class RoutingTable {
private:
    RoutingTable() {}
    virtual ~RoutingTable() {}

    static RoutingTable* _instance;

public:
    static RoutingTable* getInstance() {
        if (_instance == nullptr) {
            _instance = new RoutingTable();
        }
        return _instance;
    }

    void printRoutingTable();

#ifdef _WIN32
    std::string convertIpToString(DWORD ip);
#else
    std::string convertHexToIp(const std::string& hex);
#endif
};
