
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
        Ip ip, GatewayIp;
        Mac mymac;
        try
        {
                ip = Ip::myIp(dev);
                mymac = Mac::getMyMac(dev);
                GatewayIp = Ip::GatewayIp(dev_str);
        }
        catch (const std::exception &e)
        {
                ip = Ip("0.0.0.0");
                mymac = Mac::nullMac();
                GatewayIp = Ip("0.0.0.0");
        }

        printf("Gateway_IP %s\n", std::string(GatewayIp).c_str());
        printf("MY_IP %s\n", std::string(ip).c_str());
        printf("MY_MAC %s\n", std::string(mymac).c_str());

        return 0;
}