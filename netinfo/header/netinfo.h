#pragma once
#include <iostream>
#include <pcap.h>
#include <cstring>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include "mac.h"
#include "ip.h"

class NetInterface {
private:
    static NetInterface* _instance;
    std::set<std::string> _if_name;
    std::unordered_map<std::string, Mac> _if_mac;
    std::unordered_map<std::string, Ip> _if_ip;
    std::unordered_map<std::string, Ip> _if_mask;

    NetInterface() {
        getInterfaceNameList(&_if_name);
    }

    virtual ~NetInterface() {}

public:
    static NetInterface* getInstance() {
        if (_instance == nullptr) {
            _instance = new NetInterface();
        }
        return _instance;
    }

    void getInterfaceNameList(std::set<std::string>* _if_name);
    std::unordered_map<std::string, Mac> getInterfaceMac(const std::set<std::string>& if_set);
    std::unordered_map<std::string, Ip> getInterfaceIp(const std::set<std::string>& if_set);
    std::unordered_map<std::string, Ip> getInterfaceMask(const std::set<std::string>& if_set);
    void printInterfaceInfo();
};
