
#include <string>

#include "ip.h"
#include "mac.h"


int main(int argc, char *argv[])
{
        if (argc <= 1)
        {
                return -1;
        }
        char *dev = argv[1]; // 네트워크 인터페이스 명
        std::string dev_str = std::string(dev);
        Ip ip = Ip::myIp(dev);
        Mac mac = Mac::getMyMac(dev);
        Ip GatewayIp = Ip::GatewayIp(dev_str);
        printf("MY IP \t\t: %s\n", std::string(ip).c_str());
        printf("Getway IP \t: %s\n", std::string(GatewayIp).c_str());
        printf("MY MAC \t\t: %s\n", std::string(mac).c_str());
        

        return 0;
}