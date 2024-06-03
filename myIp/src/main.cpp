#include <sys/types.h>  // 필요한 데이터 타입 정의
#include <sys/socket.h>  // socket(), AF_INET, SOCK_DGRAM 등 정의
#include <sys/ioctl.h>  // ioctl() 정의
#include <net/if.h>  // struct ifreq, IFNAMSIZ 등 정의
#include <unistd.h>  // close() 정의
#include <netinet/in.h>  // AF_INET 등 정의
#include <string>

#include "ip.h"
#include "mac.h"
Ip myIp(char *interfaceName)
{
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0)
        {
                throw std::runtime_error("Socket error");
        }

        struct ifreq ifr;
        ifr.ifr_addr.sa_family = AF_INET;
        strncpy((char *)ifr.ifr_name, interfaceName, IFNAMSIZ - 1);

        if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
        {
                close(sock);
                throw std::runtime_error("ioctl error");
        }

        close(sock);

        uint32_t ip_address = ntohl((((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr).s_addr);

        return ip_address;
}

int main(int argc, char *argv[])
{
        if (argc <= 1)
        {
                return -1;
        }
        char *dev = argv[1]; // 네트워크 인터페이스 명
        Ip ip = myIp(dev);
        Mac mac = Mac::getMyMac(dev);
        printf("MY IP : %s\n", std::string(ip).c_str());
        printf("MY MAC : %s\n", std::string(mac).c_str());

        return 0;
}