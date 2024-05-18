#include <iostream>
#include <pcap.h>
#include <cstring>
#include <vector>
#include <map>
#include <unordered_map>


// Kernel IP routing table
// Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
// 0.0.0.0         192.168.180.2   0.0.0.0         UG    100    0        0 eth0
// 192.168.180.0   0.0.0.0         255.255.255.0   U     100    0        0 eth0
class RoutingTable{
    private:
        RoutingTable();
        virtual ~RoutingTable();
    
    private:
        std::map<int,std::string> interface_name;

    public:
        static RoutingTable* getInstance();
};