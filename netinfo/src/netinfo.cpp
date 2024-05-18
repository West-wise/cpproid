#include "netinfo.h"

NetInterface* NetInterface::_instance = nullptr;

void NetInterface::getInterfaceNameList(std::set<std::string>* _if_name) {
    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    // 모든 네트워크 인터페이스 가져오기
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cout << "pcap_findalldevs error: " << errbuf << std::endl;
        return;
    }

    // 네트워크 인터페이스 목록 순회
    for (pcap_if_t *d = alldevs; d != NULL; d = d->next) {
        // 인터페이스 이름 저장
        _if_name->insert(d->name);
    }

    // 네트워크 인터페이스 목록 해제
    pcap_freealldevs(alldevs);
}

std::unordered_map<std::string, std::string> NetInterface::getInterfaceMac() {
    std::unordered_map<std::string, std::string> result;
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    for (const auto& interface : _if_name) {
        strcpy(ifr.ifr_name, interface.c_str());
        if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1) {
            unsigned char* mac = reinterpret_cast<unsigned char*>(ifr.ifr_hwaddr.sa_data);
            char mac_str[18];
            sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            result[interface] = mac_str;
        }
    }

    close(fd);
    return result;
}

std::unordered_map<std::string, std::string> NetInterface::getInterfaceIp() {
    std::unordered_map<std::string, std::string> result;
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    for (const auto& interface : _if_name) {
        strcpy(ifr.ifr_name, interface.c_str());
        if (ioctl(fd, SIOCGIFADDR, &ifr) != -1) {
            struct sockaddr_in* ipaddr = reinterpret_cast<struct sockaddr_in*>(&ifr.ifr_addr);
            result[interface] = inet_ntoa(ipaddr->sin_addr);
        }
    }

    close(fd);
    return result;
}

std::unordered_map<std::string, std::string> NetInterface::getInterfaceMask() {
    std::unordered_map<std::string, std::string> result;
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    for (const auto& interface : _if_name) {
        strcpy(ifr.ifr_name, interface.c_str());
        if (ioctl(fd, SIOCGIFNETMASK, &ifr) != -1) {
            struct sockaddr_in* mask = reinterpret_cast<struct sockaddr_in*>(&ifr.ifr_netmask);
            result[interface] = inet_ntoa(mask->sin_addr);
        }
    }

    close(fd);
    return result;
}


#ifdef _WIN32
void printWindowsNetworkInfo() {
    ULONG bufferSize = 0;
    GetAdaptersInfo(NULL, &bufferSize);
    PIP_ADAPTER_INFO adapterInfo = (PIP_ADAPTER_INFO) malloc(bufferSize);
    GetAdaptersInfo(adapterInfo, &bufferSize);

    PIP_ADAPTER_INFO currentAdapter = adapterInfo;
    while (currentAdapter) {
        std::cout << currentAdapter->AdapterName << "\t";
        printMacAddress(currentAdapter->Address);
        std::cout << "\t" << currentAdapter->IpAddressList.IpAddress.String << "\t";
        std::cout << currentAdapter->IpAddressList.IpMask.String << std::endl;
        currentAdapter = currentAdapter->Next;
    }

    free(adapterInfo);
}
#else
void NetInterface::printInterfaceInfo() {
    // 네트워크 인터페이스 정보 수집
    std::unordered_map<std::string, std::string> macs = getInterfaceMac();
    std::unordered_map<std::string, std::string> ips = getInterfaceIp();
    std::unordered_map<std::string, std::string> masks = getInterfaceMask();

    // 출력
    std::cout << "NAME\t\tMAC\t\t\tIP\t\t\tMASK\n";
    for (const auto& interface : _if_name) {
        std::cout << interface << "\t\t";
        
        // MAC 주소 출력
        auto mac_it = macs.find(interface);
        if (mac_it != macs.end()) {
            std::cout << mac_it->second << "\t";
        } else {
            std::cout << "[NOT AVAILABLE]\t\t";
        }

        // IP 주소 출력
        auto ip_it = ips.find(interface);
        if (ip_it != ips.end()) {
            std::cout << ip_it->second << "\t\t";
        } else {
            std::cout << "[NOT AVAILABLE]\t\t";
        }

        // 마스크 출력
        auto mask_it = masks.find(interface);
        if (mask_it != masks.end()) {
            std::cout << mask_it->second << "\n";
        } else {
            std::cout << "[NOT AVAILABLE]\n";
        }
    }
}
#endif
