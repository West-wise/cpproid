#include <iostream>
#include <pcap.h>
#include <cstring>
#include "netinfo.h"
#include "rtable.h"

int main() {
    NetInterface* netInterface = NetInterface::getInstance();
    RoutingTable* rt = RoutingTable::getInstance();
    netInterface->printInterfaceInfo();
    printf("\n");
    rt->printRoutingTable();
    return 0;
}