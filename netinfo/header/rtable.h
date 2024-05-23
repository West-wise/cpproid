#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>


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

    std::string convertHexToIp(const std::string& hex);

};
