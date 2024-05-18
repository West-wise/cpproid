#pragma once
#include <iostream>
#include <pcap.h>
#include <cstring>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>

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
    std::unordered_map<std::string, std::string> getInterfaceMac();
    std::unordered_map<std::string, std::string> getInterfaceIp();
    std::unordered_map<std::string, std::string> getInterfaceMask();

    void printInterfaceInfo();
};

// // 정적 멤버 변수 초기화
// NetInterface* NetInterface::_instance = nullptr;
