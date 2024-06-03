#include <cstdio>
#include <cstdint>
#include <pcap.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <algorithm>
#include <unordered_map>

#include "mac.h"
#include "ip.h"
#include "gtrace.h"
#include "ethhdr.h"
#include "arphdr.h"

void handleSignal(int signal);
void cleanup(pcap_t *handle);
void packet_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet);

// 현재 Gateway대역 내의 모든 ip에 arp요청을 날려서 응답이 오는 경우만 저장한다.
// 저장 형식은 <Ip, Mac>으로 저장한다.
pcap_t *global_handle;

struct AddressMap
{
    Mac mac;
    Ip ip;
};

// 인수로 인터페이스 이름과, gateway의 ip를 받는다.
int main(int argc, char *argv[])
{
    gtrace_default("127.0.0.1", 8908, 1 /*stderr*/, "test.log");

    if (argc <= 2)
    {
        GTRACE("ARGUMENT COUNT ERROR");
        return -1;
    }

    GTRACE("ENTRED_MAIN");

    char *dev = argv[1]; // 네트워크 인터페이스 명
    AddressMap atk, sender, target;
    atk.ip = myIp(dev);
    atk.mac = Mac::getMyMac(dev);
    target.ip = Ip(argv[2]);

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(dev, BUFSIZ, 1, 1, errbuf);
    if (handle == nullptr)
    {
        fprintf(stderr, "couldn't open device %s(%s)\n", dev, errbuf);
        return -1;
    }

    pcap_close(handle);
    return 0;
}

void handleSignal(int signal)
{
    printf("Entered Ctrl+C, exit program\n");
    cleanup(global_handle);
    exit(0);
}

void cleanup(pcap_t *handle)
{
    printf("pcap close!\n");
    pcap_close(handle);
}

void packet_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    struct ethheader *eth = (struct ethheader *)packet;

    // IP 패킷인 경우에만 처리
    if (ntohs(eth->ether_type) == 0x0800)
    {
        struct ipheader *ip = (struct ipheader *)(packet + sizeof(struct ethheader));

        // 출발지와 목적지 IP 주소를 출력
        printf("From: %s\n", inet_ntoa(ip->iph_sourceip));
        printf("To: %s\n", inet_ntoa(ip->iph_destip));
    }
}