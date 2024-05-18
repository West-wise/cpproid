#include <iostream>
#include <pcap.h>
#include <cstring>
#include "netinfo.h"

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

void printMacAddress(const u_char* mac) {
    for(int i = 0; i < 6; ++i) {
        printf("%02x", mac[i]);
        if(i < 5) printf(":");
    }
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
void printLinuxNetworkInfo() {
    // 네트워크 인터페이스 정보를 저장하는 구조체 포인터
    struct ifaddrs *ifaddr, *ifa;
    // 주소 패밀리와 함수 반환값을 저장할 변수
    int family, s;
    // 호스트와 넷마스크 정보를 저장할 문자열 배열
    char host[NI_MAXHOST];
    char netmask[NI_MAXHOST];

    // 시스템의 네트워크 인터페이스 목록을 가져오기
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs"); // 에러 메시지 출력
        exit(EXIT_FAILURE); // 프로그램 종료
    }

    // 모든 네트워크 인터페이스에 대해 반복
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        // 네트워크 인터페이스 주소가 없는 경우 스킵
        if (ifa->ifa_addr == NULL)
            continue;

        // 네트워크 인터페이스의 주소 패밀리 가져오기
        family = ifa->ifa_addr->sa_family;

        // IPv4 주소인 경우만 처리
        if (family == AF_INET) {
            // IPv4 주소를 문자열로 변환하여 host 배열에 저장
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s)); // 오류 메시지 출력
                exit(EXIT_FAILURE); // 프로그램 종료
            }

            // 네트워크 마스크 정보를 문자열로 변환하여 netmask 배열에 저장
            s = getnameinfo(ifa->ifa_netmask, sizeof(struct sockaddr_in), netmask, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s)); // 오류 메시지 출력
                exit(EXIT_FAILURE); // 프로그램 종료
            }

            // 네트워크 인터페이스의 이름 출력
            std::cout << ifa->ifa_name << "\t";
            
            // MAC 주소 가져오기
            struct ifreq ifr;
            int fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (fd == -1) {
                perror("socket"); // 에러 메시지 출력
                continue; // 다음 인터페이스로 이동
            }
            // 네트워크 인터페이스의 이름 설정
            strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);
            // ioctl을 사용하여 MAC 주소 가져오기
            if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
                perror("ioctl"); // 에러 메시지 출력
                close(fd); // 파일 디스크립터 닫기
                continue; // 다음 인터페이스로 이동
            }
            close(fd); // 파일 디스크립터 닫기
            
            // 가져온 MAC 주소 출력
            printMacAddress((u_char *)ifr.ifr_hwaddr.sa_data);
            // 호스트와 넷마스크 출력
            std::cout << "\t" << host << "\t" << netmask << std::endl;
        }
    }

    // 할당된 메모리 해제
    freeifaddrs(ifaddr);
}

#endif

int main() {
    // NetInterface 클래스의 인스턴스 생성
    NetInterface* netInterface = NetInterface::getInstance();
    netInterface->getInterfaceMac();
    for (const auto& interface : netInterface->getInterfaceMac()) {
        std::cout << interface.first << "\t" << interface.second << std::endl;
    }
    // 네트워크 인터페이스 정보 출력
    netInterface->printInterfaceInfo();

    return 0;
}