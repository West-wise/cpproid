#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>

void getInterfaceInfo(const char* ifname, char* ip, char* mask, char* mac) {
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    // IP Address
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    // Subnet Mask
    ioctl(fd, SIOCGIFNETMASK, &ifr);
    strcpy(mask, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr));

    // MAC Address
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    sprintf(mac, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
            (unsigned char) ifr.ifr_hwaddr.sa_data[0],
            (unsigned char) ifr.ifr_hwaddr.sa_data[1],
            (unsigned char) ifr.ifr_hwaddr.sa_data[2],
            (unsigned char) ifr.ifr_hwaddr.sa_data[3],
            (unsigned char) ifr.ifr_hwaddr.sa_data[4],
            (unsigned char) ifr.ifr_hwaddr.sa_data[5]);

    close(fd);
}

// int main() {
//     char ip[16], mask[16], mac[18];
//     getInterfaceInfo("eth0", ip, mask, mac); // replace "eth0" with your interface name
//     printf("IP: %s\nMask: %s\nMAC: %s\n", ip, mask, mac);
//     return 0;
// }