#include <iostream>
#include <pcap.h>
#include <cstring>
#include "netinfo.h"
#include "rtable.h"

int main() {
    // NetInterface 클래스의 인스턴스 생성
    NetInterface* netInterface = NetInterface::getInstance();
    RoutingTable* rt = RoutingTable::getInstance();


    // 네트워크 인터페이스 정보 출력
    netInterface->printInterfaceInfo();
    printf("\n");
    rt->printRoutingTable();


    return 0;
}