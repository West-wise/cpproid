#pragma once
#include <iostream>
#include <pcap.h>
#include <cstring>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>

#ifdef _WIN32
    #include <winsock2.h>
    #include <iphlpapi.h>
    #pragma comment(lib, "IPHLPAPI.lib")
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <ifaddrs.h>
    #include <netinet/in.h>
    #include <net/if.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/ioctl.h>
#endif
// NAME        MAC                  IP                  MASK         
// eth0        00:0c:29:50:5e:11    192.168.180.128     255.255.255.0   

class NetInterface {
private:
    static NetInterface* _instance;
    std::set<std::string> _if_name;
    std::unordered_map<std::string, std::string> _if_mac;
    std::unordered_map<std::string, std::string> _if_ip;
    std::unordered_map<std::string, std::string> _if_mask;

    NetInterface() {
        // 초기화 작업 수행
        getInterfaceNameList(&_if_name);
    }

    virtual ~NetInterface() {
        // 정리 작업 수행
    }

public:
    static NetInterface* getInstance() {
        // 인스턴스가 아직 생성되지 않았다면 생성합니다.
        if (_instance == nullptr) {
            _instance = new NetInterface();
        }
        return _instance;
    }

    void getInterfaceNameList(std::set <std::string> *_if_name);
    std::unordered_map<std::string, std::string> getInterfaceMac(std::set<std::string> if_set);
    std::unordered_map<std::string, std::string> getInterfaceIp(std::set<std::string> if_set);
    std::unordered_map<std::string, std::string> getInterfaceMask(std::set<std::string> if_set);
    #ifdef _WIN32
        void printWindowsNetworkInfo()
    #else
        void printInterfaceInfo();
    #endif
};
